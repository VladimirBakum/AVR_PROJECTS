// PCF8833 Routines For Nokia 6100/6610 LCD
// NOT Compatible with Epson S1D15G10D08B000!!!
// By (R)soft 7.12.2013 - 12.1.2014
// ��� ������ ������������ ������� �������� �������� ��
// ���������� ������� ������������ - CLK ��������
// ������ �� ����������, �������� ��������� ����.
// ����������� ������� - 2.5 �
// For 6100 LCD RDDIDIF = 29,83,03 (EPSON???)
//======================================================

//=============================================================
// ������ ����� �� ���
// ������� MSB (������ ���� ������� ���)
// �������� ������ 8 ���, ��� ������� RDDIDIF
// EPSON RDDIDIF 29
// PHILIPS RDDIDIF 45
// LEADIS LDS176 RDDIDIF 45xx03
// SPFD54124B RDDID 388x4F
unsigned char pcf8833_read (void)
{
  DAT_1; // Pullup off DAT pin
  DDR_DAT=0; // Pin DAT for Input
  unsigned char counter;
  unsigned char result=0;
  unsigned char mask=0x80;
  for (counter=0; counter<8; counter++)
  {
    delay_us(4);
    CLK_1; // ������ �� ���������� SCLK, ������ �����������
    delay_us(4);
    if (PINB&(1<<PB6)) // ����� �������� ����
      {
        result |= mask; // ���� ���� ������� �� ������ OR ���������� � �����
      }
    mask>>=1;   // ������� �����
    CLK_0; // ���������� SCLK ������� � �������� ���������
  }
  
  DDR_DAT=1; // Pin DAT for Output
  return (result);
}

//=============================================================
// ������ ����� � ���
// ������� MSB (������ ���� ������� ���)

void pcf8833_write (unsigned char ch)
{
  unsigned char i;
  for (i=8;i;i--)
  {
    if (ch&0x80) // ������������� ����� ������� ��� � �����
      {
        DAT_1; // � ������������ SI � ������������
      }
    else  
      {
        DAT_0;
      }
    CLK_1; // ������ �� ���������� SCLK
    CLK_0; // ���������� SCLK ������� � �������� ���������
    ch<<=1; // �������� ���� �����
  }
}

//===========================================
// �������� ������� � ���

void pcf8833_cmd(unsigned char cmd)
{
  DAT_0; // ������� ������� A0=0
  CLK_1;  // �������� ������ ���
  CLK_0;
  pcf8833_write(cmd);
}

//=============================================
// �������� ������ � ���

void pcf8833_data(unsigned char dat)
{
  DAT_1; // ������� ������� A0=1
  CLK_1;  // �������� ������ ���
  CLK_0;
  pcf8833_write(dat);
}

//---------------------------------------------------
void pcf8833_cls(unsigned int color) 
{ 
    unsigned int i; // loop counter 
    // Row address set (command 0x2B) 
    pcf8833_cmd(P_PASET); 
    pcf8833_data(0); 
    pcf8833_data(PCF8833_PIXELY-1); 
    // Column address set (command 0x2A) 
    pcf8833_cmd(P_CASET); 
    pcf8833_data(0); 
    pcf8833_data(PCF8833_PIXELX-1); 
    // set the display memory
    pcf8833_cmd(P_RAMWR); 

    for (i = 0; i < ((PCF8833_PIXELX * PCF8833_PIXELY) / 2); i++) 
    { 
       pcf8833_data(color >> 4); 
       pcf8833_data(((color & 0xF) << 4) | ((color >> 8) & 0xF)); 
       pcf8833_data(color); 
    } 

} 

//=============================================================
// ������������� LCD

