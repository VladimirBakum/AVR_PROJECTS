// ======================= ST7628 Routine ============================
// 3-Line SPI (/CS SCLK SDATA)
// ********************************************************************
// Writen by (R)soft 12.12.2013 Last Edition: 12 Dec 2013
// Version 1.0
// Tested on Mega2561 @ 14.318 MHz
// Testing on Nokia 1600 LCD Module. Resolution 96x68 pixels. 
// ********************************************************************


/*

�� ����������� ������ ��������� � ...

��������� 12.12.2013: ����������� ���������� ������� ��� Nokia 1600 - ?.? V

*/


//=============================================================
// ������ ����� � ���
// ������� MSB (������ ���� ������� ���)

void st7628_write (unsigned char ch)
{
  unsigned char i;
  for (i=8;i;i--)
  {
    if (ch&0x80) // ������������� ����� ������� ��� � �����
      {
        SI_1; // � ������������ SDA � ������������
      }
    else  
      {
        SI_0;
      }
//    delay_us(PCF8814_DELAY);
    SCL_1; // ������ �� ���������� SCLK
//    delay_us(PCF8814_DELAY);
    SCL_0; // ���������� SCLK ������� � �������� ���������
//    delay_us(PCF8814_DELAY);
    ch<<=1; // �������� ���� �����
  }
}

//===========================================
// �������� ������� � ���

void st7628_cmd(unsigned char cmd)
{
  SI_0; // ������� ������� D/C=0
  SCL_1; // �������� ������ ���
  SCL_0; // ������� ����� � �������� ���������
  st7628_write(cmd); // �������� �����
}

//=============================================
// �������� ������ � ���

void st7628_data(unsigned char dat)
{
  SI_1; // ������� ������ D/C=1
  SCL_1; // �������� ������ ���
  SCL_0; // ������� ����� � �������� ���������
  st7628_write(dat);
}



//===========================================================================
// ��������� ������ � ���

void st7628_set_addr(unsigned char x, unsigned char y)
{
  if (y > (ST7628_ROW-1)) 
    {
      y = (ST7628_ROW-1); // �������� ������������� �������� ������ (0..8)
    }
  if (x > (ST7628_PIXELX-1)) 
    {
      x = (ST7628_PIXELX-1); // �������� ������������� �������� �������
    }
//  sed_x = x;
//  sed_y = y;

  st7628_cmd(ST7628_CASET); // ��������� X=0 Col address set
  st7628_data(x);
  st7628_data(ST7628_PIXELX-1);

  st7628_cmd(ST7628_RASET); // Y=0 Row address set
  st7628_data(y);
  st7628_data(ST7628_PIXELY-1);
}



//============================================================
// ��������� ��������� ������ x,y

void st7628_gotoxy(unsigned char x, unsigned char y)
{
  st7628_set_addr((x*6),y); // set_addr(x*6,y)
}



//==========================================================================
// ����� ������� �� ��� 

void st7628_putchar(unsigned char ch)
{
  unsigned int addr;

  // ���������� ������ ������ �������
  ch=ch-0x20; // ������������ ������� �������� ��� ������ 0x00..0x1F
  addr=ch;
  addr+=(addr<<2); // *5
  // ����� ���� ����
  for (ch=0; ch<5; ch++)  
    {  
      st7628_data(font5x7[addr++]); 
    }
  st7628_data(0); // ����� ���������� ����� ���������
}

/*
//==========================================================================
// ����� ������� �� ��� 20x32
// ������ 20 ����, ������ - 4 ����� (4*8)

void pcf8814_putchar32(unsigned char ch, unsigned char x, unsigned char y)
{
  unsigned int addr;
  unsigned char len; // ������ ������� � ��������
  unsigned char lowadd, hiadd; // ���������� ���������� � ������
  ch=ch-0x20; // ������������ ������� �������� ��� ������ 0x00..0x2F
  addr=ch;
  addr=addr*81; // ��������� �� ����� ������� � ������
  len=(arial32[addr++]); 
  for (ch=0; ch<len; ch++)  
    {  
      lowadd=(x+ch&0x0F);
      hiadd=((x+ch>>4)&0x0F);
      pcf8814_cmd(PCF8814_SETYADDR|y); // ��������� Y=0
      pcf8814_cmd(PCF8814_SETXADDR_LOWER | lowadd);
      pcf8814_cmd(PCF8814_SETXADDR_UPPER | hiadd);
      pcf8814_data(arial32[addr++]); 
      pcf8814_cmd(PCF8814_SETYADDR|y+1); // ��������� Y=1
      pcf8814_cmd(PCF8814_SETXADDR_LOWER | lowadd);
      pcf8814_cmd(PCF8814_SETXADDR_UPPER | hiadd);
      pcf8814_data(arial32[addr++]); 
      pcf8814_cmd(PCF8814_SETYADDR|y+2); // ��������� Y=2
      pcf8814_cmd(PCF8814_SETXADDR_LOWER | lowadd);
      pcf8814_cmd(PCF8814_SETXADDR_UPPER | hiadd);
      pcf8814_data(arial32[addr++]); 
      pcf8814_cmd(PCF8814_SETYADDR|y+3); // ��������� Y=3
      pcf8814_cmd(PCF8814_SETXADDR_LOWER | lowadd);
      pcf8814_cmd(PCF8814_SETXADDR_UPPER | hiadd);
      pcf8814_data(arial32[addr++]); 
    }
}

*/


