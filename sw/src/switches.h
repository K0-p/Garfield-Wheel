// *    Buttons (recommended garfield kart keybind):
// *				MODE 1 [1 pulse LED]: Full controls w/ gyro
// *				MODE 2 [2 pulse LED]:	No gryo, just use left/right buttons to steer with accelerate auto on
// *			Start: PD2	(bind to forwards)
// *			Reset: RST
// *			Up:		 PE3	(TOGGLE MODE  --  only impacts the following 3 buttons)
// *			Right: PB5	1:(bind to pause) 		2:(bind to right steering)
// *			Down:	 PE2	1:(bind to backwards) 2:(bind to toggle off/forwards)
// *			Left:	 PB4	1:(bind to drift) 		2:(bind to left steering)

#include <stdbool.h>
#include <stdint.h>

extern struct buttholes_t{
	uint32_t start;
	uint32_t up;
	uint32_t right;
	uint32_t down;
	uint32_t left;
};

struct buttholes_t getbuttons(uint32_t portD);
