
// ======================= PCF8812 Routine ============================

// ********************************************************************
// Modified by (R)soft 17.9.2011 Last Edition: 17 Oct 2011
// Version 1.2
// Tested on Mega2561 & Mega162.
// Testing on Mitsubishi Trium Mars LCD Module. Resolution 102x65 pixel. 
// Built-in PCF8812 compatible controller with SPI interface.
// Reference manual for controller - PCF8812.pdf and PCF8812_shrink_V3p1.pdf
// ********************************************************************


//=============================================================
// ������ ����� � ���
// ������� MSB (������ ���� ������� ���)

void pcf8812_write (unsigned char ch)
{
  unsigned char i;
  for (i=8;i;i--)
  {
    if (ch&0x80) // ������������� ����� ������� ��� � �����
      {
        DOUT_1; // � ������������ SDA � ������������
      }
    else  
      {
        DOUT_0;
      }
    SCK_0; // ������ �� ����� SCLK
    SCK_1; // ���������� SCLK ������� � �������� ���������
    ch<<=1; // �������� ���� �����
  }
}

//===========================================
// �������� ������� � ���

void pcf8812_cmd(unsigned char cmd)
{
  DC_0; // ������� ������� D/C=0
  pcf8812_write(cmd);
}

//=============================================
// �������� ������ � ���

void pcf8812_data(unsigned char dat)
{
  DC_1; // ������� ������ D/C=1
  pcf8812_write(dat);
  if (++lcd_x > (PCF8812_PIXELX-1)) // ������������ ������� � �������� ������
  {
    lcd_x=0; // ���� ��� ��������� ����� ������ �� ��������� ������
    lcd_y++;
  }
}

//===========================================================================
// ��������� ������ � ���

void pcf8812_set_addr(unsigned char x, unsigned char y)
{
  if (y > (PCF8812_ROW-1)) 
    {
      y = (PCF8812_ROW-1); // �������� ������������� �������� ������ (0..7)
    }
  if (x > (PCF8812_PIXELX-1)) 
    {
      x = (PCF8812_PIXELX-1); // �������� ������������� �������� �������
    }
  lcd_x = x;
  lcd_y = y;
  pcf8812_cmd(PCF8812_SETYADDR | y);
  pcf8812_cmd(PCF8812_SETXADDR | x);
}

//============================================================
// ������� � ����������� x,y

void pcf8812_gotoxy(unsigned char x, unsigned char y)
{
  pcf8812_set_addr(((x<<1)+x),y); // nokia_set_addr(x*3,y)
}

//==========================================================================
// ���������� ������ ������� � ������� ������

unsigned int char_to_addr(unsigned char ch)
{
  unsigned int addr;
  ch=ch-0x20; // ������������ ������� �������� ��� ������ 0x00..0x1F
  addr=ch;
  addr+=(addr<<2); // *5
  return addr;
}

//==========================================================================
// ����� ������� �� ���

void pcf8812_putchar(unsigned char ch)
{
  unsigned int addr = char_to_addr(ch); // ���������� ������ ������ �������
  // ����� ���� ����
  for (ch=0; ch<5; ch++)  
    {  
      pcf8812_data(font5x7[addr++]); 
    }
  pcf8812_data(0); // ����� ���������� ����� ���������
}

//====================================
// ������� ������

void pcf8812_cls(void)
{
  unsigned int i;
  pcf8812_gotoxy(0,0); // ��������� ������ ������ ������ � ����
  for (i=0; i<(102*9); i++) // ���� ��� ������ (�� ���� ������ ������)
    {
      pcf8812_data(0); // ������ � ��� LCD �����
    }
   pcf8812_gotoxy(0,0); // ����� ������� ������, ���������� - ������� ������ ������ �����
}

//=========================================
// ����� ������ (������ � ���)

void pcf8812_puts (unsigned char *s)
{
  while (*s) // �� ��� ��� ���� �� ���� - �������
    pcf8812_putchar(*s++);
}

//==========================================
// ������ �������� ���������

void pcf8812_set_contrast(unsigned char c)
{
  lcd_contrast=c;
  pcf8812_cmd(PCF8812_FUNCTIONSETEXT); // ������� ������������� � ����������� ����� ������
  pcf8812_cmd(PCF8812_SET_VOP | c); // ��������� �������� ��������� (���������� LCD)
  pcf8812_cmd(PCF8812_FUNCTIONSET); // ������������ � ��������� ������ ������
}

//=========================================
// ������ �������� ��������

void pcf8812_set_inverse(unsigned char inv)
{
  lcd_inverse=inv; // �������� �������� ���������� � ���������� ����������
  if (inv)
    {
      pcf8812_cmd(PCF8812_DISPLAY_INVERT);
    }
  else
    {
      pcf8812_cmd(PCF8812_DISPLAY_NORMAL);
    }
}

