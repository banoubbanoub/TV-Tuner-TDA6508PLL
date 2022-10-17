#include <Arduino.h>
#include<TDA6508PLL.h>
#include <IRremote.h>
#include<SN761668PLL.h>
#include<VFDI2CMaster.h>
#include<ds3231.h>
VFDI2CMaster vfd(0b1111);
struct ts t;
int RECV_PIN = 4;  // IR Receiver pin
IRrecv irrecv(RECV_PIN);
decode_results results;
#define clk 30
#define dta 31
#define cnt1 8
#define cnt2 9
uint16_t CBFraquency = 9800; // CB radio start fraquncy scan
int FM915Frequency = 17370; // 915 MHZ start fraquncy scan  17370
uint16_t TVFrequency = 2096; // tv start fraquncy scan
uint16_t FMFrequency = 2737; // fm radio start fraquncy scan
byte snpllFraquency =40; //SN761668 digital pll
int cureentclock =0;
int previceClock =0;
int FrequencyVariable=0;
uint8_t TVSelect=1;
uint8_t osci=1;
uint8_t freq=1;
byte hour=0;
byte min=0;
byte second=0;
uint8_t vol=0;
void AudioSelector(uint8_t source){
  switch (source)
  {
  case 1:
    digitalWrite(cnt1,HIGH);
    digitalWrite(cnt2,LOW);
    break;
    case 2:
    digitalWrite(cnt1,HIGH);
    digitalWrite(cnt2,HIGH);
    break;
  }
}
void TVSwitch(uint8_t source,uint8_t oscil){
  switch (source)
  {
    case 1:
    SNTVTuner(FrequencyVariable,oscil);
    break;
     case 2:
    TDATVtuner(FrequencyVariable,oscil);
    break;
  }
}
//******************
void TimeClock(){
   Wire.beginTransmission(0x70);  // TCA9548A address
  Wire.write(1 << 3);          // send byte to select bus
  Wire.endTransmission();
     DS3231_init(DS3231_CONTROL_INTCN);
  /*----------------------------------------------------------------------------
  In order to synchronise your clock module, insert timetable values below !
  ----------------------------------------------------------------------------*/
  t.hour=11; 
  t.min=15;
  t.sec=20;
  t.mday=28;
  t.mon=9;
  t.year=2022;
 
  DS3231_set(t); 
}
//************************
void UpdatDisplay(){
 Wire.beginTransmission(0x70);  // TCA9548A address
  Wire.write(1 << 2);          // send byte to select bus
  Wire.endTransmission();

  if(TVSelect == 1){
    vfd.setCursor(0,1);
    vfd.Display("TV:");
      vfd.setCursor(19,2);
  vfd.DisplayIconGrid_20("OUT");
    vfd.setCursor(19,2);
    vfd.DisplayIconGrid_20("squar1");
 }
 if(TVSelect ==2){
   vfd.setCursor(0,1);
   vfd.Display("FM:");
     vfd.setCursor(19,2);
  vfd.DisplayIconGrid_20("OUT");
   vfd.setCursor(19,2);
   vfd.DisplayIconGrid_20("squar2");
 }
 vfd.setCursor(3,1);
 vfd.Display(FrequencyVariable);
 vfd.setCursor(13,1);
 vfd.Display(osci);
 vfd.setCursor(9,1);
 vfd.Display("osci");
 vfd.setCursor(15,2);
 vfd.DisplayIconGrid_16("wifi");
 vfd.setCursor(15,2);
 vfd.DisplayIconGrid_16("tuned");
 vfd.setCursor(14,2);
 vfd.DisplayIconGrid_15("vol");
 vfd.setCursor(14,2);
 vfd.DisplayNumberGrid_15(vol);
 vfd.setCursor(14,2);
 vfd.DisplayIconGrid_15("db");
 vfd.setCursor(19,2);
 vfd.DisplayNumber_Row_2(t.hour);
 vfd.setCursor(18,2);
 vfd.DisplayNumber_Row_2(t.min);
 vfd.setCursor(17,2);
 vfd.DisplayNumber_Row_2(second);
 vfd.setCursor(14,2);
vfd.DisplayIconGrid_15("-");

 vfd.setCursor(16,2);
vfd.DisplayAlphaBet_Row_2("pm");
vfd.setCursor(8,1);
vfd.Display("&");  

 if(vfd._track >=17)vfd._track=0;
}

void setup() {
  Wire.begin();
  irrecv.enableIRIn();
 TimeClock();
  pinMode(clk,INPUT);
  pinMode(dta,INPUT);
   pinMode(cnt1,OUTPUT);
 pinMode(cnt2,OUTPUT);
previceClock = digitalRead(clk);
FrequencyVariable = TVFrequency;
osci =1;
TVSelect=1;
TVSwitch(TVSelect,osci);
AudioSelector(2);
UpdatDisplay();
delay(200);
}

void loop() {
  // put your main code here, to run repeatedly:

cureentclock = digitalRead(clk);
if(cureentclock != previceClock){
  if(cureentclock != digitalRead(dta)){
   FrequencyVariable--;

  TVSwitch(TVSelect,osci);
  }
  else{
   FrequencyVariable++;
 TVSwitch(TVSelect,osci);
  }
   UpdatDisplay();
}
previceClock = cureentclock;
    if(irrecv.decode(&results)){
      switch (results.value)
      {
        case 3772833823:
        vol++;
        if(vol>=99)vol=99;
        break;
        case 3772829743:
        vol--;
        if(vol<=0)vol=0;
        break;
        case 3772790473 : // red button for tv number 1
        TVSelect =1;
        TVSwitch(TVSelect,osci);
        AudioSelector(2);
        break;
        case 3772786903: // green button for tv number 2
        TVSelect =2;
        TVSwitch(TVSelect,osci);
        AudioSelector(1);
        break;
        case 3772778233: // up button for oscillator switch
        osci++;
        if(osci>=6)osci=6;
        TVSwitch(TVSelect,osci);
        break;
        case 3772810873: // down button for oscillator switch
        osci--;
        if(osci<=0)osci=1;
        TVSwitch(TVSelect,osci);
        break;
        case 3772794553: // right button for frq select
           freq++;
        if(freq>=5)freq=5;
        if(freq ==1) FrequencyVariable =TVFrequency;
        if(freq ==2) FrequencyVariable = FMFrequency;
        if(freq == 3) FrequencyVariable = FM915Frequency; 
        if(freq ==4) FrequencyVariable = snpllFraquency;
        if(freq == 5) FrequencyVariable = CBFraquency;
        break;
        case 3772819033: //left button for frq select
        freq--;
        if(freq<=0)freq=1;
        if(freq ==1) FrequencyVariable =TVFrequency;
        if(freq ==2) FrequencyVariable = FMFrequency;
        if(freq == 3) FrequencyVariable = FM915Frequency; 
        if(freq ==4) FrequencyVariable = snpllFraquency;
        if(freq == 5) FrequencyVariable = CBFraquency;
        break;
        case 3772795063:  //channel up
        FrequencyVariable++;
        TVSwitch(TVSelect,osci);
        break;
        case 3772778743: // channel down
        FrequencyVariable--;
       
        TVSwitch(TVSelect,osci);
        break;
      }
    UpdatDisplay();
    }
     irrecv.resume();
   //TimeClock();
 //DS3231_get(&t);
   
// delay(1000);
 // UpdatDisplay();
   
}