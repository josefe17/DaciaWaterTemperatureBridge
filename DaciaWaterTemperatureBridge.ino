#include <mcp_can.h>
#include <mcp_can_dfs.h>
#include <spi.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

long unsigned int rxId; // Received ID
unsigned char len = 0; // Received data length
unsigned char rxBuf[8]; //Reception buffer

MCP_CAN CAN1(5); // Chip select on pin 5 ((uses SPI driver below)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void displaySetupForTemperature();
void displayUpdateTemperature(int16_t temperature);

int temperature;

void setup() {
  // initialize serial communications at 115200 bps:
  Serial.begin(115200);
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
  {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
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
  Serial.println("Started..."); 
}

void loop() 
{
  // Read CAN bus
  if(!digitalRead(2)) // If pin 2 is low, read receive buffer
  {
    Serial.print("Interrupt! ");
    CAN1.readMsgBuf(&len, rxBuf); // CAN bus data read 
    unsigned long rxId = CAN1.getCanId(); // Retrieve data ID
    if(rxId == 0x5DA) // If temperature data
    {
      Serial.print("Coolant temperature message! ");
      // read value and remove offset
      temperature = rxBuf[0] - 40;
      // update display  temperature
       //displayUpdateTemperature(temperature);
      // print the results to the Serial Monitor:
      Serial.print("Received temperature = ");
      Serial.print(temperature, DEC);
      Serial.print(" ºC");
    }
    Serial.println();
  }
}

void displaySetupForTemperature()
{
  display.clearDisplay();
  display.setTextSize(4);
  display.setTextColor(SSD1306_WHITE);
  display.clearDisplay();
}

void displayUpdateTemperature(int temperature)
{
  uint16_t width;
  uint16_t height;
  String temperatureStr;
  // Avoid overflowing string
  // CAN bus specs limits this value to -40 ºC, but bettere safe than sorry
  if (temperature < -99)
  {
    temperature = -99;
  }
  if (temperature > -10 && temperature < 100)
  {
    temperatureStr = ' ' + String(temperature) + (char) 247 + 'C';
    if (temperature >= 0 && temperature < 10)
    {
      temperatureStr = ' ' + temperatureStr;
    }
  }
  else
  {
    temperatureStr = String(temperature) + (char) 247 + 'C';
  }
  display.clearDisplay();
  display.getTextBounds(temperatureStr, 0, 0, nullptr, nullptr, &width, &height);
  display.setCursor((SCREEN_WIDTH - width) / 2, (SCREEN_HEIGHT - height) / 2);
  display.println(temperatureStr);  
  display.display();
}
