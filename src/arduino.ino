#include <RedFly.h>
#include <RedFlyClient.h>

byte server[] = { 192, 168, 178, 25 };
int port = 80;
int sensorPin = A0;
int sensorValue = 0;

#define HOSTNAME "sensors.cloudcontrolapp.com"
#define BSSID "TooWLAN"
#define PASSWORD "Man lebt nur einmal"

bool connected = false;
int state = 0;

RedFlyClient client(server, 80);

char data[1024];  //receive buffer
unsigned int len = 0; //receive buffer length

void log(String message) {
	if (Serial) {
		Serial.println(message);
	}
}

bool rf_init() {
	log("rf_init");
	uint8_t ret;
	ret = RedFly.init(9600, LOW_POWER);
	if (ret) {
		log("INIT ERROR");
		return false;
	}
	return true;
}

bool rf_join() {
	log("rf_join");
	uint8_t ret;
	RedFly.scan();
	ret = RedFly.join(BSSID, PASSWORD);
	if (ret) {
		log("JOIN ERROR");
		RedFly.disconnect();
		return false;
	}
	return true;

}

bool rf_begin() {
	log("rf_begin");
	uint8_t ret;
	ret = RedFly.begin();
	if (ret) {
		log("BEGIN ERROR");
		RedFly.disconnect();
		connected = false;
		return false;
	}
	return true;
	
}

bool rf_set_client() {
	log("rf_set_client");
	client = RedFlyClient(server, 80);
	return true;
}

bool get_ip() {
	log("get_ip");
	char* hostname_char;
	if (RedFly.getip(HOSTNAME, server)) {
		log("DNS ERR");
		return false;
	}
	return true;
}

bool read_sensor() {
	log("read_sensor");
	sensorValue = analogRead(sensorPin);
	return true;
}

bool connect() {
	log("connect");
	if (client.connect(server, port)) {
		return true;
	} else {
		log("CLIENT ERR: ");
		return false;
	}
}

bool send_request() {
	log("send_request");
	char resultChar[255];
	get_request_data(resultChar);
	client.write(resultChar);
	return true;
}

void get_request_data(char* resultChar) {
	String data = "POST /sensors/hugo/points/ HTTP/1.1\r\nHost:";
	data += HOSTNAME;
	data += "\r\nContent-Type: application/json\r\nContent-length: 16\r\n\r\n{\"measure\": ";
	data += sensorValue;
	data += "}\r\n";

	data.toCharArray(resultChar, 255);
}

bool (*states[8])();

void setup() {
	Serial.begin(9600);
	states[0] = rf_init;
	states[1] = rf_join;
	states[2] = rf_begin;
	states[3] = rf_set_client;
	states[4] = get_ip;
	states[5] = read_sensor;
	states[6] = connect;
	states[7] = send_request;
}

void loop() {
	for (int i=state; i<8; i++) {
		if(!(*states[i]) ()) {
			log("failed");
			state = max(0, state-1);
			delay(1000);
			return;
		}
	}
	state = 6;
	client.stop();
	log("wait");
	delay(60000);
}
