#include <SoftwareSerial.h>
#include <Wire.h>
#include <LPS331.h>
#include "DHT.h"
#include <LiquidCrystal.h>



LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
#define btnRIGHT 0
#define btnUP 1
#define btnDOWN 2
#define btnLEFT 3
#define btnSELECT 4
#define btnNONE 5

int adc_key_in;
int lcd_key;

int read_LCD_buttons(){
adc_key_in = analogRead(0);
if (adc_key_in > 1000) return btnNONE;
if (adc_key_in < 50) return btnRIGHT;
if (adc_key_in < 250) return btnUP;
if (adc_key_in < 450) return btnDOWN;
if (adc_key_in < 650) return btnLEFT;
if (adc_key_in < 850) return btnSELECT;
return btnNONE;
}



#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
#define DHTPIN 11     // Digital pin connected to the DHT sensor
DHT dht(DHTPIN, DHTTYPE);

LPS331 ps;

#define N 23
        
SoftwareSerial pmsSerial(2, 3);

unsigned char bufor [N];



int get_PM1(unsigned char *thebuf);
int get_PM25(unsigned char *thebuf);
int get_PM10(unsigned char *thebuf);
char checksum(unsigned char *thebuf, char leng);

void menu();

float pressure = 0;
float humidity = 0;
float temp = 0;
int PM1=1, PM25 = 0, PM10 = 0;

int calc_total = 0;




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
    lcd.begin(16,2);
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
    
     pressure = ps.readPressureMillibars();    
     Serial.println(PM1);  //Debugging only
    
    
     menu();

     //delay(5000);
}


int get_PM1(unsigned char *buf)
{
  int PM1v;
  PM1v=((buf[9]<<8) + buf[10]);  
  return PM1v;
}

int get_PM25(unsigned char *buf)
{
  int PM25v;
  PM25v=((buf[11]<<8) + buf[12]);  
  return PM25v;
}

int get_PM10(unsigned char *buf)
{
  int PM10v;
  PM10v=((buf[13]<<8) + buf[14]); 
  return PM10v;
}

bool checksum(unsigned char *buf, int msgLength)
{  
  bool isOk=0;
  int sum=0;

// Obliczanie sumy kontrolnej
// Zsumowana cała wiadomosc (dodac poczatek wiadomosci, ktory nie byl w buforze)

  for(int i=0; i<(msgLength-2); i++){
  sum+=buf[i];
  }
 sum=sum + 0x42;
 
  if(sum == ((buf[msgLength-2]<<8)+buf[msgLength-1]))    // Dwa ostatnie bity dla sumny kontrolnej
  {
    sum = 0;
    isOk = 1;
  }
  return isOk;
}

void menu()
{
lcd_key = read_LCD_buttons();
switch (lcd_key){
case btnRIGHT:{
lcd.clear();
calc_total = calc_total + 1;
delay(100);
break;
}
case btnLEFT:{
lcd.clear();
calc_total = calc_total - 1;
delay(100);
break;
}
}
if (calc_total%3 == 0){
lcd.setCursor(0,0);
lcd.print("PM 2.5=");
lcd.print(PM25);
lcd.print(" ug/m3");
lcd.setCursor(0,1);
lcd.print("PM 10=");
lcd.print(PM10);
lcd.print(" ug/m3");
}
if (calc_total%3 == 1){
lcd.setCursor(0,0);
lcd.print("Temp = ");
lcd.print(temp);
lcd.print(" C");
lcd.setCursor(0,1);
lcd.print("Humid = ");
lcd.print(humidity);
lcd.print(" %");
}
if (calc_total%3 == 2){
lcd.setCursor(0,0);
lcd.print("P = ");
lcd.print(pressure);
lcd.print(" hPa");
lcd.setCursor(0,1);
lcd.print("PM 1.0=");
lcd.print(PM1);
lcd.print(" ug/m3");
}
}
