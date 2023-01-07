#ifndef PCF8563_H
#define PCF8563_H


#define UINT8 unsigned char
#define bool  unsigned char


//now time structure
struct PCF8563_Time
{
  UINT8 year;
  UINT8 month;
  UINT8 day;
  UINT8 weekday;
  UINT8 hour;
  UINT8 minute;
  UINT8 second;
};

//output frequency
enum output_frequency
{
  CLKOUT_32768_Hz,
  CLKOUT_1024_Hz,
  CLKOUT_32_Hz,
  CLKOUT_1_Hz,
};

  //general control
  void PCF8563__init();//initialize the chip
  void PCF8563__stopClock();//stop the clock
  void PCF8563__startClock();//start the clock

  //time settings functions
  void PCF8563__setYear(UINT8 year);//set year
  void PCF8563__setMonth(UINT8 month);//set month
  void PCF8563__setDay(UINT8 day);//set day
  void PCF8563__setHour(UINT8 hour);//set hour
  void PCF8563__setMinut(UINT8 minut);//set minut
  void PCF8563__setSecond(UINT8 second);//set second

  //clkout output
  void PCF8563__enableClkOutput();//enable CLK output
  void PCF8563__disableClkOutput();//disable CLK output
  void PCF8563__setClkOutputFrequency(enum output_frequency frequency);//set CLK output frequency

  //time reading functions
  void PCF8563__getTime(struct PCF8563_Time *output);//get time
  bool PCF8563__checkClockIntegrity();//check clock integrity

  UINT8 PCF8563__read(UINT8 address);//read one byte from selected register
  void PCF8563__write(UINT8 address, UINT8 data);//write one byte of data to the register
  void PCF8563__write_OR(UINT8 address, UINT8 data);//write data to the register using OR operations
  void PCF8563__write_AND(UINT8 address, UINT8 data);//write data to the register using AND operation
  unsigned char bcd_to_number(UINT8 first, UINT8 second);//convert two digits to one number
  UINT8 get_first_number(unsigned short number);//get ten’s place digit of the number
  UINT8 get_second_number(unsigned short number);//get unit place digit of the number

  //registers map
  enum registers
  {
    PCF8563_address  = 0x51,
    Control_status_1 = 0x00,
    Control_status_2 = 0x01,
    VL_seconds       = 0x02,
    Minutes          = 0x03,
    Hours            = 0x04,
    Days             = 0x05,
    Weekdays         = 0x06,
    Century_months   = 0x07,
    Years            = 0x08,
    Minute_alarm     = 0x09,
    Hour_alarm       = 0x0A,
    Day_alarm        = 0x0B,
    Weekday_alarm    = 0x0C,
    CLKOUT_control   = 0x0D,
    Timer_control    = 0x0E,
    Timer            = 0x0F,
  };
#endif
