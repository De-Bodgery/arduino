#include mcp_can.h
#include mcp_can_dfs.h
#include SPI.h

const int SPI_CS_PIN = 10;
MCP_CAN CAN(SPI_CS_PIN);                                                               Set CS pin for CANBUS shield

void setup()                                                                           Initialisation routine
{
    Serial.begin(115200);                                                              Sets the data rate in bits per second (baud) for serial data transmission to PC.
    delay(1000); 
START_INIT

    if(CAN_OK == CAN.begin(CAN_125KBPS))                                               init can bus  baudrate = 125k !!
    {
        Serial.println(CAN BUS Shield init ok!);
    }
    else
    {
        Serial.println(CAN BUS Shield init fail);
        Serial.println(Init CAN BUS Shield again);
        delay(100);
        goto START_INIT;
    }
       unsigned char stmp3[8] = {0x14, 0x14, 0x71, 0x11, 0x08, 0x20, 0x00, 0x00};     this is the serial number of the unit
       CAN.sendMsgBuf(0x05004804, 1, 8, stmp3);                                       send message to log in
       delay(10);   
       unsigned char stmp4[8] = {0xFF, 0xFF, 0x80, 0x16, 0x80, 0x16, 0x80, 0x16};     this is the command for changing the output voltage 16 80 is the maximum voltage of 57.6 V
       CAN.sendMsgBuf(0x05FF4004, 1, 8, stmp4);                                       send message to set ouput voltage to all chargers connected to the CAN-bus
  00 10 should be 40.96 returned is 43.8 measured is 44.0
  00 11 should be 43.52 returned is 43.7 measured is 43.6 
  00 12 should be 46.08 returned is 46.2 measured is 46.1
  00 13 should be 48.64 returned is 48.8 measured is 48.7
  00 14 should be 51.20 returned is 51.3 measured is 51.2 
  00 15 should be 53.76 returned is 53.8 measured is 53.8
  00 16 should be 56.32 returned is 56.4 measured is 56.4
  80 16 should be 57.60 returned is 57.0 measured is 56.9
  00 17 should be 58.88 returned is 57.7 measured is 57.6
  00 18 should be 61.44 returned is 57.7 measured is 57.6
      Serial.print(Output Voltage set to  );
      Serial.println(stmp4[3]2560.01+stmp4[2]0.01);
      

}

void loop()                                                                            main program (LOOP)
{
    unsigned char len = 0;
    unsigned char buf[8] ;
    if(CAN_MSGAVAIL == CAN.checkReceive())                                             check if data coming
    {

        CAN.readMsgBuf(&len, buf);                                                     read data,  len data length, buf data buf
          INT32U canId = CAN.getCanId();                                               read the CAN Id
          Serial.print(0);                                                           leading zero 
          Serial.print(canId,HEX);                                                     output CAN Id to serial monitor
          Serial.print(t);                                                          send Tab
        for(int i = 0; ilen; i++)                                                     print the data
        {
            if( buf[i]  0x10){ Serial.print(0);} Serial.print(buf[i],HEX);          send a leading zero if only one digit
            Serial.print( );                                                         space to seperate bytes
        }
       Serial.println();
       Serial.print(Temperature = );
       Serial.println(buf[0]);                                                        first byte is temperature in celcius
       Serial.print(Current = );
       Serial.println(buf[2]2560.1+buf[1]0.1);                                      third (msb) and second byte are current in 0.1A (deciamp) calculated to show directly in Amps
       Serial.print(OutputVoltage = );
       Serial.println(buf[4]2560.01+buf[3]0.01);                                    fifth (msb) and fourth byte are voltage in 0.01V (centivolt) calculated to show directly in Volts dc
       Serial.print(OutputPower = );
       Serial.println((buf[4]2560.01+buf[3]0.01)(buf[2]2560.1+buf[1]0.1));      Power is calculated from output voltage and current. Output is in Watts
       Serial.print(InputVoltage = );          
       Serial.println(buf[5]);                                                         sixth byte is input voltage in volts acdc
send request for new update
       unsigned char stmp3[8] = {0x14, 0x14, 0x71, 0x11, 0x08, 0x20, 0x00, 0x00};     this is the serial number of the unit
       CAN.sendMsgBuf(0x05004804, 1, 8, stmp3);                                       send message to request updated figures and keep being 'logged in' or the charger defaults to 53.5 V
       

    }
 

}


  END FILE

