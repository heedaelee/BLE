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

// define the UUID for our Custom Service
#define SERVICE_UUID        "889f06ce-b1d3-11ea-b3de-0242ac130004"
#define CHARACTERISTIC_UUID "95c0f4f2-b1d3-11ea-b3de-0242ac130004"

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};


// 여기서 부터 원소스

//int insole_type = 0; //left insole
int insole_type = 1; //right insole

#include "BluetoothSerial.h"
#include <ArduinoJson.h>

int col_s[4] = {4,0,2,15};

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)

#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it

#endif

 
void Select(int ch_num){
  //activate channel function
  // parameter "ch_num" would be connected to MUX signal pin(='SIG')
  for (int i = 0; i < 4; i++) {
    digitalWrite(col_s[i], bitRead(ch_num, i));
  }
  delay(1000);
}

int getMuxData(int ch){
  //input analog 
  //
  Select(ch);
  delay(10);
  return analogRead(34);
}

//BluetoothSerial SerialBT;

float test[9];
float P[9];
float C[2];
float T[5];


//덤프 데이터 생성
void dump(){
  for(int i=0; i<9;i++  ){
    if(i<2){
      C[i]=i;
    }
    if(i<5){
      T[i]=i;
    }
    P[i]=i;
  }
}
// std::string jsondata = "";

    StaticJsonDocument <1500> doc;
    
    JsonObject Pressure = doc.createNestedObject("Pressure");
    JsonObject Cop = doc.createNestedObject("Cop");
    JsonObject Temper =doc.createNestedObject("Temper");

void setup() {
  
  for(int i=0; i<4;i++){
    pinMode(col_s[i],OUTPUT);  
  }
  
  Serial.begin(115200);

  // Create the BLE Device
  // BLE 사용할 이름으로 초기화
  BLEDevice::init("R");

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
  
  dump(); 
}

boolean bToogle = 0;

void loop() {
  if(insole_type== 0){
    doc["type"] ="left";
  }
  else if(insole_type== 1){
    doc["type"] ="right";
  }
        //  값 삽입

  for(int i =0 ;i<14; i++){
//    Serial.println(getMuxData(i));
    delay(10);
  }

  //블루투스로 보낼 total dataSet variable
  char test[660]; 

  //test
  if(insole_type == 0){
      Pressure["PL0"] = 4096;
      Pressure["PL1"] = 4096;
      Pressure["PL2"] = 4096;
      Pressure["PL3"] = 4096;
      Pressure["PL4"] = 4096;
      Pressure["PL5"] = 4096;
      Pressure["PL6"] = 4096;
      Pressure["PL7"] = 4096;
      Pressure["PL8"] = 4096;
      Temper["TL0"] = 40;
      Temper["TL1"] = 40;
      Temper["TL2"] = 40;
      Temper["TL3"] = 40;
      Temper["TL4"] = 40;
  }
  if(insole_type == 1){
      Pressure["PR0"] = 4096;
      Pressure["PR1"] = 4096;
      Pressure["PR2"] = 4096;
      Pressure["PR3"] = 4096;
      Pressure["PR4"] = 4096;
      Pressure["PR5"] = 4096;
      Pressure["PR6"] = 4096;
      Pressure["PR7"] = 4096;
      Pressure["PR8"] = 4096;
      Temper["TR0"] = 40;
      Temper["TR1"] = 40;
      Temper["TR2"] = 40;
      Temper["TR3"] = 40;
      Temper["TR4"] = 40;
  }
  Cop["x"] = 125;
  Cop["y"] = 125;
/*
  if(insole_type == 0){
      int a = getMuxData(16-4);
      Serial.println(a);
      Pressure["PL1"] = getMuxData(16-4);
      Pressure["PL2"] = getMuxData(16-5);
      Pressure["PL3"] = getMuxData(16-7);
      Pressure["PL4"] = getMuxData(16-9);
      Pressure["PL5"] = getMuxData(16-10);
      Pressure["PL6"] = getMuxData(16-12);
      Pressure["PL7"] = getMuxData(16-13);
      Pressure["PL8"] = getMuxData(16-14);
      Pressure["PL9"] = getMuxData(16-16);
      Temper["TL1"] = getMuxData(16-3);
      Temper["TL2"] = getMuxData(16-6);
      Temper["TL3"] = getMuxData(16-8);
      Temper["TL4"] = getMuxData(16-11);
      Temper["TL5"] = getMuxData(16-15);
  }
  
  if(insole_type == 1){
      Pressure["PR1"] = getMuxData(16-4);
      Pressure["PR2"] = getMuxData(16-5);
      Pressure["PR3"] = getMuxData(16-7);
      Pressure["PR4"] = getMuxData(16-9);
      Pressure["PR5"] = getMuxData(16-10);
      Pressure["PR6"] = getMuxData(16-12);
      Pressure["PR7"] = getMuxData(16-13);
      Pressure["PR8"] = getMuxData(16-14);
      Pressure["PR9"] = getMuxData(16-16);
      Temper["TR1"] = getMuxData(16-3);
      Temper["TR2"] = getMuxData(16-6);
      Temper["TR3"] = getMuxData(16-8);
      Temper["TR4"] = getMuxData(16-11);
      Temper["TR5"] = getMuxData(16-15);
  }
  */
      //serializeJsonPretty(doc, jsondata); //JSON 이쁘게 보이는거
      //serializeJsonPretty(만들었던 jsonDocument, json넣을String 변수)
      // serializeJson(doc, jsondata); // JSON 노말하게 보이는것


  serializeJsonPretty(doc, Serial);
//  Serial.println(" ");

  // notify changed value
    if (deviceConnected) {
        //here char[]로 통신한 것, 변수 test
        serializeJson(doc, test); 
        Serial.println("test:"); 
        Serial.println(test);
        pCharacteristic->setValue(test); // This is a value of a single byte
        
        /* std::string으로 통신 하려면, 변수 jsondata
        serializeJson(doc, jsondata); 
        Serial.println("test:"); 
        Serial.println(jsondata.c_str());
        pCharacteristic->setValue(jsondata); // This is a value of a single byte
        */
        pCharacteristic->notify();
        //Serial.println(value, HEX); 
        // delay(500); // bluetooth stack will go into congestion, if too many packets are sent, in 6 hours test i was able to go as low as 3ms
    }
  // disconnecting
    if (!deviceConnected && oldDeviceConnected) {
        // delay(500); // give the bluetooth stack the chance to get things ready
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

/*기존
  if (SerialBT.available()) {
    serializeJson(doc, SerialBT);
    Serial.println("send");
//    SerialBT.write(String(jsondata));

  }
*/
  delay(1000);

}
