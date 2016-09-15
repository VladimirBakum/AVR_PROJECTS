// ======================= PCF8814 Routine ============================
// 3-Line SPI (/CS SCLK SDATA)
// ********************************************************************
// Writen by (R)soft 21.10.2011 Last Edition: 8 Dec 2013
// Version 1.0
// Version 1.1 on 7.12.2013
// Tested on Mega2561 & Mega162.
// Testing on Nokia 6510 LCD Module. Resolution 96x65 pixels. 
// Testing on Nokia 1200 LCD Module 7.12.2013
// Built-in PCF8814 controller with 3-line SPI interface.
// Reference manual for controller - PCF8814.pdf
// ********************************************************************


/*

��� �������� ��������, ������� /RST � /CS (SCE) ����� ���� ������������ � �����������.
����������� � LCD �� Nokia 6510. /RST ���������� �� VCC, ��� ���� ����������
����������� �����. ������ ������ ��������� /SCE ���������� � GND.
����� �������, �������� ����� ��������� �� ���� �������� - SCLK & SDIN, ���
����� ������ ��� ����������, ����� ���-�� ��������� ��� ����������� ����������.
�� ����� ������� �������� ������������ �������� ����� ����. ����������� �� ����
�������� ����� �������� (3wire SPI) ������� �����, ��� i2c � ����� ��������� ���������.
����, �� ���� ���� � ����. ������ �� i2c ��� ����� � ������. � �������� ��������, ���
����� �����-������ ��� SPI ������������, ���� �� �������� ����� ������ �� ����������.
����� ���������, ��� ����� �� �� ����� ������� �� ���������� �.�. �������
�� ������ �������� ��������. ������ ����� �� �����. ����� �������, ��� ���
���������� ������� 1,8 ������ ��� �������� �� ����������, ���� � ������������
���������� ����������� ���������, ������������ ��������������.

�� ����������� ������ ��������� � SED15B1 � STE2007.

��������� 8.12.2013: ����������� ���������� ������� ��� Nokia 1200 - 2.4 V

*/


//=============================================================
// ������ ����� � ���
// ������� MSB (������ ���� ������� ���)

void pcf8814_write (unsigned char ch)
{
  unsigned char i;
  for (i=8;i;i--)
  {
    if (ch&0x80) // ������������� ����� ������� ��� � �����
      {
        SDIN_1; // � ������������ SDA � ������������
      }
    else  
      {
        SDIN_0;
      }
//    delay_us(PCF8814_DELAY);
    SCLK_1; // ������ �� ���������� SCLK
//    delay_us(PCF8814_DELAY);
    SCLK_0; // ���������� SCLK ������� � �������� ���������
//    delay_us(PCF8814_DELAY);
    ch<<=1; // �������� ���� �����
  }
}

//===========================================
// �������� ������� � ���

void pcf8814_cmd(unsigned char cmd)
{
  SDIN_0; // ������� ������� D/C=0
  SCLK_1; // �������� ������ ���
  SCLK_0; // ������� ����� � �������� ���������
  pcf8814_write(cmd); // �������� �����
}

//=============================================
// �������� ������ � ���

void pcf8814_data(unsigned char dat)
{
  SDIN_1; // ������� ������ D/C=1
  SCLK_1; // �������� ������ ���
  SCLK_0; // ������� ����� � �������� ���������
  pcf8814_write(dat);
}



//===========================================================================
// ��������� ������ � ���

void pcf8814_set_addr(unsigned char x, unsigned char y)
{
  if (y > (PCF8814_ROW-1)) 
    {
      y = (PCF8814_ROW-1); // �������� ������������� �������� ������ (0..8)
    }
  if (x > (PCF8814_PIXELX-1)) 
    {
      x = (PCF8814_PIXELX-1); // �������� ������������� �������� �������
    }
//  sed_x = x;
//  sed_y = y;
  pcf8814_cmd(PCF8814_SETYADDR | y);
  pcf8814_cmd(PCF8814_SETXADDR_LOWER | (x&0x0F));
  pcf8814_cmd(PCF8814_SETXADDR_UPPER | ((x>>4)&0x0F));
}



//============================================================
// ��������� ��������� ������ x,y

void pcf8814_gotoxy(unsigned char x, unsigned char y)
{
  pcf8814_set_addr((x*6),y); // set_addr(x*6,y)
}


//==========================================================================
// ����� ������� �� ��� � ���������� ������ 
//(��-�� �������� ��������������� �� �)

void pcf8814_putchar_x(unsigned char ch)
{
  unsigned int addr;

  // ���������� ������ ������ �������
  ch=ch-0x20; // ������������ ������� �������� ��� ������ 0x00..0x1F
  addr=ch;
  addr+=(addr<<2); // *5
  addr=addr+4;
  // ����� ���� ����
  for (ch=0; ch<5; ch++)  
    {  
      pcf8814_data(font5x7[addr--]); 
    }
  pcf8814_data(0); // ����� ���������� ����� ���������
}

//==========================================================================
// ����� ������� �� ��� 

void pcf8814_putchar(unsigned char ch)
{
  unsigned int addr;

  // ���������� ������ ������ �������
  ch=ch-0x20; // ������������ ������� �������� ��� ������ 0x00..0x1F
  addr=ch;
  addr+=(addr<<2); // *5
  // ����� ���� ����
  for (ch=0; ch<5; ch++)  
    {  
      pcf8814_data(font5x7[addr++]); 
    }
  pcf8814_data(0); // ����� ���������� ����� ���������
}

