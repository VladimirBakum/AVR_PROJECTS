//unsigned char __eeprom *zp;// = (unsigned char __eeprom *) 0x00;

#define MIN_TEMPERATURE (0)   // ���. �����������, ������������ ������
#define MAX_TEMPERATURE (70)  // ����. �����������, ������������ ������
#define MIN_SPIN        (0)   // ���. ������� ������
#define MAX_SPIN        (8)   // ����. ������� ������ (8 ������������� 800 ��/���)

unsigned char __eeprom EE_tProgram8;    // ����������� ������� ��� 8 ���������
unsigned char __eeprom EE_tProgram9;    // ����������� ������� ��� 9 ���������
unsigned char __eeprom EE_rpmProgram8;  // ������� ������ ��� ��������� 8
unsigned char __eeprom EE_rpmProgram9;  // ������� ������ ��� ��������� 9
unsigned char __eeprom EE_rpmProgram12; // ������� ������ ��� ��������� 12
unsigned char __eeprom EE_rpmProgram13; // ������� ������ ��� ��������� 13
unsigned char __eeprom EE_rpmProgram14; // ������� ������ ��� ��������� 14
unsigned char __eeprom EE_rpmProgram15; // ������� ������ ��� ��������� 15

//---------------------------------------------------
// �������� � ������������� EEPROM.
// ���� ����� �������� ����� ������ � EEPROM ����������, �� ����� ��������
// ���������� �������� �� ���������.
void InitEEPROM (void)
{
  unsigned char __eeprom *zp = 0;
  unsigned char byte = *zp;       // ������ ���� �� EEPROM
  if (byte==0xFF)                 // ���� 0xFF �� ������ �� ����������������
  {
    __disable_interrupt();
    *zp++ = 10; // ���������� ���������� �������� t for program 8
    *zp++ = 10; // ���������� ���������� �������� t for program 9
    *zp++ = 3;  // ������� ������ ��� ��������� 8
    *zp++ = 3;  // ������� ������ ��� ��������� 9
    *zp++ = 4;  // ������� ������ ��� ��������� 12
    *zp++ = 4;  // ������� ������ ��� ��������� 13
    *zp++ = 5;  // ������� ������ ��� ��������� 14
    *zp++ = 4;  // ������� ������ ��� ��������� 15
    __enable_interrupt();
  }
}


//----------------------------------------------------------------------------
// Store to EEPROM
void StoreEEPROM (unsigned char __eeprom *address, unsigned char byte)
{
  __disable_interrupt();
  *address = byte;
  __enable_interrupt();  
}

//---------------------------------------------------------------------------
// Load from EEPROM
unsigned char LoadEEPROM (unsigned char __eeprom *address)
{
  unsigned char byte;

  byte = *address;

  return byte;
}


