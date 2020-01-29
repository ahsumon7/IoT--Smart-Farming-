#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include <Adafruit_ADS1015.h>
#define ONE_WIRE_BUS D4
#define DHTPIN D8

#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,16,2); 


  //DATABASE
#define FIREBASE_HOST "smart-farming-cf40c.firebaseio.com"  //maltoshibly@gmail.com dummpy
#define FIREBASE_AUTH "jEPDhlUUPQTELyouJeRfFx1e1uAtId6BRj9UySJP"
#define WIFI_SSID "Rubel"
#define WIFI_PASSWORD "12345678"
 //DATABASE

int b,c;
int16_t adc0,adc1;
#define SensorPin A0            //pH meter Analog output to Arduino Analog Input 0
#define Offset 0.00            //deviation compensate

#define samplingInterval 20
#define printInterval 800
#define ArrayLenth  40    //times of collection
int pHArray[ArrayLenth];   //Store the average value of the sensor feedback
int pHArrayIndex=0;

//

  

#define DHTTYPE    DHT22   
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);


float temp,hum;

DHT_Unified dht(DHTPIN, DHTTYPE);

uint32_t delayMS;
Adafruit_ADS1115 ads(0x48);

void setup() {
  
  lcd.init();  
   lcd.backlight();
    lcd.setCursor(0,0);
   lcd.println("SMART FARMING       ");
       lcd.setCursor(0,1);
   lcd.print("connecting");
   
  Serial.begin(9600);
WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
Serial.print("connecting");
while (WiFi.status() != WL_CONNECTED) {
Serial.print(".");
     lcd.setCursor(0,2);
   lcd.print(".");
delay(500);
}
lcd.clear();

Serial.println();
Serial.print("connected: ");
  lcd.setCursor(0,0);
   lcd.print("connected: ");
  Serial.println(WiFi.localIP());
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
   lcd.setCursor(0,1);
   lcd.print((WiFi.localIP()));



  
  dht.begin();
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  dht.humidity().getSensor(&sensor);
  delayMS = sensor.min_delay / 1000;
  sensors.begin();
  ads.begin();
  lcd.clear();
}

void loop() {


  delay(delayMS);
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  temp=(event.temperature);
  dht.humidity().getEvent(&event);
  hum=(event.relative_humidity);

       
  if (isnan(event.temperature)||(isnan(event.relative_humidity))) {
    Serial.println(F("Error reading temperature!"));
  }
  else {
    Serial.print(F("Temp: "));
     Serial.print(temp); 
     Serial.print(F("°C")); 
       lcd.setCursor(0,0);
   lcd.print("T:");
    lcd.print(temp,2);
      lcd.print("    ");
    
 
  Serial.print("    ");
    Serial.print(F("Hu: ")); 
  Serial.print(hum);
    Serial.print("|       ");
    lcd.print("H:");
    lcd.println(hum,2);
    lcd.print("    ");
     lcd.setCursor(18,0);
    lcd.print("  ");
      lcd.setCursor(20,0);
    lcd.print(" ");
     
   //
   Firebase.set("Humidity",hum);
   Firebase.set("Temp",temp);
  }





  sensors.requestTemperatures(); 
  Serial.print("S_Temp: ");
  
  Serial.print(sensors.getTempCByIndex(0)); 
 Serial.print("°C");
 Serial.print("    ");
 
    lcd.setCursor(0,1);
   lcd.print("T:");
    lcd.print((sensors.getTempCByIndex(0)),2);
     
    
    

Firebase.set("S_Temp",sensors.getTempCByIndex(0));



 //

 static unsigned long samplingTime = millis();
  static unsigned long printTime = millis();
  static float pHValue,voltage;
  if(millis()-samplingTime > samplingInterval)
  {
    adc0 = ads.readADC_SingleEnded(0);
    adc1 = ads.readADC_SingleEnded(1);
       b=map(adc0,0,26500,0,1024);
       c=map(adc0,0,26500,0,100);

      pHArray[pHArrayIndex++]=b;
       
  
      if(pHArrayIndex==ArrayLenth)pHArrayIndex=0;
      voltage = avergearray(pHArray, ArrayLenth)*5.0/1024;
      pHValue = 3.5*voltage+Offset;
      samplingTime=millis();
  }
  if(millis() - printTime > printInterval)   //Every 800 milliseconds, print a numerical, convert the state of the LED indicator
  {

    
     
      
        lcd.setCursor(8,1);
   lcd.print("PH:");
   
    lcd.print(pHValue,2);
    lcd.print(" "); 
    lcd.setCursor(16,1);
   lcd.print("M:");
   lcd.println(c);
    lcd.setCursor(20,1);
    lcd.print(" ");
 

     Serial.print("pH: ");
    Serial.print(pHValue,2);
    Serial.print("    ");
     Firebase.set("S_Moisture",c);

        Serial.print("Moist: ");
    Serial.println(c);
      Firebase.set("S_PH",pHValue);
    Serial.print("    ");
     
        printTime=millis();
  }

//decision site

int datatemp1=  Firebase.getInt("temp1");
int datahum1=  Firebase.getInt("hum1");
int datasph1=  Firebase.getInt("ph1");
//int datastemp1=Firebase.getInt(stemp1);
//int datasmois1=Firebase.getInt(smois1);
Serial.print(datatemp1);
Serial.println(datahum1);
Serial.println(datasph1);


if(datatemp1>25 && datahum1>72 && datasph1<8)
{
  Serial.println("Results:");
  lcd.setCursor(0,2);
  lcd.println("Crops: Rice");
}

else
{
  Serial.println("Results:");
  lcd.setCursor(0,2);
  lcd.println("Crops: Not Match");
  
}


  
}//loop end



double avergearray(int* arr, int number){
  int i;
  int max,min;
  double avg;
  long amount=0;
  if(number<=0){
    Serial.println("Error number for the array to avraging!/n");
    return 0;
  }
  if(number<5){   //less than 5, calculated directly statistics
    for(i=0;i<number;i++){
      amount+=arr[i];
    }
    avg = amount/number;
    return avg;
  }else{
    if(arr[0]<arr[1]){
      min = arr[0];max=arr[1];
    }
    else{
      min=arr[1];max=arr[0];
    }
    for(i=2;i<number;i++){
      if(arr[i]<min){
        amount+=min;        //arr<min
        min=arr[i];
      }else {
        if(arr[i]>max){
          amount+=max;    //arr>max
          max=arr[i];
        }else{
          amount+=arr[i]; //min<=arr<=max
        }
      }//if
    }//for
    avg = (double)amount/(number-2);
  }//if
  return avg;

}
