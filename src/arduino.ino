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

RedFlyClient client(server, 80);

char data[1024];  //receive buffer
unsigned int len = 0; //receive buffer length

void log(String message) {
	if (Serial) {
		Serial.println(message);
	}
}
	

void setup() {
	Serial.begin(9600);
	connect();
}

void connect() {
	uint8_t ret;
	ret = RedFly.init();
	if (ret) {
		log("INIT ERROR");
		connected = false;
	} else {
		RedFly.scan();
		ret = RedFly.join(BSSID, PASSWORD);
		if (ret) {
			log("JOIN ERROR");
			RedFly.disconnect();
			connected = false;
		} else {
			ret = RedFly.begin();
			if (ret) {
				log("BEGIN ERROR");
				RedFly.disconnect();
				connected = false;
			} else {
				connected = true;
			}
		}
	}
}

void print_response() {
	int c;

	//if there are incoming bytes available 
	//from the server then read them 
	if (client.available()) {
		do {
			c = client.read();
			if ((c != -1) && (len < (sizeof(data) - 1))) {
				data[len++] = c;
			}
		} while (c != -1);
	}

	//if the server's disconnected, stop the client and print the received data
	if (len && !client.connected()) {
		client.stop();
		RedFly.disconnect();

		data[len] = 0;
		log(data);
		len = 0;
	}
}

bool send_request() {
	char resultChar[255];
	get_request_data(resultChar);

	if (client.connect(server, port)) {
		//make a HTTP request
		log("Send Request");
		client.write(resultChar);
		return true;
	} else {
		log("CLIENT ERR: ");
		return false;
	}
}

bool update_server() {
	log("RedFly.getip");
	char* hostname_char;
	if (RedFly.getip(HOSTNAME, server) == 0) {
		return true;
	} else {
		log("DNS ERR");
		return false;
	}
}

void get_request_data(char* resultChar) {
	String data = "POST /sensors/hugo/points/ HTTP/1.1\r\nHost:";
	data += HOSTNAME;
	data += "\r\nContent-Type: application/json\r\nContent-length: 16\r\n\r\n{\"measure\": ";
	data += sensorValue;
	data += "}\r\n";

	data.toCharArray(resultChar, 255);
}

void loop() {
	if (!connected) {
		log("Reconnect");
		delay(1000);
		connect();
	}
	client = RedFlyClient(server, 80);

	sensorValue = analogRead(sensorPin);
	log("Read sensor: " + sensorValue);

	if (send_request()) {
		print_response();
	} else {
		if (update_server()) {
			if (send_request()) {
				print_response();
			}
		}
	}
	client.stop();
	log("Wait");
	delay(60000);
}