void pcf8833_init (void)
{
  DDR_CLK=1;
  DDR_DAT=1;
  DDR_RESX=1;
  CLK_0;
  
  RESX_0;  // Reset LCD
  delay_ms(10);
  RESX_1;  // Return Reset OFF
  delay_ms(120);

  pcf8833_cmd(P_SWRESET); 
  delay_ms(120);
  
    // Sleep out (command 0x11) 
  pcf8833_cmd(P_SLEEPOUT); 
    // Inversion on (command 0x21) 
//  pcf8833_cmd(P_INVOFF); // seems to be required for this controller 
    // Color Interface Pixel Format (command 0x3A) 
  pcf8833_cmd(P_COLMOD); 
  pcf8833_data(0x03); // 0x03 = 12 bits-per-pixel 
    // Memory access controler (command 0x36) 
  pcf8833_cmd(P_MADCTL); 
  // For Nokia 6100 - 0xC0
  // For Nokia 6030/2255/2610/2626/5140i - 0xC0
  pcf8833_data(0xC0); // 0xC0 = mirror x and y, reverse rgb 

  // Write contrast (command 0x25) 
  // ��� ���������� ������� +3,3V �������� 0x37
  // ��� ���������� ������� +2,5V �������� 0x3D
  pcf8833_cmd(P_SETCON); 
  pcf8833_data(0x3D); // contrast
    delay_ms(50); 
    // Display On (command 0x29) 
  pcf8833_cmd(P_DISPON); 

}

//-----------------------------------------
// ��������� ���������

void pcf8833_setxy(unsigned char x, unsigned char y) 
{ 
    // Page (Row) address set (command 0x2B) 
    pcf8833_cmd(P_PASET); 
    pcf8833_data(y); 
    pcf8833_data(y); 
    // Column address set (command 0x2A) 
    pcf8833_cmd(P_CASET); 
    pcf8833_data(x); 
    pcf8833_data(x); 
} 

//-----------------------------------------
// ��������� �����

void pcf8833_pixel(unsigned char x, unsigned char y, unsigned int color) 
{ 
    // Page (Row) address set (command 0x2B) 
    pcf8833_cmd(P_PASET); 
    pcf8833_data(y); 
    pcf8833_data(y); 
    // Column address set (command 0x2A) 
    pcf8833_cmd(P_CASET); 
    pcf8833_data(x); 
    pcf8833_data(x); 
     
    pcf8833_cmd(P_RAMWR); 
    pcf8833_data((unsigned char)((color >> 4) & 0xFFFF)); 
    pcf8833_data((unsigned char)(((color & 0x0F) << 4) | 0x00)); 
    pcf8833_cmd(P_NOP); 
} 

//------------------------------------------------
// ������ �������

