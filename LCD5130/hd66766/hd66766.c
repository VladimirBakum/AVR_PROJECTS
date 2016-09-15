// ======================= HD66766 Routine ============================

// ********************************************************************
// Writen by (R)soft 11.11.2011 Last Edition: 2 Dec 2011
// Version 1.2
// Tested on Mega2561
// Testing on ALCATEL 556/557 LCD Module. Resolution 160x120 pixels. 
// Built-in HD66766 controller with parallel 16-bit interface.
// Reference manual for controller - HD66766R.pdf
// In Version 1.1 Added Output Color Palette and Output BMP file (24 bit) 2.12.2011
// 3.12.2011 v1.2 ������� ������� ������ BMP, ����������� ����� ���������� � ���� �������� ���
// ********************************************************************

/*
������� � �������� ��������� ����...
�������� �������� �� �������� � ���������� ������/������.
��� ��������, ���� �����������, �������� �� �����.
��������, ��� ������� � �������������� ������������� ���������� �� ����.
� ������� ������� ��� VCC=2,8V ����� 4,7 ���. ��������, ��� �������� 1 ���
�� ����� ������� �������. ��, ��� �������� �� �����������. �����������
���� ���, � ������� �� ��, ��� ������ ���������� ����� �������� ��� ��
����������� ��� � ��� � �� ����� ����� ���������� � ��������������� �������.

���������� �� ����������� ���:
��� ������� � ����� ���, ��� �������� ��� �� �������, ��������� �������,
����� ������� ������������ �������. ������� ������� �� Mega2561 �
���������� ��� ����� ��� ������ � ���. ��� ����� ��� ���� ������ (16 ���)
� ���� ���� ���������� (CTRL). ���� HI ��� ��������� � X6 (PORTB),
���� LO ��� ��������� � X5 (PORTA) � ���� ���������� CTRL ��� ���������
� X7 (PORTC). ���������� ���� (PORTD) ��������� � ��� NOKIA 6510 �
������ 3-wire SPI � ���������� ������� �� ����������� SED15B1.


*/

//=============================================================
// ������ ����� � ���
// hd66766_write(register_select,0xffff);
// register_select=0 - index/Status
// register_select=1 - Control

void hd66766_write (unsigned char rs, unsigned int word)
{
  XCS_0; // �������� ��������

  RW_0; // RW=0 - Write mode
  
  // ������� ���������� RS � ������������
  if (rs)
    {
      RS_1;
    }
  else
    {
      RS_0;
    }
  
  // ����������� �� ���� ������ 16-������� �����
  DDRD_HI=~((unsigned char)((word>>8)&0xFF)); // �������� ������� ���� � �����������  
  DDRD_LO=~((unsigned char)((word)&0xFF)); // �������� ������� ���� � �����������

//  delay_us(1);
  
  E_1; // ������ �����

  delay_us(2); // �������� �����, �������� ��� 3 ���������, �������� �� 2

  E_0;
  
  XCS_1; // ��������� ��������

}

//=============================================================
// ������ ����� �� ���
// word=hd66766_read(register_select);
// register_select=0 - index/Status
// register_select=1 - Control
// ��� ������� �������� ������������� ��� ������� ���� �
// ����������� ������� HD66766 - ������ ���������.
// ��� ������ id ��������� �������:
// hd66766_write(0,HD66766_R00);  // Set Index Register
// id1=hd66766_read(1); // rs=1 device code read
// ����� id1 ������ ���� 0x0667

unsigned int hd66766_read (unsigned char rs)
{
  XCS_0; // �������� ��������

  RW_1;  // RW=1 - Read mode  

  // ������� ���������� RS � ������������
  if (rs)
    {
      RS_1;
    }
  else
    {
      RS_0;
    }

  DDRD_HI=0; // ����� ������� ���� ������ ��������� � Z State
  DDRD_LO=0;
  
  E_1; // ����� ������ - ������ �����������
  delay_us(5); // �������� �����������! ����� �� �������� ��������������� ������ �� ����
  
  // ������ � ���� ������ 16-������ �����
  unsigned char lo=PIN_LO;
  unsigned char hi=PIN_HI; // ������� ����

  E_0;  // ������� ������ � �������� ���������
  
  XCS_1; // ��������� ��������
  return ((unsigned int)(hi<<8|lo));
}


