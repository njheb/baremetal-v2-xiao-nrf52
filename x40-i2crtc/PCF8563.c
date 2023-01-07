/*
Based on "PCF8563 Arduino Library V0.2"
by
Bill2462 Krzysztof Adamkiewicz
commit a8873df0cb853346aff47680160c9805399bc6e4 

hasty conversion of C++ to C by search and replace in the most part

for testing of microbian on SeeedStudio nrf52840/samd21 expansion board
*/

#include "PCF8563.h"
#include "microbian.h"
#include "hardware.h"

//initialize PCF8563
//Parameters: none
//Returns: none
void PCF8563__init()
{
//  Wire.begin();//initialize the I2C interface
//will INIT elsewhere probably
//  i2c_init(I2C_EXTERNAL);

  PCF8563__write_AND(Control_status_1,~(1<<3));//clear TESTC bit
//  PCF8563__write_AND(CLKOUT_control,~(1<<7));//clear CLKOUT enable bit 
  PCF8563__write_AND(CLKOUT_control,0x7f);//clear CLKOUT enable bit 
}

//Start the clock
//Parameters: None
//Returns:    None
void PCF8563__startClock()
{
  PCF8563__write_AND(Control_status_1,~(1<<5));
}

//Start the clock
//Parameters: None
//Returns:    None
void PCF8563__stopClock()
{
  PCF8563__write_OR(Control_status_1,1<<5);
}

//Set the year
//Parameters:
// * UINT8 year -> selected year (you can set values 0-99)
//Returns: None
void PCF8563__setYear(UINT8 year)
{
  const UINT8 data = ((get_second_number(year))<<4)|(get_first_number(year));
  PCF8563__write(Years,data);
}

//Set the month
//Parameters:
// * UINT8 month -> selected month (you can set values 1-12)
//Returns: None
void PCF8563__setMonth(UINT8 month)
{
  const UINT8 data = ((get_second_number(month))<<4)|(get_first_number(month));
  PCF8563__write(Century_months,data);
}

//Set the day
//Parameters:
// * UINT8 day -> selected day (you can set values 1-31)
//Returns: None
void PCF8563__setDay(UINT8 day)
{
  const UINT8 data = ((get_second_number(day))<<4)|(get_first_number(day));
  PCF8563__write(Days,data);
}

//Set the hour
//Parameters:
// *UINT8 hour -> selected day (you can set values 0-24)
//Returns: None
void PCF8563__setHour(UINT8 hour)
{
  const UINT8 data = ((get_second_number(hour))<<4)|(get_first_number(hour));
  PCF8563__write(Hours,data);
}

//Set the minut
//Parameters:
// *UINT8 minut -> selected day (you can set values 0-59)
//Returns:    None
void PCF8563__setMinut(UINT8 minut)
{
  const UINT8 data = ((get_second_number(minut))<<4)|(get_first_number(minut));
  PCF8563__write(Minutes,data);
}

//Set the second
//Parameters:
// * UINT8 second -> selected day (you can set values 0-59)
//Returns: None
void PCF8563__setSecond(UINT8 second)
{
  const UINT8 data = ((get_second_number(second))<<4)|(get_first_number(second));
  PCF8563__write(VL_seconds,data);
}

//Get current time
//Parameters: None
//Returns: Time (current code encoded into the Time structure)
void PCF8563__getTime(struct PCF8563_Time *output)
{

  //read data registers contents
  const UINT8 YEAR    = PCF8563__read(Years);
  const UINT8 MONTH   = PCF8563__read(Century_months);
  const UINT8 DAY     = PCF8563__read(Days);
  const UINT8 WEEKDAY = PCF8563__read(Weekdays);
  const UINT8 HOUR    = PCF8563__read(Hours);
  const UINT8 MINUTE  = PCF8563__read(Minutes);
  const UINT8 SECONDS = PCF8563__read(VL_seconds);

  //convert readed data to numbers using bcd_to_number function).
  output->year    = bcd_to_number((YEAR&0b11110000)>>4,YEAR&0b00001111);
  output->month   = bcd_to_number((MONTH&0b00010000)>>4,MONTH&0b00001111);
  output->day     = bcd_to_number((DAY&0b00110000)>>4,DAY&0b00001111);
  output->weekday = bcd_to_number(0,WEEKDAY&0b00000111);
  output->hour    = bcd_to_number((HOUR&0b00110000)>>4,HOUR&0b00001111);
  output->minute  = bcd_to_number((MINUTE&0b01110000)>>4,MINUTE&0b00001111);
  output->second  = bcd_to_number((SECONDS&0b01110000)>>4,SECONDS&0b00001111);
}

