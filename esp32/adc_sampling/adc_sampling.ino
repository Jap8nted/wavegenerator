
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