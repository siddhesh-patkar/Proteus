#define __VECTORS_ATSAM3N

/*=========================================================================*/
/*  DEFINE: All extern Data                                                */
/*=========================================================================*/
extern unsigned long _estack;

void IntDefaultHandler (void) 
{
   while(1) 
   {
   }
}

/*=========================================================================*/
/*  DEFINE: Prototypes                                                     */
/*=========================================================================*/
void ResetHandler(void) __attribute__ ((weak, alias("IntDefaultHandler")));
                                            
/* Cortex-M3 core handlers */
void NMI_Handler(void) __attribute__ ((weak, alias("IntDefaultHandler")));
void HardFault_Handler(void) __attribute__ ((weak, alias("IntDefaultHandler")));
void MemManage_Handler(void) __attribute__ ((weak, alias("IntDefaultHandler")));
void BusFault_Handler(void) __attribute__ ((weak, alias("IntDefaultHandler")));
void UsageFault_Handler(void) __attribute__ ((weak, alias("IntDefaultHandler")));
void SVC_Handler(void) __attribute__ ((weak, alias("IntDefaultHandler")));
void DebugMon_Handler(void) __attribute__ ((weak, alias("IntDefaultHandler")));
void PendSV_Handler(void) __attribute__ ((weak, alias("IntDefaultHandler")));
void SysTick_Handler(void) __attribute__ ((weak, alias("IntDefaultHandler")));

/* Peripherals handlers */
void SUPC_Handler(void) __attribute__ ((weak, alias("IntDefaultHandler")));
void RSTC_Handler(void) __attribute__ ((weak, alias("IntDefaultHandler")));
void RTC_Handler(void) __attribute__ ((weak, alias("IntDefaultHandler")));
void RTT_Handler(void) __attribute__ ((weak, alias("IntDefaultHandler")));
void WDT_Handler(void) __attribute__ ((weak, alias("IntDefaultHandler")));
void PMC_Handler(void) __attribute__ ((weak, alias("IntDefaultHandler")));
void EFC_Handler(void) __attribute__ ((weak, alias("IntDefaultHandler")));
void UART0_Handler(void) __attribute__ ((weak, alias("IntDefaultHandler")));
void UART1_Handler(void) __attribute__ ((weak, alias("IntDefaultHandler")));
void PIOA_Handler(void) __attribute__ ((weak, alias("IntDefaultHandler")));
void PIOB_Handler(void) __attribute__ ((weak, alias("IntDefaultHandler")));
void PIOC_Handler(void) __attribute__ ((weak, alias("IntDefaultHandler")));
void USART0_Handler(void) __attribute__ ((weak, alias("IntDefaultHandler")));
void USART1_Handler(void) __attribute__ ((weak, alias("IntDefaultHandler")));
void TWI0_Handler(void) __attribute__ ((weak, alias("IntDefaultHandler")));
void TWI1_Handler(void) __attribute__ ((weak, alias("IntDefaultHandler")));
void SPI_Handler(void) __attribute__ ((weak, alias("IntDefaultHandler")));
void TC0_Handler(void) __attribute__ ((weak, alias("IntDefaultHandler")));
void TC1_Handler(void) __attribute__ ((weak, alias("IntDefaultHandler")));
void TC2_Handler(void) __attribute__ ((weak, alias("IntDefaultHandler")));
void TC3_Handler(void) __attribute__ ((weak, alias("IntDefaultHandler")));
void TC4_Handler(void) __attribute__ ((weak, alias("IntDefaultHandler")));
void TC5_Handler(void) __attribute__ ((weak, alias("IntDefaultHandler")));
void ADC_Handler(void) __attribute__ ((weak, alias("IntDefaultHandler")));
void DACC_Handler(void) __attribute__ ((weak, alias("IntDefaultHandler")));
void PWM_Handler(void) __attribute__ ((weak, alias("IntDefaultHandler")));

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
   ResetHandler,              // The reset handler

	NMI_Handler,
	HardFault_Handler,
	MemManage_Handler,
	BusFault_Handler,
	UsageFault_Handler,
	0, 0, 0, 0,               /* Reserved */
	SVC_Handler,
	DebugMon_Handler,
	0,                        /* Reserved  */
	PendSV_Handler,
	SysTick_Handler,

	/* Configurable interrupts  */
	SUPC_Handler,             /* 0  Supply Controller */
	RSTC_Handler,             /* 1  Reset Controller */
	RTC_Handler,              /* 2  Real Time Clock */
	RTT_Handler,              /* 3  Real Time Timer */
	WDT_Handler,              /* 4  Watchdog Timer */
 	PMC_Handler,              /* 5  PMC */
	EFC_Handler,              /* 6  EEFC */
	IntDefaultHandler,        /* 7  Reserved */
 	UART0_Handler,            /* 8  UART0 */
	UART1_Handler,            /* 9  UART1 */
	IntDefaultHandler,        /* 10 Reserved */
	PIOA_Handler,             /* 11 Parallel IO Controller A */
	PIOB_Handler,             /* 12 Parallel IO Controller B */
	PIOC_Handler,             /* 13 Parallel IO Controller C */
	USART0_Handler,           /* 14 USART 0 */
	USART1_Handler,           /* 15 USART 1 */
	IntDefaultHandler,        /* 16 Reserved */
	IntDefaultHandler,        /* 17 Reserved */
	IntDefaultHandler,        /* 18 Reserved */
	TWI0_Handler,             /* 19 TWI 0 */
	TWI1_Handler,             /* 20 TWI 1 */
 	SPI_Handler,              /* 21 SPI */
	IntDefaultHandler,        /* 22 Reserved */
	TC0_Handler,              /* 23 Timer Counter 0 */
	TC1_Handler,              /* 24 Timer Counter 1 */
	TC2_Handler,              /* 25 Timer Counter 2 */
	TC3_Handler,              /* 26 Timer Counter 3 */
 	TC4_Handler,              /* 27 Timer Counter 4 */
	TC5_Handler,              /* 28 Timer Counter 5 */
	ADC_Handler,              /* 29 ADC controller */
	DACC_Handler,             /* 30 DAC controller */
	PWM_Handler,	           /* 31 PWM */
	IntDefaultHandler         /* 32 not used */
};                                          
                                            
/*** EOF ***/                               
