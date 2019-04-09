

#include <OneWire.h>

int DS18S20_Pin = 6; //DS18S20 Signal pin on digital 2

//Temperature chip i/o
OneWire ds(DS18S20_Pin);  // on digital pin 2

int pwmPin     = 3; // digital PWM pin 9
int pwmVal     = 1; // The PWM Value
unsigned long time;
unsigned int rpm;
String stringRPM;
void setup()
{
   // generate 25kHz PWM pulse rate on Pin 3
   pinMode(pwmPin, OUTPUT);   // OCR2B sets duty cycle
   // Set up Fast PWM on Pin 3
   TCCR2A = 0x23;     // COM2B1, WGM21, WGM20 
   // Set prescaler  
   TCCR2B = 0x0A;   // WGM21, Prescaler = /8
   // Set TOP and initialize duty cycle to zero(0)
   OCR2A = 79;    // TOP DO NOT CHANGE, SETS PWM PULSE RATE
   OCR2B = 0;    // duty cycle for Pin 3 (0-79) generates 1 500nS pulse even when 0 :
   digitalWrite(2, HIGH);   // Starts reading
   Serial.begin(9600);
}

float getTemp(){
  //returns the temperature from one DS18S20 in DEG Celsius

  byte data[12];
  byte addr[8];

  if ( !ds.search(addr)) {
      //no more sensors on chain, reset search
      ds.reset_search();
      return -1000;
  }

  if ( OneWire::crc8( addr, 7) != addr[7]) {
      Serial.println("CRC is not valid!");
      return -1000;
  }

  if ( addr[0] != 0x10 && addr[0] != 0x28) {
      Serial.print("Device is not recognized");
      return -1000;
  }

  ds.reset();
  ds.select(addr);
  ds.write(0x44,1); // start conversion, with parasite power on at the end

  byte present = ds.reset();
  ds.select(addr);    
  ds.write(0xBE); // Read Scratchpad

  
  for (int i = 0; i < 9; i++) { // we need 9 bytes
    data[i] = ds.read();
  }
  
  ds.reset_search();
  
  byte MSB = data[1];
  byte LSB = data[0];

  float tempRead = ((MSB << 8) | LSB); //using two's compliment
  float TemperatureSum = tempRead / 16;
  
  return TemperatureSum;
  
}

void loop()
{
 unsigned int x;
 float temperature = getTemp();
 x = 10;
 // full on at 55 deegrees
 //x = (int)(79 / 55 * temperature)  ;
 if (temperature > 25) {
    if (temperature >= 80) {
      x = 79;
    } else {
      x = 10+temperature;
      if (x<=0) {
          x = 0;
      }
      if (x>79) {
        x = 79;  
      }
    }
 }

 
 OCR2B = x;

 time = pulseIn(2, HIGH);
 rpm = (1000000 * 60) / (time * 4);
 if (rpm==65535)
    rpm = 0;

 int pwmp = (100*x/79);
 Serial.print("temp=");
 Serial.print(temperature);
 Serial.print(" PWM%=");
 Serial.print(pwmp);
 Serial.print(" RPM=");
 Serial.println(rpm);
}



char getRPMS() {
 time = pulseIn(2, HIGH);
 rpm = (1000000 * 60) / (time * 4);
 stringRPM = String(rpm);
 if (stringRPM.length() < 5) {
   Serial.println(rpm, DEC);
 }
}
