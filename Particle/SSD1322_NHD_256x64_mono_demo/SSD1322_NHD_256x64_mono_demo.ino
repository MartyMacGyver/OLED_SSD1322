//--------------------------------------------------------------------------
/*
Program for writing to Newhaven Display 256x64 graphic OLED with SSD1322 controller (serial mode)
Original demo (c)2014 Mike LaVine - Newhaven Display International, LLC. 
Origin: http://www.newhavendisplay.com/NHD_forum/index.php/topic,64.0.html

Annotated and updated by Martin Falatic

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
*/
//--------------------------------------------------------------------------
//##########################################################################
//--------------------------------------------------------------------------

//#include <SPI.h> // Arduino

#define MODE_3WIRE    1   // BS1=0, BS0 = 1, tie RS (D/C) LOW; not working
#define MODE_4WIRE    2   // BS1=0, BS0 = 0, uses RS (D/C); SLOOOOOOW...
#define MODE_SPI4W    3   // BS1=0, BS0 = 0, uses RS (D/C); FAST HW SPI!

#define SEND_CMD      1   // Display instruction (command)
#define SEND_DAT      2   // Display instruction (data)

const uint16_t MAXROWS =   64;
const uint16_t MAXCOLS =  256;
const uint16_t Col0Off = 0x70;
const uint16_t ColDiv  =    4;

uint8_t  display_buffer[MAXROWS][MAXCOLS/2];
uint16_t curDisplayRow = 0;
uint16_t curDisplayCol = 0;

// Pin mappings for Mega2560
#define PIN_SDIN     A5  // SDIN signal
#define PIN_RES      A4  // /RES signal
#define PIN_SCLK     A3  // SCLK signal
#define PIN_RS       A2  // RS (D/C) signal (can be tied low for 3-wire SPI)
#define PIN_CS       A0  // /CS signal (can be tied low with a single display)

int SIG_MODE = MODE_SPI4W;
//int SIG_MODE = MODE_4WIRE;
//int SIG_MODE = MODE_3WIRE;

//--------------------------------------------------------------------------
//##########################################################################
//--------------------------------------------------------------------------

void InitStructsAndPins()
{
    pinMode(A0, OUTPUT); digitalWrite(A0,  LOW);
    pinMode(A1, OUTPUT); digitalWrite(A1,  LOW);
    pinMode(A2, OUTPUT); digitalWrite(A2,  LOW);
    pinMode(A3, OUTPUT); digitalWrite(A3,  LOW);
    pinMode(A4, OUTPUT); digitalWrite(A4,  LOW);
    pinMode(A5, OUTPUT); digitalWrite(A5,  LOW);
    pinMode(A6, OUTPUT); digitalWrite(A6,  LOW);
    pinMode(A7, OUTPUT); digitalWrite(A7,  LOW);
    
    pinMode(D0, OUTPUT); digitalWrite(D0,  LOW);
    pinMode(D1, OUTPUT); digitalWrite(D1,  LOW);
    pinMode(D2, OUTPUT); digitalWrite(D2,  LOW);
    pinMode(D3, OUTPUT); digitalWrite(D3,  LOW);
    pinMode(D4, OUTPUT); digitalWrite(D4,  LOW);
    pinMode(D5, OUTPUT); digitalWrite(D5,  LOW);
    pinMode(D6, OUTPUT); digitalWrite(D6,  LOW);
    pinMode(D7, OUTPUT); digitalWrite(D7,  LOW);
    
    pinMode(PIN_SCLK, OUTPUT);
    pinMode(PIN_SDIN, OUTPUT);
    pinMode(PIN_RS,   OUTPUT);
    pinMode(PIN_RES,  OUTPUT);
    pinMode(PIN_CS,   OUTPUT);
}

//--------------------------------------------------------------------------
//##########################################################################
//--------------------------------------------------------------------------
void displaySendStart(uint8_t sendType)
{
    digitalWrite(PIN_CS, LOW);
    
    if (sendType == SEND_DAT)
    {
        if (SIG_MODE == MODE_4WIRE || SIG_MODE == MODE_SPI4W)
        {
            digitalWrite(PIN_RS, HIGH);
        }
        else if (SIG_MODE == MODE_3WIRE)
        {
            digitalWrite(PIN_SDIN, HIGH);
            digitalWrite(PIN_SCLK, LOW);
            digitalWrite(PIN_SCLK, HIGH);
        }
    }
    else if (sendType == SEND_CMD)
    {
        if (SIG_MODE == MODE_4WIRE || SIG_MODE == MODE_SPI4W)
        {
            digitalWrite(PIN_RS, LOW);
        }
        else if (SIG_MODE == MODE_3WIRE)
        {
            digitalWrite(PIN_SDIN, LOW);
            digitalWrite(PIN_SCLK, LOW);
            digitalWrite(PIN_SCLK, HIGH);
        }
    }
}

