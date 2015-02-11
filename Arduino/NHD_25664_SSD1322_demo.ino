//---------------------------------------------------------
/*
NHD_2_8_25664_mega.ino
Program for writing to Newhaven Display 256x64 graphic OLED with SSD1322 controller (serial mode)

(c)2014 Mike LaVine - Newhaven Display International, LLC. 

        This program is free software; you can redistribute it and/or modify
        it under the terms of the GNU General Public License as published by
        the Free Software Foundation; either version 2 of the License, or
        (at your option) any later version.

        This program is distributed in the hope that it will be useful,
        but WITHOUT ANY WARRANTY; without even the implied warranty of
        MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
        GNU General Public License for more details.
*/
//---------------------------------------------------------


int RS = 30;      // RS (D/C) signal connected to Arduino digital pin 30 (can be tied low for 3-wire SPI)
int RW = 31;      // /WR (R/W) signal connected to Arduino digital pin 31 (can be tied low)
int E = 32;      // /RD (E) signal connected to Arduino digital pin 32  (can be tied low)
int RES = 33;     // /RES signal connected to Arduino digital pin 33  
int CS = 34;     // /CS signal conencted to Arduino digital pin 34
int SCLK = 22;   // SCLK signal connected to Arduino digital pin 22
int SDIN = 23;   // SDIN signal connected to Arduino digital pin 23

void comm_out(unsigned char c)
{
  unsigned char i;
  digitalWrite(CS, LOW);
  digitalWrite(SDIN, LOW);
  digitalWrite(SCLK, LOW);
  digitalWrite(SCLK, HIGH);
  for(i=0;i<8;i++)
  {
    digitalWrite(SCLK, LOW);
    if((c&0x80)>>7==1)
    {
      digitalWrite(SDIN, HIGH);
    }
    else
    {
      digitalWrite(SDIN, LOW);
    }
    c = c << 1;
    digitalWrite(SCLK, HIGH);
  }
  digitalWrite(CS, HIGH);
}

void data_out(unsigned char d)
{
  unsigned char i;
  digitalWrite(CS, LOW);
  digitalWrite(SDIN, HIGH);
  digitalWrite(SCLK, LOW);
  digitalWrite(SCLK, HIGH);
  for(i=0;i<8;i++)
  {
    digitalWrite(SCLK, LOW);
    if((d&0x80)>>7==1)
    {
      digitalWrite(SDIN, HIGH);
    }
    else
    {
      digitalWrite(SDIN, LOW);
    }
    d = d << 1;
    digitalWrite(SCLK, HIGH);
  }
  digitalWrite(CS, HIGH);
}

void Checkerboard_25664()
{
        unsigned int i, j;
	
	Set_Column_Address_25664(0x00,0x77);
	Set_Row_Address_25664(0x00,0x7F);
	Set_Write_RAM_25664();

	for(i=0;i<64;i++)
	{
		for(j=0;j<120;j++)
		{
			data_out(0xF0);
			data_out(0xF0);
		}
		for(j=0;j<120;j++)
		{
			data_out(0x0F);
			data_out(0x0F);
		}
	}
}
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------

void Set_Column_Address_25664(unsigned char a, unsigned char b)
{
	comm_out(0x15);			// Set Column Address
	data_out(a);				//   Default => 0x00
	data_out(b);				//   Default => 0x77
}
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------

void Set_Row_Address_25664(unsigned char a, unsigned char b)
{
	comm_out(0x75);			// Set Row Address
	data_out(a);				//   Default => 0x00
	data_out(b);				//   Default => 0x7F
}
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------

void Set_Write_RAM_25664()
{
	comm_out(0x5C);			// Enable MCU to Write into RAM
}
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------

void setup()
{
  DDRC = 0xFF;         //set PORTC of Arduino (control signals) as output
  DDRA = 0xFF;         //set PORTA of Arduino (data signals) as output
  PORTA = 0x03;        //set DB7-DB2 to LOW, and DB1-DB0 to HIGH
  digitalWrite(RS, LOW);
  digitalWrite(RW, LOW);
  digitalWrite(E, LOW);
  digitalWrite(RES, HIGH);
  delay(1000);
  
  comm_out(0xFD);
  data_out(0x12);
  
  comm_out(0xB3);
  data_out(0xD0);
  comm_out(0xCA);
  data_out(0x3F);
  comm_out(0xA2);
  data_out(0x00);
  comm_out(0xA1);
  data_out(0x00);
  comm_out(0xA0);
  data_out(0x14);
  data_out(0x11);
  comm_out(0xB5);
  data_out(0x00);
  comm_out(0xAB);
  data_out(0x01);
  comm_out(0xB4);
  data_out(0xA0);
  data_out(0xB5);
  comm_out(0xC1);
  data_out(0x7F);
  comm_out(0xC7);
  data_out(0x0F);
  comm_out(0xB9);
  comm_out(0xB1);
  data_out(0xE2);
  comm_out(0xD1);
  data_out(0xA2);
  data_out(0x20);
  comm_out(0xBB);
  data_out(0x1F);
  comm_out(0xB6);
  data_out(0x08);
  comm_out(0xBE);
  data_out(0x07);
  comm_out(0xA6);
  comm_out(0xA9);
  comm_out(0xAF);
  delay(10);
}

void loop()
{ 
  Checkerboard_25664();
  delay(1000);
}
