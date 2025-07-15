#include <Wire.h>

const int MPU_addr = 0x68;

int16_t GyX, GyY, GyZ;

void setup() {
  Wire.begin();           
  Serial.begin(9600);     
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);       
  Wire.write(0);          
  Wire.endTransmission(true);
}

void loop() {
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x43);       
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr, 6, true); 

  GyX = Wire.read() << 8 | Wire.read();  //eje X
  GyY = Wire.read() << 8 | Wire.read();  //eje Y
  GyZ = Wire.read() << 8 | Wire.read();  //eje Z

  Serial.print("GyX = "); Serial.print(GyX/131);
  Serial.print(" | GyY = "); Serial.print(GyY/131);
  Serial.print(" | GyZ = "); Serial.println(GyZ/131);

  delay(1000);
}
