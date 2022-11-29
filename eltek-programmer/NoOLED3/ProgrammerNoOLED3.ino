#include <mcp_can.h>
#include <mcp_can_dfs.h>
#include <SPI.h>

const int SPI_CS_PIN = 10;                                                          // Set CS pin to pin 10 (could be 9 for other CANshields)
MCP_CAN CAN(SPI_CS_PIN);                                                            // Set CS pin for CANBUS shield

void setup()                                                                        // Initialisation routine
{

START_INIT:

    if(CAN_OK == CAN.begin(CAN_125KBPS))                                            // init can bus : baudrate = 125k !!
    {
    }
    else
    {
    delay(100);
    goto START_INIT;
    }
    unsigned char login[8] = {0x14, 0x14, 0x71, 0x11, 0x08, 0x20, 0x00, 0x00};     //this is the serial number of the unit + 2 added bytes of 00 each, sernr on the unit reads 141471110820)
    CAN.sendMsgBuf(0x05004804, 1, 8, login);                                       //send message to log in and assign ID=1 (last 04 means ID=1, for ID=2 use 05004808 ) 

    unsigned char setdefaultvolt[5] = {0x29, 0x15, 0x00, 0x80, 0x16};              //this is the command for setting the default output voltage (Last two bytes, LSB first). 16 80 is the maximum voltage of 57.6 V
    CAN.sendMsgBuf(0x05019C00, 1, 5, setdefaultvolt);                              //send message to set ouput voltage to all chargers connected to the CAN-bus
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