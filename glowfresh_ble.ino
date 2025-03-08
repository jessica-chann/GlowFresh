#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <Preferences.h> 

#define SERVICE_UUID        "12345678-1234-5678-1234-56789abcdef0"
#define CHARACTERISTIC_UUID "abcdef01-1234-5678-1234-56789abcdef0"

BLECharacteristic *pCharacteristic = NULL;
Preferences preferences;  // Storage for the device name

class MyCallbacks : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
        String value = pCharacteristic->getValue().c_str();  
        if (value.length() > 0) {
            Serial.print("Received new name: ");
            Serial.println(value.c_str());

            // Save new name 
            preferences.begin("glowfresh", false);
            preferences.putString("device_name", value.c_str());
            preferences.end();

            // Update characteristic with confirmation
            pCharacteristic->setValue("Name set to: " + value);  // Update with String
            Serial.println("Device name updated!");

            // Stop BLE since setup is complete
            delay(2000);
            BLEDevice::deinit();
            Serial.println("BLE disabled. Switching to Wi-Fi...");
        }
    }
};

void setup() {
    Serial.begin(115200);

    // Retrieve stored name
    preferences.begin("glowfresh", true);
    String storedName = preferences.getString("device_name", "Unnamed GlowFresh");
    preferences.end();

    Serial.println("Stored device name: " + storedName);

    // Initialize BLE
    BLEDevice::init("GlowFresh_BLE");
    BLEServer *pServer = BLEDevice::createServer();
    BLEService *pService = pServer->createService(SERVICE_UUID);

    pCharacteristic = pService->createCharacteristic(
                        CHARACTERISTIC_UUID,
                        BLECharacteristic::PROPERTY_READ |  
                        BLECharacteristic::PROPERTY_WRITE
                      );

    pCharacteristic->setValue(storedName); // Send stored name
    pCharacteristic->setCallbacks(new MyCallbacks()); // Handle write requests

    pService->start();

    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pServer->getAdvertising()->start();

    Serial.println("BLE Ready! Advertised as GlowFresh_BLE");
}

void loop() {
    delay(100);
}
