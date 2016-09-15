// ======================= HD66753 Routine ============================

// ********************************************************************
// Writen by (R)soft 23.10.2011 Last Edition: 23 Oct 2011
// Version 1.0
// Tested on Mega2561 & Mega162.
// Testing on ALCATEL 715 LCD Module. Resolution 150x100 pixels. 
// Built-in HD66752 (HD66753 compatible) controller with parallel interface.
// Reference manual for controller - HD66753.pdf
// ********************************************************************
/*
������� � �������� ��������� ����...
������� �������� 16-������ ������ � ���������� ������ �� ��� ����� - 
������ ���� ������� ����, ������ ������� ����.
��� ������������� � ���������� �������� ������, ����� ������ ��������
��������� �������� ������ ������� ����� � RS=0.
���� �� �������� ��������� �� ������, ������ ��������� 68-system:
������ RW ������ ���� - �.�. ����� ������ ������ � ����������.
E - ����� ��� ������, ������������� �����.


*/

//=============================================================
// ������ ����� � ���
// ������� MSB (������ ���� ������� ���)
// ������ � ���������� ��� �� ��� ������� � ��������: RS & 16-bit Word
// hd66753_write(1,0xffff);


void hd66753_write (unsigned char rs, unsigned int word)
{
  NCS_0; // �������� ��������
//  delay_ms(1);

/*  
  unsigned char i;

  DDRDATA=0xFF; // Set 0 for Data Bus
  for (i=0;i<4;i++)
  {
    E_1; // �������������
//    delay_ms(1);
    E_0;
//    delay_ms(1);    
  }
*/
  
  // ������� ���������� RS � ������������
  if (rs)
    {
      RS_1;
    }
  else
    {
      RS_0;
    }
//  delay_ms(1);

  // ������ ���� ������� ����
  DDRDATA=~((unsigned char)((word>>8)&0xFF)); // �������� ������� ���� � �����������
//  delay_ms(1);
  E_1; // ������ �����
//  delay_ms(1);
  E_0;
//  delay_ms(1);
  
  DDRDATA=~((unsigned char)((word)&0xFF)); // �������� ������� ���� � �����������
//  delay_ms(1);
  E_1; // ������ �����
//  delay_ms(1);
  E_0;
//  delay_ms(1);
  
  NCS_1; // ��������� ��������
//  delay_ms(1);
}

/*

//==========================================================================
// ����� ������� �� ���

void hd66753_putchar(unsigned char ch)
{
  unsigned int addr;

  // ���������� ������ ������ �������
  ch=ch-0x20; // ������������ ������� �������� ��� ������ 0x00..0x1F
  addr=ch;
  addr+=(addr<<2); // *5

  // ����� ���� ����
  for (ch=0; ch<5; ch++)  
    {  
      hd66753_data(font5x7[addr++]); 
    }
  hd66753_data(0); // ����� ���������� ����� ���������
}

*/


/*

//====================================
// ������� ������

void hd66753_cls(void)
{
  unsigned int i;
  hd66753_cmd(HD66753_SETYADDR); // ��������� Y=0
  hd66753_cmd(HD66753_SETXADDR_UPPER); // X=0
  hd66753_cmd(HD66753_SETXADDR_LOWER);

  for (i=0; i<(HD66753_PIXELX * HD66753_ROW); i++) // ���� ��� ������ �����
    {
      hd66753_data(0); // ������ � ��� LCD �����
    }

  // ����� ������� ������, ���������� - ������� ������ ������ �����
  hd66753_cmd(HD66753_SETYADDR); // ��������� Y=0
  hd66753_cmd(HD66753_SETXADDR_UPPER); // X=0
  hd66753_cmd(HD66753_SETXADDR_LOWER);
}

*/

