#include "stm32f10x.h"

#include "Common.h"
#include "Board.h"
#include "scope.h"

// ===========================================================
//	File Scope Variables
// ===========================================================
//
	
U16		GTimer;
U8		GTimeout;

U16		TimerKeyScan;

//U8	GeneralBuf[50];

// ===========================================================
//	Function Definitions
// ===========================================================

//-----------------------------------------------------------------------------
// Clock_Init
//-----------------------------------------------------------------------------
//
void	Clock_Init(void)
{
 RCC->CR =  (1 << HSION)  		/*!< Internal High Speed clock enable */
			|(0 << HSIRDY)     	/*!< Internal High Speed clock ready flag */
			|(0x10 << HSITRIM)     	/*!< Internal High Speed clock trimming */
			|(0 << HSICAL)     	/*!< Internal High Speed clock Calibration */
			|(1 << HSEON)     	/*!< External High Speed clock enable */
			|(0 << HSERDY)     	/*!< External High Speed clock ready flag */
			|(0 << HSEBYP)     	/*!< External High Speed clock Bypass */
			|(0 << CSSON)     	/*!< Clock Security System enable */
			|(0 << PLLON)     	/*!< PLL enable */
			|(0 << PLLRDY);    	/*!< PLL clock ready flag */

//	MCO[2:0] : Microcontroller clock output
//		0xx: No clock
//		100: System clock (SYSCLK) selected
//		101: HSI clock selected
//		110: HSE clock selected
//		111: PLL clock divided by 2 selected /* External Quartz Oscilator */
//		
//	USBPRE: USB prescaler
//	Set and cleared by software to generate 48 MHz USB clock. This bit must be valid before
//	enabling the USB clock in the RCC_APB1ENR register. This bit can¡¯t be reset if the USB
//	clock is enabled.
//		0: PLL clock is divided by 1.5
//		1: PLL clock is not divided
//		
//	PLLMUL[3:0] : PLL multiplication factor
//	These bits are written by software to define the PLL multiplication factor. These bits can be
//	written only when PLL is disabled.
//		0000: PLL input clock x 2
//		0001: PLL input clock x 3
//		0010: PLL input clock x 4
//		0011: PLL input clock x 5
//		0100: PLL input clock x 6
//		0101: PLL input clock x 7
//		0110: PLL input clock x 8
//		0111: PLL input clock x 9
//		1000: PLL input clock x 10
//		1001: PLL input clock x 11
//		1010: PLL input clock x 12
//		1011: PLL input clock x 13
//		1100: PLL input clock x 14
//		1101: PLL input clock x 15
//		1110: PLL input clock x 16
//		1111: PLL input clock x 16
//
//	PLLXTPRE: HSE divider for PLL entry
//	Set and cleared by software to divide HSE before PLL entry. This bit can be written only
//	when PLL is disabled.
//		0: HSE clock not divided
//		1: HSE clock divided by 2
//		
//	PLLSRC: PLL entry clock source
//	Set and cleared by software to select PLL clock source. This bit can be written only when
//	PLL is disabled.
//		0: HSI oscillator clock / 2 selected as PLL input clock
//		1: HSE oscillator clock selected as PLL input clock	
//		
//	ADCPRE[1:0] : ADC prescaler
//	Set and cleared by software to select the frequency of the clock to the ADCs.
//		00: PLCK2 divided by 2
//		01: PLCK2 divided by 4
//		10: PLCK2 divided by 6
//		11: PLCK2 divided by 8
//	
//	PPRE2[2:0] : APB high-speed prescaler (APB2)
//	Set and cleared by software to control the division factor of the APB high-speed clock
//	(PCLK2).
//		0xx: HCLK not divided
//		100: HCLK divided by 2
//		101: HCLK divided by 4
//		110: HCLK divided by 8
//		111: HCLK divided by 16

//	PPRE1[2:0] : APB low-speed prescaler (APB1)
//	Set and cleared by software to control the division factor of the APB low-speed clock
//	(PCLK1).
//	Warning: the software has to set correctly these bits to not exceed 36 MHz on this domain.
//		0xx: HCLK not divided
//		100: HCLK divided by 2
//		101: HCLK divided by 4
//		110: HCLK divided by 8
//		111: HCLK divided by 16

//	HPRE[3:0] : AHB prescaler
//	Set and cleared by software to control the division factor of the AHB clock.
//		0xxx: SYSCLK not divided
//		1000: SYSCLK divided by 2
//		1001: SYSCLK divided by 4
//		1010: SYSCLK divided by 8
//		1011: SYSCLK divided by 16
//		1100: SYSCLK divided by 64
//		1101: SYSCLK divided by 128
//		1110: SYSCLK divided by 256
//		1111: SYSCLK divided by 512	
//		
//	SWS[1:0] : System clock switch status
//	Set and cleared by hardware to indicate which clock source is used as system clock.
//		00: HSI oscillator used as system clock
//		01: HSE oscillator used as system clock
//		10: PLL used as system clock
//		11: not applicable

//	SW[1:0] : System clock switch
//	Set and cleared by software to select SYSCLK source.
//	Set by hardware to force HSI selection when leaving Stop and Standby mode or in case of
//	failure of the HSE oscillator used directly or indirectly as system clock (if the Clock Security
//	System is enabled).
//		00: HSI selected as system clock
//		01: HSE selected as system clock
//		10: PLL selected as system clock
//		11: not allowed
//
 RCC->CFGR = (0 << SW)     			/*!< SW[1:0] bits (System clock Switch) */
			|(0 << SWS)     	/*!< SWS[1:0] bits (System Clock Switch Status) */
			|(0 << HPRE)     	/*!< HPRE[3:0] bits (AHB prescaler)  [HCLK] */ 
			|(0b100 << PPRE1)     	/*!< PRE1[2:0] bits (APB1 prescaler) [PCLK1] */
			|(0 << PPRE2)     	/*!< PRE2[2:0] bits (APB2 prescaler) [PCLK2] */
			|(2 << ADCPRE)     	/*!< ADCPRE[1:0] bits (ADC prescaler) */
			|(1 << PLLSRC)     	/*!< PLL entry clock source */
			|(0 << PLLXTPRE)     	/*!< HSE divider for PLL entry */
			|(7 << PLLMULL)     	/*!< PLLMUL[3:0] bits (PLL multiplication factor) */ /* 72 MHz */
			|(0 << USBPRE)     	/*!< USB Device prescaler */
			|(0 << MCO);     	/*!< MCO[2:0] bits (Microcontroller Clock Output) */

 RCC->CR =  (1 << HSION)  			/*!< Internal High Speed clock enable */
			|(0 << HSIRDY)     	/*!< Internal High Speed clock ready flag */
			|(0x10 << HSITRIM)     /*!< Internal High Speed clock trimming */
			|(0 << HSICAL)     	/*!< Internal High Speed clock Calibration */
			|(1 << HSEON)     	/*!< External High Speed clock enable */
			|(0 << HSERDY)     	/*!< External High Speed clock ready flag */
			|(0 << HSEBYP)     	/*!< External High Speed clock Bypass */
			|(0 << CSSON)     	/*!< Clock Security System enable */
			|(1 << PLLON)     	/*!< PLL enable */
			|(0 << PLLRDY);    	/*!< PLL clock ready flag */


 RCC->CIR = (0 << LSIRDYF)     			/*!< LSI Ready Interrupt flag */
			|(0 << LSERDYF)     	/*!< LSE Ready Interrupt flag */
			|(0 << HSIRDYF)     	/*!< HSI Ready Interrupt flag */
			|(0 << HSERDYF)     	/*!< HSE Ready Interrupt flag */
			|(0 << PLLRDYF)     	/*!< PLL Ready Interrupt flag */
			|(0 << CSSF)     		/*!< Clock Security System Interrupt flag */
			|(0 << LSIRDYIE )     	/*!< LSI Ready Interrupt Enable */
			|(0 << LSERDYIE)     	/*!< LSE Ready Interrupt Enable */
			|(0 << HSIRDYIE)     	/*!< HSI Ready Interrupt Enable */
			|(0 << HSERDYIE)     	/*!< HSE Ready Interrupt Enable */
			|(0 << PLLRDYIE)     	/*!< PLL Ready Interrupt Enable */
			|(0 << LSIRDYC)     	/*!< LSI Ready Interrupt Clear */
			|(0 << LSERDYC)     	/*!< LSE Ready Interrupt Clear */
			|(0 << HSIRDYC)     	/*!< HSI Ready Interrupt Clear */
			|(0 << HSERDYC)     	/*!< HSE Ready Interrupt Clear */
			|(0 << PLLRDYC)     	/*!< PLL Ready Interrupt Clear */
			|(0 << CSSC);	     	/*!< Clock Security System Interrupt Clear */


 RCC->APB2RSTR = (0 << AFIORST) 		/*!< Alternate Function I/O reset */
			|(0 << IOPARST)     	/*!< I/O port A reset */
			|(0 << IOPBRST)     	/*!< I/O port B reset */
			|(0 << IOPCRST)     	/*!< I/O port C reset */
			|(0 << IOPDRST)     	/*!< I/O port D reset */
			|(0 << IOPERST)     	/*!< I/O port E reset */
			|(0 << IOPFRST)     	/*!< I/O port F reset */
			|(0 << IOPGRST)     	/*!< I/O port G reset */
			|(0 << ADC1RST)     	/*!< ADC 1 interface reset */
			|(0 << ADC2RST)     	/*!< ADC 2 interface reset */
			|(0 << TIM1RST)     	/*!< TIM1 Timer reset */
			|(0 << SPI1RST)     	/*!< SPI 1 reset */
			|(0 << TIM8RST)     	/*!< TIM8 Timer reset */
			|(0 << USART1RST)     	/*!< USART1 reset */
			|(0 << ADC3RST);     	/*!< ADC3 interface reset */

 RCC->APB1RSTR = (0 << TIM2RST) 		/*!< Timer 2 reset */
			|(0 << TIM3RST)     	/*!< Timer 3 reset */
			|(0 << TIM4RST)     	/*!< Timer 4 reset */
			|(0 << TIM5RST)     	/*!< Timer 5 reset */
			|(0 << TIM6RST)     	/*!< Timer 6 reset */
			|(0 << TIM7RST)     	/*!< Timer 7 reset */
			|(0 << WWDGRST)     	/*!< Window Watchdog reset */
			|(0 << SPI2RST)     	/*!< SPI 2 reset */
			|(0 << SPI3RST)     	/*!< SPI 3 reset */
			|(0 << USART2RST)     	/*!< USART 2 reset */
			|(0 << USART3RST)     	/*!< RUSART 3 reset */
			|(0 << UART4RST )     	/*!< UART 4 reset */
			|(0 << UART5RST)     	/*!< UART 5 reset */
			|(0 << I2C1RST)     	/*!< I2C 1 reset */
			|(0 << I2C2RST)     	/*!< I2C 2 reset */
			|(0 << USBRST)     	/*!< USB Device reset */
			|(0 << CAN1RST)     	/*!< CAN1 reset */
			|(0 << BKPRST)     	/*!< Backup interface reset */
			|(0 << PWRRST)     	/*!< Power interface reset */
			|(0 << DACRST);     	/*!< DAC interface reset */

 
 RCC->AHBENR = (0 << SDIOEN)
				|(0 << FSMCEN)
				|(0 << CRCEN)
				|(1 << FLITFEN)
				|(1 << SRAMEN)
				|(0 << DMA2EN)
				|(1 << DMA1EN);

 RCC->APB1ENR = (0 << DACEN)
				|(0 << PWREN)
				|(0 << BKPEN)
				|(0 << CANEN)
				|(0 << USBEN)
				|(0 << I2C2EN)
				|(0 << I2C1EN)
				|(0 << UART5EN)
				|(0 << UART4EN)
				|(0 << USART3EN)
				|(0 << USART2EN)
				|(0 << SPI3EN)
				|(0 << SPI2EN)
				|(0 << WWDGEN)
				|(0 << TIM7EN)
				|(0 << TIM6EN)
				|(0 << TIM5EN)
				|(1 << TIM4EN)
				|(1 << TIM3EN)
				|(1 << TIM2EN);

 RCC->APB2ENR = (0 << ADC3EN)
				|(1 << USART1EN)
				|(0 << TIM8EN)
				|(0 << SPI1EN)
				|(1 << TIM1EN)
				|(1 << ADC2EN)
				|(1 << ADC1EN)
				|(0 << IOPGEN)
				|(0 << IOPFEN)
				|(0 << IOPEEN)
				|(1 << IOPDEN)
				|(1 << IOPCEN)
				|(1 << IOPBEN)
				|(1 << IOPAEN)
				|(1 << AFIOEN);

 RCC->BDCR = 0x00000000;
 RCC->CSR = 0x00000000;

 // Switch to HSE if it is ready
 if(BitTest(RCC->CR, (1 << HSERDY))) {
	RCC->CFGR &= ~RCC_CFGR_SW;
	RCC->CFGR |= RCC_CFGR_SW_HSE;
 	}

  // Switch to PLL if it is ready
 if(BitTest(RCC->CR, (1 << PLLRDY))) {
	RCC->CFGR &= ~RCC_CFGR_SW;
	RCC->CFGR |= RCC_CFGR_SW_PLL;
 	}

}

