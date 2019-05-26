#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <String.h>


#include <SPI.h>
#include <DMD2.h>
#include <fonts/SystemFont5x7.h>
#include <fonts/Droid_Sans_16.h>

#include "MyBigFont.h"

const char* ssid = "mahi"; // Hoofdlettergevoelig
const char* password = "jayasree"; // Hoofdlettergevoelig



// Set Width to the number of displays wide you have
const int WIDTH = 1;

// You can change to a smaller font (two lines) by commenting this line,
// and uncommenting the line after it:
const uint8_t *FONT = Droid_Sans_16;
//const uint8_t *FONT = SystemFont5x7;

 char *MESSAGE = " HELLO WELCOME ";//giving 3 or more letters

SPIDMD dmd(WIDTH,1); // DMD controls the entire display
DMD_TextBox box(dmd, 1, 1, 32, 16); // “box” provides a text box to automatically write to/scroll the display
SPIDMD dmd1(1, 1); // Number of P10 panels used X, Y

// the setup routine runs once when you press reset:
void setup() {
 //////////////////////////Letters display////////////////////////
  Serial.begin(9600);
  Serial.println("power up");

  
Serial.print("Bezig met verbinden");
  WiFi.begin(ssid, password); // Connect to WiFi

  // while wifi not connected yet, print '.'
  // then after it connected, get out of the loop
  while (WiFi.status() != WL_CONNECTED) {
     delay(500);
     Serial.print(".");
  }
  Serial.println("OK!");
  
  // Access Point (SSID).
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
  
  // IP adres.
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
Serial.println("state---------------1"); 
  // Signaalsterkte.
  long rssi = WiFi.RSSI();
  Serial.print("Signaal sterkte (RSSI): ");
  Serial.print(rssi);
  Serial.println(" dBm");
  Serial.println("");

 dmd.setBrightness(255);
 dmd.selectFont(FONT);
 //dmd.selectFont(MyBigFont);
 dmd.begin();
 
/////////////////////////////////////////////////////////////////////
/*
/////////////////////////box border//////////////////////////////////////////////
 dmd1.setBrightness(10); // Set brightness 0–255
 dmd1.selectFont(FONT); // Font used
 dmd1.begin();
 dmd1.drawBox(31, 15, 0, 0); //{x,y,} // drawing box bording led pannel

////////////////////////////////////////////////////////////////////////////
*/
}

// the loop routine runs over and over again forever:
void loop() {
  
String payload = "\0";
String dd = "\0";
char mess[]="" ;
//////////////////////////////////////////////////////////

if(WiFi.status()== WL_CONNECTED){ //Check WiFi connection status
  Serial.println("link pinged");
    HTTPClient http; //Declare an object of class HTTPClient
 http.begin("http://projectsiot.xyz/IoTProjects/LEDBoard/update.php");

int httpCode = http.GET(); //Send the request
    if (httpCode > 0) { //Check the returning code
      payload = http.getString(); //Get the request response payload
      Serial.println(payload); //Print the response payload

}
    http.end(); //Close connection
}

Serial.println(payload[payload.indexOf("sts0")+7]);

 dd = payload.substring((payload.indexOf("sts0")+7),(payload.length()-2));
Serial.print("dd=");
Serial.println(dd);

for (int k=0; k<=dd.length();k++)
{
  mess[k] = dd[k]; 
}

Serial.print("mess=");
Serial.println(mess);
//std::mess.c_str()
//*MESSAGE = mess.c_str();

Serial.print("message=");
Serial.println(MESSAGE);
Serial.println();
Serial.println();
  


/////////////////////////////////////////////////////////












  
 //const char *next = MESSAGE;
 const char *next = mess;
 while(*next) {
 Serial.print(*next);
 box.print(*next);


 delay(500);
 next++;
 }
}
