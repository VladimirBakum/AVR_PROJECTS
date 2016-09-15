// ======================= KS0717 Routines ============================

// ********************************************************************
// Writen by (R)soft 8.5.2013 Last Edition: 8 May 2013
// Version 1.0
// Tested on Mega2561 
// Testing on MOTOROLA V50 LCD Module. Resolution 96x54 pixels. 
// Built-in KS0717TB-01B (KS0717 compatible) controller with parallel interface.
// Reference manual for controller - KS0717.pdf
// ********************************************************************
/*
������� � �������� ��������� ����...
���� �� �������� ��������� �� ������, ������ ��������� 6800-system:
������ RW ������ ���� - �.�. ����� ������ ������ � ����������.
ENABLE - ����� ��� ������, ������������� �����.
������ RESET ����� �� ���������� � �����������, ���������,
������� ��� �� +VCC.
��� ��������� VCC � 2,8V �� 3,3V ������������� ����������� �� ��������
*/

//=============================================================
// ������ ����� � ���
// ���� RS=0 �� ������ �������, ���� RS=1 �� ������ ������
// ks0717_write(1,0xff);

void ks0717_write (unsigned char rs, unsigned char byte)
{
  
  // ������� ���������� RS � ������������
  if (rs)
    {
      RS_1;
    }
  else
    {
      RS_0;
    }
//  delay_us(25);

  DDRDATA=~(byte); // ����� ������� ���� �����������
//  delay_us(25);
  ENABLE_1; // ������ �����
  delay_us(1); // ����� ����� ������������ �������� ��� �������� ENABLE
  ENABLE_0;
//  delay_us(25);
  
}

//=============================================================
// ������������� LCD �� ��������:
// 1. ADC Select
// 2. SHL Select
// 3. LCD Bias Select
// 4. Voltage Converter ON
// 5. Wait > 1 ms
// 6.Voltage Regulator ON
// 7. Wait > 1 ms
// 8. Voltage Follower ON
// 9. Regulator Resistor Select
// 10. Reference Voltage Register Set (Two Instructions)
//    RAM Addressing Instructions: 
//    11. Initial Display Line (YADDR)
//    12. Set Page Address
//    13. Set Column Address (XADDR)
//    14. Set Static Indicator State (Two Instructions)
//    15. Turn Display ON


void ks0717_init (void)
{
  PIN_RS=0; // ������������� ���� ������������ ��� � ����
  PIN_ENABLE=0;
  PIN_RESETN=0;
  PDATA=0;

  ENABLE_0;  // �������� ��������� ����� ENABLE
  RESETN_0;  // Reset LCD
  delay_ms(10);
  RESETN_1;  // Return Reset OFF
  delay_ms(10);

  ks0717_write(0,KS0717_ADC_NORMAL); // �������������� �� X
  ks0717_write(0,KS0717_SHL_REVERSE); // �������������� �� Y
  ks0717_write(0,KS0717_SELECT_BIAS_18);
  ks0717_write(0,KS0717_POWER_CONTROL|0x04); // VC=1, VR=0, VF=0 - Voltage Converter ON
  delay_ms(2);
  ks0717_write(0,KS0717_POWER_CONTROL|0x06); // VC=1, VR=1, VF=0 - Voltage Regulator ON
  delay_ms(2);
  ks0717_write(0,KS0717_POWER_CONTROL|0x07); // VC=1, VR=1, VF=1 - Voltage Follower ON
  ks0717_write(0,KS0717_RESISTOR_SLCT|0x07); // 0...7 - ��� ��������� ����������� �� ��������

  ks0717_write(0,KS0717_REF_VOLT_MODE); // Two Bytes Instruction
  ks0717_write(0,0x23); // 00...3F ��� Bias 1/8 �����.����. 0x23, ��� Bias 1/6 - 0x0F
  ks0717_write(0,KS0717_SETYADDR|0x00);
  ks0717_write(0,KS0717_SET_PAGE_ADDR|0x00); // 0...8
  ks0717_write(0,KS0717_SETXADDR_LO|0x00);
  ks0717_write(0,KS0717_SETXADDR_HI|0x00);

  ks0717_write(0,KS0717_STATIC_IND_ON); // Two Bytes Instruction
  ks0717_write(0,0x03); // 0...3 (3 - Always ON)

//  ks0717_write(0,KS0717_ENTIRE_OFF);
//  ks0717_write(0,KS0717_REVERSE_OFF);

  ks0717_write(0,KS0717_DISPLAY_ON);
  delay_ms(10);
}

