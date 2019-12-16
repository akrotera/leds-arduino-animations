#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <FastLED.h>

#define UNDERGLOW_SERVICE_UUID      "8a32ae00-62dc-4672-aaf0-48a47099af60"
#define UNDERGLOW_CHARC_STATUS_UUID "8a32ae01-62dc-4672-aaf0-48a47099af60"
#define UNDERGLOW_CHARC_MODE_UUID   "8a32ae02-62dc-4672-aaf0-48a47099af60"
#define AMBIENT_SERVICE_UUID        "8a32ae10-62dc-4672-aaf0-48a47099af60"
#define AMBIENT_CHARC_STATUS_UUID   "8a32ae11-62dc-4672-aaf0-48a47099af60"

#define UNDERGLOW_DATA_PIN 22
#define NUM_LEDS 150
#define AMBIENT_LED_PIN 23

CRGB leds[NUM_LEDS];

uint8_t underglow_status = 0;
uint8_t animation = 0;

class UnderglowCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic)
    {
      std::string uuid = pCharacteristic->getUUID().toString();
      std::string value = pCharacteristic->getValue();

      if (uuid == UNDERGLOW_CHARC_STATUS_UUID) {
        if (value == "1") {
          Serial.println("Turning ON the underglow");
          digitalWrite(UNDERGLOW_DATA_PIN, HIGH);
          underglow_status = 1;
        }

        if (value == "0") {
          Serial.println("Turning OFF the underglow");
          FastLED.clear();
          digitalWrite(UNDERGLOW_DATA_PIN, LOW);
          underglow_status = 0;
        }
      }

      if (uuid == UNDERGLOW_CHARC_MODE_UUID) {
        //Posible pasar a enum?
        if (value == "0") {
          Serial.println("Underglow anim default");
          animation = 0;
        }
        if (value == "1") {
          Serial.println("Underglow anim 1");
          animation = 1;
        }
        if (value == "2") {
          Serial.println("Underglow anim 2");
          animation = 2;
        }

      }

    }
};

class AmbientCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string value = pCharacteristic->getValue();

      if (value == "1") {
        Serial.println("Turning ON the ambient light");
        digitalWrite(AMBIENT_LED_PIN, HIGH);
      }

      if (value == "0") {
        Serial.println("Turning OFF the ambient light");
        digitalWrite(AMBIENT_LED_PIN, LOW);
      }
    }
};

void setup() {
  Serial.begin(115200);
  pinMode(UNDERGLOW_DATA_PIN, OUTPUT);
  pinMode(AMBIENT_LED_PIN, OUTPUT);

  BLEDevice::init("LeslieLights");
  BLEServer *pServer = BLEDevice::createServer();

  BLEService *pServiceUnderglow = pServer->createService(UNDERGLOW_SERVICE_UUID);

  BLECharacteristic *pCharcUnderStatus = pServiceUnderglow->createCharacteristic(
      UNDERGLOW_CHARC_STATUS_UUID,
      BLECharacteristic::PROPERTY_READ |
      BLECharacteristic::PROPERTY_WRITE);

  BLECharacteristic *pCharcUnderMode = pServiceUnderglow->createCharacteristic(
                                         UNDERGLOW_CHARC_MODE_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE);

  pCharcUnderStatus->setCallbacks(new UnderglowCallbacks());
  pCharcUnderMode->setCallbacks(new UnderglowCallbacks());

  pCharcUnderStatus->setValue("0");
  pCharcUnderMode->setValue("0");

  pServiceUnderglow->start();

  BLEService *pServiceAmbient = pServer->createService(AMBIENT_SERVICE_UUID);

  BLECharacteristic *pCharcAmbientStatus = pServiceAmbient->createCharacteristic(
        AMBIENT_CHARC_STATUS_UUID,
        BLECharacteristic::PROPERTY_READ |
        BLECharacteristic::PROPERTY_WRITE);

  pCharcAmbientStatus->setCallbacks(new AmbientCallbacks());
  pCharcAmbientStatus->setValue("0");

  pServiceAmbient->start();

  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  BLEDevice::startAdvertising();

  FastLED.addLeds<WS2812B, UNDERGLOW_DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.clear();
}

void loop() {
  if (underglow_status == 1) {
    switch (animation) {
      case 0:
        Serial.println("0 - Demo");
        ida();
        break;
      case 1:
        Serial.println("1 - Fijo");
        poneMedio();
        break;
    }
  }
  delay(1000);
}

void poneMedio() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB(0, 42, 0);
  }
  FastLED.show();
}

void ida() {
  poneMedio();
  for (int i = 0; i < NUM_LEDS; i++) {
    poneMedio();
    leds[i] = CRGB(0, 100, 0);
    FastLED.show();
    delay(42);
  }
}
