
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
#include "config.h"
#include "wifi_connect.h"
//#include <MQTTClient.h>

Adafruit_ADS1015 ads;

TaskHandle_t SendDataToMqttBrokerHandler;

uint8_t CURRENT = 0;
uint8_t VOLTAGE = 1;
int LED_1 = 2;
int LED_2 = 4;

const uint8_t IO32 = 32;
const uint8_t IO33 = 33;
bool led_value = HIGH;

// ARray which contains all the measurements
const uint8_t measurementSize = 4096;

volatile bool sampleFinished;

volatile int16_t voltageMeasurement = 0;
volatile int16_t currentMeasurement = 0;

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
        10000,                        /* Stack size (Bytes) */
        NULL,                         /* Parameters of tasks */
        1,                            /* Priority of the task */
        &SendDataToMqttBrokerHandler, /* Task handler */
        ESP32_CORE_0                  /* Core where the task is going to run */
    );

    delay(500);

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

//Function which executes the task
//
void SendDataToMqttBrokerTask(void *pvParameters)
{
    Serial.print("Task running on CPU ");
    Serial.println(xPortGetCoreID());
    for (;;)
    {
        digitalWrite(IO33, HIGH);
        digitalWrite(LED_1, HIGH);
        digitalWrite(IO33, LOW);
        digitalWrite(LED_1, LOW);
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

    for (int index = 0; index < measurementSize; index++)
    {
        Serial.println(measurementsArray[index]);
        delay(5);
    }
    Serial.println("Finished");
}
// Empty because tasks are defined
void loop()
{
    // TODO:
    // Write a big array and send it when finished to somewhere to be processed
    Serial.print("Task running on CPU [LOOP]");
    Serial.println(xPortGetCoreID());
    int16_t currentMeasurements[measurementSize];

    int16_t voltageMeasurements[measurementSize];
    // Sample 1024 times
    for (int index = 0; index < measurementSize; index++)
    {
        voltageMeasurements[index] = ads.readADC_SingleEnded(0);
        currentMeasurements[index] = ads.readADC_SingleEnded(1);
        Serial.println(voltageMeasurements[index]);
        Serial.println(currentMeasurements[index]);
    }
    // Send the array via serial
    //sendMeasurementArrayToSerial(CURRENT, currentMeasurements);
    //sendMeasurementArrayToSerial(VOLTAGE, voltageMeasurements);
}
