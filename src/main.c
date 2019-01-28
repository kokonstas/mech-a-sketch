/*----------------------------------------------------------------------------
 * Name:    Blinky.c
 * Purpose: LED Flasher
 *----------------------------------------------------------------------------
 * This file is part of the uVision/ARM development tools.
 * This software may only be used under the terms of a valid, current,
 * end user licence from KEIL for a compatible version of KEIL software
 * development tools. Nothing else gives you the right to use this software.
 *
 * This software is supplied "AS IS" without warranties of any kind.
 *
 * Copyright (c) 2016 Keil - An ARM Company. All rights reserved.
 *----------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>


#include "stm32f4xx.h"                  // Device header

#include "motor_controls.h"
#include "sketch_screen.h"

extern int stdout_init (void);

/*----------------------------------------------------------------------------
 * SystemCoreClockConfigure: configure SystemCoreClock using HSI
                             (HSE is not populated on Nucleo board)
 *----------------------------------------------------------------------------*/
void SystemCoreClockConfigure(void) {

  RCC->CR |= ((uint32_t)RCC_CR_HSION);                     /* Enable HSI */
  while ((RCC->CR & RCC_CR_HSIRDY) == 0);                  /* Wait for HSI Ready */

  RCC->CFGR = RCC_CFGR_SW_HSI;                             /* HSI is system clock */
  while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI);  /* Wait for HSI used as system clock */

  FLASH->ACR  = (FLASH_ACR_PRFTEN     |                    /* Enable Prefetch Buffer */
                 FLASH_ACR_ICEN       |                    /* Instruction cache enable */
                 FLASH_ACR_DCEN       |                    /* Data cache enable */
                 FLASH_ACR_LATENCY_5WS );                  /* Flash 5 wait state */

  RCC->CFGR |= (RCC_CFGR_HPRE_DIV1  |                      /* HCLK = SYSCLK */
                RCC_CFGR_PPRE1_DIV2 |                      /* APB1 = HCLK/2 */
                RCC_CFGR_PPRE2_DIV1  );                    /* APB2 = HCLK/1 */

  RCC->CR &= ~RCC_CR_PLLON;                                /* Disable PLL */

  /* PLL configuration:  VCO = HSI/M * N,  Sysclk = VCO/P */
  RCC->PLLCFGR = ( 16ul                   |                /* PLL_M =  16 */
                 (200ul <<  6)            |                /* PLL_N = 200 */
                 (  0ul << 16)            |                /* PLL_P =   2 */
                 (RCC_PLLCFGR_PLLSRC_HSI) |                /* PLL_SRC = HSI */
                 (  7ul << 24)            |                /* PLL_Q =   7 */
                 (  2ul << 28)             );              /* PLL_R =   2 */

  RCC->CR |= RCC_CR_PLLON;                                 /* Enable PLL */
  while((RCC->CR & RCC_CR_PLLRDY) == 0) __NOP();           /* Wait till PLL is ready */

  RCC->CFGR &= ~RCC_CFGR_SW;                               /* Select PLL as system clock source */
  RCC->CFGR |=  RCC_CFGR_SW_PLL;
  while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);  /* Wait till PLL is system clock src */
}

/*----------------------------------------------------------------------------
 * main: blink LED and check button state
 *----------------------------------------------------------------------------*/
int main (void) {
  int32_t num = 0;
 
  SystemCoreClockConfigure();                              /* configure HSI as System Clock */
  SystemCoreClockUpdate();
	
  stdout_init();                                           /* Initializ Serial interface */

  SysTick_Config(SystemCoreClock / 1000);                  /* SysTick 1 msec interrupts */
	const uint8_t ** data;
	uint32_t size = imageFileInfo("main.bcode", data);
	struct sketch_screen * screen = init_all(data, size);
	
	draw_scene(screen);
	
	for(int i = 0; ; i += 1)
	{
		//screen->current_command->x++;
		//move_next_command(screen);
		//move();
	}
	
}
