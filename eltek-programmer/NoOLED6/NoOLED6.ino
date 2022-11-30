/* unsigned char login[8] = {0x14, 0x14, 0x71, 0x11, 0x08, 0x20, 0x00, 0x00};     //this is the serial number of the unit + 2 added bytes of 00 each, sernr on the unit reads 141471110820)
  CAN.sendMsgBuf(0x05005804, 1, 8, login);                                       //send message to log in and assign ID=1 (last 04 means ID=1, for ID=2 use 05004808 )

  unsigned char setdefaultvolt[5] = {0x29, 0x15, 0x00, 0x80, 0x16};              //this is the command for setting the default output voltage (Last two bytes, LSB first). 16 80 is the maximum voltage of 57.6 V
  CAN.sendMsgBuf(0x05019C00, 1, 5, setdefaultvolt);                             //send message to set ouput voltage to all chargers connected to the CAN-bus */
#include <mcp_can.h>
#include <mcp_can_dfs.h>
#include <SPI.h>
word defaultvoltage =9550; //11550;                                                         // set default voltage (divide by 100) (may not need both default and output)
//word outputvoltage = 4350;                                                         // set output voltage to (divide by 100)
word maxcurrent = 50;                                                              // set initial maxcurrent to 12.5A output
word overvoltage = 11750;
const int SPI_CS_PIN = 17;                                                          // Set CS pin to pin 10 (could be 9 for other CANshields)
unsigned char len = 0;
unsigned char serialnr[8];
unsigned char buf[8] ;
int msgreceived;
bool longwalkin = HIGH;                                                         //Choose HIGH for 60 second walkin and LOW for 5 second
long walkin = 0x05FF4005;
MCP_CAN CAN(SPI_CS_PIN);                                                            // Set CS pin for CANBUS shield

void setup()                                                                        // Initialisation routine
{ bool flag = LOW;                                                                  // set flag to low
  if (longwalkin == LOW)
  {
    walkin = 0x05FF4004;
  }
START_INIT:

  if (CAN_OK == CAN.begin(CAN_125KBPS))                                           // init can bus : baudrate = 125k !!
  {
  }
  else
  {
    delay(100);
    goto START_INIT;
  }

  //This next section sends one initial message that should set the default voltage.
  while (flag == LOW)                                                                  // cycle through until correct Canid allows message transmission
  {
    if (CAN_MSGAVAIL == CAN.checkReceive())                                             // check if data coming
    {
      CAN.readMsgBuf(&len, buf);                                                    // read data,  len: data length, buf: data buf
      INT32U canId = CAN.getCanId();                                                // read the CAN Id


      if (canId == 0x05014400)                                                          //this is the request from the Flatpack rectifier during walk-in (start-up) or normal operation when no log-in response has been received for a while
      {
        for (int i = 0; i < 8; i++)
        {
          serialnr[i] = buf[i];                                                        // transfer the message buffer to the serial number variable
        }
        CAN.sendMsgBuf(0x05004805, 1, 8, serialnr);                                  //send message to log in (DO NOT ASSIGN AN ID use 00) use 4805 for 60 second walkin and 4804 for 5 second
        msgreceived++;                                                               // increase the variable "msgreceived"
        unsigned char setdefaultvolt[5] = { 0x29, 0x15, 0x00, lowByte(defaultvoltage), highByte(defaultvoltage)};   //this is the command for setting the default output voltage (Last two bytes, LSB first). qnd long walk-in 4005 or short walk in 4004
        CAN.sendMsgBuf(0x05019C00, 1, 5, setdefaultvolt);                                                                     //(last 4 in header is for 5 sec walkin, 5 is for 60 second walkin)                                                                     //(last 4 in header is for 5 sec walkin, 5 is for 60 second walkin)
        flag = HIGH;
      }                                                                              // set flag to say that message has been sent
      else delay(100);
    }
  }
}

void loop()                                                                        // main program (LOOP)
{ delay( 20000);
  if (CAN_MSGAVAIL == CAN.checkReceive())                                             // check if data coming
  {
    CAN.readMsgBuf(&len, buf);                                                    // read data,  len: data length, buf: data buf
    INT32U canId = CAN.getCanId();                                                // read the CAN Id


    if (canId == 0x05014400)                                                          //this is the request from the Flatpack rectifier during walk-in (start-up) or normal operation when no log-in response has been received for a while
    {
      for (int i = 0; i < 8; i++)
      {
        serialnr[i] = buf[i];                                                        // transfer the message buffer to the serial number variable
      }
      CAN.sendMsgBuf(0x05004805, 1, 8, serialnr);                                  //send message to log in (DO NOT ASSIGN AN ID use 00) use 4805 for 60 second walkin and 4804 for 5 second
      msgreceived++;                                                               // increase the variable "msgreceived"
      unsigned char stmp7[8] = {lowByte(maxcurrent), highByte(maxcurrent), lowByte(defaultvoltage), highByte(defaultvoltage), lowByte(defaultvoltage), highByte(defaultvoltage), lowByte(overvoltage), highByte(overvoltage)};    // set rectifier to maxcurrent 57,0V (16 44) and OVP to 59.5V (17 3E) qnd long walk-in 4005 or short walk in 4004
      CAN.sendMsgBuf(walkin, 1, 8, stmp7);
    }                                                                              // nothing to do :)
  }
}
/*********************************************************************************************************
  END FILE
  Voltage settings
  80 16 => 1680 HEX = 57,60 Volt (= highest possible voltage
  E6 14 => 14E6 HEX = 53,50 Volt (= factory set voltage)
  FE 10 => 10FE HEX = 43,50 Volt (= lowest possible voltage)
*********************************************************************************************************/