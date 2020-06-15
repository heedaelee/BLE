/*
    Video: https://www.youtube.com/watch?v=oCMOYS71NIU
    Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleNotify.cpp
    Ported to Arduino ESP32 by Evandro Copercini
    updated by chegewara

   Create a BLE server that, once we receive a connection, will send periodic notifications.
   The service advertises itself as: 4fafc201-1fb5-459e-8fcc-c5c9c331914b
   And has a characteristic of: beb5483e-36e1-4688-b7f5-ea07361b26a8

   The design of creating the BLE server is:
   1. Create a BLE Server
   2. Create a BLE Service
   3. Create a BLE Characteristic on the Service
   4. Create a BLE Descriptor on the characteristic
   5. Start the service.
   6. Start advertising.

   A connect hander associated with the server starts a background task that performs notification
   every couple of seconds.
*/
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#include <iostream>
#include <string>

BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic = NULL;
bool deviceConnected = false;
bool oldDeviceConnected = false;
uint32_t value = 0;
int val = 0;

//#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
//#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

// define the UUID for our Custom Service
#define SERVICE_UUID        "8183d256-b358-4c62-a487-d2e7429bfc39"
#define CHARACTERISTIC_UUID "61661f32-bc34-4513-a43d-20c2f3970829"



//MUS configuration
int pinSIG = A4;    // 16채널 ADC 모듈 SIG 에 연결할 핀
int sensorValue = 0;   // 읽어온 ADC 값

//ADC MUX 컨트롤 핀
int pinEN = 14;
int s0 = 27;
int s1 = 26;
int s2 = 25;
int s3 = 33;


int controlPin[4] = { s0, s1, s2, s3 };//4, 3, 2, 1}; //

int muxChannel[16][4]={
    {0,0,0,0}, //channel 0
    {1,0,0,0}, //channel 1
    {0,1,0,0}, //channel 2
    {1,1,0,0}, //channel 3
    {0,0,1,0}, //channel 4
    {1,0,1,0}, //channel 5
    {0,1,1,0}, //channel 6
    {1,1,1,0}, //channel 7
    {0,0,0,1}, //channel 8
    {1,0,0,1}, //channel 9
    {0,1,0,1}, //channel 10
    {1,1,0,1}, //channel 11
    {0,0,1,1}, //channel 12
    {1,0,1,1}, //channel 13
    {0,1,1,1}, //channel 14
    {1,1,1,1}  //channel 15
  };


class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
      value = 16;
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};



void setup() {
  Serial.begin(115200);

  // Create the BLE Device
  // BLE 사용할 이름으로 초기화
  BLEDevice::init("L");

  // Create the BLE Server
  // BLEServer 인스턴스를 얻음. pServer로.
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());
  //콜백함수 등록해주면, BLE장치가 연결/해제 되었을 때, 콜백 호출해줌

  // Create the BLE Service
  // Service/Characteristic 구조를 만들어줌
  BLEService *pService = pServer->createService(SERVICE_UUID);
  // Create a BLE Characteristic
  //define our custom characteristic along with it's properties
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ   |
                      BLECharacteristic::PROPERTY_WRITE  |
                      BLECharacteristic::PROPERTY_NOTIFY |
                      BLECharacteristic::PROPERTY_INDICATE
                    );

  // https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.descriptor.gatt.client_characteristic_configuration.xml
  // Create a BLE Descriptor
  pCharacteristic->addDescriptor(new BLE2902());

  // Start the service
  pService->start();
  //참조 : http://www.hardcopyworld.com/ngine/aduino/index.php/archives/3226

  // Start advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(false);
  pAdvertising->setMinPreferred(0x0);  // set value to 0x00 to not advertise this parameter
  BLEDevice::startAdvertising();
  Serial.println("Waiting a client connection to notify...");

  pinMode(pinEN, OUTPUT); digitalWrite(pinEN,LOW);
  pinMode(s0, OUTPUT); 
  pinMode(s1, OUTPUT); 
  pinMode(s2, OUTPUT); 
  pinMode(s3, OUTPUT); 
}

void loop() {
    // notify changed value
    if (deviceConnected) {
       
        char reVal[47];
        for(int i=0;i<47;i++){ reVal[i]=0;}
        sprintf(reVal, "Left: %d, %d, %d, %d, %d, %d, %d", readADCMux(0), readADCMux(1), readADCMux(2), readADCMux(3), readADCMux(4), readADCMux(5), readADCMux(6));
        //Serial.println(strlen(reVal));
        //pCharacteristic->setValue((uint8_t*)&value, 4);
        pCharacteristic->setValue(reVal); // This is a value of a single byte
        pCharacteristic->notify(); //Notify 연결된 BLE장치에게..
        
        //Serial.println(value, HEX);
        Serial.println(reVal);
                
        delay(500); // bluetooth stack will go into congestion, if too many packets are sent, in 6 hours test i was able to go as low as 3ms
    }
    // disconnecting
    if (!deviceConnected && oldDeviceConnected) {
        delay(500); // give the bluetooth stack the chance to get things ready
        //pServer->startAdvertising(); // restart advertising
        Serial.println("Disconnecting");//start advertising");
        oldDeviceConnected = deviceConnected;
    }
    // connecting
    if (deviceConnected && !oldDeviceConnected) {
        // do stuff here on connecting
        oldDeviceConnected = deviceConnected;
        Serial.println("Cconnecting");//
    }
}

int readADCMux(uint32_t _ch)
{
  // 4개 컨트롤핀 (s0,s1,s2,s3) 핀 설정
  for(int i = 0; i < 4; i ++){
    digitalWrite(controlPin[i], muxChannel[_ch][i]);
  }
  return analogRead(pinSIG);
}
