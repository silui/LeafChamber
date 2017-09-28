//co2sealtest.ino
/*arduino code that lets you save data on sd card with custom servo positioning, code record light intensity, co2, temperature and humidity. These are just quick and dirty code to make the 
use putton 15(a0) and 16(a1) to move servo up and down
1. move the first servo to desired open position, press 16 or a0 putton to confirm
2. move the second servo to desired open position, make sure 2 servo on the same level. Then press confirm
3. move both servo to closed position and it will stay shut untill button is confirm button is pressed again.
*/

#include <Servo.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_TSL2561_U.h>
#include <SPI.h>
#include <SD.h>
#include <SoftwareSerial.h>
#include "cozir.h"

Adafruit_TSL2561_Unified tsl = Adafruit_TSL2561_Unified(TSL2561_ADDR_FLOAT, 12345);

#define UP 1
#define DOWN 0
#define UPBUT 14  //a0
#define DOWNBUT 15  //a1
#define ENTER 16  //a2
#define SERVODELAY 80   //delay for servo steping
Servo leftservo;
Servo rightservo;
SoftwareSerial nss(6,7);    //pin 6 and 7 are TX and RX for cozir
COZIR czr(nss);

const int chipSelect = 8;     //chip select pin for SD card
int l_servoinitpos;         //left servo open angle
int r_servoinitpos;         //right servo open angle
int l_servodestpos;         //left servo close angle
int r_servodestpos;         //right servo close angle
void setup() {
  Serial.begin(9600);
  configureSensor();            //COZIR libraryfunction
  SD.begin(chipSelect);         
  pinMode(UPBUT, INPUT);      
  pinMode(DOWNBUT, INPUT);
  pinMode(ENTER, INPUT);
  leftservo.attach(9);          //by attaching, it moves to 90 degree
  rightservo.attach(10);
  Serial.println("left servo begin calibration");
  setposition(leftservo, l_servoinitpos);
  Serial.println("right servo begin calibration");
  setposition(rightservo, r_servoinitpos);
  Serial.println("place PETG, left and right servo final destination");
  setdestination();
  //czr.CalibrateFreshAir();      //this sets what ever it reads to 450ppm, read cozir manual for more detail
  delay(10000);
  Serial.println("NUMBER  LUX  CELCIUS  HUMITY CO2");
  
}

void loop() 
{
  File dataFile = SD.open(filenamegenerator(), FILE_WRITE);       //filename generator make sure you don't overwrite what was previously saved on sd card
  dataFile.println("NUMBER  LUX  CELCIUS  HUMITY CO2");
  int num=1;
  while(digitalRead(ENTER)==LOW || num==)
  {
    String outputstring=String(num);
    outputstring+=" ";
    outputstring+=getlux();
    outputstring+=" ";
    outputstring+=czr.Celsius();
    outputstring+=" ";
    outputstring+=czr.Humidity();
    outputstring+=" ";
    outputstring+=czr.CO2();
    Serial.println(outputstring);
    dataFile.println(outputstring);
    num++;
  }
  Serial.println("done");
  dataFile.println("done");
  dataFile.close();
  
  Servo_movetoinitial();
  delay(100000000);
}


void Servo_movetofinal()            //function to move servo to close function, with both servo simultaneously
{
  while(leftservo.read()!=l_servodestpos)
  {
    leftservo.write(leftservo.read()+1);
    if(rightservo.read()!=r_servodestpos)
    {
      rightservo.write(leftservo.read()+1);
    }
    delay(SERVODELAY);
  }
  while(rightservo.read()!=r_servodestpos)
  {
    rightservo.write(rightservo.read()+1);
    delay(SERVODELAY);
  }
}
void Servo_movetoinitial()      //same thing with the Servo_movetofinal() function except it moves to open position
{
  while(leftservo.read()!=l_servoinitpos)
  {
    leftservo.write(leftservo.read()-1);
    if(rightservo.read()!=r_servoinitpos)
    {
      rightservo.write(leftservo.read()-1);
    }
    delay(SERVODELAY);
  }
  while(rightservo.read()!=r_servoinitpos)
  {
    rightservo.write(rightservo.read()-1);
    delay(SERVODELAY);
  }
}
void smallstep(Servo targetservo, int direction)    //function takes servo and direction as argumetn and move one step
{
  int currentposition= targetservo.read();
  if(direction==1)
  {
    targetservo.write(currentposition+1);
  }
  else
  {
    targetservo.write(currentposition-1);
  }
  delay(100);
}

void setdestination()     //function to let you move both servo and set destination
{
  while(digitalRead(ENTER)==LOW)
  {
    if(digitalRead(UPBUT)==HIGH)
    {
      Serial.println("turning up");
      smallstep(leftservo,UP);
      smallstep(rightservo,UP);
       Serial.print(leftservo.read());Serial.println(rightservo.read());
    }
    else if(digitalRead(DOWNBUT)==HIGH)
    { 
      Serial.println("turning up");
      smallstep(leftservo, DOWN);
      smallstep(rightservo, DOWN);
       Serial.print(leftservo.read());Serial.println(rightservo.read());
    }  
  }
  l_servodestpos=leftservo.read();
  r_servodestpos=rightservo.read();
  Serial.print(l_servodestpos);Serial.println(r_servodestpos);
}

void setposition(Servo myservo, int& angle)       //function that set argument integer to desired angle
{
  while(digitalRead(ENTER)==LOW)
  {
    if(digitalRead(UPBUT)==HIGH)
    {
      Serial.println("turning up");
      smallstep(myservo, UP);
    }
    else if(digitalRead(DOWNBUT)==HIGH)
    { 
      Serial.println("turning up");
      smallstep(myservo, DOWN);
    }
  }
  angle=myservo.read();
  Serial.println(angle);
  delay(1000);
}
//----------------------lux function---------
void configureSensor()
{
    tsl.enableAutoRange(true);            /* Auto-gain ... switches automatically between 1x and 16x */
    tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_101MS);  /* medium resolution and speed   */

}
String getlux()
{
     sensors_event_t event;
  tsl.getEvent(&event);
  return  String(event.light);
}

//---------------------SD card function
String filenamegenerator()              //function that checks file in SD card to make sure not to overwrite previous saved data
{
  for(int i=1; i<199; i++)
  {
    String temp=String(i);
    temp+=".txt";
    if(SD.exists(temp)==false)
    {
      return temp;
    }
  }
}
