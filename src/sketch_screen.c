#include "stm32f4xx.h"                  // Device header
#include "motor_controls.h"
#include "sketch_screen.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

uint8_t circ(int8_t in)
{
	if (in > 7)
		return 0;
	else
	if (in < 0)
		return 7;
	else 
		return in;
}

struct sketch_screen * init_all(const uint8_t ** data, uint32_t size)
{
	struct sketch_screen * screen = malloc(sizeof(struct sketch_screen));
	screen->x_motor = malloc(sizeof(struct motor));
	screen->y_motor = malloc(sizeof(struct motor));
	screen->current_command = malloc(sizeof(struct command));
	screen->bcode = *data;
	screen->bcode_size = size;
	screen->current_command_offset = 0;
	
	screen->x_motor->gpios = malloc(4 * sizeof(GPIO_TypeDef *));
	screen->y_motor->gpios = malloc(4 * sizeof(GPIO_TypeDef *));
	screen->x_motor->pins = malloc(4);
	screen->y_motor->pins = malloc(4);
	
	memcpy(screen->x_motor->gpios, (GPIO_TypeDef * [])LEFT_GPIOS, 4 * sizeof(void *));
	memcpy(screen->y_motor->gpios, (GPIO_TypeDef *[])RIGHT_GPIOS, 4 * sizeof(void *));
	memcpy(screen->x_motor->pins, (uint8_t [])LEFT_PINS, 4);
	memcpy(screen->y_motor->pins, (uint8_t [])RIGHT_PINS, 4);
	
	motor_init(screen->x_motor);
	motor_init(screen->y_motor);
	
	screen->current_command->x = 0;
	screen->current_command->y = 0;
	
	return screen;
}

float bintofloat(char low, char high)
{
	float out = low / (float)(1 << 8);
	out += high;
	return out;
}

void get_next_command(struct sketch_screen  * screen) {
	screen->current_command->x = bintofloat(
															screen->bcode[screen->current_command_offset++], 
															screen->bcode[screen->current_command_offset++]) * SCALE_FACTOR;
	
	screen->current_command->y = bintofloat(
															screen->bcode[screen->current_command_offset++], 
															screen->bcode[screen->current_command_offset++]) * SCALE_FACTOR;
}

void move_next_command(struct sketch_screen * screen)
{
	int32_t deltax = screen->current_command->x - screen->x_motor->loc;
	int32_t deltay = screen->current_command->y - screen->y_motor->loc;
	
	if(deltax != 0)
	{
		float deltaerr = fabs((float) deltay / deltax);
		float err = 0;
		uint32_t y_step = 0;
		while(deltax != 0)
		{
			screen->x_motor->step = circ(screen->x_motor->step + SIGN(deltax));
			err += deltaerr;
			if(err >= 0.5)
			{
				y_step = SIGN(deltay);
				err -= 1;
			}
			screen->y_motor->step = circ(screen->y_motor->step + y_step);
			motor_step(screen->x_motor, 0);
			motor_step(screen->y_motor, 4);
			screen->x_motor->loc += SIGN(deltax);
			screen->y_motor->loc += y_step;
			deltax -= SIGN(deltax);
			y_step = 0;
		}
	}
	else
	{
		while(deltay != 0)
		{
			screen->y_motor->step = circ(screen->y_motor->step + SIGN(deltay));
			motor_step(screen->y_motor, 4);
			screen->y_motor->loc += SIGN(deltay);
			deltay -= SIGN(deltay);
		}
	}
}

void draw_scene(struct sketch_screen * screen)
{
	for(uint32_t i = 0; i < screen->bcode_size; i += 2)
	{
		get_next_command(screen);
		move_next_command(screen);
	}
	uint32_t xmindelta = MIN(screen->x_motor->loc, X_SIZE - screen->x_motor->loc);
	uint32_t ymindelta = MIN(screen->y_motor->loc, Y_SIZE - screen->y_motor->loc);
	if(xmindelta < ymindelta)
	{
		screen->current_command->x = screen->x_motor->loc < X_SIZE - screen->x_motor->loc ?  screen->x_motor->loc : X_SIZE;
		move_next_command(screen);
	}
	screen->current_command->x = 0;
	screen->current_command->y = 0;
	move_next_command(screen);
}
