#include <stdbool.h>
#include <Arduino.h>

#include <FS.h>
#include <ESP8266WiFi.h>

//#define PRINTER_HOST    "192.168.8.179"
#define PRINTER_HOST    "10.42.42.202"
#define PRINTER_PORT    9100

#define PIN_BUTTON_EMPTYPAGE    D1
#define PIN_BUTTON_GRAPHPAPER   D2

static WiFiClient client;

void setup(void)
{
    pinMode(PIN_BUTTON_EMPTYPAGE, INPUT_PULLUP);
    pinMode(PIN_BUTTON_GRAPHPAPER, INPUT_PULLUP);

    Serial.begin(115200);
    Serial.println("Papiertje");
    
    SPIFFS.begin();

    WiFi.begin("revspace-pub-2.4ghz");
}

#define PJL_ESCAPE "\x1b%-12345X"

static boolean print_file(const char *filename, Stream &stream)
{
    char buffer[512];

    // get file
    File f = SPIFFS.open(filename, "r");
    if (!f) {
        Serial.print("Could not open file "); 
        Serial.println(filename);
        return false;
    }

    // PJL header to enter postscript
    stream.print(PJL_ESCAPE);
    stream.print("@PJL JOB NAME=\"graphpaper\"\r\n");
    stream.print("@PJL SET PAPER=A4\r\n");
    stream.print("@PJL ENTER LANGUAGE=POSTSCRIPT\r\n");
    
    // write postscript file contents
    size_t read = 0;
    do {
        read = f.readBytes(buffer, sizeof(buffer));
        stream.write(buffer, read);
    } while (read > 0);
    f.close();

    // PJL footer
    stream.print(PJL_ESCAPE);
    stream.print("@PJL EOF\r\n");
    stream.print(PJL_ESCAPE);

    return true;
}

void loop(void)
{
    if (digitalRead(PIN_BUTTON_EMPTYPAGE) == 0) {
        Serial.print("Button pressed, dispensing empty paper ...");
        if (client.connect(PRINTER_HOST, PRINTER_PORT)) {
            client.print(char(0x0c));
            client.flush();
            client.stop();
            Serial.println("done");
        } else {
            Serial.println("failed");
        }
        delay(50);
        while (digitalRead(PIN_BUTTON_EMPTYPAGE) == 0);
        delay(50);
    }
    if (digitalRead(PIN_BUTTON_GRAPHPAPER) == 0) {
        Serial.print("Button pressed, dispensing graph paper ...");
        if (client.connect(PRINTER_HOST, PRINTER_PORT)) {
            print_file("/graphpaper.ps", client);
            client.flush();
            client.stop();
            Serial.println("done");
        } else {
            Serial.println("failed");
        }
        delay(50);
        while (digitalRead(PIN_BUTTON_GRAPHPAPER) == 0);
        delay(50);
    }
}

