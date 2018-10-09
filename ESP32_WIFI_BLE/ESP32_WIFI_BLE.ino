#include <Arduino.h>
#include <BLEDevice.h>
#include "BluetoothSerial.h"
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>
#include <BLEAdvertising.h>

BluetoothSerial SerialBT;

bool initBtSerial();
void initBLEserver();

void setup() {
  // Initialize BT serial interface
  initBtSerial();

  // Initialize BLE server
  initBLEserver();
}

void loop() {
}

bool initBtSerial() {
  if (!SerialBT.begin("ESP32 Weather")) {
    Serial.println(" Failed to start BTSerial");
    return false;
  }
  return true;
}

// List of Service and Characteristic UUIDs
#define SERVICE_UUID        0x181A
//
//... more UUIDs
//

/** Characteristic for client notification */
BLECharacteristic *pCharacteristicNotify;
//
// ... more characteristics
//

/** BLE Advertiser */
BLEAdvertising* pAdvertising;
/** BLE Service */
BLEService *pService;
/** BLE Server */
BLEServer *pServer;

/** Flag if a client is connected */
bool bleConnected = false;

/** Digital output value received from the client */
uint8_t digitalOut = 0;
/** Flag for change in digital output value */
bool digOutChanged = false;

/**
 * MyServerCallbacks
 * Callbacks for client connection and disconnection
 */
class MyServerCallbacks: public BLEServerCallbacks {
  // TODO this doesn't take into account several clients being connected
    void onConnect(BLEServer* pServer) {
      bleConnected = true;
      pAdvertising->start();
    };

    void onDisconnect(BLEServer* pServer) {
      if (pServer->getConnectedCount() == 0) {
        bleConnected = false;
      }
    }
};

/**
 * MyCallbackHandler
 * Callbacks for client write requests
 */
class MyCallbackHandler: public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
//
// ... Handle the incoming value
//
    }
  
};

void initBLEserver() {
  // Initialize BLE
  //BLEDevice::init(apName);
  // BLEDevice::setPower(ESP_PWR_LVL_P7);

  // Create BLE Server
  pServer = BLEDevice::createServer();

  // Set server callbacks
  pServer->setCallbacks(new MyServerCallbacks());

  // Create BLE Service
  pService = pServer->createService(BLEUUID((uint16_t)SERVICE_UUID),20);
//
// ... initializing the services and characteristics
//
  // Start the service
  pService->start();

  // Start advertising
  pAdvertising = pServer->getAdvertising();

  pAdvertising->start();

  Serial.println(" BLE active now");
}