//====================================
// ������� ������

void st7628_cls(unsigned char colour)
{
  unsigned int i;
  st7628_cmd(ST7628_CASET); // ��������� X=0 Col address set
  st7628_data(0x00);
  st7628_data(ST7628_PIXELX-1);

  st7628_cmd(ST7628_RASET); // Y=0 Row address set
  st7628_data(0x00);
  st7628_data(ST7628_PIXELY-1);

  st7628_cmd(ST7628_RAM_WR);
  
  for (i=0; i<(ST7628_PIXELX * (ST7628_ROW+1)); i++) // ���� ��� ������ �����
    {
      st7628_data(colour); // ������ � ��� LCD �����
    }

  // ����� ������� ������, ���������� - ������� ������ ������ �����

}

/*
//=========================================
// ����� ������ (������ � ���)

void st7628_puts (unsigned char *s)
{
  while (*s) // �� ��� ��� ���� �� ���� - �������
    st7628_putchar(*s++);
}

*/

/*
//=========================================
// ������ �������� ��������

void pcf8814_set_inverse(unsigned char inv)
{
//  sed_inverse=inv; // �������� �������� ���������� � ���������� ����������
  if (inv)
    {
      pcf8814_cmd(PCF8814_DISPLAY_REVERSE);
    }
  else
    {
      pcf8814_cmd(PCF8814_DISPLAY_NORMAL);
    }
}
*/


/*
//=========================================
// ��������/��������� �������

void pcf8814_set_power(unsigned char power)
{
  if (power)
    {
      pcf8814_cmd(PCF8814_DISPLAY_ON);
    }
  else
    {
      pcf8814_cmd(PCF8814_DISPLAY_OFF);
    }
}
*/

//=============================================================
// ������������� LCD

void st7628_init (void)
{
  DDR_SCL=1;
  DDR_SI=1;
  DDR_XRST=1;
  SCL_0;

  delay_ms(50); // For Power Stable
  XRST_0;  // Reset LCD
  delay_ms(50);
  XRST_1;  // Return Reset OFF
//  pcf8814_cmd(PCF8814_SOFTWARE_RESET);
  delay_ms(50);
  
  st7628_cmd(ST7628_SW_RESET);  // Software reset
  delay_ms(50);

  st7628_cmd(ST7628_WR_CNTR);  // Contrast set
  st7628_data(0x39);

  st7628_cmd(ST7628_SLEEP_OUT);  // Sleep out

  st7628_cmd(ST7628_NOR_ON); 

  st7628_cmd(ST7628_INV_OFF); 

  st7628_cmd(ST7628_IDM_OFF); 

  st7628_cmd(ST7628_MADCTR);  // MAD set
  st7628_data(0x80);

  st7628_cmd(ST7628_DISPLAY_ON); 

  
//  st7628_cls(0x00); // ������ �����-��� � ������������� ����� ������ � ������
  
}


//====================================================================
// ������� �� ������ ������� CON �� ���

void st7628_update_console(void)
{
  unsigned char col,row;
  char *p;
  char c;  
  p=CON7628;

  for (row=0; row<ST7628_ROW; row++)
  {
    st7628_cmd(ST7628_CASET); // ��������� X=0 Col address set
    st7628_data(0x00);
    st7628_data(ST7628_PIXELX-1);

    st7628_cmd(ST7628_RASET); // Y=0 Row address set
    st7628_data(0x00);
    st7628_data(ST7628_PIXELY-1);

    for (col=0; col<ST7628_COL; col++)
      {
        c=*p++;
        if (c==0) 
          {
            c=0x20; // If End of Line (0x00) then "SPACE" character
          }
        st7628_cmd(ST7628_RAM_WR);
        st7628_putchar(c);
      }
   }
}


//==================================
// ������ ����� ������� ��������� ������
void st7628_clear_console (unsigned char byte)
{
 char i;
 char *p;
 p=CON7628;
 i=sizeof(CON7628);
 do
   {
     *p++=byte;
   }
  while (--i);
}

