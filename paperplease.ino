#include <stdint.h>
#include <string.h>

#include <ESP8266WiFi.h>

#define PIN_KNOP    D1

static WiFiClient client;

void setup(void)
{
    pinMode(PIN_KNOP, INPUT_PULLUP);

    Serial.begin(115200);
    Serial.println("Papiertje");

    WiFi.begin("revspace-pub-2.4ghz");
}


void loop(void)
{
    if (digitalRead(PIN_KNOP) == 0) {
        Serial.print("Button pressed, dispensing paper ...");
        client.connect("10.42.42.202", 9100);
        client.print(char(0x0c));
        client.flush();
        client.stop();
        Serial.println("done");
        delay(50);
        while (digitalRead(PIN_KNOP) == 0);
        delay(50);
    }
}



