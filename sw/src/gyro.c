#include "TM4C123GH6PM.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "gyro_depends.h"

void I2C3_Init(void);
char I2C3_Wr(int slaveAddr, char memAddr, char data);
char I2C3_Rd(int slaveAddr, char memAddr, int byteCount, char* data);
void Delay(unsigned long counter);
void MPU6050_Init(void);

float[] gyromain(void)
{
  int  accX, accY, accZ, GyroX, GyroY, GyroZ, Temper; 
	float reads[7];	//AX, AY, AZ, t, GX, GY, GZ;
	char sensordata[14];
	I2C3_Init();
	Delay(1000);
	MPU6050_Init();
	Delay(1000);
  uart5_init();
 I2C3_Rd(0x68,ACCEL_XOUT_H, 14, sensordata);
 accX = (int) ( (sensordata[0] << 8 ) | sensordata[1] );
 accY = (int) ( (sensordata[2] << 8 ) | sensordata[3] );
 accZ = (int) ( (sensordata[4] << 8 ) | sensordata[5] );
 Temper = (int) ( (sensordata[6] << 8 ) | sensordata[7] );
 GyroX = (int) ( (sensordata[8] << 8 ) | sensordata[9] );
 GyroY = (int) ( (sensordata[10] << 8 ) | sensordata[11] );
 GyroZ = (int) ( (sensordata[12] << 8 ) | sensordata[13] );
 
		
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
}



void MPU6050_Init(void)
{
 I2C3_Wr(0x68,SMPLRT_DIV, 0x07);
 I2C3_Wr(0x68,PWR_MGMT_1,  0x01);
 I2C3_Wr(0x68,CONFIG, 0x00);
 I2C3_Wr(0x68,ACCEL_CONFIG,0x00); 
 I2C3_Wr(0x68,GYRO_CONFIG,0x18);
 I2C3_Wr(0x68,INT_ENABLE, 0x01);

}



void I2C3_Init(void)
{
SYSCTL->RCGCGPIO  |= 0x00000008 ; // Enable the clock for port D
SYSCTL->RCGCI2C   |= 0x00000008 ; // Enable the clock for I2C 3
GPIOD->DEN |= 0x03; // Assert DEN for port D
// Configure Port D pins 0 and 1 as I2C 3
GPIOD->AFSEL |= 0x00000003 ;
GPIOD->PCTL |= 0x00000033 ;
GPIOD->ODR |= 0x00000002 ; // SDA (PD1 ) pin as open darin
I2C3->MCR  = 0x0010 ; // Enable I2C 3 master function
/* Configure I2C 3 clock frequency
(1 + TIME_PERIOD ) = SYS_CLK /(2*
( SCL_LP + SCL_HP ) * I2C_CLK_Freq )
TIME_PERIOD = 16 ,000 ,000/(2(6+4) *100000) - 1 = 7 */
I2C3->MTPR  = 0x07 ;
}

/* Wait until I2C master is not busy and return error code */
/* If there is no error, return 0 */
static int I2C_wait_till_done(void)
{
    while(I2C3->MCS & 1);   /* wait until I2C master is not busy */
    return I2C3->MCS & 0xE; /* return I2C error code */
}

/* Write one byte only */
/* byte write: S-(saddr+w)-ACK-maddr-ACK-data-ACK-P */
char I2C3_Wr(int slaveAddr, char memAddr, char data)
{

    char error;

    /* send slave address and starting address */
    I2C3->MSA = slaveAddr << 1;
    I2C3->MDR = memAddr;
    I2C3->MCS = 3;                      /* S-(saddr+w)-ACK-maddr-ACK */

    error = I2C_wait_till_done();       /* wait until write is complete */
    if (error) return error;

    /* send data */
    I2C3->MDR = data;
    I2C3->MCS = 5;                      /* -data-ACK-P */
    error = I2C_wait_till_done();       /* wait until write is complete */
    while(I2C3->MCS & 0x40);            /* wait until bus is not busy */
    error = I2C3->MCS & 0xE;
    if (error) return error;

    return 0;       /* no error */
}
char I2C3_Rd(int slaveAddr, char memAddr, int byteCount, char* data)
{
     char error;
    
    if (byteCount <= 0)
        return -1;         /* no read was performed */

    /* send slave address and starting address */
    I2C3->MSA = slaveAddr << 1;
    I2C3->MDR = memAddr;
    I2C3->MCS = 3;       /* S-(saddr+w)-ACK-maddr-ACK */
    error = I2C_wait_till_done();
    if (error)
        return error;

    /* to change bus from write to read, send restart with slave addr */
    I2C3->MSA = (slaveAddr << 1) + 1;   /* restart: -R-(saddr+r)-ACK */

    if (byteCount == 1)             /* if last byte, don't ack */
        I2C3->MCS = 7;              /* -data-NACK-P */
    else                            /* else ack */
        I2C3->MCS = 0xB;            /* -data-ACK- */
    error = I2C_wait_till_done();
    if (error) return error;

    *data++ = I2C3->MDR;            /* store the data received */

    if (--byteCount == 0)           /* if single byte read, done */
    {
        while(I2C3->MCS & 0x40);    /* wait until bus is not busy */
        return 0;       /* no error */
    }
 
    /* read the rest of the bytes */
    while (byteCount > 1)
    {
        I2C3->MCS = 9;              /* -data-ACK- */
        error = I2C_wait_till_done();
        if (error) return error;
        byteCount--;
        *data++ = I2C3->MDR;        /* store data received */
    }

    I2C3->MCS = 5;                  /* -data-NACK-P */
    error = I2C_wait_till_done();
    *data = I2C3->MDR;              /* store data received */
    while(I2C3->MCS & 0x40);        /* wait until bus is not busy */
    
    return 0;       /* no error */
}
		


void Delay(unsigned long counter)
{
	unsigned long i = 0;
	
	for(i=0; i< counter*10000; i++);
}