    //////////////////////////////////////////////
   //    Art Deco Weather Forecast Display     //
  //           v1.03 27 October 2020          //     
 //                                          //
//           http://www.educ8s.tv           //
/////////////////////////////////////////////


#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <Adafruit_ST7735.h>
#include <Adafruit_GFX.h>

// Color definitions
#define BLACK    0x0000
#define BLUE     0x001F
#define RED      0xF800
#define GREEN    0x07E0
#define CYAN     0x07FF
#define MAGENTA  0xF81F
#define YELLOW   0xFFE0 
#define WHITE    0xFFFF
#define GREY     0xC618

#define cs   D2  
#define dc   D3   
#define rst  D4 

const char* ssid     = "yourSSID";      // SSID of local network
const char* password = "yourPassword";  // Password on network
String APIKEY = "yourAPIkey";
String CityID = "253394"; //Sparta, Greece
int TimeZone = 2; //GMT +2

WiFiClient client;
char servername[]="api.openweathermap.org";  // remote server we will connect to
String result;

boolean night = false;
int  counter = 360;
String weatherDescription ="";
String weatherLocation = "";
float Temperature;

extern  unsigned char  cloud[];
extern  unsigned char  thunder[];
extern  unsigned char  wind[];

Adafruit_ST7735 tft = Adafruit_ST7735(cs, dc, rst);

void setup() {
  Serial.begin(115200);
  tft.initR(INITR_BLACKTAB);   // initialize a ST7735S chip, black tab
  tft.fillScreen(BLACK);

  Serial.println("Connecting");
  WiFi.begin(ssid, password);

  tft.setCursor(30,80);
  tft.setTextColor(WHITE);
  tft.setTextSize(1);
  tft.print("Connecting...");
  
  while (WiFi.status() != WL_CONNECTED) {
  delay(500);
  }
  Serial.println("Connected");

}

void loop() {

    if(counter == 360) //Get new data every 30 minutes
    {
      counter = 0;
      getWeatherData();
      
    }else
    {
      counter++;
      delay(5000);
      Serial.println(counter); 
    }
}

void getWeatherData() //client function to send/receive GET request data.
{
  Serial.println("Getting Weather Data");
  if (client.connect(servername, 80)) {  //starts client connection, checks for connection
    client.println("GET /data/2.5/forecast?id="+CityID+"&units=metric&cnt=1&APPID="+APIKEY);
    client.println("Host: api.openweathermap.org");
    client.println("User-Agent: ArduinoWiFi/1.1");
    client.println("Connection: close");
    client.println();
  } 
  else {
    Serial.println("connection failed"); //error message if no client connect
    Serial.println();
  }

  while(client.connected() && !client.available()) delay(1); //waits for data
 
    Serial.println("Waiting for data");

  while (client.connected() || client.available()) { //connected or data available
    char c = client.read(); //gets byte from ethernet buffer
      result = result+c;
    }

  client.stop(); //stop client
  result.replace('[', ' ');
  result.replace(']', ' ');
  Serial.println(result);

char jsonArray [result.length()+1];
result.toCharArray(jsonArray,sizeof(jsonArray));
jsonArray[result.length() + 1] = '\0';

DynamicJsonDocument doc(1024);
deserializeJson(doc, jsonArray);

String location = doc["city"]["name"];
String temperature = doc["list"]["main"]["temp"];
String weather = doc["list"]["weather"]["main"];
String description = doc["list"]["weather"]["description"];
String idString = doc["list"]["weather"]["id"];
String timeS = doc["list"]["dt_txt"];

timeS = convertGMTTimeToLocal(timeS);

int length = temperature.length();
if(length==5)
{
  temperature.remove(length-1);
}

Serial.println(location);
Serial.println(weather);
Serial.println(temperature);
Serial.println(description);
Serial.println(temperature);
Serial.println(timeS);

clearScreen();

int weatherID = idString.toInt();
printData(timeS,temperature, timeS, weatherID);

}

void printData(String timeString, String temperature, String time, int weatherID)
{
  tft.setCursor(35,20);
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.print(timeString);

  printWeatherIcon(weatherID);

  tft.setCursor(27,132);
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.print(temperature);

  tft.setCursor(83,130);
  tft.setTextColor(WHITE);
  tft.setTextSize(1);
  tft.print("o");
  tft.setCursor(93,132);
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.print("C");
}

