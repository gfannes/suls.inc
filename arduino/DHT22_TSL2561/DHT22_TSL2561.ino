#include <DHT.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_TSL2561_U.h>

//Calibration factors for the different measurements
struct SensorData
{
    //DHT22
    float humidity = 0.0f;
    float temperature = 0.0f;

    //TSL2561
    bool lux_ok = false;
    float lux = 0.0f;
};

SensorData calibration;


#define DHTPIN 5
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);
Adafruit_TSL2561_Unified tsl = Adafruit_TSL2561_Unified(TSL2561_ADDR_FLOAT, 12345);

void setup_sensors()
{
    dht.begin();

    tsl.enableAutoRange(true);            /* Auto-gain ... switches automatically between 1x and 16x */
    tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_402MS);  /* 16-bit data but slowest conversions, 402ms per measurement */

    //TODO: Add real calibration values here
    calibration.humidity = 1.0f;
    calibration.temperature = 1.0f;
    calibration.lux = 1.0f;
}

void get_sensor_data(SensorData &raw_data, const SensorData &calibration)
{
    //Get raw sensor data
    {
        //Get raw data from DHT22 sensor
        raw_data.humidity = dht.readHumidity();
        raw_data.temperature = dht.readTemperature();

        //Get raw data from TSL2561 sensor
        {
            sensors_event_t event;
            tsl.getEvent(&event);

            raw_data.lux_ok = event.light;
            if (raw_data.lux_ok)
                raw_data.lux = event.light;
        }
    }

    //Apply calibration
    {
        raw_data.humidity    *= calibration.humidity;
        raw_data.temperature *= calibration.temperature;
        raw_data.lux         *= calibration.lux;
    }
}