//--------------------------------------------------------------------------
void displaySendData(unsigned char v)
{
    if (SIG_MODE == MODE_SPI4W) {
        SPI.transfer(v);
    }
    else if (SIG_MODE == MODE_3WIRE || SIG_MODE == MODE_4WIRE)
    {
        for(unsigned char i=8; i>0; i--)
        {
            digitalWrite(PIN_SCLK, LOW);
            if((v&0x80)>>7==1)
            {
                digitalWrite(PIN_SDIN, HIGH);
            }
            else
            {
                digitalWrite(PIN_SDIN, LOW);
            }
            v = v<<1;
            digitalWrite(PIN_SCLK, HIGH);
        }
    }
}

//--------------------------------------------------------------------------
void displaySendEnd()
{
    digitalWrite(PIN_CS, HIGH);
}

//--------------------------------------------------------------------------
void displaySend(uint8_t sendType, unsigned char v)
{
    displaySendStart(sendType);
    displaySendData(v);
    displaySendEnd();
}

//--------------------------------------------------------------------------
void Set_Column_Address(unsigned char a, unsigned char b)
{
    curDisplayCol = a;
    displaySend(SEND_CMD, 0x15);
    displaySend(SEND_DAT, (Col0Off+a)/ColDiv);
    displaySend(SEND_DAT, (Col0Off+b)/ColDiv);
}

//--------------------------------------------------------------------------
void Set_Row_Address(unsigned char a, unsigned char b)
{
    curDisplayRow = a;
    displaySend(SEND_CMD, 0x75);
    displaySend(SEND_DAT, a);
    displaySend(SEND_DAT, b);
}

//--------------------------------------------------------------------------
void Set_Write_RAM()  // Enable MCU to Write into RAM
{
    displaySend(SEND_CMD, 0x5C);
}

//--------------------------------------------------------------------------
void Init_Device()
{
    // Lots of reset/tweaking commands follow
    displaySend(SEND_CMD, 0xFD); // Set Command Lock (MCU protection status)
    displaySend(SEND_DAT, 0x12); // 0x12 = Unlock Basic Commands; 0x16 = lock
    
    displaySend(SEND_CMD, 0xA4); // Set Display Mode = OFF
    
    displaySend(SEND_CMD, 0xB3); // Set Front Clock Divider / Oscillator Frequency
    displaySend(SEND_DAT, 0x91); // 0x91 = 80FPS; 0xD0 = default / 1100b 
    
    displaySend(SEND_CMD, 0xCA); // Set MUX Ratio
    displaySend(SEND_DAT, 0x3F); // 0x3F = 63d = 64MUX (1/64 duty cycle)
    
    displaySend(SEND_CMD, 0xA2); // Set Display Offset
    displaySend(SEND_DAT, 0x00); // 0x00 = (default)
    
    displaySend(SEND_CMD, 0xA1); // Set Display Start Line
    displaySend(SEND_DAT, 0x00); // 0x00 = register 00h
    
    displaySend(SEND_CMD, 0xA0); // Set Re-map and Dual COM Line mode
    displaySend(SEND_DAT, 0x14); // 0x14 = Default except Enable Nibble Re-map, Scan from COM[N-1] to COM0, where N is the Multiplex ratio
    displaySend(SEND_DAT, 0x11); // 0x11 = Enable Dual COM mode (MUX <= 63)
    
    displaySend(SEND_CMD, 0xB5); // Set GPIO
    displaySend(SEND_DAT, 0x00); // 0x00 = {GPIO0, GPIO1 = HiZ (Input Disabled)}
    
    displaySend(SEND_CMD, 0xAB); // Function Selection
    displaySend(SEND_DAT, 0x01); // 0x01 = Enable internal VDD regulator (default)
    
    displaySend(SEND_CMD, 0xB4); // Display Enhancement A
    displaySend(SEND_DAT, 0xA0); // 0xA0 = Enable external VSL; 0xA2 = internal VSL
    displaySend(SEND_DAT, 0xB5); // 0xB5 = Normal (default); 0xFD = 11111101b = Enhanced low GS display quality
    
    displaySend(SEND_CMD, 0xC1); // Set Contrast Current
    displaySend(SEND_DAT, 0x9F); // 0x7F = (default)
    
    displaySend(SEND_CMD, 0xC7); // Master Contrast Current Control
    displaySend(SEND_DAT, 0x0F); // 0x0F = (default)
    
    displaySend(SEND_CMD, 0xB9); // Select Default Linear Gray Scale table
    
    // displaySend(SEND_CMD, 0xB8); // Select Custom Gray Scale table (GS0 = 0)
    // displaySend(SEND_DAT, 0x00); // GS1
    // displaySend(SEND_DAT, 0x08); // GS2
    // displaySend(SEND_DAT, 0x10); // GS3
    // displaySend(SEND_DAT, 0x18); // GS4
    // displaySend(SEND_DAT, 0x20); // GS5
    // displaySend(SEND_DAT, 0x28); // GS6
    // displaySend(SEND_DAT, 0x30); // GS7
    // displaySend(SEND_DAT, 0x38); // GS8
    // displaySend(SEND_DAT, 0x40); // GS9
    // displaySend(SEND_DAT, 0x48); // GS10
    // displaySend(SEND_DAT, 0x50); // GS11
    // displaySend(SEND_DAT, 0x58); // GS12
    // displaySend(SEND_DAT, 0x60); // GS13
    // displaySend(SEND_DAT, 0x68); // GS14
    // displaySend(SEND_DAT, 0x70); // GS15
    // displaySend(SEND_CMD, 0x00); // Enable Custom Gray Scale table
    
    displaySend(SEND_CMD, 0xB1); // Set Phase Length
    displaySend(SEND_DAT, 0xE2); // 0xE2 = Phase 1 period (reset phase length) = 5 DCLKs,
                               //        Phase 2 period (first pre-charge phase length) = 14 DCLKs
    
    displaySend(SEND_CMD, 0xD1); // Display Enhancement B
    displaySend(SEND_DAT, 0xA2); // 0xA2 = Normal (default); 0x82 = reserved
    displaySend(SEND_DAT, 0x20); // 0x20 = as-is
    
    displaySend(SEND_CMD, 0xBB); // Set Pre-charge voltage
    displaySend(SEND_DAT, 0x1F); // 0x17 = default; 0x1F = 0.60*Vcc (spec example)
    
    displaySend(SEND_CMD, 0xB6); // Set Second Precharge Period
    displaySend(SEND_DAT, 0x08); // 0x08 = 8 dclks (default)
    
    displaySend(SEND_CMD, 0xBE); // Set VCOMH
    displaySend(SEND_DAT, 0x07); // 0x04 = 0.80*Vcc (default); 0x07 = 0.86*Vcc (spec example)
    
    displaySend(SEND_CMD, 0xA6); // Set Display Mode = Normal Display
    
    displaySend(SEND_CMD, 0xA9); // Exit Partial Display
    
    displaySend(SEND_CMD, 0xAF); // Set Sleep mode OFF (Display ON)
    
    delay(10);
}

