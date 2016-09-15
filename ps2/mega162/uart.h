#define SIZE_BUF        (64)   // ������ ������ ������/��������
#define MCU_QUARTZ      (8000000)
#define Host_Baudrate   (9600)
#define TIMEOUT_UART    (3)     // Timeout = 3 ms
#define Baudrate_Register ((unsigned short)(((unsigned long)MCU_QUARTZ)/(16UL*Host_Baudrate))-1)

//unsigned char *Pointer_BUF=&UART_Buf[SIZE_BUF]; // ��������� ������ ������
unsigned char HowMany_TX;  // ������� ���� �������� � ����������
unsigned char HowMany_RX;  // ���������� �������� ����
unsigned char Pointer_TX; // ��������� �������� ��������� ������ ��������
unsigned char TimeOut_RX;  // ������� �� ������ (� ��)
unsigned char ReceiveOK;  // ���� �������� ����������, ���� �� ����, �� ������ �������� �����.

unsigned char LED_counter;

char UART_Buf[SIZE_BUF]; // ����� ���������-����������� UART



