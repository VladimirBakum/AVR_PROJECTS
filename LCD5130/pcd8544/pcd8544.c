
// ======================= PCD8544 Routine ============================
// Modified by (R)soft 17.9.2011
// Version 2.0
// Testiong on LPH7366 LCD Module. Resolution 84x48 pixel. 
// Built-in PCD8544 controller with SPI interface.
// LPH7366-1, LPH7779, LPH7677 / driver PCD8544
// Reference manual for controller - PCD8544.pdf


//=============================================================
// ������ ����� � ���
// ������� MSB (������ ���� ������� ���)

void pcd8544_w (unsigned char ch)
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

void pcd8544_cmd(unsigned char cmd)
{
  DC_0; // ������� ������� D/C=0
  pcd8544_w(cmd);
}

//=============================================
// �������� ������ � ���

void pcd8544_data(unsigned char dat)
{
  DC_1; // ������� ������ D/C=0
  pcd8544_w(dat);
  if (++lcd_x > (PCD8544_PIXELX-1)) // ������������ ������� � �������� ������
  {
    lcd_x=0; // ���� ��� ��������� ����� ������ �� ��������� ������
    lcd_y++;
  }
}

//===========================================================================
// ��������� ������ � ���

void pcd8544_set_addr(unsigned char x, unsigned char y)
{
  if (y > 5) 
    {
      y=5; // �������� ������������� �������� ������ (0..5)
    }
  if (x > (PCD8544_PIXELX-1)) 
    {
      x = (PCD8544_PIXELX-1); // �������� ������������� �������� �������
    }
  lcd_x = x;
  lcd_y = y;
  pcd8544_cmd(PCD8544_SETYADDR | y);
  pcd8544_cmd(PCD8544_SETXADDR | x);
}

//============================================================
// ������� � ����������� x,y

void pcd8544_gotoxy(unsigned char x, unsigned char y)
{
  pcd8544_set_addr(((x<<1)+x),y); // nokia_set_addr(x*3,y)
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

void pcd8544_putchar(unsigned char ch)
{
  unsigned int addr = char_to_addr(ch); // ���������� ������ ������ �������
  for (ch=5; ch; ch--)  
    {  
      pcd8544_data(font5x7[addr++]); 
    } // ����� 5 ����
  pcd8544_data(0); // ����� ���������� ����� ���������
}

/*
//===================================
// ������ � LCD ����� ���� ����

void pcd8544_ddata(unsigned char b)
{
  pcd8544_data(b);
  pcd8544_data(b);
}
*/

//====================================
// ������� ������

void pcd8544_cls(void)
{
  unsigned char i;
  pcd8544_gotoxy(0,0); // ��������� ������ ������� � ����
  for (i=252; i; i--) // ���� ��� ������ ���� ����
    {
      pcd8544_data(0); // ������ � LCD 504 ������ �����
      pcd8544_data(0);
    }
   pcd8544_gotoxy(0,0); // ����� ������� ������, ���������� - ������� ������ ������ �����
}

//=========================================
// ����� ������ (������ � ���)

void pcd8544_puts (unsigned char *s)
{
  while (*s) // �� ��� ��� ���� �� ���� - �������
    pcd8544_putchar(*s++);
}

//==========================================
// ������ �������� ���������

void pcd8544_set_contrast(unsigned char c)
{
  lcd_contrast=c;
  pcd8544_cmd(PCD8544_FUNCTIONSETEXT);
  pcd8544_cmd(PCD8544_SET_VOP | c);
  pcd8544_cmd(PCD8544_FUNCTIONSET);
}

//=========================================
// ������ �������� ��������

void pcd8544_set_inverse(unsigned char inv)
{
  lcd_inverse=inv; // �������� �������� ���������� � ���������� ����������
  if (inv)
    {
      pcd8544_cmd(PCD8544_DISPLAYINVERT);
    }
  else
    {
      pcd8544_cmd(PCD8544_DISPLAYNORMAL);
    }
}

//======================================================================
// ������������� ����������� � �������

void pcd8544_init (void)
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
//  LIGHT_1; // Set Back Light
  pcd8544_cmd(PCD8544_FUNCTIONSETEXT); // Step 2 (Extended instruction Set H=1 Mode)
  // Step 3 Set VOP
  // Step 4 Set Normal Instruction Set H=0 (0x20)
  pcd8544_cmd(PCD8544_TEMPCOEF);
  pcd8544_cmd(PCD8544_SET_BIAS | PCD8544_BIAS_10);
  pcd8544_set_contrast(120);
  pcd8544_set_inverse(0);
  pcd8544_cls();
}

//====================================================================
// ������� �� ������ ������� CON �� ���
// ���������� ��������� � ���, ��� ��������� ���� ����� ���������� ��
// �������������. �.�. ��������� ��� 84 ������� �� ���������, 
// ��� �� ������ �����.
// ��� ������� ����� �������������� ��������� ��� ��� ��� ������� � ������ ����
// ����� ����������� ����� �� ���������.

void update_console(void)
{
  char *p;
  char i;
  char c;  
  pcd8544_gotoxy(0,0);
  p=CON;
  i=14*6; // How many symbols
  do
  {
    c=*p++;
    if (c==0) 
      {
        c=0x20; // If End of Line (0x00) then "SPACE" character
      }
    pcd8544_putchar(c);
  }
  while (--i);
}

//==================================
// ������ ����� ������� ��������� ������
void clear_console (unsigned char byte)
{
 char i;
 char *p;
 p=CON;
 i=sizeof(CON);
 do
   {
     *p++=byte;
   }
  while (--i);
}

