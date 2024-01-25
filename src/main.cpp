#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <Update.h>
#include "secrets.h"

const char* firmwareUrl = "https://joergschultz-twt.github.io/Understand_OTA/firmware.bin";

#define IS_NEW true

void performOTAUpdate() {
    HTTPClient httpClient;
    httpClient.begin(firmwareUrl);
    int httpCode = httpClient.GET();

    if (httpCode == 200) {
        // Get the total length of the firmware
        int contentLength = httpClient.getSize();

        // Start update process
        if (contentLength > 0 && Update.begin(contentLength)) {
            // Download firmware
            WiFiClient *client = httpClient.getStreamPtr();
            size_t written = Update.writeStream(*client);

            if (written == contentLength) {
                Serial.println("Written : " + String(written) + " successfully");
            } else {
                Serial.println("Written only : " + String(written) + "/" + String(contentLength) + ". Retry?");
            }
            if (Update.end()) {
                if (Update.isFinished()) {
                    Serial.println("OTA update complete!");
                    Serial.println("Rebooting...");
                    ESP.restart();
                } else {
                    Serial.println("Update not finished? Something went wrong!");
                }
            } else {
                Serial.println("Error #: " + String(Update.getError()));
            }
        } else {
            Serial.println("Not enough space to begin OTA");
            httpClient.end();
            return;
        }
    } else {
        Serial.println("HTTP error: " + String(httpCode));
    }
    httpClient.end();
}

void setup() {
    Serial.begin(115200);
    // Connect to Wi-Fi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi");
    delay(1000);
    if (IS_NEW) {
        Serial.println("This is the new version");
        delay(1000);
    } else {
        Serial.println("This is the old version, Starting OTA-Update in 1 sec");
        delay(1000);
        performOTAUpdate();
        Serial.println("OTA update complete!");
    }
}

void loop() {
    if (IS_NEW) {
        Serial.println("This is the new version");
    } else {
        Serial.println("This is the old version");
    }
    delay(1000);
}