#define MAX_AVERAGE_WORDS (16)  // ������������ ���-�� �������� ���������� �� ������ ������
#define AVERAGE_FACTOR_I  (3)   // �� ������� ��������� ��������� (������� �����) 1...15
#define AVERAGE_FACTOR_T  (15)  // �� ������� ��������� ��������� (������������� �����) 1...15
// #define CAPTURE_ERROR     (26) // �������� ���������� ����������� �����������

//============== ���������� ����������� ��� ��������� Moving Average
// ��������� ������ ����������� ����������
volatile unsigned int AverageBufI[BSI_N_ICHANNEL][MAX_AVERAGE_WORDS];  // ����� �������� ����������� ��������
volatile unsigned int AverageBufT[BSI_N_TCHANNEL][MAX_AVERAGE_WORDS];

unsigned long AverageSumI[BSI_N_ICHANNEL];  // ����������� ����� �� ������
unsigned long AverageSumT[BSI_N_TCHANNEL];

unsigned char AverageIndexI[BSI_N_ICHANNEL];  // ��������� �������� ��������
unsigned char AverageIndexT[BSI_N_TCHANNEL];

unsigned char AverageBufFullI[BSI_N_ICHANNEL];  // ���� ���������� ������
unsigned char AverageBufFullT[BSI_N_TCHANNEL];


//======================================================================

// BSI_N_ICHANNEL+BSI_N_TCHANNEL = 6 - ����������� ������� ������ ����������
//volatile unsigned char AverageKI[BSI_N_ICHANNEL]; // �� ������� ��������� ��������� (�������� �������� ��������������)
//volatile unsigned char AverageKT[BSI_N_TCHANNEL];

// �������� ���������� ��� ��������
//volatile unsigned int debug;

// ================ ������������ ��� ������� ====================
// 6 ������������� 0..3-�������, 4..5-�������������
//volatile signed char KHI[BSI_N_ICHANNEL]; // -128...+127
//volatile signed char KHT[BSI_N_TCHANNEL];

//volatile signed char KLI[BSI_N_ICHANNEL]; // -128...+127
//volatile signed char KLT[BSI_N_TCHANNEL];

//volatile unsigned int AI[BSI_N_ICHANNEL];  // 0...65535
//volatile unsigned int AT[BSI_N_TCHANNEL];

//volatile signed char BI[BSI_N_ICHANNEL]; // -128...+127
//volatile signed char BT[BSI_N_TCHANNEL];

//------------------------------------------------------------
// ���� ��������� ���� ������������ ������ 
// ����������� �������� ����� �������

/*

typedef union
{
  char byte;
  struct
  {
    char S0:1,
         S1:1,
         S2:1,
         S3:1,
         S4:1,
         S5:1,
         S6:1,
         S7:1;
  };
}sRGKS;
volatile sRGKS RGKS;

*/