/*

//==========================================================================
// ����� ������� �� ���

void hd66766_putchar(unsigned char ch, unsigned int color)
{
  unsigned int addr;

  // ���������� ������ ������ �������
  ch=ch-0x20; // ������������ ������� �������� ��� ������ 0x00..0x1F
  addr=ch;
  addr+=(addr<<2); // *5

  // ����� ���� ����
  for (ch=0; ch<5; ch++)  
    {  
      hd66766_data(font5x7[addr++]); 
    }
  hd66766_data(0); // ����� ���������� ����� ���������
}

*/


//====================================
// ������� ������

void hd66766_cls(void)
{
  unsigned int i;

  hd66766_write(0,HD66766_R21);  // Index Register 21 - RAM Address Set
  hd66766_write(1,0x0000);  // ��������� ���������� ������ ������=0

  hd66766_write(0,HD66766_R22);  // Index Register 22 - Write Data To CGRAM

  for(i=0; i<HD66766_PIXELX * HD66766_PIXELY; i++)   
  {   
    hd66766_write(1,WHITE_COLOR); // ��������� ������ ������� ������ �����
  }   

  // ����� ������� ������, ���������� - ������� ������ ������ �����

  hd66766_write(0,HD66766_R21);  // Index Register 21 - RAM Address Set
  hd66766_write(1,0x0000);  // ��������� ���������� ������ ������=0

}

//================================================================
// ����� �� ����� ������� ������ ��� ���������� �������������

void hd66766_colors(void)
{

  hd66766_write(0,HD66766_R21);  // Index Register 21 - RAM Address Set
  hd66766_write(1,0x0000);  // ��������� ���������� ������ ������

  
  unsigned int i;  
  hd66766_write(0,HD66766_R22);  // Index Register 22 - Write Data To CGRAM

  // ������ ���� - 20 �����, ����� 8 ������

  for(i=0;i<20*128;i++)   
  {   
    hd66766_write(1,WHITE_COLOR); // ��������� ������ ������� ������ �����
  }   

  for(i=0;i<20*128;i++)   
  {   
    hd66766_write(1,BLUE_COLOR);  // �����
  }   

  for(i=0;i<20*128;i++)   
  {   
    hd66766_write(1,GREEN_COLOR); // ������
  }   

  for(i=0;i<20*128;i++)   
  {   
    hd66766_write(1,RED_COLOR); // �������
  }   

  for(i=0;i<20*128;i++)   
  {   
    hd66766_write(1,BLACK_COLOR); // ׸����
  }   
  
  for(i=0;i<20*128;i++)   
  {   
    hd66766_write(1,YELLOW_COLOR);  // ������
  }   

  for(i=0;i<20*128;i++)   
  {   
    hd66766_write(1,LIGHTBLUE_COLOR); // �������
  }   

  for(i=0;i<20*128;i++)   
  {   
    hd66766_write(1,PURPLE_COLOR);  // ����������
  }   

}




/*

//=========================================
// ����� ������ (������ � ���)

void hd66766_puts (unsigned char *s)
{
  while (*s) // �� ��� ��� ���� �� ���� - �������
    hd66766_putchar(*s++);
}

*/


//=============================================================
// ������������� LCD � ������� ����� ������/����������

