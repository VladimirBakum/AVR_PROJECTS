
// ======================= PCF8814 Routine ============================

// ********************************************************************
// Writen by (R)soft 21.10.2011 Last Edition: 21 Oct 2011
// Version 1.0
// Tested on Mega2561 & Mega162.
// Testing on Nokia 6510 LCD Module. Resolution 96x65 pixels. 
// Built-in PCF8814 controller with 3-line SPI interface.
// Reference manual for controller - PCF8814.pdf
// ********************************************************************


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
    SCLK_1; // ������ �� ���������� SCLK
    SCLK_0; // ���������� SCLK ������� � �������� ���������
    ch<<=1; // �������� ���� �����
  }
}

//===========================================
// �������� ������� � ���

void pcf8814_cmd(unsigned char cmd)
{
  SDIN_0; // ������� ������� D/C=0
  SCE_0; // ������ - ����� ���������
  SCLK_1; // �������� ������ ���
  SCLK_0; // ������� ����� � �������� ���������
  pcf8814_write(cmd); // �������� �����
  SCE_1; // ����� ��������� � �������� ��������� - ����� ��������
}

//=============================================
// �������� ������ � ���

void pcf8814_data(unsigned char dat)
{
  SDIN_1; // ������� ������ D/C=1
  SCE_0; // ������ - ����� ���������
  SCLK_1; // �������� ������ ���
  SCLK_0; // ������� ����� � �������� ���������
  pcf8814_write(dat);
  SCE_1; // ����� ��������� � �������� ��������� - ����� ��������  
  if (++lcd8814_x > (PCF8814_PIXELX-1)) // ������������ ������� � �������� ������
  {
    lcd8814_x=0; // ���� ��� ��������� ����� ������ �� ��������� ������
    lcd8814_y++;
  }
}

//===========================================================================
// ��������� ������ � ���

void pcf8814_set_addr(unsigned char x, unsigned char y)
{
  if (y > (PCF8814_ROW-1)) 
    {
      y = (PCF8814_ROW-1); // �������� ������������� �������� ������
    }
  if (x > (PCF8814_PIXELX-1)) 
    {
      x = (PCF8814_PIXELX-1); // �������� ������������� �������� �������
    }
  lcd8814_x = x;
  lcd8814_y = y;
  pcf8814_cmd(PCF8814_SETYADDR | y);
  pcf8814_cmd(PCF8814_SETXADDR | x);
}

//============================================================
// ������� � ����������� x,y

void pcf8814_gotoxy(unsigned char x, unsigned char y)
{
  pcf8814_set_addr(((x<<1)+x),y); // nokia_set_addr(x*3,y)
}


//==========================================================================
// ���������� ������ ������� � ������� ������
// ���������������� ���� ��� ��������� �����������

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

void pcf8814_putchar(unsigned char ch)
{
  unsigned int addr = char_to_addr(ch); // ���������� ������ ������ �������
  // ����� ���� ����
  for (ch=0; ch<5; ch++)  
    {  
      pcf8814_data(font5x7[addr++]); 
    }
  pcf8814_data(0); // ����� ���������� ����� ���������
}

//====================================
// ������� ������

void pcf8814_cls(void)
{
  unsigned int i;
  pcf8814_gotoxy(0,0); // ��������� ������ ������ ������ � ����
  for (i=0; i<(PCF8814_PIXELX*(PCF8814_ROW+1)); i++) // ���� ��� ������ (�� ���� ������ ������)
    {
      pcf8814_data(0); // ������ � ��� LCD �����
    }
   pcf8814_gotoxy(0,0); // ����� ������� ������, ���������� - ������� ������ ������ �����
}

//=========================================
// ����� ������ (������ � ���)

void pcf8814_puts (unsigned char *s)
{
  while (*s) // �� ��� ��� ���� �� ���� - �������
    pcf8814_putchar(*s++);
}

//==========================================
// ������ �������� ���������

