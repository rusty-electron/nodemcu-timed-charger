#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char* ssid = "NodeMCU";  // Enter SSID here
const char* password = "Password";  //Enter Password here

IPAddress local_ip(192,168,2,1);
IPAddress gateway(192,168,2,1);
IPAddress subnet(255,255,255,0);

ESP8266WebServer server(80);

#define DEFAULT 30
int devpin = 5; // D1(gpio5)
int button = 4; //D2(gpio4)
int buzzpin = 14; //D5
int wifistatus = D6;
int pausestatus = D7;
int buttonState = 0;
int paused = 0;

int timerRunning = false;
unsigned long timerValue;
unsigned long pauseValue;

unsigned long idleTimer = 0;
float period = DEFAULT;

char bufferstr[7];

long min2msec(float arg){
  return arg*60*1000;
}

void turn_on(){
   Serial.println("[Status] ON");

   timerRunning = true;
   timerValue = millis();
   
   digitalWrite(buzzpin, HIGH); 
   delay(2000);
   digitalWrite(buzzpin, LOW);
   
   digitalWrite(devpin, HIGH); 
}

void turn_off(){
   Serial.println("[Status] OFF");
   
   //Buzzer Sound Sequence
   digitalWrite(buzzpin, HIGH); 
   delay(500);
   digitalWrite(buzzpin, LOW);
   delay(500);
   digitalWrite(buzzpin, HIGH); 
   delay(500);
   digitalWrite(buzzpin, LOW);
   //End Buzzer Sound
   
   digitalWrite(devpin, LOW);

   idleTimer = millis();
}

int checkTimer(float period){
   long timeDiff = (millis() - timerValue);
   float seconds = (float)(timeDiff/1000);
   sprintf(bufferstr,"%02d : %02d", (int)(seconds/60), (int)(seconds)%60);
   //Serial.println((String)"[Remaining] " + bufferstr);
   if(timeDiff >= min2msec(period)){
      return 0;
   }
   return -1;
}

void setTimer(int value){
    paused = 0;
    timerRunning = true;
    timerValue = millis();
    period = value;
    idleTimer = 0;
    
    digitalWrite(buzzpin, HIGH); 
    delay(1000);
    digitalWrite(buzzpin, LOW);
   
    digitalWrite(devpin, HIGH);
}

void checkpause(int buttonState){
    if(timerRunning && buttonState && !paused)
    {
         pauseValue = millis();
         paused = true;
         Serial.println("[Status] PAUSED");

         digitalWrite(pausestatus, HIGH); 
         digitalWrite(devpin, LOW); 
         
         digitalWrite(buzzpin, HIGH); 
         delay(600);
         digitalWrite(buzzpin, LOW);
    }else if(timerRunning && !buttonState && paused){
         timerValue += millis() - pauseValue;
         paused = false;
         Serial.println("[Status] RESUMED");

         digitalWrite(pausestatus, LOW); 
         digitalWrite(devpin, HIGH); 
         
         digitalWrite(buzzpin, HIGH); 
         delay(600);
         digitalWrite(buzzpin, LOW);
    }
}

void setup() {
  Serial.begin(9600);
  
  pinMode(devpin, OUTPUT);
  pinMode(buzzpin, OUTPUT);
  pinMode(wifistatus, OUTPUT);
  pinMode(pausestatus, OUTPUT);
  pinMode(button, INPUT);
  Serial.println("[Start] Setup Complete");
  
  turn_on();
  
  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  
  server.on("/", handle_OnConnect);
  server.on("/save", handle_save);
  server.onNotFound(handle_NotFound);
 
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();

  if (WiFi.softAPgetStationNum()>0)
     digitalWrite(wifistatus, HIGH);
  else
     digitalWrite(wifistatus, LOW);
  
  if(timerRunning && !paused)
      if(checkTimer(period) == 0){
          turn_off();
          timerRunning = false;
      }
   
   buttonState = digitalRead(button);
   
   checkpause(buttonState);
   
   if(idleTimer > 0){
   Serial.println(idleTimer);
      if(millis() - idleTimer >= min2msec(5)){
          Serial.println("[DeepSleep] ON");
          ESP.deepSleep(0);
       }
   }
}

void handle_OnConnect() {
  server.send(200, "text/html", SendHTML()); 
}

void handle_NotFound(){
  server.send(404, "text/plain", "Page not found");
}

void handle_save(){
  if(server.arg(1).equals("Stop Timer"))
    period = 0;

  server.arg(0).trim();
  if(server.arg(0).length() > 0)
      setTimer(server.arg(0).toInt());
  server.send(200, "text/html", SendHTML());
}

String SendHTML(){
  String stat;
  if(paused)
    stat = "Paused";
  else if(timerRunning)
    stat = "Running";
  else 
    stat = "Stopped";
  
  String  ptr = "<!DOCTYPE html>\n";
          ptr +="<html>\n";
          ptr +="<head>\n";
          ptr +="<title>[ESP8266] Charger Control</title>\n";
          ptr +="</head>\n";
          ptr +="<body>\n";
          ptr +="<p>Current Time: </p>\n";
          ptr +="<h1>"+ (String)bufferstr +"</h1>\n";
          ptr +="<p> Status: "+ stat +"</p>\n";
          ptr +="<form method=\"post\" action=\"/save\" >\n";
          
          ptr +="Turn on time:<br><input name=\"timebox\" type=\"text\" size=\"16\" value=\"\" ><br><br>\n";
          
          ptr +="<input type=\"submit\" name=\"clk_action\" value=\"Set Timer\">\n";
          ptr +="<input type=\"submit\" name=\"clk_action\" value=\"Stop Timer\">\n";
          ptr +="</form>\n";
          ptr +="</body>\n";
          ptr +="</html>\n";
  return ptr;
}
