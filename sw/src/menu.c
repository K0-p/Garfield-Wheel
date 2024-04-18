///**
// * Interfaces the ST735
// *
// * 	No cheeseburger relevant

#include "menu.h"
#include "../img/pipe.h"
#include "switches.h"

#define GARF_TEXT 0x041f

uint32_t menu_state;
uint32_t headspot;
uint32_t subspot;

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
	menu_state = 0;
	headspot = 0;
	subspot = 0;
	return;
}

void drawhead(uint32_t spot){
	ST7735_FillRect(0,43,HEAD__WIDTH,155,0x0000);
	if(spot == 0) ST7735_DrawBitmap(0, 75, head, HEAD__WIDTH, HEAD__HEIGHT);
	if(spot == 1) ST7735_DrawBitmap(0, 115, head, HEAD__WIDTH, HEAD__HEIGHT);
	if(spot == 2) ST7735_DrawBitmap(0, 155, head, HEAD__WIDTH, HEAD__HEIGHT);
	return;
}

//menu handler
void menu(void){
	ST7735_FillScreen(0x0000);
	ST7735_DrawBitmap(0, MENU_HEIGHT-5, menu_text, 128, MENU_HEIGHT);
	//headspot = 0;
	//drawhead();
	ST7735_DrawString(6,6,"CONFIGURATION", GARF_TEXT);
	ST7735_DrawString(6,10,"INFORMATION", GARF_TEXT);
	ST7735_DrawString(6,14,"CREDITS", GARF_TEXT);
	return;
}

//config handler
void submenu(void){
	ST7735_FillScreen(0x0000);
	ST7735_DrawBitmap(0, MENU_HEIGHT-5, menu_text, 128, MENU_HEIGHT);
	ST7735_DrawString(6,6,"GARFIELD KART", GARF_TEXT);
	ST7735_DrawString(6,10,"DRUNK DRIVING", GARF_TEXT);
	ST7735_DrawString(6,14,"STANDARD", GARF_TEXT);
	return;
}

void config(void){
	submenu();
	//ST7735_FillScreen(0x00FF);
	return;
}

void info(void){
	ST7735_FillScreen(0xFF00);
	return;
}

void credits(void){
	ST7735_FillScreen(0x0F0F);
	return;
}

// *	Buttons: [start,up,right,down,left]
void menuhandler(uint32_t butt){
	//invert so 1 = press
	butt = (~butt)&0x1F;
	uint32_t next_state = press_task(menu_state,butt);
	//display init
	if(next_state == 0){
		if(menu_state != 0) ST7735_DrawBitmap(0, 159, launch, 128, 160);
		menu_state = 0;
		return;
	}
	//menu
	if(next_state == 1){
		if(menu_state != 1) menu();
		else{
			//up
			if(butt == 0x8){
				if(headspot == 0) headspot = 2;
				else headspot--;
			}
			//down
			if(butt == 0x2){
				if(headspot == 2) headspot = 0;
				else headspot++;
			}
		}
		drawhead(headspot);
		menu_state = 1;
		return;
	}
	//choose submenu
	if(next_state == 69){
		//launch config menu
		if(headspot == 0){
			if(menu_state != 2) config();
			menu_state = 2;
			drawhead(subspot);
		}
		//launch info menu
		if(headspot == 1){
			menu_state = 3;
			info();
		}
		//launch credits menu
		else{
			menu_state = 4;
			credits();
		}
		return;
	}
	//config
	if(next_state == 2){
		//up
		if(butt == 0x8){
			if(subspot == 0) subspot = 2;
			else subspot--;
		}
		//down
		if(butt == 0x2){
			if(subspot == 2) subspot = 0;
			else subspot++;
		}
		drawhead(subspot);
		return;
	}
	//info	-- should never get here (not interactive)
	if(next_state == 3){
		return;
	}
	//credits	-- should never get here (not interactive)
	if(next_state == 4){
		return;
	}
	//in game
	//else return;
	else{
		printf("ENTERD\n");
		return;
	}
}

