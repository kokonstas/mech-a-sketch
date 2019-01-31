#ifndef MOTOR_CONTROLS_C
#define MOTOR_CONTROLS_C
#include "stm32f4xx.h"                  // Device header

struct motor {
	GPIO_TypeDef ** gpios;
	uint8_t * pins;
	uint8_t step;
	uint32_t loc;
};

#define RIGHT_PINS {9, 8, 8, 6}
#define RIGHT_GPIOS {GPIOC, GPIOC, GPIOB, GPIOC}
#define LEFT_PINS  {10, 11, 12, 2}
#define LEFT_GPIOS {GPIOC, GPIOC, GPIOC, GPIOD}

extern void SysTick_Handler(void);
extern void motor_init(struct motor *);
extern void motor_step(struct motor *, uint8_t sleep);
extern void Delay(uint32_t dlyTicks);
#endif