//-----------------------------------------------------------------------------
// Misc_Init
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// PORT_Init
//-----------------------------------------------------------------------------
//
// This routine configures the crossbar and GPIO ports.
//
void Port_Init(void)
{
 GPIOA->CRL = ((GPIO_CNF_AnalogIn | GPIO_Mode_In) << (0*4))	// ADC1_IN0
 			|((GPIO_CNF_Floating | GPIO_Mode_In) << (1*4))
 			|((GPIO_CNF_Floating|GPIO_Mode_In) << (2*4))	
 			|((GPIO_CNF_Floating |GPIO_Mode_In) << (3*4))		
 			|((GPIO_CNF_Floating | GPIO_Mode_In) << (4*4))
 			|((GPIO_CNF_Floating | GPIO_Mode_In) << (5*4))
 			|((GPIO_CNF_Floating | GPIO_Mode_In) << (6*4))		
 			|((GPIO_CNF_AF_PP | GPIO_Mode_Out50M) << (7*4));	//Test signal


 GPIOA->CRH = ((GPIO_CNF_Floating| GPIO_Mode_In) << (8 - 8)*4)			// TrigIn
 			|((GPIO_CNF_AF_PP |GPIO_Mode_Out50M) << (9 - 8)*4)	// TX1
 			|((GPIO_CNF_Floating | GPIO_Mode_In) << (10 - 8)*4)	// RX1
 			|((GPIO_CNF_Floating | GPIO_Mode_In) << (11 - 8)*4)
 			|((GPIO_CNF_Floating | GPIO_Mode_In) << (12 - 8)*4)
 			|((GPIO_CNF_Floating | GPIO_Mode_In) << (13 - 8)*4)
 			|((GPIO_CNF_Floating | GPIO_Mode_In) << (14 - 8)*4)
 			|((GPIO_CNF_GP_PP | GPIO_Mode_Out50M) << (15 - 8)*4);	// LED

 GPIOA->ODR = 0xFFFF;

 GPIOB->CRL = 		((GPIO_CNF_GP_PP | GPIO_Mode_Out50M) << (0*4))		// TFT port - D0
 			|((GPIO_CNF_GP_PP | GPIO_Mode_Out50M) << (1*4))		// TFT port - D1
 			|((GPIO_CNF_GP_PP | GPIO_Mode_Out50M) << (2*4))		// TFT port - D2
 			|((GPIO_CNF_GP_PP | GPIO_Mode_Out50M) << (3*4))		// TFT port - D3
 			|((GPIO_CNF_GP_PP | GPIO_Mode_Out50M) << (4*4))		// TFT port - D4
 			|((GPIO_CNF_GP_PP | GPIO_Mode_Out50M) << (5*4))		// TFT port - D5
 			|((GPIO_CNF_GP_PP | GPIO_Mode_Out50M) << (6*4))		// TFT port - D6	
 			|((GPIO_CNF_GP_PP | GPIO_Mode_Out50M) << (7*4));	// TFT port - D7		

 GPIOB->CRH = 		((GPIO_CNF_AF_PP| GPIO_Mode_Out50M) << ((8 - 8)*4))	// Output, Trigger level
 			|((GPIO_CNF_AF_PP| GPIO_Mode_Out50M) << ((9 - 8)*4))	// Output, Gen
 			|((GPIO_CNF_GP_PP | GPIO_Mode_Out50M) << ((10 - 8)*4))	// Output, TFT_nRD
 			|((GPIO_CNF_GP_PP | GPIO_Mode_Out50M) << ((11 - 8)*4))	// Output, TFT_nReset
 			|((GPIO_CNF_IPU | GPIO_Mode_In) << ((12 - 8)*4))	// SW4	
 			|((GPIO_CNF_IPU | GPIO_Mode_In) << ((13 - 8)*4))	// MINUS_BUTTON
 			|((GPIO_CNF_IPU | GPIO_Mode_In) << ((14 - 8)*4))	// PLUS_BUTTON
 			|((GPIO_CNF_IPU | GPIO_Mode_In) << ((15 - 8)*4));	// SW1

 GPIOB->ODR = 0xFFFF;

 GPIOC->CRH = ((GPIO_CNF_GP_PP | GPIO_Mode_Out50M) << (13 - 8)*4)		// TFT_nCS
 			|((GPIO_CNF_GP_PP | GPIO_Mode_Out50M) << (14 - 8)*4)	// TFT_RS
 			|((GPIO_CNF_GP_PP | GPIO_Mode_Out50M) << (15 - 8)*4);	// TFT_nWR

 GPIOC->ODR = 0xFFFF;
 
 GPIOD->CRL = ((GPIO_CNF_Floating | GPIO_Mode_In) << (0*4))		
 			|((GPIO_CNF_Floating | GPIO_Mode_In) << (1*4));		

 // Remap to make PB3 & PB4 available
 AFIO->MAPR &= ~AFIO_MAPR_SWJ_CFG;
 AFIO->MAPR |= AFIO_MAPR_SWJ_CFG_1;
 
}