void hd66766_init (void)
{
  PIN_RS=0; // ������������� ���� ������������ ��� � ����
  PIN_XCS=0;
  PIN_E=0;
  PIN_RW=0;
  PIN_XRES=0;
  PD_LO=0;
  PD_HI=0;
  
  XCS_1; // ����� ��������� ������� - �������� ��������
  E_0;  // �������� ��������� Enable=0 (������ ������������� ����������� �������)
  RW_0;  // �������� ��������� RW=0 Write Mode (0 - ������, 1 - ������)

  XRES_0;  // Reset LCD
  delay_ms(10);
  XRES_1;  // Return Reset OFF
  delay_ms(50);

  
  hd66766_write(0,HD66766_R00);  // Index Register 00 - Start Oscillation
  hd66766_write(1,0x0001);  // ��������� ����������� ����������
  delay_ms(50); // ���� ��� ������� � ��������, ����� ��������� ������� ���� :)

  // 0 0 0 0 | 0 0 CMS SGS | 0 0 0 NL4 | NL3 NL2 NL1 NL0
  // 0 0 0 0 | 0 0  0   0  | 0 0 0  1  |  0   0   1   1
  // CMS=0 COM1-COM176
  // SGS=0 SEG1-SEG396
  // NL4..0=10011 - ������ �� �������� �� ���������������� TM128160CKFWG
  // Duty Ratio for 1/160 pixels - 0x13
  hd66766_write(0,HD66766_R01);  // Index Register 01 - Driver Output Control
  hd66766_write(1,0x0013);      // Set Duty Ratio

  // 0 0 0 0 | 0 RST B/C EOR | 0 0 NW5 NW4 | NW3 NW2 NW1 NW0
  // 0 0 0 0 | 0  0   0   0  | 0 0  0   0  |  0   0   0   0
  // RST=0 - Software Reset (1 - Active)
  // B/C=0
  // EOR=0
  // NW5..0=0 Pattern Waveform
  hd66766_write(0,HD66766_R02);  // Index Register 02 - LCD Driving Waveform Control
  hd66766_write(1,0x0000);
  
  // 0 0 0 0 | 0 VR2 VR1 VR0 | 0 CT6 CT5 CT4 | CT3 CT2 CT1 CT0
  // 0 0 0 0 | 0  1   1   0  | 0  0   1   0  |  1   1   0   0
  // VR2..0=110 - V1REF Voltage Settings: 000 - VrefL*1,1   111 - VrefL*3,4
  // CT6..0=1111000 - Variable Resistor VR: 0000000 - 1.016R    1111111 - 0.00R
  // �������� ������������� 0x2C �������� ����������� ��� ����������� ����������
  hd66766_write(0,HD66766_R04);  // Index Register 04 - Contrast Control
  hd66766_write(1,0x062C); // Low Byte - Contrast Value 00..7F (2C - optimal)
  
  // BS3 BS2 BS1 BS0 | BT3 BT2 BT1 BT0 |  0  DC2 DC1 DC0 | AP1 AP0 SLP STB
  //  0   1   0   1  |  1   1   1   1  |  0   1   1   1  |  1   0   0   0
  // BS3..0=0101 - BIAS=1/12; Booster2=5x; BIAS Ratio=1,3; VCi2=2,6xVM; VCH=VCi2x5
  // BT3=1 - Operation(1) or Halt(0) Volage Inverter
  // BT2..0=111 - Step-Up Voltage Multiplier, VOUT=3xVCi1, VCH=5xVCi2
  // DC2..0=111 Step-Up Freq Select: 000 - High (max) Freq->Better Quality but Increase Current
  //            (����������� ����� ������ ������� - ������� ��� �����������)
  // AP1..0=10 - Adjust Fixed Current. 00 - No Display (OpAmp & Booster Not Operate)
  //                                   01:Small Current 10:Middle 11:Large
  // SLP=0 - Sleep Mode
  // STB=0 - Standby Mode
  hd66766_write(0,HD66766_R03);  // Index Register 03 - Power Control 1
  hd66766_write(1,0x5F78);

  // 0 0 0 0 | 0 0 0 0 | 0 0 0 0 | 0 VC2 VC1 VC0
  // 0 0 0 0 | 0 0 0 0 | 0 0 0 0 | 0  0   1   1
  // VC2..0=111 - Sets adjusment factor for VCi Voltage 111 - External VCi=VCC=2,8V
  // =000 VCi=0,92xVCC
  hd66766_write(0,HD66766_R0C);  // Index Register 0C - Power Control 2
  hd66766_write(1,0x0003); // ���-�� �� ��������

  delay_ms(100);
  
  // SPR 0 0 0 | 0 0 HWM  0 | 0 0 I/D1 I/D0 | AM LG2 LG1 LG0
  //  0  0 0 0 | 0 0  0   0 | 0 0  1    1   |  0  0   0   0
  // I/D1=1 - Horisontal Address Counter Increment(1)/Decrement(0)
  // I/D0=1 - Vertical Address Counter Increment(1)/Decrement(0)
  // AM=0 - AC Counter Mode =0: Data written parallel =1: Data written vertically
  //         (by window address range)
  // LG2..0=000 - Logical Operations Write to CGRAM
  // HWM=0 - High Speed Mode, if = 1 then High Speed Mode Enable
  // SPR=0 - If SPR=1 then 4096 colors mode enable (12 bit - 3 ���� �� ����)
  hd66766_write(0,HD66766_R05);  // Index Register 05 - Entry Mode
  hd66766_write(1,0x0030);

  // CP15 CP4 CP13 CP12 | CP11 CP10 CP9 CP8 | CP7 CP6 CP5 CP4 | CP3 CP2 CP1 CP0
  // CP15...0=0
  hd66766_write(0,HD66766_R06);  // Index Register 06 - Compare Register
  hd66766_write(1,0x0000);

  // 0 0 0 0 | 0 VLE2 VLE1 SPT | 0 0 0 0 | B/W REV D1 D0
  // 0 0 0 0 | 0  0    0    0  | 0 0 0 0 |  0   0  1  1
  // SPT=0 (no screen division) - screen division driving
  // VLE=0 (co scroll) - Vertical Scroll
  // B/W=0 - 0: Normal operation; 1: All pixel displayed
  // REV=0 (not reversal) - Reversed display
  // D1..0=11 - Display ON
  hd66766_write(0,HD66766_R07);  // Index Register 07 - Display Control
  hd66766_write(1,0x0003);

  // 0 0 0 0 | 0 0 DIV1 DIV0 | 0 0 0 0 | RTN3 RTN2 RTN1 RTN0
  // 0 0 0 0 | 0 0  0    1   | 0 0 0 0 |  0    0    0    0
  // DIV1..0=01 fosc/2 - division ratio 2
  // RTN3..0=0000
  hd66766_write(0,HD66766_R0B);  // Index Register 0B - Frame Frequency Control
  hd66766_write(1,0x0100);

  // VL27 VL26 VL25 VL24 | VL23 VL22 VL21 VL20 | VL17 VL16 VL15 VL14 | VL13 VL12 VL11 VL10
  //  0    0    0    0   |  0    0    0    0   |  0    0    0    0   |  0    0    0    0
  // VL27..20=0
  // VL17..10=0
  hd66766_write(0,HD66766_R11);  // Index Register 11 - Vertical Scroll Control
  hd66766_write(1,0x0000);

  // R14 - 1st Screen Driving Position
  // SE17 SE16 SE15 SE14 | SE13 SE12 SE11 SE10 | SS17 SS16 SS15 SS14 | SS13 SS12 SS11 SS10
  // SE17..10=160 End
  // SS17..10=0 (com1-160) Start
  hd66766_write(0,HD66766_R14);  // Index Register 14 - 1st Screen Driving Position
  hd66766_write(1,0xA000); //???
  
  // R15 - 2nd Screen Driving Position
  // SE27 SE26 SE25 SE24 | SE23 SE22 SE21 SE20 | SS27 SS26 SS25 SS24 | SS23 SS22 SS21 SS20
  // SE27..20=00h
  // SS27..20=00h (com1-160)
  
  hd66766_write(0,HD66766_R15);  // Index Register 15 - 2nd Screen Driving Position
  hd66766_write(1,0x0000);
  
  // R16 - Horizontal RAM Address Position
  // HEA7 HEA6 HEA5 HEA4 | HEA3 HEA2 HEA1 HEA0 | HSA7 HSA6 HSA5 HSA4 | HSA3 HSA2 HSA1 HSA0
  //  1    0    0    0   |  0    0    1    1   |  0    0    0    0   |  0    0    0    0
  // HEA7..0= (end address) (127=0x7F) �� ������� ������ ��������������� ���������
  // HSA7..0=00 (start address)
  hd66766_write(0,HD66766_R16);  // Index Register 16 - Horizontal RAM Address Position
  hd66766_write(1,0x7F00);

  // R17 - Vertical RAM Address Position
  // VEA7 VEA6 VEA5 VEA4 | VEA3 VEA2 VEA1 VEA0 | VSA7 VSA6 VSA5 VSA4 | VSA3 VSA2 VSA1 VSA0
  //  1    0    0    1   |  1    1    1    1   |  0    0    0    0   |  0    0    0    0
  // VEA7..0=9F end address (159=0x9F) - �� ������� ������ ������������� ����������
  // VSA7..0=00 (start address)
  hd66766_write(0,HD66766_R17);  // Index Register 17 - Vertical RAM Address Position
  hd66766_write(1,0x9F00);

  hd66766_write(0,HD66766_R20);  // Index Register 20 - RAM Mask
  hd66766_write(1,0x0000);

  //------------------------------------------
  // Grayscale Setup
  // ������� ����������� �� ����� �.�. ����� ������ HD66766
  // �������� ������� ���������������� �������������.
  
//  delay_ms(10);  

  //-----------------------------------------------------------------
  
//  hd66766_colors(); // ������� ������� ������

  
//  hd66753_cls(); // ������ �����-��� � ������������� ����� ������ � ������


  
}


