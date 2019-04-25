#include <TheThingsNetwork.h>
#include <SoftwareSerial.h>


// First install "DHT sensor library" via the Library Manager
#include <DHT.h>

// Set your devADddr, nwskey and appsKey (for ABP verification)

const char *devAddr = "26011CE7";
const char *nwkSKey = "FA00494ABE2EF1D679F52DC13763D608";
const char *appSKey = "53FEB3FAF9A169D95DDC7B7B62204A3F";

#define loraSerial Serial
#define debugSerial Serial



// Replace REPLACE_ME with TTN_FP_EU868 or TTN_FP_US915
#define freqPlan TTN_FP_EU868

#define DHTPIN 5

//Choose your DHT sensor moddel
//#define DHTTYPE DHT11
//#define DHTTYPE DHT21
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

TheThingsNetwork ttn(loraSerial, debugSerial, freqPlan);

void setup()
{
  loraSerial.begin(9600);
  debugSerial.begin(9600);

  // Wait a maximum of 10s for Serial Monitor
  while (!debugSerial && millis() < 10000)
    ;

  debugSerial.println("-- STATUS");
  ttn.showStatus();

  debugSerial.println("-- JOIN");
ttn.personalize(devAddr, nwkSKey, appSKey);

  dht.begin();
}

void loop()
{
  debugSerial.println("-- LOOP");

  // Read sensor values and multiply by 100 to effictively have 2 decimals
  uint16_t humidity = dht.readHumidity(false) * 100;

  // false: Celsius (default)
  // true: Farenheit
  uint16_t temperature = dht.readTemperature(false) * 100;

  // Split both words (16 bits) into 2 bytes of 8
  byte payload[4];
  payload[0] = highByte(temperature);
  payload[1] = lowByte(temperature);
  payload[2] = highByte(humidity);
  payload[3] = lowByte(humidity);

  debugSerial.print("Temperature: ");
  debugSerial.println(temperature);
  debugSerial.print("Humidity: ");
  debugSerial.println(humidity);

  ttn.sendBytes(payload, sizeof(payload));

  delay(20000);
}
