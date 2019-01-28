#ifndef SKETCH_SCREEN_H
#define SKETCH_SCREEN_H

#include "stm32f4xx.h"                  // Device header
#include "motor_controls.h"

#define X_SIZE 30000
#define Y_SIZE 20000
#define SCALE_FACTOR 50
#define STEP_SIZE 

struct command {
	float x, y;
};

struct sketch_screen {
	struct motor * x_motor;
	struct motor * y_motor;
	const uint8_t * bcode;
	uint32_t bcode_size;
	uint32_t current_command_offset;
	struct command * current_command;
};

#define MIN(a,b) (a < b ? a : b)
#define SIGN(a) (a < 0 ? -1 : 1)
extern struct sketch_screen * init_all(const uint8_t ** data, uint32_t size);
extern void get_next_command(struct sketch_screen  * screen);
void move_next_command(struct sketch_screen * screen);
void draw_scene(struct sketch_screen * screen);

#endif
