/*
 SD card datalogger
 Design Details:Field Data Logger For remote logging of temperature and humidity 
 created  24 Dec 2016
 by Aravind
*/
#include <SPI.h>
#include <SD.h>
#include <dht.h>
#include "Wire.h"    
#include <avr/sleep.h> 
#include <avr/interrupt.h>
#include <avr/wdt.h>
#define DS3231_I2C_ADDRESS 0x68
                                                
byte decToBcd(byte val)
{
    return( (val/10*16) + (val%10) );
}
                                                
byte bcdToDec(byte val)
{
    return( (val/16*10) + (val%16) );
}
#define dht_dpin A0                            
dht DHT;
const int analogPin = 0;
const int chipSelect = 4;
byte INTERRUPT_PIN = 2;
int sleepStatus = 0;            
void wakeUpNow()        
{

  
}
void setup()
{
  
 
   pinMode(INTERRUPT_PIN, INPUT); 
  Wire.begin();
  Serial.begin(9600);
   pinMode(13, OUTPUT);
   if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    return;
  }

} 
void readDS3231time(byte *second,byte *minute,byte *hour,byte *dayOfWeek,byte *dayOfMonth,byte *month,byte *year)
{
    Wire.beginTransmission(DS3231_I2C_ADDRESS);
    Wire.write(0);
    Wire.endTransmission();
    Wire.requestFrom(DS3231_I2C_ADDRESS, 7);
    *second = bcdToDec(Wire.read() & 0x7f);
    *minute = bcdToDec(Wire.read());
    *hour = bcdToDec(Wire.read() & 0x3f);
    *dayOfWeek = bcdToDec(Wire.read());
    *dayOfMonth = bcdToDec(Wire.read());
    *month = bcdToDec(Wire.read());
    *year = bcdToDec(Wire.read());
}
void loop()
{
   
    digitalWrite(13, HIGH);  
    delay(1000);              
    digitalWrite(13, LOW);    
    delay(1000);   
    int flag=0;
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    sleep_enable();    
    byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
    readDS3231time(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month,&year);
    String dataString = "";
    DHT.read11(dht_dpin);
    dataString +=String(hour);
    dataString +=":";
    dataString +=String(minute);
    dataString +=":";
    dataString +=String(second);
    dataString +="  ";
    dataString +=String(dayOfMonth);
    dataString +="/";
    dataString +=String(month);
    dataString +="/";
    dataString +=String(year);
    dataString +="  ";
    dataString += "Current humidity = ";
    dataString +=String(DHT.humidity);
    dataString +="%  ";
    dataString +="temperature = ";
    dataString +=String(DHT.temperature);
    dataString +="C  ";
   File dataFile = SD.open("datalog.txt", FILE_WRITE);
  if(dataFile) 
  {
    dataFile.println(dataString);
    dataFile.close()
    Serial.println(dataString);
  }
  else {
    Serial.println("error opening datalog.txt");
  }
  sleep_mode();            
  sleep_disable();         
  detachInterrupt(0);   

}