void pcf8833_putchar(unsigned char c, unsigned int x, unsigned int y, 
                     unsigned char size, unsigned int fColor, unsigned int bColor) 
{ 
    unsigned char i,j; 
    unsigned char nCols; 
    unsigned char nRows; 
    unsigned char nBytes; 
    unsigned char PixelRow; 
    unsigned char Mask; 
    unsigned int Word0; 
    unsigned int Word1; 
    unsigned char pChar[16]; 
    // get pointer to the beginning of the selected font table 
    // & 
    // get the nColumns, nRows and nBytes 
    switch(size) 
    { 
       case 0: 
          nCols = FONT6x8[0][0]; 
          nRows = FONT6x8[0][1]; 
          nBytes= FONT6x8[0][2]; 
          memcpy_P(pChar, FONT6x8[c - 0x1F], nBytes); 
       break; 
       
       case 1: 
          nCols = FONT8x8[0][0]; 
          nRows = FONT8x8[0][1]; 
          nBytes= FONT8x8[0][2]; 
          memcpy_P(pChar, FONT8x8[c - 0x1F], nBytes); 
       break; 
       
       case 2: 
          nCols = FONT8x16[0][0]; 
          nRows = FONT8x16[0][1]; 
          nBytes= FONT8x16[0][2]; 
          memcpy_P(pChar, FONT8x16[c - 0x1F], nBytes); 
       break; 
    } 
    // Page (Row) address set (command 0x2B) 
    pcf8833_cmd(P_PASET); 
    pcf8833_data(y); 
    pcf8833_data(y + nRows - 1); 
    // Column address set (command 0x2A) 
    pcf8833_cmd(P_CASET); 
    pcf8833_data(x); 
    pcf8833_data(x + nCols - 1); 

    // WRITE MEMORY 
    pcf8833_cmd(P_RAMWR); 
    // loop on each row, working backwards from the bottom to the top 
    for (i=0; i < nRows; i++) //for (i = nRows - 1; i != 255; i--) 
    { 
       // copy pixel row from font table and then decrement row 
       PixelRow = pChar[i]; //PixelRow = *pChar--; 
       // loop on each pixel in the row (left to right) 
       // Note: we do two pixels each loop 
       Mask = 0x80; 
       for (j = 0; j < nCols; j += 2) 
       { 
          // if pixel bit set, use foreground color; else use the background color 
          // now get the pixel color for two successive pixels 
          if ((PixelRow & Mask) == 0) 
             Word0 = bColor; 
          else 
             Word0 = fColor; 
          Mask = Mask >> 1; 
          if ((PixelRow & Mask) == 0) 
             Word1 = bColor; 
          else 
             Word1 = fColor; 
          Mask = Mask >> 1; 
          // use this information to output three data bytes 
          pcf8833_data((Word0 >> 4) & 0xFF); 
          pcf8833_data(((Word0 & 0xF) << 4) | ((Word1 >> 8) & 0xF)); 
          pcf8833_data(Word1 & 0xFF); 
       } 
    } 
    // terminate the Write Memory command 
    pcf8833_cmd(P_NOP); 

} 

//------------------------------------------
// ����� ��������� �������

void pcf8833_putchar32(unsigned char ch, unsigned char x, unsigned char y, 
                       unsigned int fColor, unsigned int bColor) 
{ 
  unsigned int addr;
  unsigned char len; // ������ ������� � ��������
  ch=ch-0x20; // ������������ ������� �������� ��� ������ 0x00..0x2F
  addr=ch;
  addr=addr*81; // ��������� �� ����� ������� � ������
  len=(arial32[addr++]); 

  unsigned char i,j; 

  unsigned char byte; 
  unsigned char mask; 
  unsigned int word0; 
  unsigned int word1; 

  // ������ ����������� ������ � ��������������� �� ������������
  pcf8833_cmd(P_MADCTL); 
  // For Nokia 6100 - 0xE0
  // For Nokia 6030/2255/2610/2626/5140i - 0x60
  pcf8833_data(0xE0); // 0xC0 = mirror x and y, reverse rgb 


  // Page (Row) address set (command 0x2B) 
  pcf8833_cmd(P_PASET); 
  pcf8833_data(y); 
  pcf8833_data(y + 31); // nRows

  // Column address set (command 0x2A) 
  pcf8833_cmd(P_CASET); 
  pcf8833_data(x); 
  pcf8833_data(x + len - 1); // nCols


  // WRITE MEMORY 
  pcf8833_cmd(P_RAMWR); 

  // loop on each row, working backwards from the bottom to the top 
  for (i=0; i < len*4; i++) //
    { 
       // copy pixel row from font table and then decrement row 
       byte = arial32[addr++]; //PixelRow = *pChar--; 
       // loop on each pixel in the row (left to right) 
       // Note: we do two pixels each loop 
       mask = 0x01; 
       for (j = 0; j < 8; j += 2) 
       { 
          // if pixel bit set, use foreground color; else use the background color 
          // now get the pixel color for two successive pixels 
          if ((byte & mask) == 0) 
             word0 = bColor; 
          else 
             word0 = fColor; 
          mask = mask << 1; 
          if ((byte & mask) == 0) 
             word1 = bColor; 
          else 
             word1 = fColor; 
          mask = mask << 1; 
          // use this information to output three data bytes 
          pcf8833_data((word0 >> 4) & 0xFF); 
          pcf8833_data(((word0 & 0xF) << 4) | ((word1 >> 8) & 0xF)); 
          pcf8833_data(word1 & 0xFF); 
//          delay_ms(50);
       } 
    } 
    // terminate the Write Memory command 
    pcf8833_cmd(P_NOP); 
/*
    pcf8833_cmd(P_MADCTL); 
    // For Nokia 6100 - 0xC0
    // For Nokia 6030/2255/2610/2626/5140i - 0xC0
    pcf8833_data(0xC0); // ������ ����������� �������
*/
  
} 

