//===============================================================
// ---=== AD7714 Routines for AVR family ===---
// AVR - ATmega2561-16 on Oscillator 14.318 MHz
// ��������� ������ �� SPI ����� ��� 24-������� �����-������ ��� AD7714AN-3
// (��������� ��������� 2,4576 MHz)
// *** Version 0.01 *** 16 Aug 2011 04:10 by (R)soft
// ��������� ����������� ���������� ���:
// POL=1 (���������� ������������ - ������ ������������� �� ������������ ������);
// /STANDBY=1 (����� ������ ����������������� - ��������);
// /RESET=1 (����� ��� - �� ������������; ������������ ����������� �����);
// /CS=PB6 (����� ��������� - PORT B bit 6);
// /SYNC=1 (������������� ����������� ���������� ��� - �� ������������);
// /DRDY=PB4 (����� ���������� ������ �������������� - PORT B bit 4)
// BUFFER=1 (��������������� ��������);
// ��� ������ � ��� ������������ ���������� SPI, � ���� SPI ����������
// ������ ��� ��������������:
// DIN=PB2 MOSI (����� �������� ������ �� MCU � ���);
// DOUT=PB3 MISO (����� �������� ������ �� ��� � MCU);
// SCLK=PB1 SCK (����� ������������ ������ �� SPI).

//============================================================
// ��������� ������ ����� � SPI
// 

unsigned char Read_SPI_7714 (void)
{
//  AD7714_PORT &= ~(1<<CS_7714); // CS=0

  SPDR = 0xFF;
  while (!(SPSR & (1<<SPIF)));

//  AD7714_PORT |= (1<<CS_7714);  // CS=1

  return SPDR;
}

//============================================================
// ��������� ������ ����� �� SPI
// 

void Write_SPI_7714 (unsigned char byte)
{
//  AD7714_PORT &= ~(1<<CS_7714); // CS=0

  SPDR = byte;
  while (!(SPSR & (1<<SPIF)));

//  AD7714_PORT |= (1<<CS_7714); // CS=1
}

//=================================================
// ��������� ������������� ���������� 7714

void Init_7714 (void)
{
  // ����������� ����� SPI � ���� ���.
  // �������������� ����� � ��������� ������ � ��������� � ���. 1.
  // PB6 CS_7714 - �� �����, �������� �� 1;
  // PB4 DRDY_7714 - �� ����, �������� �� 1;
  // PB3 MISO - �� ����, �������� �� 1;
  // PB2 MOSI - �� �����, �������� �� 1;
  // PB1 SCK - �� �����, �������� �� 1;
  // PB0 SS - ����������� ���������� � 1 �� �����, ����� SPI �� ����� ��������.
  // 0xE7 1110 0111
  AD7714_DIR =  (1<<PB7)|(1<<CS_7714)|(1<<PB5)|(0<<DRDY_7714)|(0<<DD_MISO)|(1<<DD_MOSI)|(1<<DD_SCK)|(1<<PB0);

  // 0xFF 1111 1111
  AD7714_PORT = (1<<PB7)|(1<<CS_7714)|(1<<PB5)|(1<<DRDY_7714)|(1<<DD_MISO)|(1<<DD_MOSI)|(1<<DD_SCK)|(1<<PB0);

  // ========================= SPI SETUP
  // (0x5B by Default 0101 1011)
  // SPIE=0 SPE=1 DORD=0 MSTR=1  CPOL=1 CPHA=0 SPR1=1 SPR0=1
  // Bit7 SPIE=0 Interrupt Disable
  // Bit6 SPE=1 SPI Enable
  // Bit5 DORD=0 Data Order (MSB First)
  // Bit4 MSTR=1 SPI Mode (Master)
  // Bit3 CPOL=0 Clock Polarity=1 _/
  // Bit2 CPHA=0 Clock Phase
  // Bit1..0 SPR1=SPR0=1 Clock Rate 1/128 (������ ����� = 16 us)
  //         SPR1=SPR0=0 Clock Rate 1/4   (������ ����� = 500 ��) ��� ���������
  
  // 0x5F 0101 0011
  SPCR = (0<<SPIE)|(1<<SPE)|(0<<DORD)|(1<<MSTR)|(0<<CPOL)|(0<<CPHA)|(1<<SPR1)|(1<<SPR0);
  
}

//==========================================================
// ��������� ������������ ������ 7714
// ����� ������� ����������� ����� ���, ����� ������ �� ����
// ������ 64 ���� ��� �������, �.�. 8 ���� 0xFF ������.

void Reset_7714 (void)
{
//  AD7714_PORT &= ~(1<<CS_7714); // CS=0

  unsigned char i;
  for (i=0;i<8;i++)
  {
    SPDR = 0xFF;
    while (!SPSR_SPIF);
  }

//  AD7714_PORT |= (1<<CS_7714); // CS=1
}

