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

Adafruit_ADS1015 ads;

TaskHandle_t SampleDataFromADCTaskHandler;
TaskHandle_t SendDataToMqttBrokerHandler;

int LED_1 = 2;
int LED_2 = 4;

const uint8_t IO32 = 32;
const uint8_t IO33 = 33;
bool led_value;

// ARray which contains all the measurements
volatile uint16_t currentMeasurements[1000];
volatile uint16_t voltageMeasurements[1000];
volatile bool sampleFinished;

volatile uint16_t voltageMeasurement = 0x0;
volatile uint16_t currentMeasurement = 0x0;

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
     * Task for sending data somewhere
     * 
    **/
    xTaskCreatePinnedToCore(
        SendDataToMqttBrokerTask,     /* Task function */
        "SendDataToMqttBroker",       /* name of the task */
        1000,                         /* Stack size (Bytes) */
        NULL,                         /* Parameters of tasks */
        1,                            /* Priority of the task */
        &SendDataToMqttBrokerHandler, /* Task handler */
        ESP32_CORE_0                  /* Core where the task is going to run */
    );

    delay(500);

    /**
     * Task which samples the data from the ADC
     * */

    xTaskCreatePinnedToCore(
        SampleDataFromADCTask,         /* Task function */
        "SampleDataFromADC",           /* name of the task */
        1000,                          /* Stack size (Bytes) */
        NULL,                          /* Parameters of tasks */
        1,                             /* Priority of the task */
        &SampleDataFromADCTaskHandler, /* Task handler */
        ESP32_CORE_1                   /* Core where the task is going to run */
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
        delay(1000);
        digitalWrite(IO33, LOW);
        digitalWrite(LED_1, LOW);
        delay(1000);
    }
}

//Function which executes the task
//
void SampleDataFromADCTask(void *pvParameters)
{
    Serial.print("Task running on CPU ");
    Serial.println(xPortGetCoreID());
    for (;;)
    {
        digitalWrite(IO32, HIGH);
        digitalWrite(LED_2, LOW);
        delay(700);
        digitalWrite(IO32, LOW);
        digitalWrite(LED_2, LOW);
        delay(700);
    }
}
// Empty because tasks are defined
void loop()
{
}