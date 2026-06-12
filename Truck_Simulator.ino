#define LED 13
#define BUTTON1 5
#define BUTTON2 6
#include "exo.h"
#include "math.h"

float deg2rad(float deg) {
    return 3.14159265359 * deg / 180.0;
}

simdata_t getSimData() {
    simdata_t simdata;
    simdata.header1    = 0xaa;
    simdata.header2    = 0x10;

    simdata.embrague   = 128;
    simdata.acelerador = 128;
    simdata.freno      = 128;
    simdata.volante    = 0;

    simdata.luzbaja    = 1;
    simdata.giroizq    = 1;
    simdata.giroder    = 1;
    simdata.arranque   = 1;
    simdata.contacto   = 1;
    simdata.bocina     = 1;
    simdata.cinturon   = 1;
    simdata.padding1   = 1;

    simdata.luzalta    = 1;
    simdata.parabrisa1 = 1;
    simdata.parabrisa2 = 1;
    simdata.padding2   = 1;

    simdata.marcha5    = 1;
    simdata.bocina2    = 1;
    simdata.frenomano  = 1;
    simdata.marcha4    = 1;
    simdata.marcha2    = 1;
    simdata.reversa    = 1;
    simdata.marcha1    = 1;
    simdata.marcha3    = 1;

    simdata.padding3     = 1;
    simdata.marchasaltas = 1;
    simdata.calentador   = 1;
    simdata.padding4     = 1;

    simdata.checksum   = 0xff;
    simdata.footer     = 0xbb;
    return simdata;
}

simdata_t simdata;

void sendStruct() {
    // copiar simdata a newsimdata
    simdata_t newsimdata = simdata;
    uint8_t *puntero = (uint8_t *) &newsimdata;

    // intercambiar byte5 por byte6 (little endian -> big endian)
    uint8_t msb = *(puntero + 5);
    *(puntero + 5) = *(puntero + 6);
    *(puntero + 6) = msb;

    // invertir el cinturon (byte 7)
    *(puntero + 7) = *(puntero + 7) ^ (0b01000000);

    // invertir el freno de mano (byte 9)
    *(puntero + 9) = *(puntero + 9) ^ (0b00000100);

    // invertir las marchas altas (byte 10)
    *(puntero + 10) = *(puntero + 10) ^ (0b00100000);

    // hardcoded
    newsimdata.luzbaja = digitalRead(BUTTON1);
    
    // escupir resultado por puerto serial
    Serial.write( (uint8_t *) &newsimdata, sizeof(newsimdata));
}

void setup()
{
    pinMode(LED, OUTPUT);
    pinMode(BUTTON1, INPUT_PULLUP);
    pinMode(BUTTON2, INPUT_PULLUP);
    pinMode(A0, INPUT);
    pinMode(A1, INPUT);
    Serial.begin(19200);
}

void loop()
{
    int i, j;
    int minVal,maxVal,midVal;
    uint8_t *puntero = (uint8_t *) &simdata;
    
    simdata = getSimData();
    simdata.acelerador = 144;
    simdata.freno=120;
    simdata.embrague=136;
    simdata.volante=0;  

    for (i=1; i<6; i++)
    {
        switch(i) 
        {
            case 1:
                simdata.marcha1=0;
                simdata.marcha2=1;
                simdata.marcha3=1;
                simdata.marcha4=1;
                simdata.marcha5=1;
                break;
            case 2:
                simdata.marcha1=1;
                simdata.marcha2=0;
                simdata.marcha3=1;
                simdata.marcha4=1;
                simdata.marcha5=1;
                break;
            case 3:
                simdata.marcha1=1;
                simdata.marcha2=1;
                simdata.marcha3=0;
                simdata.marcha4=1;
                simdata.marcha5=1;
                break;
            case 4:
                simdata.marcha1=1;
                simdata.marcha2=1;
                simdata.marcha3=1;
                simdata.marcha4=0;
                simdata.marcha5=1;
                break;
            case 5:
                simdata.marcha1=1;
                simdata.marcha2=1;
                simdata.marcha3=1;
                simdata.marcha4=1;
                simdata.marcha5=0;
                break;      
        }  
        for (j=0; j<12; j++)
        {
            delay(80);                                                     
            sendStruct();        
        }
        for (j=0; j<12; j++) {
           // punto muerto
            simdata.marcha1=1;
            simdata.marcha2=1;
            simdata.marcha3=1;
            simdata.marcha4=1;
            simdata.marcha5=1;
            sendStruct();
            delay(80);
        }
    }
    

    // acelerador
    minVal=144;
    maxVal=220;
    midVal = minVal + ((maxVal+1 - minVal)/2);
    for (i = -90; i <= 270; i += 2) {
        simdata.acelerador = midVal + (sin(deg2rad(i)) * ((maxVal+1-minVal)/2));
        sendStruct();
    }
    
    // freno
    minVal=120;
    maxVal=213;
    midVal = minVal + ((maxVal+1 - minVal)/2);
    for (i = -90; i <= 270; i += 2) {
        simdata.freno = midVal + (sin(deg2rad(i)) * ((maxVal+1-minVal)/2));
        sendStruct();
    }

    // embrague
    minVal=136;
    maxVal=217;
    midVal = minVal + ((maxVal+1 - minVal)/2);
    for (i = -90; i <= 270; i += 2) {
        simdata.embrague = midVal + (sin(deg2rad(i)) * ((maxVal+1-minVal)/2));
        sendStruct();
    }

    // volante
    minVal=-188;
    maxVal=230;
    midVal = minVal + ((maxVal+1 - minVal)/2);
    for (i = 0; i <= 360 ; i += 2) {
        simdata.volante = midVal + (sin(deg2rad(i)) * ((maxVal+1-minVal)/2));
        sendStruct();
    }
    simdata.volante =0;

    // prender balizas
    *(puntero+7) = 0b11111001;
    sendStruct();
    delay(30);

    // saltear al byte 7 y sequenciar un bitshift en cada byte
    *(puntero += 7);
    for (i = 7; i < 11; i += 1) {
        for (j = 0; j < 8; j++) {
            if (i ==  7 && j >= 7) continue;
            if (i ==  8 && j >= 3) continue;
            if (i == 10 && j <= 4) continue;
            if (i == 10 && j >= 7) continue;
            if (i ==  9 && j == 1) continue;

            // 0b1 << j  pone todos los bits 0 excepto el bit #j
            // ^0xff invierte todos los bits 
            // esto es porque en el simulador chino 1=off 0=0n
            *puntero = (0b1 << j) ^0xff;
            simdata.marchasaltas=0;
            sendStruct();
            delay(300);
        }
        *puntero = 0xff;
        *puntero++;
    }
}