//-------------------------------------------------------------
//

void pcf8833_putstr(char *pString, unsigned char x, unsigned char y, 
                    unsigned char Size, unsigned int fColor, unsigned int bColor) 
{ 
    // loop until null-terminator is seen 
    while (*pString != '\0') 
    { 
       // draw the character 
       pcf8833_putchar(*pString++, x, y, Size, fColor, bColor); 
       // advance the y position 
       if (Size == SMALL) 
          x = x + 6; 
       else 
          x = x + 8; 
       // bail out if y exceeds 131 
       if (y > 131) break; 
    } 
} 

//-------------------------------------------------
// ********************************************************
// LCDSetLine.c 
// 
// Draws a line in the specified color from (x0,y0) to (x1,y1) 
// 
// Inputs: x = row address (0 .. 131) 
// y = column address (0 .. 131) 
// color = 12-bit color value rrrrggggbbbb 
// rrrr = 1111 full red 
// : 
// 0000 red is off 
// 
// gggg = 1111 full green 
// : 
// 0000 green is off 
// 
// bbbb = 1111 full blue 
// : 
// 0000 blue is off 
// 
// Returns: nothing 
// 
// Note: good write-up on this algorithm in Wikipedia (search for Bresenham's line algorithm) 
// see lcd.h for some sample color settings 
// 
// Authors: Dr. Leonard McMillan, Associate Professor UNC 
// Jack Bresenham IBM, Winthrop University (Father of this algorithm, 1962) 
// 
// Note: taken verbatim from Professor McMillan's presentation: 
// http://www.cs.unc.edu/~mcmillan/comp136/Lecture6/Lines.html 
// 
// *****************************************************************
void pcf8833_draw(unsigned char x0, unsigned char y0, unsigned char x1, 
                     unsigned char y1, unsigned int color) 
{ 
 int dy = y1 - y0; 
 int dx = x1 - x0; 
 int stepx, stepy; 

 if (dy < 0) 
   { dy = -dy; 
      stepy = -1; 
   } 
 else { stepy = 1; } 

 if (dx < 0) { dx = -dx; stepx = -1; } else { stepx = 1; } 
 dy <<= 1; // dy is now 2*dy 
 dx <<= 1; // dx is now 2*dx 
 
 pcf8833_pixel(x0, y0, color); 

 if (dx > dy) 
  { 
   int fraction = dy - (dx >> 1); // same as 2*dy - dx 
   while (x0 != x1) 
   { 
     if (fraction >= 0) 
     { 
       y0 += stepy; 
       fraction -= dx; // same as fraction -= 2*dx 
     } 
     x0 += stepx; 
     fraction += dy; // same as fraction -= 2*dy 
     pcf8833_pixel(x0, y0, color); 
    } 
   } 
 else 
  { 
   int fraction = dx - (dy >> 1); 
   while (y0 != y1) 
   { 
     if (fraction >= 0) 
     { 
       x0 += stepx; 
       fraction -= dy; 
     } 
     y0 += stepy; 
     fraction += dx; 
     pcf8833_pixel(x0, y0, color); 
    } 
  } 
} 

