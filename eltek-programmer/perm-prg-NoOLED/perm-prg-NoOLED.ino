// Eltek Flatpack Permanent Voltage changer

// Change the PSU voltage:
// 1. Flatpack serial number converted to HEX
// 2. Change the serial number line to your HEX number
// 3. Upload code to Arduino
// 4. Disconnect Arduino from usb so it powers down
// 5. Power on the Flatpack
// 6. Connect CANBUS from the programmer board to the PSU
// 7. Power on programmer board

// 00 10 should be 40.96 returned is 43.8 measured is 44.0
// 00 11 should be 43.52 returned is 43.7 measured is 43.6
// 00 12 should be 46.08 returned is 46.2 measured is 46.1
// 00 13 should be 48.64 returned is 48.8 measured is 48.7
// 00 14 should be 51.20 returned is 51.3 measured is 51.2
// 00 15 should be 53.76 returned is 53.8 measured is 53.8
// 00 16 should be 56.32 returned is 56.4 measured is 56.4
// 80 16 should be 57.60 returned is 57.0 measured is 56.9
// 00 17 should be 58.88 returned is 57.7 measured is 57.6
// 00 18 should be 61.44 returned is 57.7 measured is 57.6

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
    unsigned char login[8] = {0x15, 0x12, 0x72, 0x00, 0x08, 0x46, 0x00, 0x00};     //this is the serial number of the Flatpack + 2 added bytes of 00 each)
    CAN.sendMsgBuf(0x05004804, 1, 8, login);                                       //send message to log in and assign ID=1 (last 04 means ID=1, for ID=2 use 05004808 ) 

    unsigned char setdefaultvolt[5] = {0x2B, 0x15, 0x00, 0x80, 0x16};              //this is the command for setting the default output voltage (Last two bytes, LSB first). 16 80 is the maximum voltage of 57.6 V
    CAN.sendMsgBuf(0x05009C02, 1, 5, setdefaultvolt);                              //send message to set ouput voltage to all Flatpacks connected to the CAN-bus
digitalWrite(23, HIGH); 
}

void loop()                                                                        // main program (LOOP)
{                                                                                  // nothing to do :)
}

/*********************************************************************************************************
  END FILE
  Voltage settings 
  80 16 => 1680 HEX = 57,60 Volt (= highest possible voltage
  E6 14 => 14E6 HEX = 53,50 Volt (= factory set voltage)
  FE 10 => 10FE HEX = 43,50 Volt (= lowest possible voltage)  
*********************************************************************************************************/