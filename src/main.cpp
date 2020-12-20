#include <Arduino.h>
#include <SPI.h>
#include "../lib/RF24/RF24.h"
#include <Wire.h>
struct Data
{
    int16_t x1;
    int16_t y1;
    int16_t x2;
    int16_t y2;
    int8_t canal;
    int16_t sum ;
};
struct senddata{
    int16_t a1x;
    int16_t a1y;
    int16_t a1z;
    int16_t a2x;
    int16_t a2y;
    int16_t a2z;
    int16_t a3x;
    int16_t a3y;
    int16_t a3z;
    bool bg = false;
    bool by = false;
    bool br = false;
    bool w1 = false;
    bool w2 = false;
    bool AWD = false;
    bool onoffL = false;
    bool onoffR = false;
    uint8_t code = 18;
};

struct receivedata{
    int16_t a1x;
    int16_t a1y;
    int16_t a1z;
    int16_t a2x;
    int16_t a2y;
    int16_t a2z;
    int16_t a3x;
    int16_t a3y;
    int16_t a3z;
    uint8_t button;
    uint8_t code = 18;
};


senddata Sdata;
receivedata Rdata;

RF24 myRadio (7, 8);
byte addres[][6]={"stero","kont"};
uint8_t binaryV[8] = {128,64,32,16,8,4,2,1};
bool buttons[8] = {false,false,false,false,false,false,false,false};
bool typeTransmision = false; //// Flaga odpowiedzialna za typ transmisji

void setup() {
    pinMode(4 , INPUT);
    Serial.begin(9600);
    myRadio.begin();
    myRadio.setDataRate(RF24_250KBPS);
    myRadio.setAutoAck(false);
    myRadio.setChannel(108);
    myRadio.setPALevel(RF24_PA_MAX);
    myRadio.openWritingPipe(addres[1]);
    myRadio.openReadingPipe(1,addres[0]);
    myRadio.startListening();
    Wire.begin();
    while (digitalRead(4) == LOW) {}
}

void loop() {
    while(digitalRead(4) == LOW) {
    }

    if(myRadio.available())
    {


        myRadio.read(&Rdata, sizeof(receivedata));
        if(Rdata.code == 31)
        {

            ////Konwersja z struktury receivedata na strukture senddata
            Sdata.a1x = Rdata.a1x;
            Sdata.a2x = Rdata.a2x;
            Sdata.a3x = Rdata.a3x;

            Sdata.a1y = Rdata.a1y;
            Sdata.a2y = Rdata.a2y;
            Sdata.a3y = Rdata.a3y;

            Sdata.a1z = Rdata.a1z;
            Sdata.a2z = Rdata.a2z;
            Sdata.a3z = Rdata.a3z;
            Sdata.code = Rdata.code;
            ////Konwersja liczby na bity;
            for(short i =0;i<8;i++)
            {
                if(Rdata.button >= binaryV[i]) {
                    buttons[i] = true;
                    Rdata.button -= binaryV[i];
                }else
                {
                    buttons[i] = false;
                }
            }

            Sdata.bg = buttons[0];
            Sdata.br = buttons[1];
            Sdata.by = buttons[2];
            Sdata.w1 = buttons[3];
            Sdata.w2 = buttons[4];
            Sdata.onoffL = buttons[5];
            Sdata.onoffR = buttons[6];
            Sdata.AWD = buttons[7];
            ////Konwersja z struktury receivedata na strukture senddata


            if(!typeTransmision) {
                //// Wysyłanie danych do sterownika po serial porcie
                Serial.write((const char *) &Sdata, sizeof(senddata));

            } else {
                ////Wysyłanie danych do sterownika po I2C
                Wire.beginTransmission(20); // transmit to device #44 (0x2c)
                // device address is specified in datasheet
                Wire.write((const char *) &Sdata, sizeof(senddata));            // sends value byte
                Wire.endTransmission();
            }
        }

    }
}
