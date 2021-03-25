#define __VECTORS_NXP13X

/*=========================================================================*/
/*  DEFINE: All extern Data                                                */
/*=========================================================================*/
extern unsigned long _estack;

/*=========================================================================*/
/*  DEFINE: Prototypes                                                     */
/*=========================================================================*/
void ResetHandler(void);

void NMI_Handler (void) __attribute__((weak));
void HardFault_Handler (void) __attribute__((weak));
void IntDefaultHandler (void) __attribute__((weak));

/*=========================================================================*/
/*  DEFINE: All code exported                                              */
/*=========================================================================*/
/*
 * This is our vector table.
 */
__attribute__ ((section(".vectors"), used))
void (* const gVectors[])(void) = 
{
    (void (*)(void))((unsigned long)&_estack), // The initial stack pointer
    ResetHandler,                           // The reset handler

    // Various fault handlers
    NMI_Handler,                            // The NMI handler        
    HardFault_Handler,                      // The hard fault handler 
    IntDefaultHandler,                      // The MPU fault handler  
    IntDefaultHandler,                      // The bus fault handler  
    IntDefaultHandler,                      // The usage fault handler
    0,                                      // Reserved               
    0,                                      // Reserved               
    0,                                      // Reserved               
    0,                                      // Reserved               
    IntDefaultHandler,                      // SVCall handler         
    IntDefaultHandler,                      // Debug monitor handler  
    0,                                      // Reserved               
    IntDefaultHandler,                      // The PendSV handler     
    IntDefaultHandler,                      // The SysTick handler    

    // Wakeup I/O pins handlers
    IntDefaultHandler,                      // PIO0_0  Wakeup
    IntDefaultHandler,                      // PIO0_1  Wakeup
    IntDefaultHandler,                      // PIO0_2  Wakeup
    IntDefaultHandler,                      // PIO0_3  Wakeup
    IntDefaultHandler,                      // PIO0_4  Wakeup
    IntDefaultHandler,                      // PIO0_5  Wakeup
    IntDefaultHandler,                      // PIO0_6  Wakeup
    IntDefaultHandler,                      // PIO0_7  Wakeup
    IntDefaultHandler,                      // PIO0_8  Wakeup
    IntDefaultHandler,                      // PIO0_9  Wakeup
    IntDefaultHandler,                      // PIO0_10 Wakeup
    IntDefaultHandler,                      // PIO0_11 Wakeup
    IntDefaultHandler,                      // PIO1_0  Wakeup
    IntDefaultHandler,                      // PIO1_1  Wakeup
    IntDefaultHandler,                      // PIO1_2  Wakeup
    IntDefaultHandler,                      // PIO1_3  Wakeup
    IntDefaultHandler,                      // PIO1_4  Wakeup
    IntDefaultHandler,                      // PIO1_5  Wakeup
    IntDefaultHandler,                      // PIO1_6  Wakeup
    IntDefaultHandler,                      // PIO1_7  Wakeup
    IntDefaultHandler,                      // PIO1_8  Wakeup
    IntDefaultHandler,                      // PIO1_9  Wakeup
    IntDefaultHandler,                      // PIO1_10 Wakeup
    IntDefaultHandler,                      // PIO1_11 Wakeup
    IntDefaultHandler,                      // PIO2_0  Wakeup
    IntDefaultHandler,                      // PIO2_1  Wakeup
    IntDefaultHandler,                      // PIO2_2  Wakeup
    IntDefaultHandler,                      // PIO2_3  Wakeup
    IntDefaultHandler,                      // PIO2_4  Wakeup
    IntDefaultHandler,                      // PIO2_5  Wakeup
    IntDefaultHandler,                      // PIO2_6  Wakeup
    IntDefaultHandler,                      // PIO2_7  Wakeup
    IntDefaultHandler,                      // PIO2_8  Wakeup
    IntDefaultHandler,                      // PIO2_9  Wakeup
    IntDefaultHandler,                      // PIO2_10 Wakeup
    IntDefaultHandler,                      // PIO2_11 Wakeup
    IntDefaultHandler,                      // PIO3_0  Wakeup
    IntDefaultHandler,                      // PIO3_1  Wakeup

    // Specific peripheral irq handlers
    IntDefaultHandler,                      // I2C0
    IntDefaultHandler,                      // CT16B0 (16-bit Timer 0)
    IntDefaultHandler,                      // CT16B1 (16-bit Timer 1)
    IntDefaultHandler,                      // CT32B0 (32-bit Timer 0)
    IntDefaultHandler,                      // CT32B1 (32-bit Timer 1)
    IntDefaultHandler,                      // SSP0
    IntDefaultHandler,                      // UART0
    IntDefaultHandler,                      // USB IRQ
    IntDefaultHandler,                      // USB FIQ
    IntDefaultHandler,                      // ADC (A/D Converter)
    IntDefaultHandler,                      // WDT (Watchdog Timer)
    IntDefaultHandler,                      // BOD (Brownout Detect)
    IntDefaultHandler,                      // Flash (IP2111 Flash Memory Controller)
    IntDefaultHandler,                      // PIO INT3
    IntDefaultHandler,                      // PIO INT2
    IntDefaultHandler,                      // PIO INT1
    IntDefaultHandler                       // PIO INT0
};                                          
                                            
void NMI_Handler(void){while(1);}           
void HardFault_Handler(void) {while(1);}    
void IntDefaultHandler(void) {}             
                                            
/*** EOF ***/                               
                                            
// PIO3_2  Wakeup
// PIO3_3  Wakeup