/*
 * Copyright (c) 2012, NVIDIA Corporation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <linux/linkage.h>

#include <asm/assembler.h>
#include <asm/asm-offsets.h>

#include "fuse.h"
#include "sleep.h"
#include "flowctrl.h"

#define TEGRA30_POWER_HOTPLUG_SHUTDOWN	(1 << 27) /* Hotplug shutdown */

#if defined(CONFIG_HOTPLUG_CPU) || defined(CONFIG_PM_SLEEP)
/*
 * tegra30_hotplug_shutdown(void)
 *
 * Powergates the current CPU.
 * Should never return.
 */
ENTRY(tegra30_hotplug_shutdown)
	/* Powergate this CPU */
	mov	r0, #TEGRA30_POWER_HOTPLUG_SHUTDOWN
	bl	tegra30_cpu_shutdown
	mov	pc, lr			@ should never get here
ENDPROC(tegra30_hotplug_shutdown)

/*
 * tegra30_cpu_shutdown(unsigned long flags)
 *
 * Puts the current CPU in wait-for-event mode on the flow controller
 * and powergates it -- flags (in R0) indicate the request type.
 *
 * r10 = SoC ID
 * corrupts r0-r4, r10-r12
 */
ENTRY(tegra30_cpu_shutdown)
	cpu_id	r3
	tegra_get_soc_id TEGRA_APB_MISC_VIRT, r10
	cmp	r10, #TEGRA30
	bne	_no_cpu0_chk	@ It's not Tegra30

	cmp	r3, #0
	moveq	pc, lr		@ Must never be called for CPU 0
_no_cpu0_chk:

	ldr	r12, =TEGRA_FLOW_CTRL_VIRT
	cpu_to_csr_reg r1, r3
	add	r1, r1, r12	@ virtual CSR address for this CPU
	cpu_to_halt_reg r2, r3
	add	r2, r2, r12	@ virtual HALT_EVENTS address for this CPU

	/*
	 * Clear this CPU's "event" and "interrupt" flags and power gate
	 * it when halting but not before it is in the "WFE" state.
	 */
	movw	r12, \
		FLOW_CTRL_CSR_INTR_FLAG | FLOW_CTRL_CSR_EVENT_FLAG | \
		FLOW_CTRL_CSR_ENABLE
	cmp	r10, #TEGRA30
	moveq	r4, #(1 << 4)			@ wfe bitmap
	movne	r4, #(1 << 8)			@ wfi bitmap
 ARM(	orr	r12, r12, r4, lsl r3	)
 THUMB(	lsl	r4, r4, r3		)
 THUMB(	orr	r12, r12, r4		)
	str	r12, [r1]

	/* Halt this CPU. */
	mov	r3, #0x400
delay_1:
	subs	r3, r3, #1			@ delay as a part of wfe war.
	bge	delay_1;
	cpsid	a				@ disable imprecise aborts.
	ldr	r3, [r1]			@ read CSR
	str	r3, [r1]			@ clear CSR

	tst	r0, #TEGRA30_POWER_HOTPLUG_SHUTDOWN
	beq	flow_ctrl_setting_for_lp2

	/* flow controller set up for hotplug */
	mov	r3, #FLOW_CTRL_WAITEVENT		@ For hotplug
	b	flow_ctrl_done
flow_ctrl_setting_for_lp2:
	/* flow controller set up for LP2 */
	cmp	r10, #TEGRA30
	moveq   r3, #FLOW_CTRL_WAIT_FOR_INTERRUPT	@ For LP2
	movne	r3, #FLOW_CTRL_WAITEVENT
flow_ctrl_done:
	cmp	r10, #TEGRA30
	str	r3, [r2]
	ldr	r0, [r2]
	b	wfe_war

__cpu_reset_again:
	dsb
	.align 5
	wfeeq					@ CPU should be power gated here
	wfine
wfe_war:
	b	__cpu_reset_again

	/*
	 * 38 nop's, which fills reset of wfe cache line and
	 * 4 more cachelines with nop
	 */
	.rept 38
	nop
	.endr
	b	.				@ should never get here

ENDPROC(tegra30_cpu_shutdown)
#endif

#ifdef CONFIG_PM_SLEEP
/*
 * tegra30_sleep_cpu_secondary_finish(unsigned long v2p)
 *
 * Enters LP2 on secondary CPU by exiting coherency and powergating the CPU.
 */
ENTRY(tegra30_sleep_cpu_secondary_finish)
	mov	r7, lr

	/* Flush and disable the L1 data cache */
	bl	tegra_disable_clean_inv_dcache

	/* Powergate this CPU. */
	mov	r0, #0                          @ power mode flags (!hotplug)
	bl	tegra30_cpu_shutdown
	mov	r0, #1                          @ never return here
	mov	pc, r7
ENDPROC(tegra30_sleep_cpu_secondary_finish)

/*
 * tegra30_tear_down_cpu
 *
 * Switches the CPU to enter sleep.
 */
ENTRY(tegra30_tear_down_cpu)
	mov32	r6, TEGRA_FLOW_CTRL_BASE

	b	tegra30_enter_sleep
ENDPROC(tegra30_tear_down_cpu)

/*
 * tegra30_enter_sleep
 *
 * uses flow controller to enter sleep state
 * executes from IRAM with SDRAM in selfrefresh when target state is LP0 or LP1
 * executes from SDRAM with target state is LP2
 * r6 = TEGRA_FLOW_CTRL_BASE
 */
tegra30_enter_sleep:
	cpu_id	r1

	cpu_to_csr_reg	r2, r1
	ldr	r0, [r6, r2]
	orr	r0, r0, #FLOW_CTRL_CSR_INTR_FLAG | FLOW_CTRL_CSR_EVENT_FLAG
	orr	r0, r0, #FLOW_CTRL_CSR_ENABLE
	str	r0, [r6, r2]

	mov	r0, #FLOW_CTRL_WAIT_FOR_INTERRUPT
	orr	r0, r0, #FLOW_CTRL_HALT_CPU_IRQ | FLOW_CTRL_HALT_CPU_FIQ
	cpu_to_halt_reg r2, r1
	str	r0, [r6, r2]
	dsb
	ldr	r0, [r6, r2] /* memory barrier */

halted:
	isb
	dsb
	wfi	/* CPU should be power gated here */

	/* !!!FIXME!!! Implement halt failure handler */
	b	halted

#endif