//------------------------------------------------------------------
// LCDSetRect.c 
// 
// Draws a rectangle in the specified color from (x1,y1) to (x2,y2) 
// Rectangle can be filled with a color if desired 
// 
// Inputs: x = row address (0 .. 131) 
// y = column address (0 .. 131) 
// fill = 0=no fill, 1-fill entire rectangle 
// color = 12-bit color value for lines rrrrggggbbbb 
// rrrr = 1111 full red 
// : 
// 0000 red is off 
// 
// gggg = 1111 full green 
// : 
// 0000 green is off 
// 
// bbbb = 1111 full blue 
// : 
// 0000 blue is off 
// 
// Returns: nothing 
// 
// Notes: 
// 
// The best way to fill a rectangle is to take advantage of the "wrap-around" featute 
// built into the Epson S1D15G00 controller. By defining a drawing box, the memory can 
// be simply filled by successive memory writes until all pixels have been illuminated. 
// 
// 1. Given the coordinates of two opposing corners (x0, y0) (x1, y1) 
// calculate the minimums and maximums of the coordinates 
// 
// xmin = (x0 <= x1) ? x0 : x1; 
// xmax = (x0 > x1) ? x0 : x1; 
// ymin = (y0 <= y1) ? y0 : y1; 
// ymax = (y0 > y1) ? y0 : y1; 
// 
// 2. Now set up the drawing box to be the desired rectangle 
// 
// WriteSpiCommand(PASET); // set the row boundaries 
// WriteSpiData(xmin); 
// WriteSpiData(xmax); 
// WriteSpiCommand(CASET); // set the column boundaries 
// WriteSpiData(ymin); 
// WriteSpiData(ymax); 
// 
// 3. Calculate the number of pixels to be written divided by 2 
// 
// NumPixels = ((((xmax - xmin + 1) * (ymax - ymin + 1)) / 2) + 1) 
// 
// You may notice that I added one pixel to the formula. 
// This covers the case where the number of pixels is odd and we 
// would lose one pixel due to rounding error. In the case of 
// odd pixels, the number of pixels is exact. 
// in the case of even pixels, we have one more pixel than 
// needed, but it cannot be displayed because it is outside 
// the drawing box. 
// 
// We divide by 2 because two pixels are represented by three bytes. 
// So we work through the rectangle two pixels at a time. 
// 
// 4. Now a simple memory write loop will fill the rectangle 
// 
// for (i = 0; i < ((((xmax - xmin + 1) * (ymax - ymin + 1)) / 2) + 1); i++) { 
// WriteSpiData((color >> 4) & 0xFF); 
// WriteSpiData(((color & 0xF) << 4) | ((color >> 8) & 0xF)); 
// WriteSpiData(color & 0xFF); 
// } 
// 
// 
// In the case of an unfilled rectangle, drawing four lines with the Bresenham line 
// drawing algorithm is reasonably efficient. 
// 
// 
// Author: James P Lynch August 30, 2007 
// ***************************************************************************************** 
 
void pcf8833_setrect(unsigned char x0, unsigned char y0, unsigned char x1, 
                     unsigned char y1, unsigned char fill, unsigned int color) 
{ 
 int xmin, xmax, ymin, ymax; 
 int i; 
 
 // ���� 1, �� ������������� �����������
 if (fill == 1) 
 { 
   // best way to create a filled rectangle is to define a drawing box 
   // and loop two pixels at a time 
 
   // calculate the min and max for x and y directions 
   xmin = (x0 <= x1) ? x0 : x1; 
   xmax = (x0 > x1) ? x0 : x1; 
   ymin = (y0 <= y1) ? y0 : y1; 
   ymax = (y0 > y1) ? y0 : y1; 
 
   // specify the controller drawing box according to those limits 
   // Row address set (command 0x2B) 
   pcf8833_cmd(P_PASET); 
   pcf8833_data(ymin); 
   pcf8833_data(ymax); 
 
   // Column address set (command 0x2A) 
   pcf8833_cmd(P_CASET); 
   pcf8833_data(xmin); 
   pcf8833_data(xmax); 
   
   // WRITE MEMORY 
   pcf8833_cmd(P_RAMWR); 
 
   // loop on total number of pixels / 2 
   for (i = 0; i < ((((xmax - xmin + 1) * (ymax - ymin + 1)) / 2) + 130); i++) 
     { 
       // use the color value to output three data bytes covering two pixels 
       pcf8833_data((color >> 4) & 0xFF); 
       pcf8833_data(((color & 0xF) << 4) | ((color >> 8) & 0xF)); 
       pcf8833_data(color & 0xFF); 
     } 
 
   } 
 else 
 { 
  // best way to draw un unfilled rectangle is to draw four lines 
   pcf8833_draw(x0, y0, x1, y0, color); 
   pcf8833_draw(x0, y1, x1, y1, color); 
   pcf8833_draw(x0, y0, x0, y1, color); 
   pcf8833_draw(x1, y0, x1, y1, color); 
 }
} 

