/*
 * Click
 *   Author:      Thorinair
 *   Version:     v1.0.0
 *   Description: A e WiFi power switch.
 *   
 *   This is the main source code file. All configuration is to be done inside the Configuration.h file.
*/

#include <ESP8266WiFi.h>
#include <TwiFi.h>

#include "Configuration.h"
#include "ConfigurationWiFi.h"
#include "ConfigurationLuna.h"

/* PIN Definitions */
#define PIN_LED   13
#define PIN_POWER A0

/* RGB LED Statuses */
#define LED_MODE_OFF 0
#define LED_MODE_DIM 1
#define LED_MODE_ON  2

/* URL results */
#define URL_RESULT_DONE 0
#define URL_RESULT_FAIL 1

/* Flash Types */
#define FLASH_TYPE_DONE 0
#define FLASH_TYPE_FAIL 1

/* Flash Timings */
#define FLASH_TIME_SHORT 50
#define FLASH_TIME_LONG  500

/* Setup Functions */
void setupPins();

/* Utility Functions */
void flashStatusLED(int type);
int openURL(String url);

/* Processing Functions */
void processInterval();

void setLED(int ledMode);

void connectAttempt(int idEntry, int attempt);
void connectSuccess(int idEntry);
void connectFail(int idEntry);

/* Variables */
int loopCounter;
bool wifiConnected;
bool initial;
bool powerState;
bool blinking;

/* Setup Functions */
void setupPins() {
    if (LED_ENABLE) {
        pinMode(PIN_LED, OUTPUT);
        setLED(LED_MODE_DIM);
    }
}



/* Utility Functions */
void flashStatusLED(int type) {
    if (LED_ENABLE) {
        if (type == FLASH_TYPE_DONE) {
            setLED(LED_MODE_ON);
            delay(FLASH_TIME_SHORT);
            setLED(LED_MODE_DIM);
            delay(FLASH_TIME_SHORT);
        }
        else if (type == FLASH_TYPE_FAIL) {
            setLED(LED_MODE_ON);
            delay(FLASH_TIME_LONG);
            setLED(LED_MODE_DIM);
            delay(FLASH_TIME_SHORT);
        }
    }
}

int openURL(String url) {
    if (LUNA_DEBUG)
        Serial.println("Opening URL: " + String(LUNA_IP) + url);
        
    WiFiClient client;
    if (!client.connect(LUNA_IP, LUNA_PORT)) {  
        if (LUNA_DEBUG)
            Serial.println("Error connecting!");
        return URL_RESULT_FAIL;
    }

    client.print("GET " + url + " HTTP/1.1\r\n" +
                 "Host: " + LUNA_IP + "\r\n" + 
                 "Connection: close\r\n\r\n");
    client.stop();
    
    if (LUNA_DEBUG)
        Serial.println("Connection success.");

    return URL_RESULT_DONE;
}



/* Processing Functions */
void processInterval() {
    String url = String(LUNA_URL_POWER) + "&key=" + String(LUNA_KEY) + "&power=";
    
    //Serial.println(analogRead(PIN_POWER));        
    if (analogRead(PIN_POWER) > POWER_THRESHOLD) {
        if (!powerState && !initial) {
            if (openURL(url + "on") == URL_RESULT_DONE)
                 flashStatusLED(FLASH_TYPE_DONE);
            else     
                 flashStatusLED(FLASH_TYPE_FAIL);
            loopCounter = 0;    
        }
        powerState = true;
    }
    else {
        if (powerState && !initial) {
            if (openURL(url + "off") == URL_RESULT_DONE)
                 flashStatusLED(FLASH_TYPE_DONE);  
            else     
                 flashStatusLED(FLASH_TYPE_FAIL);  
            loopCounter = 0;        
        }
        powerState = false;
    }        

    if ((loopCounter >= (NOTIFICATION_INTERVAL * 1000) / POLLING_INTERVAL) || initial) {
        int result;
        if (powerState)
            result = openURL(url + "on");
        else
            result = openURL(url + "off");
        if (result == URL_RESULT_DONE)
             flashStatusLED(FLASH_TYPE_DONE);  
        else     
             flashStatusLED(FLASH_TYPE_FAIL);
        loopCounter = 0;
        initial = false;
    }
    else
        loopCounter++;

    delay(POLLING_INTERVAL);
}

void setLED(int ledMode) {
    if (LED_ENABLE) {
        switch (ledMode) {
            case LED_MODE_OFF: 
                analogWrite(PIN_LED, PWMRANGE - 0);
                break;
            case LED_MODE_DIM: 
                analogWrite(PIN_LED, PWMRANGE - LED_BRIGHTNESS_DIM);
                break;
            case LED_MODE_ON: 
                analogWrite(PIN_LED, PWMRANGE - LED_BRIGHTNESS_ON);
                break;         
        }
    }
}

void connectAttempt(int idEntry, int attempt) {
    if (attempt % 2) {
        setLED(LED_MODE_OFF);
    }
    else {
        setLED(LED_MODE_DIM);
    }
}

void connectSuccess(int idEntry) {
}

void connectFail(int idEntry) {
    setLED(LED_MODE_ON);
}

void setup() {
    Serial.begin(9600);
    loopCounter = 0;
    initial = true;
    powerState = false;
    blinking = false;
    setupPins();
    twifiInit(
        wifis,
        WIFI_COUNT,
        WIFI_HOST,
        WIFI_TIMEOUT,
        &connectAttempt,
        &connectSuccess,
        &connectFail,
        WIFI_DEBUG
        );
    wifiConnected = twifiConnect(true);
    setLED(LED_MODE_DIM);
    openURL(String(LUNA_URL_BOOT) + "&key=" + String(LUNA_KEY) + "&device=" + String(WIFI_HOST));
}

void loop() {
    if (!twifiIsConnected())
        wifiConnected = twifiConnect(true);
    else
        processInterval();
}
