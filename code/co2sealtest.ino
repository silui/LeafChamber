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
#define UPBUT 14
#define DOWNBUT 15
#define ENTER 16
#define SERVODELAY 80
Servo leftservo;
Servo rightservo;
SoftwareSerial nss(6,7);
COZIR czr(nss);

const int chipSelect = 8;
int l_servoinitpos;
int r_servoinitpos;
int l_servodestpos;
int r_servodestpos;
void setup() {
  Serial.begin(9600);
  configureSensor();
  SD.begin(chipSelect);
  pinMode(UPBUT, INPUT);
  pinMode(DOWNBUT, INPUT);
  pinMode(ENTER, INPUT);
  leftservo.attach(9);
  rightservo.attach(10);
  Serial.println("left servo begin calibration");
  setposition(leftservo, l_servoinitpos);
  Serial.println("right servo begin calibration");
  setposition(rightservo, r_servoinitpos);
  Serial.println("place PETG, left and right servo final destination");
  setdestination();
  czr.CalibrateFreshAir();
  delay(10000);
  Serial.println("NUMBER  LUX  CELCIUS  HUMITY CO2");
  
}

void loop() 
{
  File dataFile = SD.open(filenamegenerator(), FILE_WRITE);
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
void Servo_movetofinal()
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
void Servo_movetoinitial()
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
void smallstep(Servo targetservo, int direction)
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

void setdestination()
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

void setposition(Servo myservo, int& angle)
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
String filenamegenerator()
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
