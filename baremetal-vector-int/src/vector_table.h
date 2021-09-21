/*
   Baremetal main program with timer interrupt.
   SPDX-License-Identifier: Unlicense

   https://five-embeddev.com/

   Tested with sifive-hifive-revb, but should not have any
   dependencies to any particular implementation.

   Declarations of interrupt service routine entry points. 

   If no implementation is defined then an alias to a default "NOP"
   implementation will be linked instead.
   
*/

#ifndef VECTOR_TABLE_H
#define VECTOR_TABLE_H


/** Symbol for machine mode vector table - do not call 
 */
void riscv_mtvec_table(void)  __attribute__ ((naked));
void riscv_stvec_table(void)  __attribute__ ((naked));
void riscv_utvec_table(void)  __attribute__ ((naked));

/** Machine mode syncronous exception handler.

http://five-embeddev.com/riscv-isa-manual/latest/machine.html#machine-trap-vector-base-address-register-mtvec

When vectored interrupts are enabled, interrupt cause 0, which
corresponds to user-mode software interrupts, are vectored to the same
location as synchronous exceptions. This ambiguity does not arise in
practice, since user-mode software interrupts are either disabled or
delegated to user mode.

 */
void riscv_mtvec_exception(void) __attribute__ ((interrupt ("machine")) );

/** Machine mode software interrupt */
void riscv_mtvec_msi(void) __attribute__ ((interrupt ("machine") )); 
/** Machine mode timer interrupt */
void riscv_mtvec_mti(void) __attribute__ ((interrupt ("machine") ));
/** Machine mode al interrupt */
void riscv_mtvec_mei(void) __attribute__ ((interrupt ("machine") ));

/** Supervisor mode software interrupt */
void riscv_mtvec_ssi(void) __attribute__ ((interrupt ("machine")) );
/** Supervisor mode timer interrupt */
void riscv_mtvec_sti(void) __attribute__ ((interrupt ("machine")) );
/** Supervisor mode al interrupt */
void riscv_mtvec_sei(void) __attribute__ ((interrupt ("machine")) );

/** Supervisor mode syncronous exception handler. */
void riscv_stvec_exception(void) __attribute__ ((interrupt ("supervisor")) );

/** Supervisor mode software interrupt */
void riscv_stvec_ssi(void) __attribute__ ((interrupt ("supervisor")) );
/** Supervisor mode timer interrupt */
void riscv_stvec_sti(void) __attribute__ ((interrupt ("supervisor")) );
/** Supervisor mode al interrupt */
void riscv_stvec_sei(void) __attribute__ ((interrupt ("supervisor")) );

/** User mode software interrupt */
void riscv_utvec_usi(void) __attribute__ ((interrupt ("user")) );
/** User mode timer interrupt */
void riscv_utvec_uti(void) __attribute__ ((interrupt ("user")) );
/** User mode al interrupt */
void riscv_utvec_uei(void) __attribute__ ((interrupt ("user")) );


#endif // #ifndef VECTOR_TABLE_H

