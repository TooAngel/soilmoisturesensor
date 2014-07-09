#include <RedFly.h>
#include <RedFlyClient.h>

byte server[] = { 192, 168, 178, 25 }; //{  85, 13,145,242 }; //ip from www.watterott.net (server)
int port = 80;
int sensorPin = A0;    // select the input pin for the potentiometer
int sensorValue = 0;  // variable to store the value coming from the sensor

#define HOSTNAME "sensors.cloudcontrolled.com"
#define BSSID "TooWLAN"
#define PASSWORD "Man lebt nur einmal"

bool connected = false;

RedFlyClient client(server, 80);

char data[1024];  //receive buffer
unsigned int len = 0; //receive buffer length

void setup() {
#if defined(__AVR_ATmega32U4__) //Leonardo boards use USB for communication
	Serial.begin(9600); //init serial port and set baudrate
	while(!Serial);//wait for serial port to connect (needed for Leonardo only)
#endif

	connect();
}

void connect() {
	uint8_t ret;
	ret = RedFly.init();
	if (ret) {
		Serial.println("INIT ERROR");
		connected = false;
	} else {
		RedFly.scan();
		ret = RedFly.join(BSSID, PASSWORD);
		if (ret) {
			Serial.println("JOIN ERROR");
			RedFly.disconnect();
			connected = false;
		} else {
			ret = RedFly.begin();
			if (ret) {
				Serial.println("BEGIN ERROR");
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
		Serial.print(data);

		len = 0;
	}
}

bool send_request() {
	char resultChar[255];
	get_request_data(resultChar);

	if (client.connect(server, port)) {
		//make a HTTP request
		Serial.println("Send Request");
		client.write(resultChar);
		return true;
	} else {
		Serial.print("CLIENT ERR: ");
		Serial.print(server[0]);
		Serial.print(server[1]);
		Serial.print(server[2]);
		Serial.print(server[3]);
		Serial.println(port);
		return false;
//		RedFly.disconnect();
//		for (;;)
//			; //do nothing forevermore
	}
}

bool update_server() {
	Serial.println("RedFly.getip");
	if (RedFly.getip(HOSTNAME, server) == 0) {
		return true;
	} else {
		Serial.print("DNS ERR: ");
		Serial.println(HOSTNAME);
		return false;
	}
}

void get_request_data(char* resultChar) {
	String data =
			"POST /sensors/hugo/points/ HTTP/1.1\r\nHost: "HOSTNAME"\r\nContent-Type: application/json\r\nContent-length: 16\r\n\r\n{\"measure\": ";
	data += sensorValue;
	data += "}\r\n";

	data.toCharArray(resultChar, 255);
//	Serial.println(resultChar);
}

void loop() {
	if (!connected) {
		Serial.println("Reconnect");
		delay(1000);
		connect();
	}
	client = RedFlyClient(server, 80);

	Serial.print("Read sensor: ");
	sensorValue = analogRead(sensorPin);
	Serial.println(sensorValue);

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
	Serial.println("Wait");
	delay(60000);
}
