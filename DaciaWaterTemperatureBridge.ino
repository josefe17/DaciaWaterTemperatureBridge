#include <mcp_can.h>
#include <mcp_can_dfs.h>
#include <spi.h>

long unsigned int rxId; // Received ID
unsigned char len = 0; // Received data length
unsigned char rxBuf[8]; //Reception buffer
MCP_CAN CAN1(5); // Chip select on pin 5 ((uses SPI driver below)


int mapPWM(int temperature);

const int analogOutPin = 3; // Analog output pin that the gauge is attached

int temperature = 0;
int outputValue = 0;        // value output to the PWM (analog out)

void setup() {
  // initialize serial communications at 115200 bps:
  Serial.begin(115200);

 //PWM as fast as possible
  TCCR2B = TCCR2B & B11111000 | B00000001;

  // Start CAN bus controller SPI driver
  CAN1.begin(CAN_500KBPS, MCP_8MHz); // init can bus : baudrate = 500k / 
  
  pinMode(2, INPUT); //Interrupt pin
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
      // map it into the range of the analog out:  
      outputValue = mapPWM(temperature);      
      // change the analog out value:
      analogWrite(analogOutPin, outputValue);      
      // print the results to the Serial Monitor:
      Serial.print("Received temperature = ");
      Serial.print(temperature, DEC);
      Serial.print(" ºC");  
      Serial.print(" Output = ");
      Serial.print(outputValue);    º
    }
    Serial.println();
  }
}

int mapPWM(int temperature)
{
  if (temperature <= 40)return 86;
  if (temperature <=45) return map(temperature, 45, 40, 82, 86);
  if (temperature <=50) return map(temperature, 50, 45, 80, 82);
  if (temperature <=55) return map(temperature, 55, 50, 76, 80);
  if (temperature <=60) return map(temperature, 60, 55, 74, 76);
  if (temperature <=65) return map(temperature, 65, 60, 71, 74);
  if (temperature <=70) return map(temperature, 70, 65, 67, 71);
  if (temperature <=75) return map(temperature, 75, 70, 64, 67);
  if (temperature <=80) return map(temperature, 80, 75, 60, 64);
  if (temperature <=85) return map(temperature, 85, 80, 54, 60);
  if (temperature <=90) return map(temperature, 90, 85, 50, 54);
  if (temperature <=95) return map(temperature, 95, 90, 47, 50);
  if (temperature <=100) return map(temperature, 100, 95, 40, 47);
  if (temperature <=105) return map(temperature, 105, 100, 36, 40);
  if (temperature <=110) return map(temperature, 110, 105, 30, 36);
  if (temperature <=115) return map(temperature, 115, 110, 25, 30);
  if (temperature <=120) return map(temperature, 120, 115, 19, 25);
  if (temperature <=130) return map(temperature, 130, 120, 16, 19);
  if (temperature <=135) return map(temperature, 135, 130, 11, 16);
  if (temperature <=145) return map(temperature, 145, 135, 5, 11);
  if (temperature > 145) return 0;
  return 0;
}
