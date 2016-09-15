// APIS washing program by (R)soft 2015



//-------------------------------------------------------------------------------------
// ����������� ������ ��������� ������. �������� ������ �������
// ����� �������, ����� ��� ��������� ����� � ������� ���������������� ������� ������
// ���� ����������� ������� ������ 4 � ���������� ���������� ���������.
#define PRG_Pause         0xA0  // 2 bytes - ����� (�������� �������)
#define PRG_LockState     0xA1  // 2 bytes - ���������� ������ ����
#define PRG_WaterState    0xA2  // 3 bytes - ���������� ������� ����
#define PRG_PompState     0xA3  // 3 bytes - ���������� ������
#define PRG_HeatingState  0xA4  // 3 bytes - ���������� �����
#define PRG_Temperature   0xA5  // ��������� ����������� ��������� ���� (�� ��������� 60 ��������)
#define PRG_RotorState    0xA6  // 3 bytes - ���������� ��������� ������
#define PRG_ForwardState  0xA7  // 3 bytes - ��������� ������ ������
#define PRG_ReverseState  0xA8  // 3 bytes - ��������� ������ �����
#define PRG_SaveState     0xA9  // ��������� ������� ��������� ������������ �������� � EEPROM
#define PRG_Buzzer        0xAA  // 2 bytes - �������� ������ �� ����� t
#define PRG_MotorSpin     0xAB  // 3 bytes - ������� ������ � ���������� ������ � ������ �����
#define PRG_MotorZigZag   0xAC  // 3 bytes - ��� ����� ������, �����, ����, �����, ���� ����� �����, �����, ����, �����
#define PRG_LockWait      0xAD  // 1 byte - �������� �������� ����
#define PRG_TWait         0xAE  // 1 byte - �������� ������� ���� �� ������������� �����������
#define PRG_L2Wait        0xAF  // 1 byte - �������� ������������ ������� ������ LEVEL2=0
#define PRG_End           0xEE  // 1 byte - ����� ���������


//-----------------------------------------------------------------------------------
// --- ������� �������� ������ ---
// ���� ���� ��������� �������� ��� �����:
// ������ - �������, ������ - �������� �������, ������ - ����� ���������� � ��������
// ������ �������� ����� ���� ���� (���� �� ����� �������� �������).
// ��������� ������� ������� ��������, �������� ���������� ���� ��� ������� � ����.
// �� ��� ���, ���� ��������� ������� ��������� �� ����� ���������, ��������� ���������
// ����� ������ ����� ����� � �������� (�� ������ ��� ��� ������ �������� ����).
// ��� �������� ����� �������� ������� �����/���� ��������.
// ��������� ���� ������ ���� ���� PRG_End - ����� ���������.


//-----------------------------------------------------------------------
// ��������� 9 - ������� ������ ��� ����������, ���� � ���������� �����
static __flash unsigned char PROGRAM9[]=
{
  PRG_LockState,    0xFF, // ������������� ���
//  PRG_LockWait, // ���� �������� � ���������� ����
  PRG_Buzzer,     0x05, // ������
  PRG_L2Wait,   // ���� ������ LEVEL2=0 (� ���� ���� ����)

  PRG_MotorZigZag, 0x37, 10, // ������ ���� 10 �
  PRG_MotorZigZag, 0x37, 10,
  PRG_MotorZigZag, 0x37, 10,
  PRG_MotorZigZag, 0x37, 10,
  PRG_MotorZigZag, 0x37, 10,
  PRG_MotorZigZag, 0x37, 10,
  PRG_MotorZigZag, 0x37, 10,
  PRG_MotorZigZag, 0x37, 10,  
  PRG_MotorZigZag, 0x37, 10,
  PRG_MotorZigZag, 0x37, 10,

  PRG_Pause, 2,

  PRG_PompState,    0xFF, 1, // �������� �����, ���������

  PRG_MotorSpin,    0x41, 30, // ������ ����������� �� �������� 0x41 � ������ 30 �

  PRG_PompState,    0x00, 0, // ��������� �����
  PRG_LockState,    0x00,    // �������������� ���

  PRG_End

};

//---------------------------------------
// ��������� 14 - ���� � ����������� �����
static __flash unsigned char PROGRAM14[]=
{
  PRG_LockState,    0xFF, // ������������� ���
//  PRG_LockWait, // ���� �������� ����
  PRG_Buzzer,     0x05, // ������
  PRG_PompState,    0xFF, 1, // �������� �����, ���������

  PRG_MotorSpin,    0x50, 40, // ������ ����������� �� �������� 0x50 � ������ 40 �

  PRG_PompState,    0x00, 0, // ��������� �����
  PRG_LockState,    0x00,    // �������������� ���
  
  PRG_End

};


//---------------------------------------
// ��������� 15 - ���� � ���������� �����
static __flash unsigned char PROGRAM15[]=
{
  PRG_LockState,    0xFF, // ������������� ���
//  PRG_LockWait, // ���� �������� ����
  PRG_Buzzer,     0x05, // ������
  PRG_PompState,    0xFF, 1, // �������� �����, ���������

  PRG_MotorSpin,    0x41, 30, // ������ ����������� �� �������� 0x41 � ������ 30 �

  PRG_PompState,    0x00, 0, // ��������� �����
  PRG_LockState,    0x00,    // �������������� ���

  PRG_End
};

//---------------------------------------
// ��������� 16 - ���� ����
static __flash unsigned char PROGRAM16[]=
{
  PRG_LockState,  0xFF, // ������������� ���
//  PRG_LockWait, // ����� ��������� �������� ����
  PRG_Buzzer,     0x05, // ������
  PRG_PompState,  0xFF, 20,  // �������� ����� �� 20 �
  PRG_PompState,  0x00, 0,   // ��������� �����
  PRG_LockState,  0x00,  // �������������� ���

  PRG_End // ����� ���������
};
