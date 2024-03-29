#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "./inc/tm4c123gh6pm.h"
#include "gyro_depends.h"
#include "./inc/ST7735.h"


void I2C3_Init(void);
char I2C3_Wr(int slaveAddr, char memAddr, char data);
char I2C3_Rd(int slaveAddr, char memAddr, int byteCount, char* data);
void Delay(unsigned long counter);
void uart5_init(void);
void UART5_Transmitter(unsigned char data);
void printstring(char *str);
void MPU6050_Init(void);

int main(void){
  int  accX, accY, accZ, GyroX, GyroY, GyroZ, Temper; 
	//float reads[7];	//AX, AY, AZ, t, GX, GY, GZ;
	float AX, AY, AZ, t, GX, GY, GZ;
	char sensordata[14];
	ST7735_InitR(INITR_REDTAB);
	ST7735_InvertDisplay(1);
	I2C3_Init();
	Delay(1000);
	MPU6050_Init();
	Delay(1000);
	uart5_init();
	while(1){
		I2C3_Rd(0x68,ACCEL_XOUT_H, 14, sensordata);
		accX = (int) ( (sensordata[0] << 8 ) | sensordata[1] );
		accY = (int) ( (sensordata[2] << 8 ) | sensordata[3] );
		accZ = (int) ( (sensordata[4] << 8 ) | sensordata[5] );
		Temper = (int) ( (sensordata[6] << 8 ) | sensordata[7] );
		GyroX = (int) ( (sensordata[8] << 8 ) | sensordata[9] );
		GyroY = (int) ( (sensordata[10] << 8 ) | sensordata[11] );
		GyroZ = (int) ( (sensordata[12] << 8 ) | sensordata[13] );
 
		/*
		// Convert The Readings
		reads[0] = (float)accX/16384.0;
		reads[1] = (float)accY/16384.0;
		reads[2] = (float)accZ/16384.0;
		reads[3] = ((float)Temper/340.00)+36.53;
		reads[4] = (float)GyroX/131.0;
		reads[5] = (float)GyroY/131.0;
		reads[6] = (float)GyroZ/131.0;
		//reads[5] = (float)GyroX/131.0;
		//reads[6] = (float)GyroX/131.0;
		 
		//return array of gyro values
		return reads;
		*/
		
		//Convert the Readings
		AX = (float)accX/16384.0;
		AY = (float)accY/16384.0;
		AZ = (float)accZ/16384.0;
		GX = (float)GyroX/131.0;
		GY = (float)GyroY/131.0;
		GZ = (float)GyroZ/131.0;
		t = ((float)Temper/340.00)+36.53;
		char* msg;
		ST7735_SetCursor(1, 1);
		ST7735_OutString("Gx = ");
		ST7735_SetCursor(5, 1);
		ST7735_sDecOut2(GX);
		ST7735_SetCursor(1, 2);
		ST7735_OutString("Gy = ");
		ST7735_SetCursor(5, 2);
		ST7735_sDecOut2(GY);
		ST7735_SetCursor(1, 3);
		ST7735_OutString("Gz = ");
		ST7735_SetCursor(5, 3);
		ST7735_sDecOut2(GZ);
		
		ST7735_SetCursor(1, 5);
		ST7735_OutString("Ax = ");
		ST7735_SetCursor(5, 5);
		ST7735_sDecOut2(AX);
		ST7735_SetCursor(1, 6);
		ST7735_OutString("Ay = ");
		ST7735_SetCursor(5, 6);
		ST7735_sDecOut2(AY);
		ST7735_SetCursor(1, 7);
		ST7735_OutString("Az = ");
		ST7735_SetCursor(5, 7);
		ST7735_sDecOut2(GZ);
		
		Delay(100);
	}
}


void MPU6050_Init(void){
	 //set the sampling rate to 1kHz
	 I2C3_Wr(0x68,SMPLRT_DIV, 0x07);
	 //uses clock of gyro with PLL
	 I2C3_Wr(0x68,PWR_MGMT_1,  0x01);
	 // disable external input and use highest bandwidth frequency
	 I2C3_Wr(0x68,CONFIG, 0x00);
	 // select accelerometer full scale range ±2g
	 I2C3_Wr(0x68,ACCEL_CONFIG,0x00);
	 // select gyro full scale range ±2g
	 I2C3_Wr(0x68,GYRO_CONFIG,0x18);
   // select gyro full scale range ±2g
	 I2C3_Wr(0x68,INT_ENABLE, 0x01);
}

void uart5_init(void){	
  SYSCTL_RCGCUART_R |= 0x20;  /* enable clock to UART5 */
	SYSCTL_RCGCGPIO_R |= 0x10;  /* enable clock to PORTE for PE4/Rx and RE5/Tx */
  Delay(1);
  /* UART0 initialization */
	UART5_CTL_R = 0;         /* UART5 module disbable */
	UART5_IBRD_R = 104;      /* for 9600 baud rate, integer = 104 */
	UART5_FBRD_R = 11;       /* for 9600 baud rate, fractional = 11*/
	UART5_CC_R = 0;          /*select system clock*/
	UART5_LCRH_R = 0x60;     /* data lenght 8-bit, not parity bit, no FIFO */
	UART5_CTL_R = 0x301;     /* Enable UART5 module, Rx and Tx */
	
  /* UART5 TX5 and RX5 use PE4 and PE5. Configure them digital and enable alternate function */
	GPIO_PORTE_DEN_R = 0x30;      /* set PE4 and PE5 as digital */
	GPIO_PORTE_AFSEL_R = 0x30;    /* Use PE4,PE5 alternate function */
	GPIO_PORTE_AMSEL_R = 0;    		/* Turn off analg function*/
	GPIO_PORTE_PCTL_R = 0x00110000;     /* configure PE4 and PE5 for UART */
}

	


