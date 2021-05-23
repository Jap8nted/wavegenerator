
#include "config.h"
#include <WiFi.h>

void connectToWifi()
{
    WiFi.begin(WIFI_NETWORK_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(" -*0*- ");
    }

    Serial.println("");
    Serial.println("Conectado a WiFi, papu");
    Serial.print("La dirección IP del Módulo ESP32 es: ");
    Serial.print(WiFi.localIP());
}