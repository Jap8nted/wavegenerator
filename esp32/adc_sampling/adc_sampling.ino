
/*************************
 * 
 * Authors: 
 *      Jonatan Aponte
 *      Santiago Aponte
 * 
 * IoT AC Current and Voltage measurement real time measurement and transmission to remote 
 * broker
 * ***********************/

#include <Adafruit_ADS1X15.h>
#include "PubSubClient.h"
#include "config.h"
#include "WiFi.h"
#include <ArduinoJson.h>

WiFiClient wifiClient;

Adafruit_ADS1015 ads;

uint8_t CURRENT = 0;
uint8_t VOLTAGE = 1;
int16_t MQTT_PORT = 1883;
int LED_1 = 2;
int LED_2 = 4;

const char *MQTT_SERVER = "192.168.178.21";
const char *TOPIC = "home/energy";
const char *MQTT_USERNAME = "japonted";
const char *MQTT_PASSWORD = "japonted";
const char *MQTT_CLIENT_ID = "power_meter";

PubSubClient mqttClient(MQTT_SERVER, MQTT_PORT, wifiClient);

TaskHandle_t SendDataToMqttBrokerHandler;

const uint8_t IO32 = 32;
const uint8_t IO33 = 33;
bool status = HIGH;

int counter = 0;
const int measurementSize = 4096;
volatile int16_t currentMeasurements[measurementSize];

volatile int16_t voltageMeasurements[measurementSize];
volatile bool sampleFinished;

volatile int16_t voltageMeasurement = 0;
volatile int16_t currentMeasurement = 0;

void connectToMqtt()
{
    Serial.print("connecting to MQTT");
    Serial.print(WIFI_NETWORK_SSID);

    WiFi.begin(WIFI_NETWORK_SSID, WIFI_PASSWORD);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println("Wifi connected");
    Serial.print("IP Address");
    Serial.println(WiFi.localIP());

    if (mqttClient.connect(MQTT_CLIENT_ID, MQTT_USERNAME, MQTT_PASSWORD))
    {
        Serial.print("Connected to MQTT Broker");
    }
    else
    {
        Serial.println("Connection to MQTT broker failed");
    }
}

void setup(void)
{
    /**
     * Configure Serial Port
     * */
    Serial.begin(115200);
    Serial.println("Hello!");

    Serial.println("Getting single-ended readings from AIN0..3");
    Serial.println("ADC Range: +/- 6.144V (1 bit = 3mV/ADS1015, 0.1875mV/ADS1115)");
    /**
     * Wifi Setup
     * */

    /**
     * Task for sending data somewhere
     * 
    **/
    xTaskCreatePinnedToCore(
        SendDataToMqttBrokerTask,     /* Task function */
        "SendDataToMqttBroker",       /* name of the task */
        10000,                        /* Stack size (bytes) */
        NULL,                         /*Parameters of tasks */
        1,                            /* Priority of task */
        &SendDataToMqttBrokerHandler, /* Task handler */
        ESP32_CORE_0                  /* Core where the task is going to run */
    );
    delay(500);
    /**
     *  MQTT - WIFI
     * */
    connectToMqtt();
    /**
     * Configure ADC
     * */
    ads.begin(0x48);
    ads.setDataRate(RATE_ADS1115_860SPS);

    /**
     * Configure gpios
     * */
    pinMode(IO32, OUTPUT);
    pinMode(IO33, OUTPUT);
    pinMode(LED_1, OUTPUT);
    pinMode(LED_2, OUTPUT);
}

void SendDataToMqttBrokerTask(void *pvParameters)
{
    Serial.print("Task running on CPU");
    Serial.println(xPortGetCoreID());
    for (;;)
    {
        if (status == HIGH)
        {
            status = LOW;
        }
        else if (status == LOW)
        {
            status = HIGH;
        }
        //digitalWrite(IO33, status);
        //digitalWrite(LED_1, status);
        StaticJsonDocument<512> mqttMessageDocument;
        mqttMessageDocument["sensor_id"] = SENSOR_ID;
        mqttMessageDocument["sensor_phase"] = SENSOR_PHASE;

        JsonArray current = mqttMessageDocument.createNestedArray("current");
        JsonArray voltage = mqttMessageDocument.createNestedArray("voltage");

        for (int i = 0; i < 20; i++)
        {
            voltage.add(voltageMeasurements[i]);
            current.add(currentMeasurements[i]);
        }

        String json_string;
        serializeJson(mqttMessageDocument, json_string);
        if (true) //mqttClient.publish(TOPIC, json_string.c_str()))
        {
            Serial.println("Message Sent to Server");
        }
        else
        {
            Serial.println("Fail, traing to reconnect to MQTT Broker");

            //connectToMqtt();
            //TODO : Wrong it should check and try to reconnect or skipp but not write until is connected
            //delay(10);
            //mqttClient.publish(TOPIC, json_string.c_str());
        }
        Serial.println();
        delay(1000);
    }
}

void sendMeasurementArrayToSerial(uint8_t measurementType, volatile int16_t measurementsArray[])
{
    Serial.println("Sending data");
    if (measurementType == CURRENT)
    {
        Serial.print("Current:");
    }
    else if (measurementType == VOLTAGE)
    {
        Serial.print("Voltage:");
    }

    for (int ix = 0; ix < measurementSize; ix++)
    {
        Serial.println(measurementsArray[ix]);
    }
    Serial.println("Finished");
}
// Empty because tasks are defined
void loop()
{
    // TODO:
    // Write a big array and send it when finished to somewhere to be processed

    // Sample 1024 times
    digitalWrite(IO32, HIGH);
    voltageMeasurement = ads.readADC_SingleEnded(0);
    mqttClient.publish(TOPIC, String(voltageMeasurement).c_str());
    voltageMeasurements[counter] = voltageMeasurement;
    digitalWrite(IO32, LOW);
    currentMeasurement = ads.readADC_SingleEnded(1);
    mqttClient.publish(TOPIC, String(voltageMeasurement).c_str());

    currentMeasurements[counter] = currentMeasurement;

    counter++;
    if (counter >= measurementSize)
    {
        counter = 0;
    }
}
