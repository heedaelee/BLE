  
int insole_type = 0; //left insole
//int insole_type = 1; //right insole

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

BluetoothSerial SerialBT;

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
 String jsondata = "";
    
    StaticJsonDocument <1500> doc;
    
    JsonObject Pressure = doc.createNestedObject("Pressure");
    JsonObject Cop =doc.createNestedObject("Cop");
    JsonObject Temper =doc.createNestedObject("Temper");

void setup() {
  
  for(int i=0; i<4;i++){
    pinMode(col_s[i],OUTPUT);  
  }
  
  Serial.begin(115200);
  
  if(insole_type == 0){
    SerialBT.begin("left insole"); //Bluetooth device name
  }
  
  else if (insole_type == 1){
    SerialBT.begin("right insole"); //Bluetooth device name
  }

  Serial.println("The device started, now you can pair it with bluetooth!");

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
    Serial.println(getMuxData(i));
    delay(1000);
  }
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
      // serializeJson(doc, jsondata); // JSON 노말하게 보이는것
      
  
  serializeJsonPretty(doc, Serial);
  Serial.println(" ");


  if (SerialBT.available()) {
    serializeJson(doc, SerialBT);
    Serial.println("send");
//    SerialBT.write(String(jsondata));

  

 

 

  }

  delay(1000);

}