void	USART1_Init(void)
{
 USART_InitTypeDef USART_InitStructure;
 
  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  
  /* Configure USART1 */
  USART_Init(USART1, &USART_InitStructure);
  
  /* Enable the USART1 */
  USART_Cmd(USART1, ENABLE);

}

void	UartPutc(U8 ch, USART_TypeDef* USARTx)
{
 while(USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET) {
	}
 USART_SendData(USARTx, ch);
}

void	uputs(U8 *s, USART_TypeDef* USARTx)
{
 while(*s != 0) {
 	UartPutc(*s, USARTx);
	s++;
 	}
}
void uputU16(U16 data, USART_TypeDef* USARTx) 
{
	UartPutc((U8) data, USARTx);
	UartPutc(data >> 8, USARTx);
}

void	SysTick_Init(void)
{
 SysTick->VAL = 0;				// Write this register will clear itself and the settings in 
						// SysTick->CTRL
								
 SysTick->CTRL = (1 << SysTick_ENABLE)         
				| (1 << SysTick_TICKINT)         // Counting down to 0 pends the SysTick handler 
				| (1 << SysTick_CLKSOURCE)   	// Clock source. 0 = HCLK/8; 1 = HCLK
				| (0 << SysTick_COUNTFLAG);   	// Count Flag

 SysTick->LOAD = 72000;

// SysTick->CALRB         
// This register is read-only. When clock source is set to HCLK/8 (CLKSOURCE bit is 0) the 
//	TENMS value in this register will be used to generate 1ms tick.
//

}