/*

//=========================================
// ����� ������ (������ � ���)

void hd66753_puts (unsigned char *s)
{
  while (*s) // �� ��� ��� ���� �� ���� - �������
    hd66753_putchar(*s++);
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
// ������������� �� ��������:

// Instruction Set Initialization
// 1. Start Oscillation Executed
// 2. Driver Output Control (NL4-0=10000, SGS=0, CMS=0)
// 3. B-pattern waveform AC drive (B/C = 0, ECR = 0, NW4�0 = 00000)
// 4. Power control (PS1�0 = 00, DC1�0 = 00, AP1�0 = 00: LCD power off, SLP = 0: Sleep mode off, 
//    STB = 0: Standby mode off)
// 5. 1/11 bias drive (BS2�0 = 000), Three-times step-up (BT1�0 = 00), Weak contrast (CT6�0 = 0000000),
// 2.8-times output voltage for LCD-drive reference voltage generator (VR2�0 = 000)
// 6. Entry mode set (I/D = 1: Increment by 1, AM1�0 = 00: Horizontal move, LG1�0 = 00: Replace mode)
// 7. Rotation (RT2�0 = 000: No shift)
// 8. Display control (SPT = 0: GSH1-0 = GSL1-0 = 00, REV = 0, GS = 0, D = 0: Display off)
// 9. Cursor control (C = 0: Cursor display off, CM1�0 = 00)
// 10. 1st screen division (SS17�10 = 00000000, SE17�10 = 00000000)
// 11. 2nd screen division (SS27�20 = 00000000, SE27�20 = 00000000)
// 12. Window cursor display position (HS7�0 = HE7�0 = VS7�0 = VE7�0 = 00000000)
// 13. RAM write data mask (WM15�0 = 0000H: No mask)
// 14. RAM address set (AD12�0 = 000H)



void hd66753_init (void)
{
  PIN_RS=0; // ������������� ���� ������������ ��� � ����
  PIN_E=0;
  PIN_NCS=0;
  PIN_NRST=0;
  PDATA=0;
  
  NCS_0; // ����� ��������� �������
  E_0;  // �������� ��������� ����� ENABLE

  NRST_0;  // Reset LCD
  delay_ms(10);
  NRST_1;  // Return Reset OFF
  delay_ms(10);

  NCS_1;
  delay_ms(1);
  
  hd66753_write(0,HD66753_R00);  // Index Register 00 - Start Oscillation
  hd66753_write(1,0x0001);
  delay_ms(50); // ���� ��� ������� � ��������, ����� ��������� ������� ���� :)

  // * * * * | * * CMS SGS | * * * NL4 | NL3 NL2 NL1 NL0
  // 0 0 0 0 | 0 0  0   0  | 0 0 0  1  |  0   0   0   0
  // Duty Ratio for 1/132 pixels - 0x10
  hd66753_write(0,HD66753_R01);  // Index Register 01 - Driver Output Control
  hd66753_write(1,0x0010);      // Set Duty Ratio

  // * * * * | * * * * | * B/C EOR NW4 | NW3 NW2 NW1 NW0
  hd66753_write(0,HD66753_R02);  // Index Register 02 - Driving Waveform Control
  hd66753_write(1,0x000F);

  // * * * BS2 | BS1 BS0 BT1 BT0 | PS1 PS0 DC1 DC0 | AP1 AP0 SLP STB
  // 0 0 0 0   |  0   0   1   0  |  0   0   1   0  |  1   0   0   0
  // BS2..0 - Set Bias 000:1/11 001:1/10 010:1/9 011:1/8 100:1/7 101:1/6 110:1/5 111:1/4
  //          Optimum Bias 000 for 1/104 Duty Ratio
  // BT1..0 - Step-Up Voltage Multiplier 00:3x 01:5x 10:6x 11:7x
  // PS1..0 - Internal/External Power Supply as Reference Voltage for LCD Drive Voltage Generator
  // DC1..0 - Step-Up Freq Select - High->Better Quality but Increase Current
  //          00:32-divided 01:16-divided 10:8-divided 11:Inhibited(������� ���������)
  // AP1..0 - Adjust Fixed Current. 00 - No Display (OpAmp & Booster Not Operate)
  //          01:Small Current 10:Middle 11:Large
  // SLP - Sleep Mode
  // STB - Standby Mode
  hd66753_write(0,HD66753_R03);  // Index Register 03 - Power Control
  hd66753_write(1,0x0228);

  // * * * * | * VR2 VR1 VR0 | * CT6 CT5 CT4 | CT3 CT2 CT1 CT0
  // VR2...VR0 - V1REF Voltage Settings: 000 - Vreg*2,8   111 - Vreg*6,5
  // CT6...CT0 - Variable Resistor VR: 0000000 - 6,4*VR    1111111 - 0,05*VR
  hd66753_write(0,HD66753_R04);  // Index Register 04 - Contrast Control
  hd66753_write(1,0x0756); //00..7F
  
  // * * * * | * * * * | * * * I/D | AM1 AM0 LG1 LG0
  // 0 0 0 0 | 0 0 0 0 | 0 0 0  1  |  0   0   0   0  
  // I/D - Address Counter Increment/Decrement
  // AM1..0 - Automatic Update Method 00:Parallel 01:Vertically 10:Vertically With Two Words
  //          00: ��������� � ������� ������� (162 �������)
  //          01: ��������� ��������� ���
  //          10: ���������� 01
  // LG1..0 - Logical Operations Write to CGRAM 00:No Operation 01:OR 10:AND 11:EOR
  hd66753_write(0,HD66753_R05);  // Index Register 05 - Entry Mode
  hd66753_write(1,0x0010);
  
  // * * * * | * * * * | * * * * | * RT2 RT1 RT0
//  hd66753_write(0,HD66753_R06);  // Index Register 06 - Rotation
//  hd66753_write(1,0x0000);


  // * * * * | * * * SPT | * * GSH1 GSH0 | GSL1 GSL0 REV D
  // 0 0 0 0 | 0 0 0  0  | 0 0  0    0   |  0    0    0  1
  // SPT - 2-division LCD drive
  // GSH1..0 - Grayscale Level Control 00:3/4 01:2/3 10:2/4 11:No Grayscale (DB=10)
  // GSL1..0 - Grayscale Level Weakly-Conrolled 00:1/4 01:1/3 10:1/4 11:No Grayscale (DB=01)
  // REV - Reverse all display
  // D - Display ON
  hd66753_write(0,HD66753_R07);  // Index Register 07 - Display Control
  hd66753_write(1,0x0001);

  // * * * * | * * * * | * * * * | * C CM1 CM0
  // 0 0 0 0 | 0 0 0 0 | 0 0 0 0 | 0 1  1   1
  // C - Cursor Started
  // CM1..0 - Mode Cursor 00:White Blink 01:Black Blink 10:Black-White 11:Black-White Blink
  hd66753_write(0,HD66753_R08);  // Index Register 08 - Cursor Control
  hd66753_write(1,0x0007);

  // * * * AD12 | AD11 AD10 AD9 AD8 | AD7 AD6 AD5 AD4 | AD3 AD2 AD1 AD0
  // CGRAM ������������ ��� ��� ��� �� �� ������ ���� 256 ����� �� ���������� ��������
  // � Alcatel715 ����� ����� ������ 150 �������� (150*2)/16=18,75 ���� � ������
  // ������� ����������� ����� - �� �������� � ��������
  // 0x0000..0x0014 - COM1 (20 ���� �� 16 ��� = 320 ��� - 160 �������� � 4Grayscales
  // 0x0020..0x0034 - COM2 - ����� ������ ������
  // 0x0040..0x0054 - COM3
  // 0x0060..0x0074 - COM4
  // 0x0080..0x0094 - COM5
  // 0x00A0..0x00B4 - COM6
  // 0x1060..0x1074 - COM132 (� ��� 100)
  hd66753_write(0,HD66753_R11);  // Index Register 11 - LCD RAM Address Set
  hd66753_write(1,0x0400); // ������ ������� ����� - First Line 32*0x20

  unsigned int i;  
  hd66753_write(0,HD66753_R12);  // Index Register 12 - Write Data To CGRAM
  for(i=0;i<20*25;i++)   
  {   
//    hd66753_write(1,0xCCCC);
    hd66753_write(1,0x16B9); // 0001 0110 1011 1001 �������� ������
  }   

//  hd66753_write(0,HD66753_R11);  // Index Register 11 - LCD RAM Address Set
//  hd66753_write(1,0x0800); // 32 Line

//  hd66753_write(0,HD66753_R12);  // Index Register 12 - Write Data To CGRAM
  for(i=0;i<20*25;i++)
  {   
    hd66753_write(1,0x7777); // 0011 0011 0011 0011
  }   

//  hd66753_write(0,HD66753_R12);  // Index Register 12 - Write Data To CGRAM
  for(i=0;i<20*25;i++)
  {   
    hd66753_write(1,0xCCCC); // 1100 1100 1100 1100
  }   

//  hd66753_write(0,HD66753_R12);  // Index Register 12 - Write Data To CGRAM  
  for(i=0;i<20*25;i++)
  {   
    hd66753_write(1,0xFFFF);
  }   


  
//  hd66753_cls(); // ������ �����-��� � ������������� ����� ������ � ������
  
}

/*

//====================================================================
// ������� �� ������ ������� CON �� ���
// ���������� ��������� � ���, ��� ��������� ���� ����� ���������� ��
// �������������. �.�. ��������� ��� 128 (16*8) �������� �� ���������, 
// ��� �� ������ �����. ��, �.�. ���������� ��������� ����� �� ����������
// �� �������� 4 ���� (�.�. ��� ��������), ����� ����� ������ ���� ����
// �������� ����. ��� ������� ����� �������������� � ��������������
// ��������.

void hd66753_update_console(void)
{
  unsigned char col,row;
  char *p;
  char c;  
  p=CON66753;

  for (row=0; row<HD66753_ROW; row++)
  {
  hd66753_cmd(HD66753_SETYADDR | row); // ��������� Y
  hd66753_cmd(HD66753_SETXADDR_UPPER); 
  hd66753_cmd(HD66753_SETXADDR_LOWER); // X=0
    for (col=0; col<HD66753_COL; col++)
      {
        c=*p++;
        if (c==0) 
          {
            c=0x20; // If End of Line (0x00) then "SPACE" character
          }
        hd66753_putchar(c);
      }
   }
}
*/

//==================================
// ������ ����� ������� ��������� ������
void hd66753_clear_console (unsigned char byte)
{
 unsigned int i;
 char *p;
 p=CON66753;
 i=sizeof(CON66753);
 do
   {
     *p++=byte;
   }
  while (--i);
}

