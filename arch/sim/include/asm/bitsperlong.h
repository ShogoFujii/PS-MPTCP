#ifndef _ASM_SIM_BITSPERLONG_H
#define _ASM_SIM_BITSPERLONG_H

#ifdef CONFIG_64BIT
#define BITS_PER_LONG 64
#else
#define BITS_PER_LONG 32
#endif /* CONFIG_64BIT */

#define __BITS_PER_LONG BITS_PER_LONG

#endif /* _ASM_SIM_BITSPERLONG_H */
