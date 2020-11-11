#include <SoftwareSerial.h>
#include <LowPower.h>
#include <Wire.h>
#include <LPS.h>
#include "DHT.h"


#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
#define DHTPIN 11     // Digital pin connected to the DHT sensor
DHT dht(DHTPIN, DHTTYPE);

LPS ps;

#define N 23
        
SoftwareSerial pmsSerial(2, 3);
unsigned char bufor [N];
int get_PM1(unsigned char *thebuf);
int get_PM25(unsigned char *thebuf);
int get_PM10(unsigned char *thebuf);
char checksum(unsigned char *thebuf, char leng);

String response = "";
String Latitude = "";
String Longitude = "";

float pressure = 0;
float humidity = 0;
float temp = 0;
int PM1=0, PM25 = 0, PM10 = 0;





void setup()
{
   Serial.begin(9600);
   pmsSerial.begin(9600);
   dht.begin(); 

    Wire.begin();

    if (!ps.init())
    {
      Serial.println("Failed to autodetect pressure sensor!");  //Debugging only
    }

    ps.enableDefault();

    Serial.println("Initializing...");
    delay(1000);
  
    Serial.println("Setup finished"); //Debugging only
}

void(* resetFunc) (void) = 0;

void loop()
{
     Serial.println("loop start"); //Debugging only
     pmsSerial.listen();
     if(pmsSerial.find(0x42))    
     pmsSerial.readBytes(bufor,N);
    
     if(bufor[0] == 0x4d){
       if(checksum(bufor,N)){ 
         PM1=get_PM1(bufor); 
         PM25=get_PM25(bufor);
         PM10=get_PM10(bufor);  
       }
     }
       
     humidity = dht.readHumidity();
     temp = dht.readTemperature();
     if (isnan(humidity) || isnan(temp)) {
       Serial.println(F("Failed to read from DHT sensor!"));
       return;
     }
    
    
}