void printWeatherIcon(int id)
{
 switch(id)
 {
  case 800: drawClearWeather(); break;
  case 801: drawFewClouds(); break;
  case 802: drawFewClouds(); break;
  case 803: drawCloud(); break;
  case 804: drawCloud(); break;
  
  case 200: drawThunderstorm(); break;
  case 201: drawThunderstorm(); break;
  case 202: drawThunderstorm(); break;
  case 210: drawThunderstorm(); break;
  case 211: drawThunderstorm(); break;
  case 212: drawThunderstorm(); break;
  case 221: drawThunderstorm(); break;
  case 230: drawThunderstorm(); break;
  case 231: drawThunderstorm(); break;
  case 232: drawThunderstorm(); break;

  case 300: drawLightRain(); break;
  case 301: drawLightRain(); break;
  case 302: drawLightRain(); break;
  case 310: drawLightRain(); break;
  case 311: drawLightRain(); break;
  case 312: drawLightRain(); break;
  case 313: drawLightRain(); break;
  case 314: drawLightRain(); break;
  case 321: drawLightRain(); break;

  case 500: drawLightRainWithSunOrMoon(); break;
  case 501: drawLightRainWithSunOrMoon(); break;
  case 502: drawLightRainWithSunOrMoon(); break;
  case 503: drawLightRainWithSunOrMoon(); break;
  case 504: drawLightRainWithSunOrMoon(); break;
  case 511: drawLightRain(); break;
  case 520: drawModerateRain(); break;
  case 521: drawModerateRain(); break;
  case 522: drawHeavyRain(); break;
  case 531: drawHeavyRain(); break;

  case 600: drawLightSnowfall(); break;
  case 601: drawModerateSnowfall(); break;
  case 602: drawHeavySnowfall(); break;
  case 611: drawLightSnowfall(); break;
  case 612: drawLightSnowfall(); break;
  case 615: drawLightSnowfall(); break;
  case 616: drawLightSnowfall(); break;
  case 620: drawLightSnowfall(); break;
  case 621: drawModerateSnowfall(); break;
  case 622: drawHeavySnowfall(); break;

  case 701: drawFog(); break;
  case 711: drawFog(); break;
  case 721: drawFog(); break;
  case 731: drawFog(); break;
  case 741: drawFog(); break;
  case 751: drawFog(); break;
  case 761: drawFog(); break;
  case 762: drawFog(); break;
  case 771: drawFog(); break;
  case 781: drawFog(); break;

  default:break; 
 }
}

String convertGMTTimeToLocal(String timeS)
{
 int length = timeS.length();
 timeS = timeS.substring(length-8,length-6);
 int time = timeS.toInt();
 time = time+TimeZone;

 if(time > 21 ||  time<7)
 {
  night=true;
 }else
 {
  night = false;
 }
 timeS = String(time)+":00";
 return timeS;
}


void clearScreen()
{
    tft.fillScreen(BLACK);
}

void drawClearWeather()
{
  if(night)
  {
    drawTheMoon();
  }else
  {
    drawTheSun();
  }
}

void drawFewClouds()
{
  if(night)
  {
    drawCloudAndTheMoon();
  }else
  {
    drawCloudWithSun();
  }
}

void drawTheSun()
{
    tft.fillCircle(64,80,26,YELLOW);
}

void drawTheFullMoon()
{
    tft.fillCircle(64,80,26,GREY);
}

void drawTheMoon()
{
    tft.fillCircle(64,80,26,GREY);
    tft.fillCircle(75,73,26,BLACK);
}

void drawCloud()
{
     tft.drawBitmap(0,35,cloud,128,90,GREY);
}

void drawCloudWithSun()
{
     tft.fillCircle(73,70,20,YELLOW);
     tft.drawBitmap(0,36,cloud,128,90,BLACK);
     tft.drawBitmap(0,40,cloud,128,90,GREY);
}

void drawLightRainWithSunOrMoon()
{
  if(night)
  {
    drawCloudTheMoonAndRain();
  }else
  {
    drawCloudSunAndRain();
  }
}

void drawLightRain()
{
     tft.drawBitmap(0,35,cloud,128,90,GREY);
     tft.fillRoundRect(50, 105, 3, 13, 1, BLUE);
     tft.fillRoundRect(65, 105, 3, 13, 1, BLUE);
     tft.fillRoundRect(80, 105, 3, 13, 1, BLUE);
}

void drawModerateRain()
{
     tft.drawBitmap(0,35,cloud,128,90,GREY);
     tft.fillRoundRect(50, 105, 3, 15, 1, BLUE);
     tft.fillRoundRect(57, 102, 3, 15, 1, BLUE);
     tft.fillRoundRect(65, 105, 3, 15, 1, BLUE);
     tft.fillRoundRect(72, 102, 3, 15, 1, BLUE);
     tft.fillRoundRect(80, 105, 3, 15, 1, BLUE);
}