void I2C3_Init(void){
	SYSCTL_RCGCGPIO_R |= 0x00000008; 			// Enable the clock for port D
	SYSCTL_RCGCI2C_R |= 0x00000008; 			// Enable the clock for I2C 3
	while((SYSCTL_PRGPIO_R&0x0008) == 0); //Ready?
	
	// Configure Port D pins 0 and 1 as I2C 3
	GPIO_PORTD_AFSEL_R |= 0x00000003;  		//Enable alt functions on PD0,1
	GPIO_PORTD_ODR_R |= 0x00000001; 			// SDA (PD0) pin as open darin
	GPIO_PORTD_DEN_R |= 0x03; 						// Enable deigital I/O on PD1,0
	GPIO_PORTD_PCTL_R |= 0x00000033;
	GPIO_PORTD_AMSEL_R &= ~0x03;					// disable analog functionality on PD1,0
	I2C3_MCR_R = I2C_MCR_MFE; 						//master function enable
	/* Configure I2C 3 clock frequency
	(1 + TIME_PERIOD ) = SYS_CLK /(2*( SCL_LP + SCL_HP ) * I2C_CLK_Freq );
	TIME_PERIOD = 16 ,000 ,000/(2(6+4) *100000) - 1 = 7; */
	I2C3_MTPR_R = 0x07;
}

/* Wait until I2C master is not busy and return error code */
/* If there is no error, return 0 */
static int I2C_wait_till_done(void)
{
    while(I2C3_MCS_R & 1);   /* wait until I2C master is not busy */
    return I2C3_MCS_R & 0xE; /* return I2C error code */
}

/* Write one byte only */
/* byte write: S-(saddr+w)-ACK-maddr-ACK-data-ACK-P */
char I2C3_Wr(int slaveAddr, char memAddr, char data)
{

    char error;

    /* send slave address and starting address */
    I2C3_MSA_R = slaveAddr << 1;
    I2C3_MDR_R = memAddr;
    I2C3_MCS_R = 3;                      /* S-(saddr+w)-ACK-maddr-ACK */

    error = I2C_wait_till_done();       /* wait until write is complete */
    if (error) return error;

    /* send data */
    I2C3_MDR_R = data;
    I2C3_MCS_R = 5;                      /* -data-ACK-P */
    error = I2C_wait_till_done();       /* wait until write is complete */
    while(I2C3_MCS_R & 0x40);            /* wait until bus is not busy */
    error = I2C3_MCS_R & 0xE;
    if (error) return error;

    return 0;       /* no error */
}
char I2C3_Rd(int slaveAddr, char memAddr, int byteCount, char* data)
{
     char error;
    
    if (byteCount <= 0)
        return -1;         /* no read was performed */

    /* send slave address and starting address */
    I2C3_MSA_R = slaveAddr << 1;
    I2C3_MDR_R = memAddr;
    I2C3_MCS_R = 3;       /* S-(saddr+w)-ACK-maddr-ACK */
    error = I2C_wait_till_done();
    if (error)
        return error;

    /* to change bus from write to read, send restart with slave addr */
    I2C3_MSA_R = (slaveAddr << 1) + 1;   /* restart: -R-(saddr+r)-ACK */

    if (byteCount == 1)             /* if last byte, don't ack */
        I2C3_MCS_R = 7;              /* -data-NACK-P */
    else                            /* else ack */
        I2C3_MCS_R = 0xB;            /* -data-ACK- */
    error = I2C_wait_till_done();
    if (error) return error;

    *data++ = I2C3_MDR_R;            /* store the data received */

    if (--byteCount == 0)           /* if single byte read, done */
    {
        while(I2C3_MCS_R & 0x40);    /* wait until bus is not busy */
        return 0;       /* no error */
    }
 
    /* read the rest of the bytes */
    while (byteCount > 1)
    {
        I2C3_MCS_R = 9;              /* -data-ACK- */
        error = I2C_wait_till_done();
        if (error) return error;
        byteCount--;
        *data++ = I2C3_MDR_R;        /* store data received */
    }

    I2C3_MCS_R = 5;                  /* -data-NACK-P */
    error = I2C_wait_till_done();
    *data = I2C3_MDR_R;              /* store data received */
    while(I2C3_MCS_R & 0x40);        /* wait until bus is not busy */
    
    return 0;       /* no error */
}
		
void UART5_Transmitter(unsigned char data)  
{
    while((UART5_FR_R & (1<<5)) != 0); /* wait until Tx buffer not full */
    UART5_DR_R = data;                  /* before giving it another byte */
}

void printstring(char *str)
{
  while(*str)
	{
		UART5_Transmitter(*(str++));
	}
}

void Delay(unsigned long counter)
{
	unsigned long i = 0;
	
	for(i=0; i< counter*10000; i++);
}