//=========================================
// ��������/��������� �������

void pcf8812_set_power(unsigned char power)
{
  if (power)
    {
      pcf8812_cmd(PCF8812_DISPLAY_ON);
    }
  else
    {
      pcf8812_cmd(PCF8812_DISPLAY_OFF);
    }
}

//======================================================================
// ������������� ����������� � �������

void pcf8812_init (void)
{
//  MCUCR=0x00;
//  XMCRB=0x00;
  PIN_DC=0; // ������������� ���� ������������ ��� � ����
  PIN_SCK=0;
  PIN_DOUT=0;
  PIN_RESET=0;
//  PIN_CS=0;
//  PIN_LIGHT=0;
  
  SCK_1;
//  CS_0; // Chip Select
  RESET_0;  // Reset LCD
  delay_us(500);
  RESET_1;  // Return Reset OFF
  delay_us(500);

  // Step 2 (Extended instruction Set H=1 Mode)
  pcf8812_cmd(PCF8812_FUNCTIONSETEXT);             // 001xxxxx
//  pcf8812_cmd(0x01);                               // 00000001 Not Use
//  pcf8812_cmd(0x02 | 0x01);                        // 0000001x Not Use
  pcf8812_cmd(PCF8812_TEMPCOEF);                   // 000001xx Temperature Coefficient

  // MUL5X for VCC=1,8V ===  MUL3X for VCC=2,8V
  pcf8812_cmd(PCF8812_VOLTAGE_MUL5X);              // 00001xxx Voltage Multiplier 3X for 2,8V

  // BIAS_65 for VCC=1,8V === BIAS_48 for VCC=2,8V
  pcf8812_cmd(PCF8812_SET_BIAS | PCF8812_BIAS_65); // 0001xxxx Set Bias (BIAS_48 for 2,8V)
//  pcf8812_cmd(0x40 | 0x3F);                               // 01xxxxxx Not Use
  // Step 3 Set VOP
  unsigned char c=120;
  lcd_contrast=c;
  pcf8812_cmd(PCF8812_SET_VOP | c);                // 1xxxxxxx Voltage Value

  // Step 4 Set Normal Instruction Set H=0 (0x20)
  pcf8812_cmd(PCF8812_FUNCTIONSET);         // 001xMxxx ��� M - Mirror Y
//  pcf8812_cmd(0x01);                               // 00000001 Not Use
//  pcf8812_cmd(0x02 | 0x01);                        // 0000001x Not Use
//  pcf8812_cmd(0x04 | 0x03);                               // 000001xx Not Use
//  pcf8812_cmd(0x08|0x04|0x00|0x00);                               // 00001d0e Display Control
  pcf8812_cmd(PCF8812_SET_VOP_RANGE_HIGH);         // 0001xxxx Set Voltage Range
                                                   // 01xxxxxx Set Y coordinate
                                                   // 1xxxxxxx Set X coordinate
  pcf8812_set_inverse(0); // Displaynormal

  pcf8812_cls(); // ������� ��� ������ � ��������� ��������� � ����
}

//====================================================================
// ������� �� ������ ������� CON �� ���
// ���������� ��������� � ���, ��� ��������� ���� ����� ���������� ��
// �������������. �.�. ��������� ��� 128 (16*8) �������� �� ���������, 
// ��� �� ������ �����. ��, �.�. ���������� ��������� ����� �� ����������
// �� �������� 4 ���� (�.�. ��� ��������), ����� ����� ������ ���� ����
// �������� ����. ��� ������� ����� �������������� � ��������������
// ��������.

void pcf8812_update_console(void)
{
  unsigned char col,row;
  char *p;
  char c;  
  p=CON8812;
  // ��-�� ������������ ����������� � ���������� ������� �������� �������� ����������
  // ����� ���������� :) ������� �������������� �� Y ���� �������, �� �������
  // �������������� �� � ���������� � ������ ���������� � ������ �����������, �������
  // ������ ������������ ������� �� ����������������� ��������� ������ �� �������.
  // ������� ����� ����� �����������.
  row=PCF8812_ROW;
  do {
        --row;
        pcf8812_gotoxy(0,row);
        for (col=0 ;col<PCF8812_COL; col++)
          {
            c=*p++;
            if (c==0) 
              {
                c=0x20; // If End of Line (0x00) then "SPACE" character
              }
            pcf8812_putchar(c);
          }
      }
  while (row);
}

//==================================
// ������ ����� ������� ��������� ������
void pcf8812_clear_console (unsigned char byte)
{
 char i;
 char *p;
 p=CON8812;
 i=sizeof(CON8812);
 do
   {
     *p++=byte;
   }
  while (--i);
}

