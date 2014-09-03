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

void log(String message) {
	if (Serial) {
		Serial.print(message);
	}
}

void logln(String message) {
	if (Serial) {
		Serial.println(message);
	}
}

bool rf_init() {
	logln("rf_init");
	uint8_t ret;
	ret = RedFly.init(9600, LOW_POWER);
	if (ret) {
		logln("INIT ERROR");
		return false;
	}
	return true;
}

bool rf_join() {
	logln("rf_join");
	uint8_t ret;
	RedFly.scan();
	ret = RedFly.join(BSSID, PASSWORD);
	if (ret) {
		logln("JOIN ERROR");
		RedFly.disconnect();
		return false;
	}
	return true;

}

bool rf_begin() {
	logln("rf_begin");
	uint8_t ret;
	ret = RedFly.begin();
	if (ret) {
		logln("BEGIN ERROR");
		RedFly.disconnect();
		connected = false;
		return false;
	}
	return true;

}

bool rf_set_client() {
	logln("rf_set_client");
	client = RedFlyClient(server, 80);
	return true;
}

bool get_ip() {
	logln("get_ip");
	char* hostname_char;
	if (RedFly.getip(HOSTNAME, server)) {
		logln("DNS ERR");
		return false;
	}
	return true;
}

bool read_sensor() {
	log("read_sensor ");
	sensorValue = analogRead(sensorPin);
	logln(String(sensorValue));
	return true;
}

bool connect() {
	logln("connect");
	if (client.connect(server, port)) {
		return true;
	} else {
		logln("CLIENT ERR: ");
		return false;
	}
}

bool send_request() {
	logln("send_request");
	char resultChar[255];

	get_request_data(resultChar);
	client.write(resultChar);

	return true;
}

void get_request_data(char* resultChar) {
	String data = "POST /sensors/fifi/points/ HTTP/1.1\r\nHost:";
	data += HOSTNAME;
	data +=
			"\r\nContent-Type: application/json\r\nContent-length: 16\r\n\r\n{\"measure\": ";
	data += sensorValue;
	data += "}\r\n";

	data.toCharArray(resultChar, 255);
}

int read_response() {
	logln("read_response");
	int a = 0;
	int c;
	char data[1024];
	unsigned int len = 0;

	while (true) {
		if (client.available()) {
			logln("client.available");
			do {
				c = client.read();
				if ((c != -1) && (len < (sizeof(data) - 1))) {
					data[len++] = c;
				}
			} while (c != -1);
			a = data[168] - '0';
			break;
		}

		if (!client.connected()) {
			break;
		}
	}
	client.stop();
	return a;
}

bool (*states[8])();

void setup() {
	pinMode(12, OUTPUT);
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
	int motorTime = 0;
	for (int i = state; i < 8; i++) {
		if (!(*states[i])()) {
			logln("failed");
			state = max(0, state - 1);
			delay(1000);
			return;
		}
	}

	motorTime = read_response();
	if (motorTime > 0) {
		logln("Start motor");
		logln(String(motorTime));
		digitalWrite(12, HIGH);
		delay(motorTime * 1000);
		digitalWrite(12, LOW);
	}

	state = 5;
	logln("wait");
	delay(60000 - (motorTime * 1000));
}
