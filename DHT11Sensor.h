// Library:
//      *HumiditySensor.hpp: uses ada unified dht

#ifndef DHT11_SENSOR_H
#define DHT11_SENSOR_H

#include "Arduino.h"
#include <MySensors.h>

#define CHILD_ID_HUM 1
#define CHILD_ID_TEMP 2

MyMessage msgHum(CHILD_ID_HUM, V_HUM);
MyMessage msgTemp(CHILD_ID_TEMP, V_TEMP);

#include <DHT.h>
#include <DHT_U.h>

class DHT11Sensor {
  private:
    uint8_t _pin;
    DHT_Unified* dht;
    uint32_t minimalDelay;
    float lastTemp;
    float lastHum;

  public:
    DHT11Sensor(DHT_Unified & _dht)
    {
      dht = &_dht;
    }
    void begin()
    {
      Serial.println("Seting up Temperature/Humidity Sensor");
      dht->begin();

      sensor_t sensor;
      dht->temperature().getSensor(&sensor);

      Serial.println("------------------------------------");
      Serial.println("Temperature");
      Serial.println("------------------------------------");
      Serial.print  ("Sensor:       "); Serial.println(sensor.name);
      Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
      Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
      Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println(" *C");
      Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println(" *C");
      Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println(" *C");

      // Print humidity sensor details.
      dht->humidity().getSensor(&sensor);
      Serial.println("------------------------------------");
      Serial.println("Humidity");
      Serial.println("------------------------------------");
      Serial.print  ("Sensor:       "); Serial.println(sensor.name);
      Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
      Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
      Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println("%");
      Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println("%");
      Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println("%");

      // Set delay between sensor readings based on sensor details.
      minimalDelay = sensor.min_delay / 1000;
      Serial.print  ("Update Delay:   "); Serial.print(minimalDelay); Serial.println(" ms");
    }

    void sync()
    {
        // Delay between measurements.
        delay(minimalDelay); // //delay the minimum time needed by the DHT

        // Get temperature event and print its value.
        sensors_event_t event;
        dht->temperature().getEvent(&event);

        if (isnan(event.temperature)) {
          Serial.println("Error reading temperature!");
        }
        else if (event.temperature != lastTemp) {
          lastTemp = event.temperature;
          Serial.print("Temperature: ");
          Serial.print(event.temperature);
          Serial.println(" *C");

          send(msgTemp.set(event.temperature, 1), true);
        }

        // Get humidity event and print its value.
        dht->humidity().getEvent(&event);
        if (isnan(event.relative_humidity)) {
          Serial.println("Error reading humidity!");
        }
        else if (event.relative_humidity != lastHum) {
          lastHum = event.relative_humidity;
          Serial.print("Humidity: ");
          Serial.print(event.relative_humidity);
          Serial.println("%");

          send(msgHum.set(event.relative_humidity, 1), true);
        }
     }

     void present(uint8_t humidity = CHILD_ID_HUM, uint8_t temperature = CHILD_ID_TEMP)
     {
        // Register all sensors to gw (they will be created as child devices)
        ::present(humidity, S_HUM);
        ::present(temperature, S_TEMP);
     }
};

#endif
