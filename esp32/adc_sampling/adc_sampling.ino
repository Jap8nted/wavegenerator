
#include <Adafruit_ADS1X15.h>

Adafruit_ADS1015 ads;

int LED_BUILTIN = 2;
const uint8_t IO32 = 32;
const uint8_t IO33 = 33;
bool led_value;

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
        10000,                        /* Stack size (Bytes) */
        NULL,                         /* Parameters of tasks */
        1,                            /* Priority of the task */
        &SendDataToMqttBrokerHandler, /* Task handler */
        0                             /* Core where the task is going to run */
    );

    delay(500);

    /**
     * Task which samples the data from the ADC
     * */

    xTaskCreatePinnedToCore(
        SampleDataFromADCTask,         /* Task function */
        "SampleDataFromADC",           /* name of the task */
        10000,                         /* Stack size (Bytes) */
        NULL,                          /* Parameters of tasks */
        1,                             /* Priority of the task */
        &SampleDataFromADCTaskHandler, /* Task handler */
        1                              /* Core where the task is going to run */
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
}

//Function which executes the task
//
void SendDataToMqttBrokerTask(void *pvParameters)
{
    Serial.print("Task running on CPU ");
    Serial.print(xPortGetCoreID());
    for (;;)
    {
        digitalWrite(IO32, HIGH);
        delay(1000);
        digitalWrite(IO32, LOW);
        delay(1000);
    }
}

void loop(void)
{
    int16_t adc0;
    float volts0;

    digitalWrite(IO32, led_value);

    adc0 = ads.readADC_SingleEnded(0);
    volts0 = (adc0 * 0.1875) / 1000;
    if (led_value == HIGH)
    {
        led_value = LOW;
    }
    else
    {
        led_value = HIGH;
    }

    adc1 = ads.readADC_SingleEnded(1);
    volts1 = (adc1 * 0.1875) / 1000;
    if (led_value == HIGH)
    {
        led_value = LOW;
    }
    else
    {
        led_value = HIGH;
    }
}