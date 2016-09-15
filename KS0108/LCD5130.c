#include "font5x7.h" // Font
#include "LCD5130.h"

// ======================= LCD 5130 Routine ============================
// Modified by (R)soft 20.5.2011
// LPH7366 LCD Module. Resolution 84x48 pixel. Built-in PCD8544 controller with SPI interface.
// LPH7366-1, LPH7779, LPH7677 / driver PCD8544
// Reference manual for controller - PCD8544.pdf

// ������ ����� � ���
// ������� MSB (������ ���� ������� ���)
void nokia_w (unsigned char ch)
{
  unsigned char i;
  for (i=8;i;i--)
  {
    if (ch&0x80) NOKIA_DOUT=1; // ������������� ����� ������� ��� � �����
    else  NOKIA_DOUT=0;        // � ������������ SDA � ������������
    NOKIA_SCK=0; // ������ �� ����� SCLK
    NOKIA_SCK=1; // ���������� SCLK ������� � �������� ���������
    ch<<=1; // �������� �����
  }
}

// �������� ������� � ���
void nokia_cmd(unsigned char cmd)
{
  NOKIA_D_C=0; // ������� ������� D/C=0
  nokia_w(cmd);
}

// �������� ������ � ���
void nokia_data(unsigned char dat)
{
  NOKIA_D_C=1; // ������� ������ D/C=0
  nokia_w(dat);
  if (++nokia_x>GLCD_PIXELX-1) // ������������ ������� � �������� ������
  {
    nokia_x=0; // ���� ��� ��������� ����� ������ �� ��������� ������
    nokia_y++;
  }
}

// ��������� ������ � ���
void nokia_set_addr(unsigned char x, unsigned char y)
{
  if (y>5) y=5; // �������� ������������� �������� ������ (�� ������ 5!)
  if (x>GLCD_PIXELX-1) x=GLCD_PIXELX-1; // �������� ������������� �������� �������
  nokia_x=x;
  nokia_y=y;
  nokia_cmd(GLCD_SETYADDR | y);
  nokia_cmd(GLCD_SETXADDR | x);
}

// ������� � ����������� x,y
void nokia_gotoxy(unsigned char x, unsigned char y)
{
  nokia_set_addr(((x<<1)+x),y); // nokia_set_addr(x*6,y)
}

// ���������� ������ ������� � ������� ������
unsigned int char_to_pos(unsigned char ch)
{
  unsigned int pos;
  ch=ch-0x20; // ������������ ������� �������� ��� ������ 0x00..0x1F
  pos=ch;
  pos+=(pos<<2); // *5
  return pos;
}

// ����� ������� �� ���
void nokia_putchar(unsigned char ch)
{
  unsigned int pos = char_to_pos(ch); // ���������� ������ ������ �������
  for (ch=5; ch; ch--)  {  nokia_data(font5x7[pos++]); } // ����� 5 ����
  nokia_data(0); // ����� ���������� ����� ���������
}

// ������ � LCD ����� ���� ����
void nokia_ddata(unsigned char b)
{
  nokia_data(b);
  nokia_data(b);
}

// ������� ������
void nokia_cls(void)
{
  unsigned char i;
  nokia_gotoxy(0,0); // ��������� ������ ������� � ����
  for (i=252; i; i--) // ���� ��� ������ ������� ����
    nokia_ddata(0); // ������ � LCD 504 ������ �����
  nokia_gotoxy(0,0);
}

// ����� ������ (������ � ���)
void nokia_puts (unsigned char *s)
{
  while (*s)
    nokia_putchar(*s++);
}

// ������ �������� ���������
void nokia_set_contrast(unsigned char c)
{
  nokia_contrast=c;
  nokia_cmd(GLCD_FUNCTIONSETEXT);
  nokia_cmd(GLCD_SET_VOP | c);
  nokia_cmd(GLCD_FUNCTIONSET);
}

// ������ �������� ��������
void nokia_set_inverse(unsigned char inv)
{
  nokia_inverse=inv;
  if (inv)
    nokia_cmd(GLCD_DISPLAYINVERT);
  else
    nokia_cmd(GLCD_DISPLAYNORMAL);
}

// ������������� �����������
void nokia_init (void)
{
  MCUCR=0x00;
  XMCRB=0x00;
  DDRA=0xFF; // ��� �� �����
  NOKIA_SCK=1;
  NOKIA_CS=0; // Chip Select
  NOKIA_RESET=0;  // Reset LCD
  delay_us(500);
  NOKIA_RESET=1;  // Return Reset OFF
  delay_us(500);
  NOKIA_BLIGHT=1; // Set Back Light
  nokia_cmd(GLCD_FUNCTIONSETEXT); // Step 2 (Extended instruction Set H=1 Mode)
  // Step 3 Set VOP
  // Step 4 Set Normal Instruction Set H=0 (0x20)
  nokia_cmd(GLCD_TEMPCOEF);
  nokia_cmd(GLCD_SET_BIAS | GLCD_BIAS_10);
  nokia_set_contrast(120);
  nokia_set_inverse(FALSE);
  nokia_cls();
}

// ������� �� ������ ������� CON �� ���
void UpdateConsole(void)
{
  char *p;
  char i;
  char c;  
  nokia_gotoxy(0,0);
  p=CON;
  i=14*6; // How many symbols
  do
  {
    c=*p++;
    if (c==0) c=32; // If End of Line then "SPACE" character
    nokia_putchar(c);
  }
  while (--i);
}

// ������ ����� ���������
void ClrScr (void)
{
  memset (CON,' ',sizeof(CON));
}

/*
// ����� �����
void LcdPixel (unsigned char x, unsigned char y)
{
    unsigned char offset;
    unsigned char data;

    if (x > 84) return;
    if (y > 48) return;

    offset = y % 8;
    data = (0x01<<offset);
        
    nokia_gotoxy(x, (y/8));
    nokia_data(data);
} 
 
// �������� �����
void LcdPixelErase (unsigned char x, unsigned char y)
{
    unsigned char offset;
    unsigned char data;

    if (x > 84) return;
    if (y > 48) return;

    offset = y % 8;
    data = (0x01<<offset);
        
    nokia_gotoxy(x, (y/8));
    nokia_data(~data);
} 

// �������� �������
void erase_column (unsigned char x, unsigned char y)
{
   unsigned char line;
   for(line=y; line<5; line++)
   {
      nokia_gotoxy(x, line);
      nokia_data(0x00);
   }
}

*/