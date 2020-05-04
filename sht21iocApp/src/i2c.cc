#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <thread>
#include <chrono>

#include "i2c.hh"

uint8	I2cError;
int	I2cDevHandle;

//--------------------------------------------------------------------------------------------------
// Name:	  I2C_Open 
// Function:  Open device/port
//			  Raspberry Hardwarerevision 1.0	P1 = /dev/i2c-0
//			  Raspberry Hardwarerevision 2.0	P1 = /dev/i2c-1
//            
// Parameter: Devicename (String)
// Return:    
//--------------------------------------------------------------------------------------------------
void I2C_Open(const char *dev){
  I2cError = 0;
  if ((I2cDevHandle = open(dev, O_RDWR)) < 0)
    I2cError |= ERROR_I2C_OPEN;
}	

//--------------------------------------------------------------------------------------------------
// Name:      I2C_Close
// Function:  Close port/device
//            
// Parameter: -
// Return:    -
//--------------------------------------------------------------------------------------------------
void I2C_Close(void){
  close(I2cDevHandle);
}	

//--------------------------------------------------------------------------------------------------
// Name:      I2C_Setup
// Function:  Setup port for communication
//            
// Parameter: mode (typical "I2C_SLAVE"), Device address (typical slave address from device) 
// Return:    -
//--------------------------------------------------------------------------------------------------
void I2C_Setup(uint32 mode,uint8 addr){	
  if(!I2cError){
    if (ioctl(I2cDevHandle, mode,addr) < 0)
      I2cError |= ERROR_I2C_SETUP;					
  }						
}	

//--------------------------------------------------------------------------------------------------
// Name:      I2C_Write1
// Function:  Write a singel byte to I2C-Bus
//            
// Parameter: Byte to send
// Return:    -
//--------------------------------------------------------------------------------------------------
void I2C_Write1(uint8 d){
  if(!I2cError){
    if((write(I2cDevHandle, &d, 1)) != 1)
      I2cError |= ERROR_I2C_WRITE;	
  }
}

//--------------------------------------------------------------------------------------------------
// Name:      I2C_Write2
// Function:  Write two bytes to I2C
//            
// Parameter: First byte, second byte
// Return:    -
//--------------------------------------------------------------------------------------------------
void I2C_Write2(uint8 d0,uint8 d1){
  uint8 buf[2];
  if(!I2cError){
    buf[0]=d0;
    buf[1]=d1;
    if ((write(I2cDevHandle, buf,2)) != 2)
      I2cError |= ERROR_I2C_WRITE;		
  }
}

//--------------------------------------------------------------------------------------------------
// Name:      I2C_Read
// Function:  Read a number of bytes
//            
// Parameter: Pointer to buffer, Number of bytes to read
// Return:    -
//--------------------------------------------------------------------------------------------------
void I2C_Read(uint8 *data,uint8 length){
  if(!I2cError){
    if (read(I2cDevHandle, data,length) != length)
      I2cError |= ERROR_I2C_READ;		
  }
}

//--------------------------------------------------------------------------------------------------
// Name:      I2C_PrintError
// Function:  Print error flags as readable text.
//            
// Parameter: -
// Return:    -
//--------------------------------------------------------------------------------------------------
void I2C_PrintError(void){	
  if(I2cError & ERROR_I2C_OPEN)	 printf("Failed to open I2C-Port\r\n");
  if(I2cError & ERROR_I2C_SETUP) printf("Failed to setup I2C-Port\r\n");
  if(I2cError & ERROR_I2C_READ)	 printf("I2C read error\r\n");
  if(I2cError & ERROR_I2C_WRITE) printf("I2C write error\r\n");
}


uint8 I2C_GetError(void){
  return I2cError;
}

void I2C_ClearError(void){
  I2cError = 0;
}


uint8	Sht21Error;

//--------------------------------------------------------------------------------------------------
// Name:      SHT21_Read
// Function:  
//            
// Parameter: 
// Return:    
//--------------------------------------------------------------------------------------------------
uint8 SHT21_Read(int16 *temperature,uint8 *humidity){
  uint32 val;
  uint8	buf[4];
		
  Sht21Error = 0;
	
  //=== Softreset ==================================================
	
  I2C_Write1(0xFE);			// softreset < 15ms
  std::this_thread::sleep_for (std::chrono::milliseconds(50));
	
  //=== Temperature =================================================

  I2C_Write1(0xF3);			// start temperature measurement
  // Temperature@14Bit typ=66ms max=85ms
  std::this_thread::sleep_for (std::chrono::milliseconds(260));
  I2C_Read(buf,3);			// read temperature data
	
  if(buf[2] == SHT21_CalcCrc(buf,2)){  // check CRC
    val = buf[0];
    val <<= 8;
    val += buf[1];
    val &= 0xFFFC;
    *temperature = ((val * 512) / 9548);
    *temperature = ((*temperature) - 937) / 2;       
  }
  else{
    Sht21Error |= ERROR_SHT21_CRC_TEMP;
  }
	
  //=== Humidity ===================================================

  I2C_Write1(0xF5);			// start humidity measurement
  // RH@12Bit typ=22ms max=20ms
  std::this_thread::sleep_for (std::chrono::milliseconds(60));
  I2C_Read(buf,3);			// read humidity data
  
  if(buf[2] == SHT21_CalcCrc(buf,2)){	
    val = buf[0];
    val <<= 8;
    val += buf[1];
    val &= 0xFFFC;
    val = ((val * 256) / 134215) - 6;
    *humidity = val;
  }	
  else{
    Sht21Error |= ERROR_SHT21_CRC_TEMP;
  }
	
  if(I2cError) Sht21Error |= ERROR_SHT21_I2C;
  return Sht21Error;
}


//------------------------------------------------------------------------------
// Name:      
// Function:  
//            
// Parameter: 
// Return:    
//------------------------------------------------------------------------------
uint8 SHT21_CalcCrc(uint8 *data,uint8 nbrOfBytes){
  // CRC
  //const u16t POLYNOMIAL = 0x131; //P(x)=x^8+x^5+x^4+1 = 100110001
  uint8 byteCtr,bit,crc;
  crc = 0;
  //calculates 8-Bit checksum with given polynomial
  for (byteCtr = 0; byteCtr < nbrOfBytes; ++byteCtr){ 
    crc ^= (data[byteCtr]);
    for (bit = 8; bit > 0; --bit){
      if (crc & 0x80) crc = (crc << 1) ^ 0x131;
      else
	crc = (crc << 1);
    }
  }
  return(crc);
}

//--------------------------------------------------------------------------------------------------
// Name:      PrintSht21Error
// Function:  Print error flags as readable text.
//            
// Parameter: -
// Return:    -
//--------------------------------------------------------------------------------------------------
void SHT21_PrintError(void){
  if(Sht21Error & ERROR_SHT21_I2C)		printf("ERROR I2C-Port\n");
  if(Sht21Error & ERROR_SHT21_CRC_TEMP)		printf("ERROR Temperature CRC\n");
  if(Sht21Error & ERROR_SHT21_CRC_HUMIDITY)	printf("ERROR Humidity CRC\n");
}

uint8 SHT21_GetError(void){
  return Sht21Error;
}
void SHT21_ClearError(void){
  Sht21Error = 0;
}