//--------------------------------------------------------------------------
void ClearDisplay()
{
    // Turn off display while clearing (also hides noise at powerup)
    displaySend(SEND_CMD, 0xA4); // Set Display Mode = OFF
    
    Set_Column_Address(0x00, MAXCOLS-1);
    Set_Row_Address(0x00, MAXROWS-1);
    Set_Write_RAM();
    
    displaySendStart(SEND_DAT);
    for(int i=0; i<MAXROWS; i++)
    {
        for(int j=0; j<MAXCOLS/2; j++)
        {
            displaySendData(0x00);
        }
    }
    displaySendEnd();
    displaySend(SEND_CMD, 0xA6); // Set Display Mode = Normal Display
}

//--------------------------------------------------------------------------
void FillDisplay()
{
    Set_Column_Address(0x00, MAXCOLS-1);
    Set_Row_Address(0x00, MAXROWS-1);
    Set_Write_RAM();
    
    displaySendStart(SEND_DAT);
    for(int i=0; i<MAXROWS; i++)
    {
        for(int j=0; j<MAXCOLS/2; j++)
        {
            displaySendData(0xFF);
        }
    }
    displaySendEnd();
}

//--------------------------------------------------------------------------
void CheckerboardOdd(uint8_t gsVal)
{
    Set_Column_Address(0x00, MAXCOLS-1);
    Set_Row_Address(0x00, MAXROWS-1);
    Set_Write_RAM();
    uint8_t gs0 = (gsVal & 0x0f) << 4;
    uint8_t gs1 = (gsVal & 0x0f);
    
    displaySendStart(SEND_DAT);
    for(int i=0; i<MAXROWS; i++)
    {
        for(int j=0; j<MAXCOLS/2; j++)
        {
            displaySendData(gs0);
        }
        i++;
        for(int j=0; j<MAXCOLS/2; j++)
        {
            displaySendData(gs1);
        }
    }
    displaySendEnd();
}

