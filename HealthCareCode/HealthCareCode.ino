#include <Wire.h>
#include "MAX30105.h"
#include "spo2_algorithm.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include <DHT.h>

// Configuración para el sensor MAX30105
MAX30105 particleSensor;
const byte RATE_SIZE = 4;
uint32_t irBuffer[100];
uint32_t redBuffer[100];
int32_t bufferLength;
int32_t spo2;
int8_t validSPO2;
int32_t heartRate;
int8_t validHeartRate;
byte pulseLED = 11;
byte readLED = 13;

// Configuración para el sensor DHT11
#define DHT_PIN 5
DHT dht(DHT_PIN, DHT11);

// Configuración para el sensor DS18B20
#define ONE_WIRE_BUS 4
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

void setup()
{
  Serial.begin(115200);
  pinMode(pulseLED, OUTPUT);
  pinMode(readLED, OUTPUT);

  // Inicializar el sensor MAX30105
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST))
  {
    Serial.println(F("MAX30105 was not found. Please check wiring/power."));
    while (1);
  }
  particleSensor.setup(60, 4, 2, 100, 411, 4096); // Configura el sensor

  // Inicializar el sensor DHT11
  dht.begin();

  // Inicializar el sensor DS18B20
  sensors.begin();
}

void loop()
{
  bufferLength = 100;

  // Leer los primeros 100 valores de los sensores MAX30105
  for (byte i = 0; i < bufferLength; i++)
  {
    while (particleSensor.available() == false)
      particleSensor.check();

    redBuffer[i] = particleSensor.getRed();
    irBuffer[i] = particleSensor.getIR();
    particleSensor.nextSample();

    Serial.print(F("red="));
    Serial.print(redBuffer[i], DEC);
    Serial.print(F(", ir="));
    Serial.println(irBuffer[i], DEC);
  }

  maxim_heart_rate_and_oxygen_saturation(irBuffer, bufferLength, redBuffer, &spo2, &validSPO2, &heartRate, &validHeartRate);

  // Leer datos del sensor DHT11
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  // Leer datos del sensor DS18B20
  sensors.requestTemperatures();
  float ds18b20Temperature = sensors.getTempCByIndex(0);

  // Imprimir los resultados en el Monitor Serie
  Serial.print(F("HR="));
  Serial.print(heartRate, DEC);
  Serial.print(F(", HRvalid="));
  Serial.print(validHeartRate, DEC);
  Serial.print(F(", SPO2="));
  Serial.print(spo2, DEC);
  Serial.print(F(", SPO2Valid="));
  Serial.print(validSPO2, DEC);
  Serial.print(F(", Temperature="));
  Serial.print(temperature);
  Serial.print(F(" °C, Humidity="));
  Serial.print(humidity);
  Serial.print(F("%, DS18B20 Temperature="));
  Serial.print(ds18b20Temperature);
  Serial.println(F(" °C"));

  // Aquí puedes agregar la lógica para enviar los datos a través de Wi-Fi o Bluetooth, si es necesario.

  delay(1000);
}
