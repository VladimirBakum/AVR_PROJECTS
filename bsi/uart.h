#define SIZE_BUF        (256)   // ������ ������ ������/��������
#define ModBus_Baudrate (9600)
#define TIMEOUT         (3)     // Timeout = 3 ms
#define Baudrate_Register ((unsigned short)(((unsigned long)BSI_QUARTZ)/(16UL*ModBus_Baudrate))-1)

unsigned char Net_Addr; // ������� ����� ����� � ������� (1 ��� 2)

//unsigned char *Pointer_BUF=&UART_Buf[SIZE_BUF]; // ��������� ������ ������
unsigned char HowMany_TX;  // ������� ���� �������� � ����������
volatile unsigned char HowMany_RX;  // ���������� �������� ���� (unsigned int ������, ��� ����� ������ 300 ����)
volatile unsigned char Pointer_TX; // ��������� �������� ��������� ������ ��������
volatile unsigned char TimeOut_RX;  // ������� �� ������ (� ��)
volatile unsigned char ReceiveOK;  // ���� �������� ����������, ���� �� ����, �� ������ �������� �����.
volatile unsigned char SecundaOK;   // ��������� ����. ���� �� ����, �� ������ �������

unsigned char UART_Buf[SIZE_BUF]; // ����� ���������-����������� UART



