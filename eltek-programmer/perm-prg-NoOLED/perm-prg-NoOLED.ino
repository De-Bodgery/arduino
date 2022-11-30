// Eltek Flatpack Permanent Voltage changer

// Change the PSU voltage:
// 1. Flatpack serial number converted to HEX
// 2. Change the serial number line to your HEX number
// 3. Upload code to Arduino
// 4. Disconnect Arduino from usb so it powers down
// 5. Power on the Flatpack
// 6. Connect CANBUS from the programmer board to the PSU
// 7. Power on programmer board

// Voltage settings:
// unsigned char setdefaultvolt[5] = {0x2B, 0x15, 0x00, 0x80, 0x16}
// Last 2 segements are the voltage: 0x80, 0x16
// 0x80, 0x10 = 40.96v returned is 43.8 measured is 44.0
// 0x80, 0x11 = 43.52v returned is 43.7 measured is 43.6
// 0x80, 0x12 = 46.08v returned is 46.2 measured is 46.1
// 0x80, 0x13 = 48.64v returned is 48.8 measured is 48.7
// 0x80, 0x14 = 51.20v returned is 51.3 measured is 51.2
// 0x80, 0x15 = 53.76v returned is 53.8 measured is 53.8
// 0x80, 0x16 = 56.32v returned is 56.4 measured is 56.4
// 0x80, 0x16 = 57.60v returned is 57.0 measured is 56.9
// 0x80, 0x17 = 58.88v returned is 57.7 measured is 57.6
// 0x80, 0x18 = 61.44v returned is 57.7 measured is 57.6
 
// 57.6v = 0x80, 0x16 (highest possible voltage)
// 53.5v = 0x14, 0xE6 (factory default voltage)
// 43.5v = 0x10, 0xFE (lowest possible voltage) 

// After about 10 seconds or so the output voltage should change to the new default voltage.
// Now wait 30 seconds and disconnect the power to the rectifier. 
// When you power up the PSU again the voltage will default to the new default voltage. 
// The default voltage is determined by the last 2 bytes of the second command. 
// Multiply your desired voltage with 100 (57.50 => 5750). 
// Convert this number to HEX values (5700 => 0x1680). Switch the two bytes (0x1680 => 0x80 0x16) and you have your code for setting the voltage

// NOTE: Some Flatpacks needs to run this procedure several times to change the Voltage. 
// Sometimes the Flatpack shows an yellow led, but no Voltage change. Just repeat the procedure until it changes the voltage.

//CHANGES: LED showing start/end proces

#include <mcp_can.h>
#include <mcp_can_dfs.h>
#include <SPI.h>

const int SPI_CS_PIN = 17;                                                          // Set CS pin to pin 17
MCP_CAN CAN(SPI_CS_PIN);                                                            // Set CS pin for CANBUS shield

void setup()                                                                        // Initialisation routine
{
  pinMode(23, OUTPUT);                                                                // Set pin 9 to output (backlight of the LCD)
  digitalWrite(23, HIGH);                                                            // LED on
  delay(250);
  digitalWrite(23, LOW);

START_INIT:

    if(CAN_OK == CAN.begin(CAN_125KBPS))                                            // init can bus : baudrate = 125k !!
    {
    }
    else
    {
    delay(100);
    goto START_INIT;
    }
    unsigned char login[8] = {0x15, 0x12, 0x72, 0x00, 0x08, 0x46, 0x00, 0x00};     // Flatpack SN + 2 added bytes of 00 each)
    CAN.sendMsgBuf(0x05004804, 1, 8, login);                                       // Log in and assign ID=1 (last 04 means ID=1, for ID=2 use 05004808 ) 

    unsigned char setdefaultvolt[5] = {0x2B, 0x15, 0x00, 0x80, 0x16};              // Set the default output voltage (Last two bytes, LSB first)
    CAN.sendMsgBuf(0x05009C02, 1, 5, setdefaultvolt);                              // Send message to set default output voltage
digitalWrite(23, HIGH); 
}

void loop()                                                                        // main program (LOOP)
{                                                                                  // nothing to do :)
}