//==========================================================================
// ����� ������� �� ��� 20x32
// ������ 20 ����, ������ - 4 ����� (4*8)
/*
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

void pcf8814_cls(void)
{
  unsigned int i;
  pcf8814_cmd(PCF8814_SETYADDR); // ��������� Y=0
  pcf8814_cmd(PCF8814_SETXADDR_UPPER); // X=0
  pcf8814_cmd(PCF8814_SETXADDR_LOWER);

  for (i=0; i<(PCF8814_PIXELX * (PCF8814_ROW+1)); i++) // ���� ��� ������ �����
    {
      pcf8814_data(0); // ������ � ��� LCD �����
    }

  // ����� ������� ������, ���������� - ������� ������ ������ �����
  pcf8814_cmd(PCF8814_SETYADDR); // ��������� Y=0
  pcf8814_cmd(PCF8814_SETXADDR_UPPER); // X=0
  pcf8814_cmd(PCF8814_SETXADDR_LOWER);
}

//=========================================
// ����� ������ (������ � ���)

void pcf8814_puts (unsigned char *s)
{
  while (*s) // �� ��� ��� ���� �� ���� - �������
    pcf8814_putchar(*s++);
}

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

void pcf8814_init (void)
{
  DDR_SCLK=1;
  DDR_SDIN=1;
  DDR_RST=1;
  SCLK_0;

  delay_ms(50); // For Power Stable
  RST_0;  // Reset LCD
  delay_ms(50);
  RST_1;  // Return Reset OFF
//  pcf8814_cmd(PCF8814_SOFTWARE_RESET);
  delay_ms(50);
  
  pcf8814_cmd(PCF8814_DISPLAY_NORMAL);  

// ��������� ��� ��������������� ��������  
//  pcf8814_cmd(PCF8814_ROW_MIRROR_ON); //�������������� �� � �� ��������
  pcf8814_cmd(PCF8814_VERTICAL_MIRROR_OFF); // �������������� �� Y

// BIAS 1/7 ��� ������� "���������", �.�. ���������� ������ ����
// BIAS 1/9 ��� �� ��������... ������ ������ ���� (for VCC=2,8V)
  pcf8814_cmd(PCF8814_SET_BIAS_19); // Set BIAS

  // ���������� ������������� ������� - ����� ����������� ����� �������� ��
  // ��������� ���������, ����� �������� ���������� �������� ��������

  // 0x04 for VCC=2,8V
  // 0x01 - smallest, 0x06 - biggest
  pcf8814_cmd(PCF8814_RESISTOR_RATIO | 0x04); // V0 Voltage Settings 0...6 (�����)
  // 0x1A for VCC=2,8V
  // 0x1F - biggest contrast
  pcf8814_cmd(PCF8814_ELECTRONIC_VOLUME | 0x1A); // 0...1F (�����)

  pcf8814_cmd(PCF8814_ALL_POINTS_OFF); // ���������� ����������� ���� ��������

  pcf8814_cmd(PCF8814_POWER_CTRL_SET_ON); // �������� ������-���������� ����������

  // ����� ���������� ������ ������ (��� �������������� ����������� �����)
  pcf8814_cmd(PCF8814_START_LINE_ADDR | 0x00);

  pcf8814_cmd(PCF8814_DISPLAY_ON);
  pcf8814_cls(); // ������ �����-��� � ������������� ����� ������ � ������
  
}


//====================================================================
// ������� �� ������ ������� CON �� ���
// ���������� ��������� � ���, ��� ��������� ���� ����� ���������� ��
// �������������. �.�. ��������� ��� 128 (16*8) �������� �� ���������, 
// ��� �� ������ �����. ��, �.�. ���������� ��������� ����� �� ����������
// �� �������� 4 ���� (�.�. ��� ��������), ����� ����� ������ ���� ����
// �������� ����. ��� ������� ����� �������������� � ��������������
// ��������.
// ��-�� �������� �������������� �� �, �������� ������ �������.
// ����� �������� ��������� putchar.

void pcf8814_update_console_x(void)
{
  unsigned char col,row;
  char *p;
  char c;  
  p=CON8814;

  for (row=0; row<PCF8814_ROW; row++)
  {
    col=PCF8814_COL; // X
    do
      {
        col--;
        pcf8814_gotoxy(col,row);
        c=*p++;
        if (c==0) 
          {
            c=0x20; // If End of Line (0x00) then "SPACE" character
          }
        pcf8814_putchar_x(c);
      }
    while (col);
   }
}

//====================================================================
// ������� �� ������ ������� CON �� ���

void pcf8814_update_console(void)
{
  unsigned char col,row;
  char *p;
  char c;  
  p=CON8814;

  for (row=0; row<PCF8814_ROW; row++)
  {
  pcf8814_cmd(PCF8814_SETYADDR | row); // ��������� Y
  pcf8814_cmd(PCF8814_SETXADDR_UPPER); 
  pcf8814_cmd(PCF8814_SETXADDR_LOWER); // X=0
    for (col=0; col<PCF8814_COL; col++)
      {
        c=*p++;
        if (c==0) 
          {
            c=0x20; // If End of Line (0x00) then "SPACE" character
          }
        pcf8814_putchar(c);
      }
   }
}


//==================================
// ������ ����� ������� ��������� ������
void pcf8814_clear_console (unsigned char byte)
{
 char i;
 char *p;
 p=CON8814;
 i=sizeof(CON8814);
 do
   {
     *p++=byte;
   }
  while (--i);
}

