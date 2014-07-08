#include <RedFly.h>
#include <RedFlyClient.h>

byte server[] = { 0, 0, 0, 0 }; //{  85, 13,145,242 }; //ip from www.watterott.net (server)

#define HOSTNAME "sensors.cloudcontrolled.com"  //host

RedFlyClient client(server, 80);

void setup() {
	uint8_t ret;

#if defined(__AVR_ATmega32U4__) //Leonardo boards use USB for communication
	Serial.begin(9600); //init serial port and set baudrate
	while(!Serial);//wait for serial port to connect (needed for Leonardo only)
#endif

	Serial.println("init");
	ret = RedFly.init();
	if (ret) {
		Serial.print("INIT ERR: ");
		Serial.println(ret);
	} else {
		Serial.println("scan");
		RedFly.scan();
		ret = RedFly.join("TooWLAN", "Man lebt nur einmal");
		if (ret) {
			Serial.print("JOIN ERR: ");
			Serial.println(ret);
			for (;;)
				; //do nothing forevermore
		} else {
			Serial.println("begin");
			ret = RedFly.begin(); //DHCP
			if (ret) {
				Serial.print("BEGIN ERR: ");
				Serial.println(ret);
				RedFly.disconnect();
				for (;;)
					; //do nothing forevermore
			} else {
				Serial.println("getip");
				if (RedFly.getip(HOSTNAME, server) == 0) //get ip
						{
					if (client.connect(server, 80)) {
						//make a HTTP request
						Serial.println("send");
						client.print_P(
								PSTR(
										"POST /sensors/hugo/points/ HTTP/1.1\r\nHost: "HOSTNAME"\r\n\r\n {}"));
					} else {
						Serial.println("CLIENT ERR");
						RedFly.disconnect();
						for (;;)
							; //do nothing forevermore
					}
				} else {
					Serial.println("DNS ERR");
					RedFly.disconnect();
					for (;;)
						; //do nothing forevermore
				}
			}
		}
	}
}

char data[1024];  //receive buffer
unsigned int len = 0; //receive buffer length

void loop() {
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
