// I use SED15B1 taken from a cell phone (Nokia 1200). It's on SPI. 
// This is my initialization subroutine. 

// ======================= SED15B1 Routine ============================
// 3-Wire SPI (/CS SCL SI)
// ********************************************************************
// Written by (R)soft 17.10.2011-2013 Last Edition: 4 Nov 2013
// Version 1.3
// Tested on Mega2561.
// Testing on Nokia 1200 LCD Module. Resolution 96x65 pixels.
// Built-in SED15B1 controller with 3-Wite SPI interface.
// Reference manual for controller - sed15b1.pdf
// ********************************************************************

// ���������� SED15B1 ��������� �� �������� � PCF8814.
// ����� ���������, ��� �������� ������������� � SSD1818. �� �� ���������.
// ��������� ��� ������ ��� ��� 5.51 � �����2561@14.318

//=============================================================
// ������ ����� � ���
// ������� MSB (������ ���� ������� ���)

void sed15b1_write (unsigned char ch)
{
  unsigned char i;
  for (i=8;i;i--)
  {
    if (ch&0x80) // ������������� ����� ������� ��� � �����
      {
        SI_1; // � ������������ SI � ������������
      }
    else  
      {
        SI_0;
      }
//    delay_us(10);
    SCL_0; // ������ �� ����� SCLK
//    delay_us(10);
    SCL_1; // ���������� SCLK ������� � �������� ���������
//    delay_us(10);
    ch<<=1; // �������� ���� �����
  }
}

//===========================================
// �������� ������� � ���

void sed15b1_cmd(unsigned char cmd)
{
  SI_0; // ������� ������� A0=0
  SCL_0;  // �������� ������ ���
  SCL_1;
  sed15b1_write(cmd);
}

//=============================================
// �������� ������ � ���

void sed15b1_data(unsigned char dat)
{
  SI_1; // ������� ������� A0=1
  SCL_0;  // �������� ������ ���
  SCL_1;
  sed15b1_write(dat);
}

/*
//===========================================================================
// ��������� ������ � ���

void sed15b1_set_addr(unsigned char x, unsigned char y)
{
  if (y > (SED15B1_ROW-1)) 
    {
      y = (SED15B1_ROW-1); // �������� ������������� �������� ������ (0..8)
    }
  if (x > (SED15B1_PIXELX-1)) 
    {
      x = (SED15B1_PIXELX-1); // �������� ������������� �������� �������
    }
  sed_x = x;
  sed_y = y;
  sed15b1_cmd(SED15B1_SETYADDR | y);
  sed15b1_cmd(SED15B1_SETXADDR_LOWER | (x&0x0F));
  sed15b1_cmd(SED15B1_SETXADDR_UPPER | ((x>>4)&0x0F));
}
*/

/*
//============================================================
// ��������� ��������� ������ x,y

void sed15b1_gotoxy(unsigned char x, unsigned char y)
{
  sed15b1_set_addr(((x<<1)+x),y); // set_addr(x*3,y)
}
*/

//==========================================================================
// ����� ������� �� ��� 5x7

void sed15b1_putchar(unsigned char ch)
{
  unsigned int addr;
  ch=ch-0x20; // ������������ ������� �������� ��� ������ 0x00..0x1F
  addr=ch;
  addr=addr*5; // *5

  // ����� ���� ����
  for (ch=0; ch<5; ch++)  
    {  
      sed15b1_data(font5x7[addr++]); 
    }
  sed15b1_data(0); // ����� ���������� ����� ���������
}


/*
//==========================================================================
// ����� ������� �� ��� 11x16
// ������ 11 ����, ������ - 2 ����� (2*8)

void sed15b1_putchar16(unsigned char ch)
{
  unsigned int addr;
  unsigned char lowadd, hiadd; // ���������� ���������� � ������
  unsigned char len; // ������ ������� � ��������
  if (ch==' ') { ch=0x3A; }
  if (ch=='.') { ch=0x3B; }
  if (ch=='A') { ch=0x3C; }
  if (ch=='V') { ch=0x3D; }

  ch=ch-0x30; // ������������ ������� �������� ��� ������ 0x00..0x2F
  addr=ch;
  addr=addr*33; // ��������� �� ����� ������� � ������
  len=(arial16x16[addr++]); 
  for (ch=0; ch<len; ch++)  
    {  
      lowadd=(x+ch&0x0F);
      hiadd=((x+ch>>4)&0x0F);
      sed15b1_cmd(SED15B1_SETYADDR|y); // ��������� Y=0
      sed15b1_cmd(SED15B1_SETXADDR_LOWER | lowadd);
      sed15b1_cmd(SED15B1_SETXADDR_UPPER | hiadd);
      sed15b1_data(arial16x16[addr++]); 
      sed15b1_cmd(SED15B1_SETYADDR|y+1); // ��������� Y=1
      sed15b1_cmd(SED15B1_SETXADDR_LOWER | lowadd);
      sed15b1_cmd(SED15B1_SETXADDR_UPPER | hiadd);
      sed15b1_data(arial16x16[addr++]); 
    }
  x=x+len;
}
*/

