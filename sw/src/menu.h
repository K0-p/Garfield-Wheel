///**
// * Interfaces the ST735
// *
// * 	No cheeseburger relevant

#include "inc/ST7735.h"
#include "inc/Unified_Port_Init.h"
#include <stdint.h>
#include <stdbool.h>

extern uint32_t gyro_state;

void displayinit(void);
void menu(void);
void menuhandler(uint32_t butt);