//=======================================================
// ��������� ����������� ������ ���������� 7714 �� ����� /DRDY.
// ������������� ���� ErrorFlag_7714=0 ���� ����� ��� 0xFF ���� �� ����� (�������).
// ������� ���������� 5 ������.

/*
void Ready_7714 (void)
{
//  AD7714_PORT &= ~(1<<CS_7714); // CS=0

  unsigned int i;
  for (i=0;i<5000;i++)
  {
    if (!(AD7714_PIN&(1<<DRDY_7714))) // ���� ��� ���������� DRDY=0
    {
      ErrorFlag_7714 = 0x00;
//      AD7714_PORT |= (1<<CS_7714); // CS=1
      return;
    }
    delay_ms(1);  // ������� ��������, ���� ��� �� ����� � ��������� ����� � �����
  }

  ErrorFlag_7714 = 0xFF; // ���� ���� ���� ������ � ������, �� ���-�� ����� ����� ������ ���... ����� ������
//  AD7714_PORT |= (1<<CS_7714); // CS=1
}
*/

//=======================================================
// ��������� ������������ ������ ���������� 7714.
// ������������� ���� ErrorFlag_7714=0 ���� ����� ��� 0xFF ���� �� ����� (�������).
// ������� ���������� 12 ������.

void Ready_Soft_7714 (void)
{
//  AD7714_PORT &= ~(1<<CS_7714); // CS=0

  unsigned int i;
  for (i=0;i<12000;i++)
  {
    Write_SPI_7714 (0x08|(ADC7714_Channel&0x07));
//    unsigned char result = Read_SPI_7714();
    
    if ((Read_SPI_7714() & 0x80)==0) // ���� ��� ���������� DRDY=0 (Bit7)
    {
      ErrorFlag_7714 = 0x00;
//      AD7714_PORT |= (1<<CS_7714); // CS=1
      return;
    }
    delay_ms(1);  // ������� ��������, ���� ��� �� ����� � ��������� ����� � �����
  }

  ErrorFlag_7714 = 0xFF; // ���� ���� ���� ������ � ������, �� ���-�� ����� ����� ������ ���... ����� ������
//  AD7714_PORT |= (1<<CS_7714); // CS=1
}

//===========================================================
// ��������� 8-������� ������ �� �������� 7714
// ����: ����� �������� ��� [0..4]
// �����: ������ �������� ���;
//        ���� Error=0 ���� �� ���� ������ �������� � ���� Error=0xFF ���� ��� �������

unsigned char ReadReg_7714 (unsigned char reg)
{
  Ready_Soft_7714(); // ����� ���������� ���
  if (ErrorFlag_7714) // ���� ��� ������� ��� ������ ���������� ���, ��
    {
      return 0; // ������� �� ���������
    }
  // command=0x08 (0B00001000) - ������� ������ ��������
  // ��������� ������� � ������� ������ � � ������� �������� (����� �������� � ������� �������)
  Write_SPI_7714 (((reg&0x07)<<4)|0x08|(ADC7714_Channel&0x07)); 
  return Read_SPI_7714 (); // ������ � ���������� ����������� ������� ���
}

//===========================================================
// ��������� 8-������ ������ � ������� 7714
// ����: ����� �������� ��� [0..4];
//       ���� ������ ��� ������.
// �����: ���� ErrorFlag_7714=0 ���� �� ���� ������ �������� � ���� ErrorFlag_7714=0xFF ���� ��� �������

void WriteReg_7714 (unsigned char reg, unsigned char byte)
{  
  Ready_Soft_7714();
  if (ErrorFlag_7714==0)
    {
      // ��������� ����� ������ � ����� �������� (����� �������� � ������� �������)
      Write_SPI_7714 (((reg&0x07)<<4)|(ADC7714_Channel&0x07));
      Write_SPI_7714 (byte);
    }
}

//============================================================
// ��������� ������ 24-������� ���������� 7714
// ����: -
// �����: ���� ErrorFlag_7714=0 ���� �� ���� ������ �������� � ���� ErrorFlag_7714=0xFF ���� ��� �������
//        ���� ����=0, �� ��������� ������������ � ���������� ADC7714_A, ADC7714_B, ADC7714_C.

void Read_ADC_7714 (void)
{
  Ready_Soft_7714 (); // ����� ���������� ���
  if (ErrorFlag_7714==0) // ���� ���������� ��� - ��, ��
    {
      Write_SPI_7714 (0x58|(ADC7714_Channel&0x07)); // ������� ������ 24-������� �������� �� ��� 0B0101 1000
      ADC7714_C = Read_SPI_7714();
      ADC7714_B = Read_SPI_7714();
      ADC7714_A = Read_SPI_7714();
    }
}