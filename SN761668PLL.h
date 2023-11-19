#include <Arduino.h>
#include<Wire.h>
const byte I2C_multiplexerAddressS = 0x70;
const byte SNTV_TURER_ADDRESS = 0x60;


byte osc =0x01; 
int CBFraquency = 4500; // CB radio start fraquncy scan
int FM915Frequency = 17370; // 915 MHZ start fraquncy scan  17370
int TVFrequency = 2030; // tv start fraquncy scan
int FMFrequency = 2708; // fm radio start fraquncy scan
int AirPortFrequency =3385;
int frSelect =0;
int fraquncy[5] = {TVFrequency,FMFrequency,AirPortFrequency,FM915Frequency,CBFraquency,};
void SNTVTuner(int data ,byte osci){
  Wire.beginTransmission(I2C_multiplexerAddressS);
  Wire.write(1 << 0);
  Wire.endTransmission();

  int fpd =  data + 107;
    
  Wire.beginTransmission(SNTV_TURER_ADDRESS);
  Wire.write(fpd >> 8);
  Wire.write(fpd & 0xFF);
  Wire.write(0xC0);
  Wire.write(osci == 1 ? 0x01 : osci == 2 ? 0x02 : osci == 3 ? 0x04 : 0x01);
  Wire.endTransmission();
}