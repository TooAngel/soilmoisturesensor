#include <RedFly.h>
#include <RedFlyClient.h>

int sensorPin = A0;
int sensorValue = 0;

void setup() {
  Serial.begin(9600);
  Serial.println("Arduino starting");
  
}

void loop() {
  // read the value from the sensor:
//  sensorValue = analogRead(sensorPin);    
//  Serial.println(sensorValue);
  
//  delay(1000);
  uint8_t ret;
  delay(10000);
  ret = RedFly.init();
  Serial.print("RedFly.init: ");
  Serial.println(ret);
  RedFly.scan();
  ret = RedFly.join("TooWlan", "Man lebt nur einmal", INFRASTRUCTURE);
  Serial.print("RedFly.join: ");
  Serial.println(ret);
  for(;;){}
}

