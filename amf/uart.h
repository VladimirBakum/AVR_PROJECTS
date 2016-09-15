#define QUARTZ    (8000000)
#define SIZE_BUF  (128)   // ������ ������ ������/��������
#define Baudrate  (9600)
#define TIMEOUT   (3)     // Timeout = 3 ms
#define Baudrate_Register ((unsigned short)(((unsigned long)QUARTZ)/(16UL*Baudrate))-1)

unsigned char HowMany_TX;  // ������� ���� �������� � ����������
volatile unsigned char HowMany_RX;  // ���������� �������� ����
volatile unsigned char Pointer_TX; // ��������� �������� ��������� ������ ��������
volatile unsigned char TimeOut_RX;  // ������� �� ������ (� ��)
volatile unsigned char ReceiveOK;  // ���� �������� ����������, ���� �� ����, �� ������ �������� �����.
volatile unsigned char SecundaOK;   // ��������� ����. ���� �� ����, �� ������ �������

char UART_Buf[SIZE_BUF]; // ����� ���������-����������� UART



