#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>

#include <cstdio>
#include <cstring>
#include <cstdint>
#include <chrono>
#include <sstream>
#include <vector>
#include <iostream>
#include <fstream>
#include <thread>
#include <cmath>

#include "gpios.h"
#include "i2c.hh"
#include "result.h"
#include "sht21.h"

using std::uint32_t;

void read_sht21(std::atomic_uchar &run)
{
  run = 1;

  const std::string export_str("/sys/class/gpio/export");
  const std::string unexport_str("/sys/class/gpio/unexport");
  const std::string gpio_str("/sys/class/gpio/gpio");
  for(uint32_t i=0; i<gpio.size(); i++){
    std::ofstream of_exp(export_str);
    of_exp << gpio[i];
  }
  std::this_thread::sleep_for (std::chrono::seconds(4));
  for(uint32_t i=0; i<gpio.size(); i++){
    std::ofstream of_dir(gpio_str+std::to_string(gpio[i])+"/direction");
    of_dir << "out";
    std::this_thread::sleep_for (std::chrono::seconds(1));
    std::ofstream of_val(gpio_str+std::to_string(gpio[i])+"/value");
    if(i == 0)
      of_val << "1";
    else
      of_val << "0";
  }

  const std::string i2cdev("/dev/i2c-1"); //NOTE: old version raspi is i2c-0
  I2C_Open(i2cdev.c_str());
  I2C_Setup(I2C_SLAVE, 0x40);

  if(I2C_GetError()){	
    I2C_PrintError();
    exit(1);
  }

  uint8_t i = 0;
  float hr = 1, tr = 1, td = 1;

  while(run){
    
    std::ofstream of_gpio(gpio_str+std::to_string(gpio[i])+"/value");
    of_gpio << "1";
    of_gpio.close();
    std::this_thread::sleep_for (std::chrono::seconds(1));
   
    std::int16_t t;
    std::uint8_t h;
    SHT21_Read(&t,&h);

    hr=(float)h;
    if(hr<1e-5) hr=1;
    tr=(float)t/10.0;
    td=pow(hr/100.,1.0/8.0)*(112+0.9*tr)+0.1*tr-112;

    //time_t timev;
    //time(&timev);

    //printf("%u\t%.2f\t%u\t%f\t%s", i, ((float)t)/10, h, td, ctime(&timev));
 
    result r;
    if(!SHT21_GetError()){
	    r.temperature = ((float)t)/10;
		r.humidity = h;
		r.dew_point = td;
		r.ok = true;
    }
    else{
      I2C_PrintError();
      SHT21_PrintError();
      I2C_ClearError();
    }
	result_list::instance()->update_result(gpio[i], std::move(r));

    std::ofstream of_gpio_xx(gpio_str+std::to_string(gpio[i])+"/value");
    of_gpio_xx << "0";
    of_gpio_xx.close();
    i++;
    if(i==gpio.size()) i=0;
  }
  I2C_Close();

}
