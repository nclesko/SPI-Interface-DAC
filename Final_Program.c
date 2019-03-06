#include <alt_types.h>
#include "GenericExcepHndlr.c"
#include <stdio.h>

#define TIM_ADDR 0x00081060;
#define SPI_ADDR 0x00081040;

void IsrTimer();

volatile alt_u32 increment = 0;  // Global variable for stepping
// ---------------------------------------------------------------------------
// Reset Code...
// Giving this code a section __attribute__ with named ".reset" directs
// the linker program to locate this code at the proper reset vector
// address. This code just calls the main program upon system reset.
// ---------------------------------------------------------------------------
void ResetHandler (void) __attribute__ ((section (".reset")));

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

// -----------------------------------------------------------------------------
// IRQ Source Parser for all external interrupts.
// Func: Examine reg ctrl4 (ipending) to determine which device caused the IRQ,
//       then call the correct ISR to service that device.
// Note: 1. In this example application, there is ONLY the Pushbutton ISR,
//          but others can easily be edited into this function for other apps.
//       2. This example ASSUMES pushbutton IRQ is mapped to bit 1 of ctrl regs
//          ctl3 & ctl4. The ACTUAL mapping will be done by QSYS.
// -----------------------------------------------------------------------------
void IrqParser(void)             // called by global exception handler
{
  alt_u32 irqPend;
  irqPend = __builtin_rdctl(4);  // Read ipending reg (ctl4) into a C variable
  if (irqPend & 0x1)             // If ipending reg [bit 1] = 1,
     IsrTimer();               // then call Pushbutton ISR to service the IRQ

  return;                        // Return to global exception handler
}

// -----------------------------------------------------------------------------
// Application dependent Interrupt Service Routine (ISR) for Timer IRQ.
// Func: When the timer produces a time out, a Timer IRQ will be generated
//       and all of the red leds will be toggled.
// -----------------------------------------------------------------------------
void IsrTimer(void)           // called by IRQ Source Parser (IrqParser)
{
  volatile alt_u16 * ptrTimer = (alt_u16 *) 0x00081060;   // init ptr to Timer
  volatile alt_u16 * ptrSpi = (alt_u16 *) 0x00081040;
  *ptrTimer = 0x00;  // Clear the timer's timeout flag bit
  
  if(increment == 0xFFF)
  {
    increment = 0;          // reset step value
  }
    else
  {
    increment = increment + 1;   // increment step value
  }
	
  while( (*(ptrSpi + 4) & 0x40) == 0) {};
  *(ptrSpi + 2) =  0x1000 | increment;    // Save new output

  return;
}

//----------------------------------------------------------------------------
void main ()
{
  volatile alt_u16 * ptrTimer = (alt_u16 *) 0x00081060;   // init ptr to Timer
  volatile alt_u32 * pioPtr = (alt_u32 *)   0x00081090;
  // Union to create 32 bit snapshot
  union
  {                             
    alt_u16 timHalf[2];         // out of two 16 bit arrays half snaps.
    alt_u32 timFull;
  } period;

  period.timFull = 1221;
  __builtin_wrctl(0, 5);   // Write 5 to status reg to enable all exceps & IRQs
  __builtin_wrctl(3, 1);   // Write 2 to ienable reg to enable timer IRQ

  *(ptrTimer + 4) = 1221;      // Set the period of timer to 1221 cycles
  *(ptrTimer + 6) = 0;        
  
  *(ptrTimer + 2) |= 0x1;  // Set the timer TimeOut Interrupt enable
  *(ptrTimer + 2) |= 0x2;  // Enable continuous mode
  *(ptrTimer + 2) |= 4;    // Start the timer
  *(pioPtr + 3) = 0;
  
  while(1) {
	  
	// If push button 1 is pressed
	if ((*(pioPtr + 3) & 0x02) == 0x02)
    {
      *ptrTimer = 0;             // Clear IRQ signal by clearing timer TO bit
      *(pioPtr + 3) = 0;          
      period.timFull = period.timFull / 2;
      *(ptrTimer + 4) = period.timHalf[0];     
      *(ptrTimer + 6) = period.timHalf[1];     
      *(ptrTimer + 2) |= 0x1;  // Set the timer TimeOut Interrupt enable
  *(ptrTimer + 2) |= 0x2;  // Enable continuous mode
  *(ptrTimer + 2) |= 4;    // Start the timer
    }
	
	// If push button 2 is pressed
    if ((*(pioPtr + 3) & 0x04) == 0x04)
    {
      *ptrTimer = 0;             // Clear IRQ signal by clearing timer TO bit
      *(pioPtr + 3) = 0;
      period.timFull = period.timFull*2;
      *(ptrTimer + 4) = period.timHalf[0];      
      *(ptrTimer + 6) = period.timHalf[1];     
  *(ptrTimer + 2) |= 0x1;  // Set the timer TimeOut Interrupt enable
  *(ptrTimer + 2) |= 0x2;  // Enable continuous mode
  *(ptrTimer + 2) |= 4;    // Start the timer    
    } 
  }	  
	  
}
    