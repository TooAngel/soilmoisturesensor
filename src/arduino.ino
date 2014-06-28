
int sensorPin = A0;    // select the input pin for the potentiometer
int sensorValue = 0;  // variable to store the value coming from the sensor

void setup() {
  Serial.begin(9600);           // set up Serial library at 9600 bps
  Serial.println("Hello world!");  // prints hello with ending line break 
}

void loop() {
  // read the value from the sensor:
  sensorValue = analogRead(sensorPin);    
  Serial.println(sensorValue);
  
  delay(60000);  
}