// ************************************************************************************* 
// LCDSetCircle.c 
// 
// Draws a line in the specified color at center (x0,y0) with radius 
// 
// Inputs: x0 = row address (0 .. 131) 
// y0 = column address (0 .. 131) 
// radius = radius in pixels 
// color = 12-bit color value rrrrggggbbbb 
// 
// Returns: nothing 
// 
// Author: Jack Bresenham IBM, Winthrop University (Father of this algorithm, 1962) 
// 
// Note: taken verbatim Wikipedia article on Bresenham's line algorithm 
// http://www.wikipedia.org 
// 
// ************************************************************************************* 
 
void pcf8833_circle(unsigned char x0, unsigned char y0, 
                    unsigned char radius, unsigned int color) 
{ 
 int f = 1 - radius; 
 int ddF_x = 0; 
 int ddF_y = -2 * radius; 
 int x = 0; 
 int y = radius; 
 
 pcf8833_pixel(x0, y0 + radius, color); 
 pcf8833_pixel(x0, y0 - radius, color); 
 pcf8833_pixel(x0 + radius, y0, color); 
 pcf8833_pixel(x0 - radius, y0, color); 
 
 while(x < y) 
 { 
   if(f >= 0) 
   { 
     y--; 
     ddF_y += 2; 
     f += ddF_y; 
   } 
   x++; 
   ddF_x += 2; 
   f += ddF_x + 1; 
   pcf8833_pixel(x0 + x, y0 + y, color); 
   pcf8833_pixel(x0 - x, y0 + y, color); 
   pcf8833_pixel(x0 + x, y0 - y, color); 
   pcf8833_pixel(x0 - x, y0 - y, color); 
   pcf8833_pixel(x0 + y, y0 + x, color); 
   pcf8833_pixel(x0 - y, y0 + x, color); 
   pcf8833_pixel(x0 + y, y0 - x, color); 
   pcf8833_pixel(x0 - y, y0 - x, color); 
 } 
} 

//--------------------------------------------------------
//  id1=pcf8833_read_id(P_RDID1);
//  id2=pcf8833_read_id(P_RDID2);
//  id3=pcf8833_read_id(P_RDID3);

unsigned char pcf8833_read_id(unsigned char id_command)
{
  pcf8833_cmd(id_command); // Read ID
  CLK_1; // Dummy clock
  delay_us(5);
  CLK_0;
  unsigned char id;
  id=pcf8833_read();
  return (id);
}

//----------------------------------------------
// Get ID of Driver Chip
unsigned long pcf8833_read_id0 (void)
{
  unsigned long result=0;
  pcf8833_cmd(P_RDDIDIF); // Read ID
  CLK_1; // Dummy clock
  delay_us(5);
  CLK_0;
  unsigned char id;
  id=pcf8833_read();
  result|=id;
  id=pcf8833_read();
  result=(result<<8)|id;
  id=pcf8833_read();
  result=(result<<8)|id;
  return (result);
}