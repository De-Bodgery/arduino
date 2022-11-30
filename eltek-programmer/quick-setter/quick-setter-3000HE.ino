/* 

Quick setter:
	flatpak2 eltek 48v/3000w HE front end model 480875, 43.5-56vdc 10-60amp

HOW TO:
  Not always needed, but connect 5v from psu 5v+ pin to arduino 5v+ pin
  Connect psu dc out gnd to arduino gnd and psu short pin
  Psu sda and scl pins to arduino i2c pins
  Psu i2c ground pin to arduino gnd pin, I needed to do this on mine
 
 
 */
 
#include <Wire.h>

uint16_t voltage = 4800;                // default voltage output in centivolts, i.e. 48.00v == 4800
// uint16_t amp = 6000;                    // default amps max output in centivolts
// uint16_t vsd = 5200;                    // default max voltage shutdown in centivolts

void setup() {
  Wire.begin();
  Wire.setClock(20000);
  
  Serial.begin(115200);
  
  Serial.println("Waiting 15 seconds for PSU");

  delay(15000);
  
  scan_i2c ();                          // check for power supplies on the i2c bus
  
  delayMicroseconds(10000);
  
  commandPsu(0x00, 0x02, 0x72, voltage);// i2c command for setting voltage
  // commandPsu(0x00, 0x02, 0x74, amp);    // i2c command for setting max amps
  // commandPsu(0x00, 0x02, 0x76, vsd);    // i2c command for setting max voltage shutdown
  
  Serial.println("done");
  Serial.println("better way: disconnect short pin for 2 seconds then reconnect");
  Serial.println("other way: unplug power supply and wait 30 seconds");
}

void scan_i2c () {
  byte error, address;
  int nDevices = 0;

  Serial.println("Scanning for power supplies");

  for (address = 1; address < 127; address++ ) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0) {
      Serial.print("Power supply found at address 0x");
      if (address < 16){
        Serial.print("0");
      }
      Serial.println(address, HEX);
      nDevices++;
    }
    if (error == 4) {
      Serial.print("Power supply error at address 0x");
      if (address < 16){
        Serial.print("0");
      }
      Serial.println(address, HEX);      
    }
  }
  
  if (nDevices == 0) {
    Serial.println("No power supplies found");
    Serial.println("Hualting, fix i2c problem");
    while(1);
  } else {
    Serial.println("Setting default voltage");
  }
}

uint8_t crc82(uint8_t *p, uint8_t len) {
  byte chk = 0;
  for ( int i = 0; i < len; i++) {
    chk += p[i];
  };
  chk = 256 - chk;
  return chk;
}

void commandPsu(uint8_t addr, uint8_t p, uint8_t o, uint16_t value) {
  uint8_t h = value >> 8;
  uint8_t l = value & 0xFF;
  uint8_t data[] = {addr, p, o, h, l};
  uint8_t crc = crc82(data, sizeof(data));
  Wire.beginTransmission(addr);
  Wire.write(p);
  Wire.write(o);
  Wire.write(l);
  Wire.write(h);
  Wire.write(crc);
  Wire.endTransmission();
  delayMicroseconds(10000);
}

void loop() {
}