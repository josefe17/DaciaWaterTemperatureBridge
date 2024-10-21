#include <mcp_can.h>
#include <mcp_can_dfs.h>
#include <spi.h>
#include <Wire.h>
#include <U8g2lib.h>


long unsigned int rxId; // Received ID
unsigned char len = 0; // Received data length
unsigned char rxBuf[8]; //Reception buffer

MCP_CAN CAN1(5); // Chip select on pin 5 ((uses SPI driver below)
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

void displaySetupForTemperature();
void displayUpdateTemperature(int16_t temperature);

int temperature;

void setup() {   
  u8g2.begin();  
  // initialize variables
  for (byte i = 0; i < 8; i++)
  {
    rxBuf[i] = 0;
  }
  temperature = 0;
  // Start CAN bus controller SPI driver
  CAN1.begin(CAN_500KBPS, MCP_8MHz); // init can bus : baudrate = 500k / HIGH SPEED CAN BUS    
  pinMode(2, INPUT_PULLUP); //Interrupt pin
  displaySetupForTemperature();  
}

void loop() 
{  
  // Read CAN bus
  if(!digitalRead(2)) // If pin 2 is low, read receive buffer
  {
    CAN1.readMsgBuf(&len, rxBuf); // CAN bus data read 
    unsigned long rxId = CAN1.getCanId(); // Retrieve data ID
    if(rxId == 0x5DA) // If temperature data
    {      
      // read value and remove offset
      temperature = rxBuf[0] - 40;
      // update display  temperature
      displayUpdateTemperature(temperature);
      // print the results to the Serial Monitor:     
    }
  }
}

void displaySetupForTemperature()
{
  u8g2.clearBuffer();
  u8g2.sendBuffer();
  u8g2.setFont(u8g2_font_logisoso42_tf);
}

void displayUpdateTemperature(int temperature)
{
  String temperatureStr;
  // Avoid overflowing string
  // CAN bus specs limits this value to -40 ºC, but bettere safe than sorry
  if (temperature < -99)
  {
    temperature = -99;
  }
  if (temperature > -10 && temperature < 100)
  {
    temperatureStr = " " + String(temperature) + "\xB0""C";
    if (temperature >= 0 && temperature < 10)
    {
      temperatureStr = ' ' + temperatureStr;
    }
  }
  else
  {
    temperatureStr = String(temperature) + "\xB0""C";
  }  
  u8g2.clearBuffer();
  u8g2.setCursor(0, (42 / 2) + (u8g2.getDisplayHeight() / 2));
  u8g2.print(temperatureStr);
  u8g2.sendBuffer();
}
