///**
// * Interfaces the ST735
// *
// * 	No cheeseburger relevant

#include "menu.h"
#include "../img/pipe.h"

#define GARF_TEXT 0x041f

//initial display screen upon device launch
void displayinit(void){
	ST7735_InitR(INITR_REDTAB);
	//ST7735_FillScreen(ST7735_BLACK);
  ST7735_SetCursor(0, 0);
	Unified_Port_Init();
	int x = 0;
  int y = 159;	//height-1
	//inputs: x,y,file,width,heigth
	ST7735_DrawBitmap(x, y, launch, 128, 160);
	return;
}

//menu handler
void menu(void){
	ST7735_FillScreen(0x0000);
	ST7735_DrawBitmap(0, MENU_HEIGHT-5, menu_text, 128, MENU_HEIGHT);
	ST7735_DrawBitmap(0, 75, head, HEAD__WIDTH, HEAD__HEIGHT);
	ST7735_DrawBitmap(0, 115, head, HEAD__WIDTH, HEAD__HEIGHT);
	ST7735_DrawBitmap(0, 155, head, HEAD__WIDTH, HEAD__HEIGHT);
	ST7735_DrawString(6,6,"CONFIGURATION", GARF_TEXT);
	ST7735_DrawString(6,10,"INFORMATION", GARF_TEXT);
	ST7735_DrawString(6,14,"CREDITS", GARF_TEXT);
	return;
}