//==========================================================================
// ����� ������� �� ��� 20x32
// ������ 20 ����, ������ - 4 ����� (4*8)
/*
void sed15b1_putchar32(unsigned char ch, unsigned char x, unsigned char y)
{
  unsigned int addr;
  unsigned char len; // ������ ������� � ��������
  unsigned char lowadd, hiadd; // ���������� ���������� � ������
  if (ch=='+') { ch=0x3A; }
  if (ch=='-') { ch=0x3B; }
  if (ch=='.') { ch=0x3C; }
  ch=ch-0x30; // ������������ ������� �������� ��� ������ 0x00..0x2F
  addr=ch;
  addr=addr*81; // ��������� �� ����� ������� � ������
  len=(arial32[addr++]); 
  for (ch=0; ch<len; ch++)  
    {  
      lowadd=(x+ch&0x0F);
      hiadd=((x+ch>>4)&0x0F);
      sed15b1_cmd(SED15B1_SETYADDR|y); // ��������� Y=0
      sed15b1_cmd(SED15B1_SETXADDR_LOWER | lowadd);
      sed15b1_cmd(SED15B1_SETXADDR_UPPER | hiadd);
      sed15b1_data(arial32[addr++]); 
      sed15b1_cmd(SED15B1_SETYADDR|y+1); // ��������� Y=1
      sed15b1_cmd(SED15B1_SETXADDR_LOWER | lowadd);
      sed15b1_cmd(SED15B1_SETXADDR_UPPER | hiadd);
      sed15b1_data(arial32[addr++]); 
      sed15b1_cmd(SED15B1_SETYADDR|y+2); // ��������� Y=2
      sed15b1_cmd(SED15B1_SETXADDR_LOWER | lowadd);
      sed15b1_cmd(SED15B1_SETXADDR_UPPER | hiadd);
      sed15b1_data(arial32[addr++]); 
      sed15b1_cmd(SED15B1_SETYADDR|y+3); // ��������� Y=3
      sed15b1_cmd(SED15B1_SETXADDR_LOWER | lowadd);
      sed15b1_cmd(SED15B1_SETXADDR_UPPER | hiadd);
      sed15b1_data(arial32[addr++]); 
    }
}
*/

//====================================
// ������� ������

void sed15b1_cls(void)
{
  unsigned int i;
  sed15b1_cmd(SED15B1_SETYADDR); // ��������� Y=0

  // ��� �������� �����-��� �������� ������! ������ ��!!! :)
  sed15b1_cmd(SED15B1_SETXADDR_UPPER); // X=0
  sed15b1_cmd(SED15B1_SETXADDR_LOWER);

  for (i=0; i<(132*8); i++) // ���� ��� ������ (�� ���� ������ ������)
    {
      sed15b1_data(0); // ������ � ��� LCD �����
    }

  // ����� ������� ������, ���������� - ������� ������ ������ �����
  sed15b1_cmd(SED15B1_SETYADDR); // ��������� Y=0
  sed15b1_cmd(SED15B1_SETXADDR_UPPER); // X=0
  sed15b1_cmd(SED15B1_SETXADDR_LOWER);
}

//=========================================
// ����� ������ (������ � ���)
/*
void sed15b1_puts (unsigned char *s)
{
  while (*s) // �� ��� ��� ���� �� ���� - �������
    sed15b1_putchar(*s++);
}
*/

//=========================================
// ������ �������� ��������
/*
void sed15b1_set_inverse(unsigned char inv)
{
//  sed_inverse=inv; // �������� �������� ���������� � ���������� ����������
  if (inv)
    {
      sed15b1_cmd(SED15B1_DISPLAY_REVERSE);
    }
  else
    {
      sed15b1_cmd(SED15B1_DISPLAY_NORMAL);
    }
}
*/

//=========================================
// ��������/��������� �������
/*
void sed15b1_set_power(unsigned char power)
{
  if (power)
    {
      sed15b1_cmd(SED15B1_DISPLAY_ON);
    }
  else
    {
      sed15b1_cmd(SED15B1_DISPLAY_OFF);
    }
}
*/

//=============================================================
// ������������� LCD
// Stage �������� ��� ���, � ������������ � ��������� �� SED15B1
// � ������ �������� �� ��������, ������ ��� �������� - ����� ������ (���� ��� ����) :)