void pcf8814_set_contrast(unsigned char c)
{
  lcd8814_contrast=c;
  pcf8814_cmd(PCF8814_FUNCTIONSETEXT); // ������� ������������� � ����������� ����� ������
  pcf8814_cmd(PCF8814_SET_VOP | c); // ��������� �������� ��������� (���������� LCD)
  pcf8814_cmd(PCF8814_FUNCTIONSET); // ������������ � ��������� ������ ������
}

//=========================================
// ������ �������� ��������

void pcf8814_set_inverse(unsigned char inv)
{
  lcd8814_inverse=inv; // �������� �������� ���������� � ���������� ����������
  if (inv)
    {
      pcf8814_cmd(PCF8814_DISPLAY_INVERT);
    }
  else
    {
      pcf8814_cmd(PCF8814_DISPLAY_NORMAL);
    }
}

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

//======================================================================
// ������������� ����������� � �������

void pcf8814_init (void)
{
  PIN_SCE=0; // ������������� ���� ������������ ��� � ����
  PIN_SCLK=0;
  PIN_SDIN=0;
  PIN_RST=0;
//  PIN_CS=0;
//  PIN_LIGHT=0;
  
  SCLK_0;
//  CS_0; // Chip Select
  RST_0;  // Reset LCD
  delay_us(500);
  RST_1;  // Return Reset OFF
  delay_us(500);

  // Step 2 (Extended instruction Set H=1 Mode)
  pcf8814_cmd(PCF8814_FUNCTIONSETEXT);             // 001xxxxx

  pcf8814_cmd(PCF8814_TEMPCOEF);                   // 000001xx Temperature Coefficient
  pcf8814_cmd(PCF8814_VOLTAGE_MUL3X);              // 00001xxx Voltage Multiplier
  pcf8814_cmd(PCF8814_SET_BIAS | PCF8814_BIAS_48); // 0001xxxx Set Bias

  // Step 3 Set VOP
  unsigned char c=120;
  lcd8814_contrast=c;
  pcf8814_cmd(PCF8814_SET_VOP | c);                // 1xxxxxxx Voltage Value

  // Step 4 Set Normal Instruction Set H=0 (0x20)
  pcf8814_cmd(PCF8814_FUNCTIONSET);         // 001xMxxx ��� M - Mirror Y

  pcf8814_cmd(PCF8814_SET_VOP_RANGE_HIGH);         // 0001xxxx Set Voltage Range
                                                   // 01xxxxxx Set Y coordinate
                                                   // 1xxxxxxx Set X coordinate
  pcf8814_set_inverse(0); // Displaynormal

  pcf8814_cls(); // ������� ��� ������ � ��������� ��������� � ����
}

//====================================================================
// ������� �� ������ ������� CON �� ���
// ���������� ��������� � ���, ��� ��������� ���� ����� ���������� ��
// �������������. �.�. ��������� ��� 128 (16*8) �������� �� ���������, 
// ��� �� ������ �����. ��, �.�. ���������� ��������� ����� �� ����������
// �� �������� 4 ���� (�.�. ��� ��������), ����� ����� ������ ���� ����
// �������� ����. ��� ������� ����� �������������� � ��������������
// ��������.

void pcf8814_update_console(void)
{
  unsigned char col,row;
  char *p;
  char c;  
  p=CON8814;
  // ��-�� ������������ ����������� � ���������� ������� �������� �������� ����������
  // ����� ���������� :) ������� �������������� �� Y ���� �������, �� �������
  // �������������� �� � ���������� � ������ ���������� � ������ �����������, �������
  // ������ ������������ ������� �� ����������������� ��������� ������ �� �������.
  // ������� ����� ����� �����������.
  row=PCF8814_ROW;
  do {
        --row;
        pcf8814_gotoxy(0,row);
        for (col=0 ;col<PCF8814_COL; col++)
          {
            c=*p++;
            if (c==0) 
              {
                c=0x20; // If End of Line (0x00) then "SPACE" character
              }
            pcf8814_putchar(c);
          }
      }
  while (row);
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