void drawHeavyRain()
{
     tft.drawBitmap(0,35,cloud,128,90,GREY);
     tft.fillRoundRect(43, 102, 3, 15, 1, BLUE);
     tft.fillRoundRect(50, 105, 3, 15, 1, BLUE);
     tft.fillRoundRect(57, 102, 3, 15, 1, BLUE);
     tft.fillRoundRect(65, 105, 3, 15, 1, BLUE);
     tft.fillRoundRect(72, 102, 3, 15, 1, BLUE);
     tft.fillRoundRect(80, 105, 3, 15, 1, BLUE);
     tft.fillRoundRect(87, 102, 3, 15, 1, BLUE);
}

void drawThunderstorm()
{
     tft.drawBitmap(0,40,thunder,128,90,YELLOW);
     tft.drawBitmap(0,35,cloud,128,90,GREY);
     tft.fillRoundRect(48, 102, 3, 15, 1, BLUE);
     tft.fillRoundRect(55, 102, 3, 15, 1, BLUE);
     tft.fillRoundRect(74, 102, 3, 15, 1, BLUE);
     tft.fillRoundRect(82, 102, 3, 15, 1, BLUE);
}

void drawLightSnowfall()
{
     tft.drawBitmap(0,30,cloud,128,90,GREY);
     tft.fillCircle(50, 100, 3, GREY);
     tft.fillCircle(65, 103, 3, GREY);
     tft.fillCircle(82, 100, 3, GREY);
}

void drawModerateSnowfall()
{
     tft.drawBitmap(0,35,cloud,128,90,GREY);
     tft.fillCircle(50, 105, 3, GREY);
     tft.fillCircle(50, 115, 3, GREY);
     tft.fillCircle(65, 108, 3, GREY);
     tft.fillCircle(65, 118, 3, GREY);
     tft.fillCircle(82, 105, 3, GREY);
     tft.fillCircle(82, 115, 3, GREY);
}

void drawHeavySnowfall()
{
     tft.drawBitmap(0,35,cloud,128,90,GREY);
     tft.fillCircle(40, 105, 3, GREY);
     tft.fillCircle(52, 105, 3, GREY);
     tft.fillCircle(52, 115, 3, GREY);
     tft.fillCircle(65, 108, 3, GREY);
     tft.fillCircle(65, 118, 3, GREY);
     tft.fillCircle(80, 105, 3, GREY);
     tft.fillCircle(80, 115, 3, GREY);
     tft.fillCircle(92, 105, 3, GREY);     
}

void drawCloudSunAndRain()
{
     tft.fillCircle(73,70,20,YELLOW);
     tft.drawBitmap(0,32,cloud,128,90,BLACK);
     tft.drawBitmap(0,35,cloud,128,90,GREY);
     tft.fillRoundRect(50, 105, 3, 13, 1, BLUE);
     tft.fillRoundRect(65, 105, 3, 13, 1, BLUE);
     tft.fillRoundRect(80, 105, 3, 13, 1, BLUE);
}

void drawCloudAndTheMoon()
{
     tft.fillCircle(94,60,18,GREY);
     tft.fillCircle(105,53,18,BLACK);
     tft.drawBitmap(0,32,cloud,128,90,BLACK);
     tft.drawBitmap(0,35,cloud,128,90,GREY);
}

void drawCloudTheMoonAndRain()
{
     tft.fillCircle(94,60,18,GREY);
     tft.fillCircle(105,53,18,BLACK);
     tft.drawBitmap(0,32,cloud,128,90,BLACK);
     tft.drawBitmap(0,35,cloud,128,90,GREY);
     tft.fillRoundRect(50, 105, 3, 11, 1, BLUE);
     tft.fillRoundRect(65, 105, 3, 11, 1, BLUE);
     tft.fillRoundRect(80, 105, 3, 11, 1, BLUE);
}

void drawWind()
{  
     tft.drawBitmap(0,35,wind,128,90,GREY);   
}

void drawFog()
{
  tft.fillRoundRect(45, 60, 40, 4, 1, GREY);
  tft.fillRoundRect(40, 70, 50, 4, 1, GREY);
  tft.fillRoundRect(35, 80, 60, 4, 1, GREY);
  tft.fillRoundRect(40, 90, 50, 4, 1, GREY);
  tft.fillRoundRect(45, 100, 40, 4, 1, GREY);
}

void clearIcon()
{
     tft.fillRect(0,40,128,100,BLACK);
}
