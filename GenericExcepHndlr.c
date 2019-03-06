// ---------------------------------------------------------------------------
// This file contains generic handlers for the NIOS Reset signal and 
// general exceptions. These functions should be used as is (un-edited) to 
// implement exception handling in a Nios C program.
//
// 2012.11.18 RMK: Cosmetic cleanups
// 2017.03.22 RMK: More cosmetic cleanups
// 2017.10.09 RMK: Added more detailed comments to replace some vague ones
// ---------------------------------------------------------------------------

// function prototypes
void IrqParser(void);      // to be called by "ExcepHandler"

// global variables 
extern int  key_pressed;

// ---------------------------------------------------------------------------
// Reset Code...
// Giving this code a section __attribute__ with named ".reset" directs  
// the linker program to locate this code at the proper reset vector 
// address. This code just calls the main program upon system reset. 
// ---------------------------------------------------------------------------
void ResetHandler (void) __attribute__ ((section (".reset")));
void ResetHandler (void)
{
  asm (".set  noat");           // Issue NO warning if asm uses at reg
  asm (".set  nobreak");	    // Issue NO warning if asm uses bt & ba regs
  asm ("movia r2, main");	    // Load Address of C "main" program
  asm ("jmp   r2");             // Jump to C "main" program 
}


// ---------------------------------------------------------------------------
// Generic, Global Exception Handler... 
// Giving the code a section __attribute__ named ".exceptions" directs the 
// linker program to locate this code at the exceptions vector address.
// 
// This code does the following:
// 1. Push all registers onto the stack,
// 2. Call IrqParser() to identify the IRQ source & call correct ISR,
// 3. Pop  all registers from the stack.
// 
// WARNING!! This code should NOT be modified! Instead, the C lang. code 
// in the function IrqParser() should be edited and modified as 
// needed for a given application program.
// ---------------------------------------------------------------------------
void ExcepHandler (void) __attribute__ ((section (".exceptions")));
void ExcepHandler (void)
{
  asm ( ".set  noat" );           // Issue NO warning if asm uses at reg
  asm ( ".set  nobreak" );        // Issue NO warning if asm uses bt & ba regs
  asm ( "subi  sp, sp, 128" );    // create stack frame for all regs
  asm ( "stw   et, 96(sp)" );     // save R24 "exception temp" reg

  asm ( "rdctl et, ctl4" );            // read ctl4 (external IRQs ipending reg)
  asm ( "beq  et, r0, SKIP_EA_DEC" );  // IF IRQ is NOT external, skip next line
  asm ( "subi ea, ea, 4" );            // ELSE IRQ IS external, so decrem 
                                       // return addr in R29 (ea) by    
                                       // one instruction, so that the 
	                                   // interrupted instruc will be re-run

  asm ( "SKIP_EA_DEC:" );         // Save all regs before calling IRQ parser
  asm (	"stw	r1,  4(sp)" );	     
  asm (	"stw	r2,  8(sp)" );
  asm (	"stw	r3,  12(sp)" );
  asm (	"stw	r4,  16(sp)" );
  asm (	"stw	r5,  20(sp)" );
  asm (	"stw	r6,  24(sp)" );
  asm (	"stw	r7,  28(sp)" );
  asm (	"stw	r8,  32(sp)" );
  asm (	"stw	r9,  36(sp)" );
  asm (	"stw	r10, 40(sp)" );
  asm (	"stw	r11, 44(sp)" );
  asm (	"stw	r12, 48(sp)" );
  asm (	"stw	r13, 52(sp)" );
  asm (	"stw	r14, 56(sp)" );
  asm (	"stw	r15, 60(sp)" );
  asm (	"stw	r16, 64(sp)" );
  asm (	"stw	r17, 68(sp)" );
  asm (	"stw	r18, 72(sp)" );
  asm (	"stw	r19, 76(sp)" );
  asm (	"stw	r20, 80(sp)" );
  asm (	"stw	r21, 84(sp)" );
  asm (	"stw	r22, 88(sp)" );
  asm (	"stw	r23, 92(sp)" );    
                                        // skip r24 = et, it was saved above
  asm (	"stw	r25, 100(sp)" );        // r25 = bt 
  asm (	"stw	r26, 104(sp)" );        // r26 = gp
                                        // skip r27 = sp, do not push it
  asm (	"stw	r28, 112(sp)" );        // r28 = fp
  asm (	"stw	r29, 116(sp)" );        // r29 = ea
  asm (	"stw	r30, 120(sp)" );        // r30 = ba
  asm (	"stw	r31, 124(sp)" );        // r31 = ra
  asm (	"addi	fp,  sp, 128" );

  
  asm (	"call	IrqParser" );	  // Call C language IRQ parsing routine, which
                                  // must be included in the application code.
 
  asm (	"ldw	r1,  4(sp)" );	  // Restore all regs upon return fm IRQ parser	
  asm (	"ldw	r2,  8(sp)" );
  asm (	"ldw	r3,  12(sp)" );
  asm (	"ldw	r4,  16(sp)" );
  asm (	"ldw	r5,  20(sp)" );
  asm (	"ldw	r6,  24(sp)" );
  asm (	"ldw	r7,  28(sp)" );
  asm (	"ldw	r8,  32(sp)" );
  asm (	"ldw	r9,  36(sp)" );
  asm (	"ldw	r10, 40(sp)" );
  asm (	"ldw	r11, 44(sp)" );
  asm (	"ldw	r12, 48(sp)" );
  asm (	"ldw	r13, 52(sp)" );
  asm (	"ldw	r14, 56(sp)" );
  asm (	"ldw	r15, 60(sp)" );
  asm (	"ldw	r16, 64(sp)" );
  asm (	"ldw	r17, 68(sp)" );
  asm (	"ldw	r18, 72(sp)" );
  asm (	"ldw	r19, 76(sp)" );
  asm (	"ldw	r20, 80(sp)" );
  asm (	"ldw	r21, 84(sp)" );
  asm (	"ldw	r22, 88(sp)" );
  asm (	"ldw	r23, 92(sp)" );
  asm (	"ldw	r24, 96(sp)" );
  asm (	"ldw	r25, 100(sp)" );        // r25 = bt
  asm (	"ldw	r26, 104(sp)" );        // r26 = gp
                                        // r27 = sp, do not pop; was not pushed
  asm (	"ldw	r28, 112(sp)" );        // r28 = fp
  asm (	"ldw	r29, 116(sp)" );        // r29 = ea
  asm (	"ldw	r30, 120(sp)" );        // r30 = ba
  asm (	"ldw	r31, 124(sp)" );        // r31 = ra

  asm (	"addi	sp,  sp, 128" );  // delete stack frame created before call

  asm (	"eret" );                 // return from exception
}

