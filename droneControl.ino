#include <WiFi.h> //Connect to WiFi Network
#include "Arduino.h"

#define PWM_PIN 27
#define HZ 50
#define PWM_RES 12
#define PWM_READ 5
#define PWM_THRESH 1500
#define VARIANCE 400
//Output at 50hz

char network[] = "MIT";  //SSID for 6.08 Lab
char password[] = ""; //Password for 6.08 Lab
const int RESPONSE_TIMEOUT = 6000; //ms to wait for response from host
const uint16_t IN_BUFFER_SIZE = 500; //size of buffer to hold HTTP request
const uint16_t OUT_BUFFER_SIZE = 500; //size of buffer to hold HTTP response
char request_buffer[IN_BUFFER_SIZE]; //char array buffer to hold HTTP request
char response_buffer[OUT_BUFFER_SIZE]; //char array buffer to hold HTTP response
const int LOOP_TIME = 500;

uint32_t timer;
int pwm_out = 0; //0->255
int current_out = 0;

void setup() {
  pinMode(PWM_PIN, OUTPUT);
  pinMode(PWM_READ, INPUT);
  pinMode(26, INPUT);

  ledcSetup(0, HZ, PWM_RES);
  ledcAttachPin(PWM_PIN, 0);
  ledcWrite(0, int(0.2049*1109));

  
  Serial.begin(9600);
  delay(100); //wait a bit (100 ms)

  WiFi.begin(network); //attempt to connect to wifi
  uint8_t count = 0; //count used for Wifi check times
  Serial.print("Attempting to connect to ");
  Serial.println(network);
  while (WiFi.status() != WL_CONNECTED && count<12) {
    delay(500);
    Serial.print(".");
    count++;
  }
  delay(2000);
  if (WiFi.isConnected()) { //if we connected then print our IP, Mac, and SSID we're on
    Serial.println("CONNECTED!");
    Serial.println(WiFi.localIP().toString() + " (" + WiFi.macAddress() + ") (" + WiFi.SSID() + ")");
    delay(500);
  } else { //if we failed to connect just Try again.
    Serial.println("Failed to Connect :/  Going to restart");
    Serial.println(WiFi.status());
    ESP.restart(); // restart the ESP (proper way)
  }
  //*/
  delay(10000);
  timer = millis();

}

void loop() {
 
    sprintf(request_buffer,"GET http://608dev.net/sandbox/sc/awpotter/drone/droneServer.py HTTP/1.1\r\n");
    strcat(request_buffer,"Host: 608dev.net\r\n"); //add more to the end
    strcat(request_buffer,"\r\n"); //add blank line!
    do_http_request("608dev.net", request_buffer, response_buffer, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT,true);
    Serial.println(response_buffer); //print to serial monitor
    //Serial.println(pulseIn(PWM_READ,HIGH));
    
  if(pulseIn(PWM_READ,HIGH)>=PWM_THRESH)
  { 
    pwm_out = atoi(strtok(response_buffer, ":"));
    Serial.println("POWER="); 
    pwm_out = (pulseIn(PWM_READ,HIGH)-PWM_THRESH)+ pwm_out*(VARIANCE/100)+1000;
    if(pwm_out>2000){pwm_out=2000;}
    if(pwm_out<1109){pwm_out=1109;}
    Serial.println(pwm_out);
    if(pwm_out!=current_out && millis()-timer>LOOP_TIME)
      {
        ledcWrite(0, int(0.2049*pwm_out));
        Serial.println("OUTPUT CHANGED");
        current_out=pwm_out;
        timer=millis();
      }
  }
  else if(current_out!=1109 && millis()-timer>LOOP_TIME)
    {
      ledcWrite(0, int(0.2049*1109));
      current_out=1109;
      timer = millis();
      Serial.println("MOTOR SET ZERO");
    }
//*/
}

//if(pulseIn(PWM_READ,HIGH)<PWM_THRES){ledcWrite(0, int(0.2049*1109); current_out=1109;}

/*
  for (int i=1109; i<1600; i++){
    ledcWrite(0, int(0.2049*i));
    delay(2);
  }
  delay(5000);
    for (int i=1600; i>1109; i--){
    ledcWrite(0, int(0.2049*i));
    delay(2);
  }
  delay(5000);
  //*/
//Serial.println(pulseIn(PWM_READ, HIGH));
//Serial.println(pulseIn(26, HIGH));