void sed15b1_init (void)
{
  // ������������� ������������ ���� ������������ ���
  DDR_SCL=1;
  DDR_SI=1;
  DDR_RESN=1;
  SCL_1;
  
  RESN_0;  // Reset LCD
  delay_ms(1);
  RESN_1;  // Return Reset OFF
  delay_ms(1);
  
  sed15b1_cmd(SED15B1_DISPLAY_NORMAL);  // ��������

// ��������� ��� ��������������� ��������  
  sed15b1_cmd(SED15B1_SEGMENT_DIR_NORMAL); //�������������� �� �
  sed15b1_cmd(SED15B1_COMMON_DIR_REVERSE); // �������������� �� Y

// BIAS 1/7 ��� ������� "���������", �.�. ���������� ������ ����
// BIAS 1/9 ��� �� ��������...
  sed15b1_cmd(SED15B1_SET_BIAS_19); // Set BIAS

  // ���������� ������������� ������� - ����� ����������� ����� �������� ��
  // ��������� ���������, ����� �������� ���������� �������� ��������
  // � ���� ����� ������� ��� �����, ��-�� ����� ������ ���������� ����������
  // ������� ������������� :(
  sed15b1_cmd(SED15B1_RESISTOR_RATIO | 0x04); // V0 Voltage Settings 0...6 (�����)
  sed15b1_cmd(SED15B1_ELECTRONIC_VOLUME | 0x0F); // 0...1F (�����)

  sed15b1_cmd(SED15B1_ALL_POINTS_OFF); // Stage 9 ���������� ����������� ���� ��������

  // ��� ����������� voltage follover ����� �������� �������
  // ��� ����������� voltage booster ������ ������ �� �����
  sed15b1_cmd(SED15B1_POWER_CTRL_SET | 0x07); // �� �������� - 0x07

  // Stage 10 ����� ���������� ������ ������ (��� �������������� ����������� �����)
  sed15b1_cmd(SED15B1_START_LINE_ADDR | 0x00);

//  sed15b1_cmd(SED15B1_SETYADDR); // Stage 11  ��������� Y=0
//  sed15b1_cmd(SED15B1_SETXADDR_UPPER | SED15B1_ADD_XHI); // Stage 12 X=0+add
//  sed15b1_cmd(SED15B1_SETXADDR_LOWER | SED15B1_ADD_XLO);

  //  sed15b1_cmd(SED15B1_ALL_POINTS_ON); // ������� ����������� ��� �������

  sed15b1_cmd(SED15B1_DISPLAY_ON);
  sed15b1_cls(); // ������ �����-��� � ������������� ����� ������ � ������
  
}


//====================================================================
// ������� �� ������ ������� CON �� ���
// ���������� ��������� � ���, ��� ��������� ���� ����� ���������� ��
// �������������. �.�. ��������� ��� 128 (16*8) �������� �� ���������, 
// ��� �� ������ �����. ��, �.�. ���������� ��������� ����� �� ����������
// �� �������� 4 ���� (�.�. ��� ��������), ����� ����� ������ ���� ����
// �������� ����. ��� ������� ����� �������������� � ��������������
// ��������.

void sed15b1_update_console(void)
{
  unsigned char col,row;
  char *p;
  unsigned char c;  
  p=CON15B1;

  for (row=0; row<SED15B1_ROW; row++)
  {
  sed15b1_cmd(SED15B1_SETYADDR | row); // ��������� Y
  sed15b1_cmd(SED15B1_SETXADDR_UPPER); 
  sed15b1_cmd(SED15B1_SETXADDR_LOWER); // X=0
    for (col=0; col<SED15B1_COL; col++)
      {
        c=*p++;
        if (c==0) 
          {
            c=0x20; // If End of Line (0x00) then "SPACE" character
          }
        sed15b1_putchar(c);
      }
   }
}


/*
//---------------------------------------------------------
void sed15b1_update_console16(void)
{
  unsigned char col,row;
  char *p;
  unsigned char c;  
  p=CON15B1;

  for (row=0; row<SED15B1_ROW; row=row+2) // y
  {
  sed15b1_cmd(SED15B1_SETYADDR | row); // ��������� Y
  y=row;
  sed15b1_cmd(SED15B1_SETXADDR_UPPER); 
  sed15b1_cmd(SED15B1_SETXADDR_LOWER); // X=0
  x=0;
  for (col=0; col<9; col++)
      {
        c=*p++;
        if (c==0) 
          {
            c=0x20; // If End of Line (0x00) then "SPACE" character
          }
        sed15b1_putchar16(c);
      }
   }
}

*/

//==================================
// ������ ����� ������� ��������� ������
void sed15b1_clear_console (unsigned char byte)
{
 unsigned char i;
 char *p;
 p=CON15B1;
 i=sizeof(CON15B1);
 do
   {
     *p++=byte;
   }
  while (--i);
}