/*

//====================================================================
// ������� �� ������ ������� CON �� ���
// ���������� ��������� � ���, ��� ��������� ���� ����� ���������� ��
// �������������.

void hd66766_update_console(void)
{
  unsigned char col,row;
  char *p;
  char c;  
  p=CON66753;

  for (row=0; row<HD66766_ROW; row++)
  {
  hd66753_cmd(HD66766_SETYADDR | row); // ��������� Y
  hd66753_cmd(HD66766_SETXADDR_UPPER); 
  hd66753_cmd(HD66766_SETXADDR_LOWER); // X=0
    for (col=0; col<HD66766_COL; col++)
      {
        c=*p++;
        if (c==0) 
          {
            c=0x20; // If End of Line (0x00) then "SPACE" character
          }
        hd66766_putchar(c);
      }
   }
}

*/


//==================================
// ������ ����� ������� ��������� ������

void hd66766_clear_console (unsigned char byte)
{
 unsigned int i;
 char *p;
 p=CON66766;
 i=sizeof(CON66766);
 do
   {
     *p++=byte;
   }
  while (--i);
}


//=============================== BMP Output ============================
// width - ������ ��������
// heigth - ������ ��������
// *data - ��������� �� ������ RGB � ������� 24 ���� �� ���� � ���������� ������ BGR
// ������ ������������� �� 24-������� ����� � 16-������ ����� ������������ �������
// ����� (�������, ����� - ������������� ��� �������� ����; ������ - ��� ����).


void hd66766_bmp_output(unsigned char width, unsigned char height, unsigned char __flash *data)
{
  unsigned char w,h;
  unsigned int index=0; // ��������� �� ������� ������ ������� bmp
  unsigned char red, green, blue;
  unsigned int out_color;
  
  
  hd66766_write(0,HD66766_R21);  // Index Register 21 - RAM Address Set
  hd66766_write(1,0x0000);  // ��������� ���������� ������ ������

  hd66766_write(0,HD66766_R22);  // Index Register 22 - Write Data To CGRAM
  
  for (h=0; h<height; h++)  // ��������� ����� �������� �������-������
    {
      for (w=0; w<width; w++)
        {
          red=data[index];  // ���� ��������������� �� ������� �����
          index++;
          green=data[index];
          index++;
          blue=data[index];
          index++;

          red=(red>>3)&0x1F;  // � ������ ����� ����������� ������ ���� ���� ������ � �����
          green=(green>>2)&0x3F;
          blue=(blue>>3)&0x1F;
          out_color=((blue<<11)|(green<<5)|red); // �������� ��� ����� ������� � ����� �����
          
          hd66766_write(1,out_color); // ��������� ������ �������          
        }
    }
}










