#include <mcp_can.h>
#include <mcp_can_dfs.h>
#include <SPI.h>

const int SPI_CS_PIN = 10;
MCP_CAN CAN(SPI_CS_PIN);                                                              // Set CS pin for CANBUS shield

void setup()                                                                          // Initialisation routine
{
    Serial.begin(115200);                                                             // Sets the data rate in bits per second (baud) for serial data transmission to PC.

START_INIT:

    if(CAN_OK == CAN.begin(CAN_125KBPS))                                              // init can bus : baudrate = 125k !!
    {
        Serial.println("CAN BUS Shield init ok!");
    }
    else
    {
        Serial.println("CAN BUS Shield init fail");
        Serial.println("Init CAN BUS Shield again");
        delay(100);
        goto START_INIT;
    }
}

void loop()                                                                           // main program (LOOP)
{
    unsigned char len = 0;
    unsigned char buf[8] ;
    if(CAN_MSGAVAIL == CAN.checkReceive())                                            // check if data coming
    {

        CAN.readMsgBuf(&len, buf);                                                    // read data,  len: data length, buf: data buf
          INT32U canId = CAN.getCanId();                                              // read the CAN Id
          Serial.print("0");                                                          // leading zero 
          Serial.print(canId,HEX);                                                    // output CAN Id to serial monitor
          Serial.print("\t");                                                         // send Tab
        for(int i = 0; i<len; i++)                                                    // print the data
        {
            if( buf[i] < 0x10){ Serial.print("0");} Serial.print(buf[i],HEX);         // send a leading zero if only one digit
            Serial.print(" ");                                                        // space to seperate bytes
        }
       Serial.println();
       Serial.print("Temperature = ");
       Serial.println(buf[0]);                                                        //first byte is temperature in celcius
       Serial.print("Current = ");
       Serial.println(buf[2]*255*0.1+buf[1]*0.1);                                     // third (msb) and second byte are current in 0.1A (deciamp) calculated to show directly in Amps
       Serial.print("OutputVoltage = ");
       Serial.println(buf[4]*255*0.01+buf[3]*0.01);                                   // fifth (msb) and fourth byte are voltage in 0.01V (centivolt) calculated to show directly in Volts dc
       Serial.print("OutputPower = ");
       Serial.println((buf[4]*255*0.01+buf[3]*0.01)*(buf[2]*255*0.1+buf[1]*0.1));     // Power is calculated from output voltage and current. Output is in Watts
       Serial.print("InputVoltage = ");          
       Serial.println(buf[5]);                                                        // sixth byte is input voltage in volts ac/dc
//send request for new update
       unsigned char stmp3[8] = {0x14, 0x14, 0x71, 0x11, 0x08, 0x20, 0x00, 0x00};     //this is the serial number of the unit
       CAN.sendMsgBuf(0x05004804, 1, 8, stmp3);                                       //send message to request updated figures
    }
 

}

/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
