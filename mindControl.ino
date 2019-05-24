#include <WiFi.h> //Connect to WiFi Network
#include <string.h>  //used for some string handling and processing.
#include <Brain.h>
// Arduino Brain Library - Brain Serial Test
// Description: Grabs brain data from the serial RX pin and sends CSV out over the TX pin (Half duplex.)
// More info: https://github.com/kitschpatrol/Arduino-Brain-Library


char network[] = "MIT";  //SSID for 6.08 Lab
char password[] = ""; //Password for 6.08 Lab

const uint8_t LOOP_PERIOD = 10; //milliseconds

const char USER[] = "Adam";

char mindData[200];
int strength=0;
int attention=0;
int meditation=0; 

//Some constants and some resources:
const int RESPONSE_TIMEOUT = 6000; //ms to wait for response from host

const uint16_t IN_BUFFER_SIZE = 1000; //size of buffer to hold HTTP request
const uint16_t OUT_BUFFER_SIZE = 1000; //size of buffer to hold HTTP response
char request_buffer[IN_BUFFER_SIZE]; //char array buffer to hold HTTP request
char response_buffer[OUT_BUFFER_SIZE]; //char array buffer to hold HTTP response

// Set up the brain parser, pass it the hardware serial object you want to listen on.
Brain brain(Serial);
uint32_t timer;

void setup() {
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
  timer = millis();
}

void loop() {

    // Expect packets about once per second.
    // The .readCSV() function returns a string (well, char*) listing the most recent brain data, in the following format:
    // "signal strength, attention, meditation, delta, theta, low alpha, high alpha, low beta, high beta, low gamma, high gamma"
    if (brain.update()) {
        Serial.println(brain.readErrors());
        Serial.println(brain.readCSV());
        delimit(brain.readCSV());

      Serial.println("GOOD DATA");
      char body[200];
      sprintf(body,"user=%s&str=%d&att=%d&med=%d",USER, strength, attention, meditation);
      
      sprintf(request_buffer,"POST http://608dev.net/sandbox/sc/awpotter/drone/droneServer.py HTTP/1.1\r\n");
      strcat(request_buffer,"Host: 608dev.net\r\n");
      strcat(request_buffer,"Content-Type: application/x-www-form-urlencoded\r\n");
      sprintf(request_buffer+strlen(request_buffer),"Content-Length: %d\r\n", strlen(body)); //append string formatted to end of request buffer
      strcat(request_buffer,"\r\n"); //new line from header to body
      strcat(request_buffer, body);
      strcat(request_buffer,"\r\n"); //header
      //Serial.println(request_buffer);
      
      do_http_request("608dev.net", request_buffer, response_buffer, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT,true);
      
      //*/
    }
  /*
  while (millis()-timer < LOOP_PERIOD);
  timer = millis();
   */
}

void delimit(char input[]){
  if (strlen(input)>10){
  Serial.println(input);
  char * pointer;
  //Breaks data string into pieces and sets global variables accordingly
    pointer = strtok (input,",");
    strength = atoi(pointer);
    pointer = strtok (NULL, ",");
    attention = atoi(pointer);
    pointer = strtok (NULL, ",");
    meditation = atoi(pointer);
    Serial.print(strength); Serial.print(": ");
    Serial.print(attention); Serial.print(": ");
    Serial.print(meditation);
  }//end of it
}