//==================================
// ������ ����� ������� ��������� ������
void ks0717_clear_console (unsigned char byte)
{
 unsigned int i;
 char *p;
 p=CON0717;
 i=sizeof(CON0717);
 do
   {
     *p++=byte;
   }
  while (--i);
}

//==========================================================================
// ����� ������� �� ���

void ks0717_putchar(unsigned char ch)
{
  unsigned int addr;
  ch=ch-0x20; // ������������ ������� �������� ��� ������ 0x00..0x1F
  addr=ch;
  addr+=(addr<<2); // *5

  // ����� ���� ����
  for (ch=0; ch<5; ch++)  
    {  
      ks0717_write(1,font5x7[addr++]); 
    }
  ks0717_write(1,0); // ����� ���������� ����� ���������
}

//=========================================
// ����� ������ (������ � ���)

void ks0717_puts (unsigned char *s)
{
  while (*s) // �� ��� ��� ���� �� ���� - �������
    ks0717_putchar(*s++);
}

//====================================================================
// ������� �� ������ ������� CON �� ���

void ks0717_update_console(void)
{
  unsigned char col,row;
  char *p;
  char c;  
  p=CON0717;

  for (row=0; row<KS0717_ROW; row++)
  {
  ks0717_write(0,KS0717_SET_PAGE_ADDR | row); // ��������� Y
  ks0717_write(0,KS0717_SETXADDR_HI); 
  ks0717_write(0,KS0717_SETXADDR_LO);
    for (col=0; col<KS0717_COL; col++)
      {
        c=*p++;
        if (c==0) 
          {
            c=0x20; // If End of Line (0x00) then "SPACE" character
          }
        ks0717_putchar(c);
      }
   }
}

//==========================================================================
// ���������/����� ���������� � ������� ������.
// � ���������� ����������� ���� �� �������, ������� �������,
// ������ ������������� ����� ��� ������ ������� �����.
// ���������: ����� �����������, ���� ������ (0 - �����, 0xFF - ���������)
// ������ ���������� ����� ���������� �������, ���� ���������
// ������ ��� ������ � �������������:
// 02 - �������
// 06 - ������ ������� �������
// 10 - ������ ������� �������
// 14 - ������ ������� �������
// 18 - ��������� ������� �������
// 22 - ����� ������� �������
// 30 - ������ �������
// 42 - �����������
// 54 - �����
// 66 - �������
// 70 - ����������� �����
// 72 - �����������
// 74 - 3� ����� �������
// 78 - 1� �������
// 82 - 2� �������
// 86 - ������ �������

void ks0717_piktogramm (unsigned char num, unsigned char byte)
{
  ks0717_write(0,KS0717_SET_PAGE_ADDR|0x08); // ��������� Y ��� ����������
  ks0717_write(0,KS0717_SETXADDR_HI|(num>>4)); // �������� ������� �������
  ks0717_write(0,KS0717_SETXADDR_LO|(num&0x0F)); // �������� ������� �������
  ks0717_write(1,byte);
}

//====================================
// ������� ������

void ks0717_cls(void)
{
  unsigned char x,y;

  for (y=0; y<(KS0717_ROW+3); y++)  // ��������� �������� 9 - �����������
  {
    ks0717_write(0,KS0717_SET_PAGE_ADDR|y); // ��������� Y
    ks0717_write(0,KS0717_SETXADDR_HI); // X=0
    ks0717_write(0,KS0717_SETXADDR_LO);
    for (x=0; x<KS0717_PIXELX; x++) // ���� ��� ������ �����
      {
        ks0717_write(1,0x00); // ������ � ��� LCD �����
      }
  }
  // ����� ������� ������, ���������� - ������� ������ ������ �����
//  ks0717_write(0,KS0717_SETYADDR); // ��������� Y=0
//  ks0717_write(0,KS0717_SETXADDR_HI); // X=0
//  ks0717_write(0,KS0717_SETXADDR_LO);
}