U16	ADC_Poll(ADC_TypeDef * adc, U8 chn)
{
 // Assuming that the ADC refered has been properly initialized with channel and sample time selected.
  adc->SQR3 = (chn << SQ1); /*!<SQ1[4:0] bits (1st conversion in regular sequence) */
 
 // Start conversion
 adc->CR2 |= (1 << ADON); 
 while(!BitTest(adc->SR, (1 << EOC))) {
 	// Wait for end of conversion
 	}
 return (adc->	DR);
}


void	TFT_Init_Ili9341(void)
{
 U8  tmp;

 // Reset TFT controller (Ili9341)
 SetToHigh(TFT_nRESET_Port, (1 << TFT_nRESET_Bit));
 Delay(5000);	// About 1.1ms
 SetToLow(TFT_nRESET_Port, (1 << TFT_nRESET_Bit));
 Delay(65000);	// About 15ms
 SetToHigh(TFT_nRESET_Port, (1 << TFT_nRESET_Bit));
 tmp = 10;
 while(tmp) {
 	Delay(65535);
	tmp--;
 	}
 
	write_comm(0xcf); 
	write_data(0x00);
	write_data(0xC1);
	write_data(0x30);

	write_comm(0xed); 
	write_data(0x67);
	write_data(0x03);
	write_data(0x12);
	write_data(0x81);

	write_comm(0xcb); 
	write_data(0x39);
	write_data(0x2c);
	write_data(0x00);
	write_data(0x34);
	write_data(0x02);

	write_comm(0xea); 
	write_data(0x00);
	write_data(0x00);

	write_comm(0xe8); 
	write_data(0x85);
	write_data(0x0a);
	write_data(0x78);

	write_comm(0xF7); 
	write_data(0x20);

	write_comm(0xC0); //Power control
	write_data(0x26); //VRH[5:0]

	write_comm(0xC1); //Power control
	write_data(0x01); //SAP[2:0];BT[3:0]

	write_comm(0xC5); //VCM control
	write_data(0x2b);
	write_data(0x2F);

	write_comm(0xc7); 
	write_data(0xc7);

	write_comm(0x3A); 
	write_data(0x55);

	write_comm(0x36); // Memory Access Control
//	write_data(0x08);
	write_data(0x20);
	
	write_comm(0xB1); // Frame Rate Control
	write_data(0x00);
	write_data(0x10);
	
	write_comm(0xB6); // Display Function Control
	write_data(0x0a);
	write_data(0xE2);
	
	write_comm(0xF2); // 3Gamma Function Disable
	write_data(0x00);
	write_comm(0x26); //Gamma curve selected
	write_data(0x01);
	write_comm(0xE0); //Set Gamma
	write_data(0x0f);
	write_data(0x1d);
	write_data(0x1a);
	write_data(0x09);
	write_data(0x0f);
	write_data(0x09);
	write_data(0x46);
	write_data(0x88);
	write_data(0x39);
	write_data(0x05);
	write_data(0x0f);
	write_data(0x03);
	write_data(0x07);
	write_data(0x05);
	write_data(0x00);

	write_comm(0XE1); //Set Gamma
	write_data(0x00);
	write_data(0x22);
	write_data(0x25);
	write_data(0x06);
	write_data(0x10);
	write_data(0x06);
	write_data(0x39);
	write_data(0x22);
	write_data(0x4a);
	write_data(0x0a);
	write_data(0x10);
	write_data(0x0c);
	write_data(0x38);
	write_data(0x3a);
	write_data(0x0F);

	write_comm(0x11); //Exit Sleep
//	delay(120);
	 tmp = 100;
	 while(tmp) {
	 	Delay(50000);
		tmp--;
	 	}
	write_comm(0x29); //display on	
//	write_comm(0x2C);	

 Delay(50000);
 Delay(50000);
 
}


