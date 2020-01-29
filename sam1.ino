#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>

#define FIREBASE_HOST "smart-farming-cf40c.firebaseio.com" 
#define FIREBASE_AUTH "jEPDhlUUPQTELyouJeRfFx1e1uAtId6BRj9UySJP"
#define WIFI_SSID "sam"
#define WIFI_PASSWORD "s12345678"


#include <Keypad.h>
String V_pH="";
const byte ROWS = 4; //four rows
const byte COLS = 3; //three columns0O
char keys[ROWS][COLS] = {
  {'9','8','7'},
  {'6','5','4'},
  {'3','2','1'},
  {'.','0','#'}
};


byte rowPins[ROWS] = {4, 0, 2, 14}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {12,13,15}; //connect to the column pinouts of the keypad

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>



#include <BlynkSimpleEsp8266.h>
char auth[] = "eARW4o_wT_tYh3rtIxs1v3QGVshkk-nL";
char ssid[] = "sam";
char pass[] = "s12345678";
WidgetLCD lcd(V6);

void setup()
{
Serial.begin(9600);  
WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
Serial.print("connecting");
while (WiFi.status() != WL_CONNECTED) {
Serial.print(".");
delay(500);
}
Serial.println();
Serial.print("connected: ");
Serial.println(WiFi.localIP());
Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

  Blynk.begin(auth, ssid, pass);
}

void loop()
{

  
   float  data2=Firebase.getFloat("S_PH");
   float data3=Firebase.getFloat("S_Moisture");
   float data4=Firebase.getFloat("Temp");
   float data5=Firebase.getFloat("Humidity");
   float data6=Firebase.getFloat("S_Temp");
     String results=Firebase.getString("results");
   Serial.println("results:");
      Serial.println(results);
      Serial.println("resultsend:");
      
   lcd.clear();
    lcd.print(0, 0,"OUTPUT: ");
   lcd.print(0, 1,results);
   
  Blynk.virtualWrite(V1, data2);
  Blynk.virtualWrite(V2, data3);
  Blynk.virtualWrite(V3, data4);
    Blynk.virtualWrite(V4, data5);
  Blynk.virtualWrite(V5, data6);

  
char key = keypad.getKey();
    // just print the pressed key
   if (key){
    if(key=='.')
    {
 
    }
    else
    {
       V_pH+=key;
    Serial.println(key);
    }
  } 
   // this checkes if 4 is pressed, then do something. Here  we print the text but you can control something.
  if (key =='.'){
   // Blynk.virtualWrite(V3, V_pH);
    Firebase.setString("V_pH",V_pH);
     Blynk.virtualWrite(V0, V_pH);
    Serial.println("Submitted");
    V_pH="";
  } 


  








  
  Blynk.run();
}