//--------------------------------------------------------------------------
void CheckerboardEven(uint8_t gsVal)
{
    Set_Column_Address(0x00, MAXCOLS-1);
    Set_Row_Address(0x00, MAXROWS-1);
    Set_Write_RAM();
    uint8_t gs0 = (gsVal & 0x0f);
    uint8_t gs1 = (gsVal & 0x0f) << 4;
    
    displaySendStart(SEND_DAT);
    for(int i=0; i<MAXROWS; i++)
    {
        for(int j=0; j<MAXCOLS/2; j++)
        {
            displaySendData(gs0);
        }
        i++;
        for(int j=0; j<MAXCOLS/2; j++)
        {
            displaySendData(gs1);
        }
    }
    displaySendEnd();
}

//--------------------------------------------------------------------------
void Something1()
{
  Set_Column_Address(0x00, MAXCOLS-1);
  Set_Row_Address(0x00, MAXROWS-1);
  Set_Write_RAM();

  for(int i=0; i<50; i++)
  {
    for(int j=0; j<1; j++)
    {
      displaySend(SEND_DAT, 0xF0);
      displaySend(SEND_DAT, 0x0F);
      delay(1);
    }
    delay(15);
  }

  Set_Column_Address(0x01, MAXCOLS-1);
  Set_Row_Address(0x01, MAXROWS-1);
  Set_Write_RAM();

  for(int i=0; i<50; i++)
  {
    for(int j=0; j<1; j++)
    {
      displaySend(SEND_DAT, 0xF0);
      displaySend(SEND_DAT, 0x0F);
      delay(1);
    }
    delay(15);
  }

  Set_Column_Address(0x00, MAXCOLS-1);
  Set_Row_Address(0x02, MAXROWS-1);
  Set_Write_RAM();
  for(int i=0; i<28; i++)
  {
    for(int j=0; j<1; j++)
    {
      displaySend(SEND_DAT, 0xF0);
      displaySend(SEND_DAT, 0x0F);
      delay(1);
    }
    delay(15);
  }
  displaySend(SEND_DAT, 0xFF);
  displaySend(SEND_DAT, 0xFF);

  Set_Column_Address(0x00, MAXCOLS-1);
  Set_Row_Address(0x03, MAXROWS-1);
  Set_Write_RAM();
  for(int i=0; i<29; i++)
  {
    for(int j=0; j<1; j++)
    {
      displaySend(SEND_DAT, 0xF0);
      displaySend(SEND_DAT, 0x0F);
      delay(1);
    }
    delay(15);
  }
  displaySend(SEND_DAT, 0xFF);
  displaySend(SEND_DAT, 0xFF);

  Set_Column_Address(0x00, MAXCOLS-1);
  Set_Row_Address(0x04, MAXROWS-1);
  Set_Write_RAM();
  for(int i=0; i<148; i++)
  {
    for(int j=0; j<1; j++)
    {
      displaySend(SEND_DAT, 0xF0);
      displaySend(SEND_DAT, 0x0F);
      delay(1);
    }
    delay(15);
  }

  Set_Column_Address(0x00, MAXCOLS-1);
  Set_Row_Address(0x05, MAXROWS-1);
  Set_Write_RAM();
  for(int i=0; i<148; i++)
  {
    for(int j=0; j<1; j++)
    {
      displaySend(SEND_DAT, 0xF0);
      displaySend(SEND_DAT, 0x0F);
      delay(1);
    }
    delay(15);
  }
}

//--------------------------------------------------------------------------
//##########################################################################
//--------------------------------------------------------------------------
void setup()
{
    InitStructsAndPins();
    digitalWrite(PIN_RS,  LOW);
    digitalWrite(PIN_RES, HIGH);
    delay(1000);
    
    if (SIG_MODE == MODE_SPI4W)
    {
        SPI.begin();
        SPI.setBitOrder(MSBFIRST);
        SPI.setClockDivider(SPI_CLOCK_DIV2);
    }
    
    Init_Device();
    ClearDisplay();
}

//--------------------------------------------------------------------------
void loop()
{ 
    // displaySend(SEND_CMD, 0xA4); // Entire Display OFF, all pixels turns OFF in GS level 0
    // displaySend(SEND_CMD, 0xA5); // Entire Display ON,  all pixels turns ON  in GS level 15

    // Set_Column_Address(0x1C, 0x5B); //0x1C - 5Bd + 1 = 64 16-bit (quad pixel) values
    // Set_Row_Address(0x00,0x3F); // 0 - 63d = 64 rows!
    
    for(int i=0; i<16; i++)
    {
        CheckerboardOdd(i);
        CheckerboardEven(i);
    }
    delay(1000);

    FillDisplay();
    delay(1000);

    ClearDisplay();
    Something1();
    delay(3000);

    ClearDisplay();
}

//--------------------------------------------------------------------------