void	write_comm(U8 commport)
{
 // Set TFT_nCS low
 SetToLow(TFT_nCS_Port, (1 << TFT_nCS_Bit));
 // Set up to access Index Register (RS == 0)
 SetToLow(TFT_RS_Port, (1 << TFT_RS_Bit));
// Delay(2);

 TFT_Port = (TFT_Port & 0xFF00) | commport;
 SetToLow(TFT_nWR_Port, (1 << TFT_nWR_Bit));
 SetToHigh(TFT_nWR_Port, (1 << TFT_nWR_Bit));

 // Set up to access Data Register (RS == 1)
 SetToHigh(TFT_RS_Port, (1 << TFT_RS_Bit));
// Delay(2);

 // Set TFT_nCS high
 SetToHigh(TFT_nCS_Port, (1 << TFT_nCS_Bit));
 
}

void write_data(U8 data)
{
 // Set TFT_nCS low
 SetToLow(TFT_nCS_Port, (1 << TFT_nCS_Bit));

 // Set up to access Data Register (RS == 1)
 SetToHigh(TFT_RS_Port, (1 << TFT_RS_Bit));

 TFT_Port = (TFT_Port & 0xFF00) | data;
 SetToLow(TFT_nWR_Port, (1 << TFT_nWR_Bit));
 SetToHigh(TFT_nWR_Port, (1 << TFT_nWR_Bit));

 // Set TFT_nCS high
 SetToHigh(TFT_nCS_Port, (1 << TFT_nCS_Bit));
 
}


void assert_failed(U8 * file, U32 line)
//void assert_failed((U8 *) file, U32 line)
{
}

void	OutputTLvl(void)
{
 TIM4->CCR3 = GetTrigLvl() + GetVPosOfs() + 0x800;
}


