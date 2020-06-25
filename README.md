BT소스를 BLE로 변환하는 중.
setValue에서 에러남

-> 에러 잡음 c++ type 문제임. 
걍 char[] 만들어 던지니 잘 됨. char나 byte, std::string만 가능 
String은 안됨.