//Check clock integrity
//Parameters: None
//Returns: clock status (bool)
bool PCF8563__checkClockIntegrity()
{
    const UINT8 data = PCF8563__read(VL_seconds);//read the data

    if(data & (1<<7))
    {
      return 0;//if clock integrity is not guaranteed return 0
    }

    else
    {
      return 1;//otherwise return 1
    }
}

//Enable CLK OUTPUT
//Parameters: None
//Returns: None
void PCF8563__enableClkOutput()
{
  PCF8563__write_OR(CLKOUT_control,1<<7);//set FE bit in CLKOUT_control register
}

//Disable CLK OUTPUT
//Parameters: None
//Returns: None
void PCF8563__disableClkOutput()
{
//  PCF8563__write_AND(CLKOUT_control,~(1<<7));//clear FE bit in CLKOUT_control register
  PCF8563__write_AND(CLKOUT_control,0x7f);//clear FE bit in CLKOUT_control register

}

//Set CLK OUTPUT frequency
//Parameters:
// * output_frequency frequency -> Selected frequency
//Returns: None
void PCF8563__setClkOutputFrequency(enum output_frequency frequency)
{
  switch(frequency)
  {
    case CLKOUT_32768_Hz:
      PCF8563__write_AND(CLKOUT_control,~((1<<0)|(1<<1)));
      break;

    case CLKOUT_1024_Hz:
      PCF8563__write_AND(CLKOUT_control,~(1<<1));
      PCF8563__write_OR(CLKOUT_control,1<<0);
      break;

    case CLKOUT_32_Hz:
      PCF8563__write_AND(CLKOUT_control,~(1<<0));
      PCF8563__write_OR(CLKOUT_control,1<<1);
      break;

    case CLKOUT_1_Hz:
      PCF8563__write_OR(CLKOUT_control,(1<<0)|(1<<1));
      break;
  }
}

//Read one byte of data
//Parameters:
// * UINT8 address  - register read_address
//Returns: readed byte of data (UINT8)
UINT8 PCF8563__read(UINT8 cmd) //cmd was called address
{
/*
  Wire.beginTransmission(PCF8563_address);//begin transmission
  Wire.write(address);//inform chip what register we want to read
  Wire.endTransmission();
  Wire.requestFrom(PCF8563_address,1);//request one byte from the chip
  UINT8 data = Wire.read();//read the data
*/
//guess at microbian functional equivalent
//cmd=address where address = register address
return i2c_read_reg(I2C_EXTERNAL, PCF8563_address, cmd);

//  return data;
}


//Convert BCD format to number
//Parameters:
// * UINT8 first -> first digit
// * UINT8 second -> second digit
//Returns: the result of the conversion (unsigned char)
unsigned char bcd_to_number(UINT8 first, UINT8 second)
{
  unsigned char output;
  output = first*10;
  output = output + second;
  return output;
}

//Get first digit of the number
//Parameters:
// * unsigned short ->
//Returns: digit (UINT8)
UINT8 get_first_number(unsigned short number)
{
  UINT8 output = number%10;
  return output;
}

//Get second digit of the number
//Parameters:
// * unsigned short ->
//Returns: digit (UINT8)
UINT8  get_second_number(unsigned short number)
{
  UINT8 output = number/10;
  return output;
}

//Write one byte of data
//Parameters:
// * UINT8 address  - register read_address
// * UINT8 data     - byte of data that we want to write to the register
//Returns: none
void PCF8563__write(UINT8 cmd, UINT8 data)
{
/*
  Wire.beginTransmission(PCF8563_address);
  Wire.write(address);
  Wire.write(data);
  Wire.endTransmission();
*/
  i2c_write_reg(I2C_EXTERNAL, PCF8563_address, cmd, data);
}

//Change state of the register using OR operation
//Parameters:
// * UINT8 address    - register address
// * UINT8 data       - one byte of data that we want to put in the register
//Returns: none
void PCF8563__write_OR(UINT8 address, UINT8 data)
{
  UINT8 c = PCF8563__read(address);
  c = c | data;
  PCF8563__write(address,c);
}

//Change state of the register using AND operation
//Parameters:
// * UINT8 address    - register address
// * UINT8 data       - one byte of data that we want to put in the register
//Returns: none
void PCF8563__write_AND(UINT8 address, UINT8 data)
{
  UINT8 c = PCF8563__read(address);
  c = c & data;
  PCF8563__write(address,c);
}
