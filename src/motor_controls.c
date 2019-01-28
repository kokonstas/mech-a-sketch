#include "stm32f4xx.h"                  // Device header
#include "motor_controls.h"


volatile uint32_t msTicks;                            /* counts 1ms timeTicks */

const uint8_t step_seq[8][4] = {{1,0,0,1},
	   {1,0,0,0},
	   {1,1,0,0},
	   {0,1,0,0},
	   {0,1,1,0},
	   {0,0,1,0},
	   {0,0,1,1},
	   {0,0,0,1}};
/*----------------------------------------------------------------------------
 * SysTick_Handler:
 *----------------------------------------------------------------------------*/
void SysTick_Handler(void) {
  msTicks++;
}

/*----------------------------------------------------------------------------
 * Delay: delays a number of Systicks
 *----------------------------------------------------------------------------*/
void Delay (uint32_t dlyTicks) {
	if(dlyTicks == 0)
		return;
  uint32_t curTicks;

  curTicks = msTicks;
  while ((msTicks - curTicks) < dlyTicks) { __NOP(); }
}


void motor_init(struct motor * mot) {
	RCC->AHB1ENR |=  (1ul << 1);                  /* Enable GPIOB clock         */
	RCC->AHB1ENR |=  (1ul << 2);                  /* Enable GPIOC clock         */
	RCC->AHB1ENR |=  (1ul << 3);                  /* Enable GPIOD clock         */
	for(int i = 0; i < 4; ++i)
	{
		/* Configure LED (PA.5) pins as push-pull outputs, No pull-up, pull-down */
		mot->gpios[i]->MODER   &= ~((1ul << 2*mot->pins[i]));
		mot->gpios[i]->MODER   |=  ((1ul << 2*mot->pins[i]));
		mot->gpios[i]->OTYPER  &= ~((1ul <<   mot->pins[i]));
		mot->gpios[i]->OSPEEDR |= ~((3ul << 2*mot->pins[i]));
		mot->gpios[i]->PUPDR   &= ~((2ul << 2*mot->pins[i]));
		mot->gpios[i]->PUPDR   |=  ((2ul << 2*mot->pins[i]));
	}
}

void motor_step(struct motor * mot, uint8_t sleep) {
	for(int i = 0; i < 4; ++i)
	{
    mot->gpios[i]->BSRR = (1 << (mot->pins[i] + 16));
    mot->gpios[i]->BSRR = (step_seq[mot->step][i] << mot->pins[i]);
    //gpio[i]->BSRR = (1 << pins[i]);
	}
	Delay(sleep);
	
}

