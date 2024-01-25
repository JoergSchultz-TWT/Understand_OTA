#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <Update.h>
#include "secrets.h"

const char* firmwareUrl = "http://example.com/firmware.bin"; // Replace with your firmware URL


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

    // Perform OTA update
    // performOTAUpdate();
}

void loop() {
    Serial.println("This is the new version");
    delay(1000);
}