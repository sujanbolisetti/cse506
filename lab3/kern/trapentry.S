/* See COPYRIGHT for copyright information. */

#include <inc/mmu.h>
#include <inc/memlayout.h>
#include <inc/trap.h>
#include <kern/macro.h>



###################################################################
# exceptions/interrupts
###################################################################

/* TRAPHANDLER defines a globally-visible function for handling a trap.
 * It pushes a trap number onto the stack, then jumps to _alltraps.
 * Use TRAPHANDLER for traps where the CPU automatically pushes an error code.
 *
 * You shouldn't call a TRAPHANDLER function from C, but you may
 * need to _declare_ one in C (for instance, to get a function pointer
 * during IDT setup).  You can declare the function with
 *   void NAME();
 * where NAME is the argument passed to TRAPHANDLER.
 */
#define TRAPHANDLER(name, num)						\
	.globl name;		/* define global symbol for 'name' */	\
	.type name, @function;	/* symbol type is function */		\
	.align 2;		/* align function definition */		\
	name:			/* function starts here */		\
	pushq $(num);							\
	jmp _alltraps

/* Use TRAPHANDLER_NOEC for traps where the CPU doesn't push an error code.
 * It pushes a 0 in place of the error code, so the trap frame has the same
 * format in either case.
 */
#define TRAPHANDLER_NOEC(name, num)					\
	.globl name;							\
	.type name, @function;						\
	.align 2;							\
	name:								\
	pushq $0;							\
	pushq $(num);							\
	jmp _alltraps

.text


/* Lab 3: Your code here for generating entry points for the different traps.
   Find the PUSHA,POPA macros in kern/macro.h. Since amd64 doesn't support
   pusha,popa so you will find these macros handy. */

TRAPHANDLER_NOEC(su_divbyzero, T_DIVIDE);
TRAPHANDLER_NOEC(su_debug, T_DEBUG);
TRAPHANDLER_NOEC(su_nmi, T_NMI);
TRAPHANDLER_NOEC(su_breakpoint, T_BRKPT);
TRAPHANDLER_NOEC(su_overflow, T_OFLOW);
TRAPHANDLER_NOEC(su_bound, T_BOUND);
TRAPHANDLER_NOEC(su_illop, T_ILLOP);

TRAPHANDLER_NOEC(su_device, T_DEVICE);
TRAPHANDLER(su_dblflt, T_DBLFLT);
TRAPHANDLER(su_tss, T_TSS);
TRAPHANDLER(su_segnp, T_SEGNP);
TRAPHANDLER(su_stack, T_STACK);
TRAPHANDLER(su_gpflt, T_GPFLT);
TRAPHANDLER(su_pgflt, T_PGFLT);
TRAPHANDLER_NOEC(su_fperr, T_FPERR);
TRAPHANDLER_NOEC(su_align, T_ALIGN);
TRAPHANDLER_NOEC(su_mchk, T_MCHK);
TRAPHANDLER_NOEC(su_simd, T_SIMDERR);
TRAPHANDLER_NOEC(su_syscall, T_SYSCALL)
TRAPHANDLER(su_default, T_DEFAULT);


/* Lab 3: Your code here for _alltraps
 *
 * Hint: Be sure to review the x64 calling convention from lab1
 *       for how to pass a pointer to the trapframe.*/
_alltraps:
        subq $8, %rsp
	movw %es, (%rsp)
	subq $8, %rsp
	movw %ds, (%rsp)
        PUSHA
        movw $GD_KD, %ax
        movw %ax, %ds
        movw %ax, %es
	movq %rsp, %rdi
        call trap
