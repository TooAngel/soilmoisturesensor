#include <RedFly.h>
#include <RedFlyClient.h>

int sensorPin = A0;    // select the input pin for the potentiometer
int sensorValue = 0;  // variable to store the value coming from the sensor

void setup() {
  Serial.begin(9600);           // set up Serial library at 9600 bps
  Serial.println("Hello world!");  // prints hello with ending line break
  
  ret = RedFly.init();
  Serial.println('init');
  Serial.println(ret);
  RedFly.scan();
  ret = RedFly.join("TooWlan", "Man lebt nur einmal", INFRASTRUCTURE);
  Serial.println('join');
  Serial.println(ret);
}

void loop() {
  // read the value from the sensor:
  sensorValue = analogRead(sensorPin);    
  Serial.println(sensorValue);
  
  delay(60000);  
}

