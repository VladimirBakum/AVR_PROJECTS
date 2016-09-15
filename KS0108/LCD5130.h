// �� ����� ����� ����� ������� LCD

#define NOKIA_D_C    PORTA_Bit0 // Data/Command Bit
#define NOKIA_SCK    PORTA_Bit1 // Clock Data
#define NOKIA_DOUT   PORTA_Bit2 // Serial Data
#define NOKIA_CS     PORTA_Bit3 // Chip Select
#define NOKIA_RESET  PORTA_Bit4 // Reset LCD
#define NOKIA_BLIGHT PORTA_Bit5 // Back Light LCD

// ������� ������
#define GLCD_PIXELX (84)
#define GLCD_PIXELY (48)

// ������� ��� (Normal Functionset)
#define GLCD_FUNCTIONSET    (0x20)
#define GLCD_SETYADDR       (0x40)
#define GLCD_SETXADDR       (0x80)
#define GLCD_DISPLAYBLANK   (0x08)
#define GLCD_DISPLAYFLUSH   (0x09)
#define GLCD_DISPLAYNORMAL  (0x0C)
#define GLCD_DISPLAYINVERT  (0x0D)

// ������� ��� (Extended Functionset)
#define GLCD_FUNCTIONSETEXT (0x21)
#define GLCD_SET_BIAS       (0x10)
#define GLCD_SET_VOP        (0x80)
#define GLCD_TEMPCOEF       (0x06)  // TF=2

#define GLCD_BIAS_100       (0x00) // 1:100
#define GLCD_BIAS_80        (0x01) // 1:80
#define GLCD_BIAS_65        (0x02)
#define GLCD_BIAS_48        (0x03)
#define GLCD_BIAS_40        (0x04)
#define GLCD_BIAS_24        (0x05)
#define GLCD_BIAS_18        (0x06)
#define GLCD_BIAS_10        (0x07)

// �������� ���������� (volatile) ���������� LCD
volatile unsigned char nokia_x=0; // ������� X, ������� ����������� ��� ������ ������
volatile unsigned char nokia_y=0;
volatile unsigned char nokia_contrast=0; // �������� �������������
volatile unsigned char nokia_inverse=0; // �������� ������ (0 - ���������)

// 14 �������� � ������, ����� 6 ����� ������
char CON [14*6+1]; // CONsole buffer ��� ������ �������� printf

// function prototypes
void nokia_w (unsigned char ch); // ������ ����� � ���
void nokia_cmd(unsigned char cmd); // �������� ������� � ���
void nokia_data(unsigned char dat); // �������� ������ � ���
void nokia_set_addr(unsigned char x, unsigned char y); // ��������� ������ � ���
void nokia_gotoxy(unsigned char x, unsigned char y); // ������� � ����������� x,y
void nokia_putchar(unsigned char ch); // ����� ������� �� ���
void nokia_cls(void); // ������� ������
void nokia_puts (unsigned char *s); // ����� ������ (������ � ���)
void nokia_set_contrast(unsigned char c); // ������ �������� ���������
void nokia_set_inverse(unsigned char inv); // ������ �������� ��������
void nokia_init (void); // ������������� �����������
void UpdateConsole(void); // ������� �� ������ ������� CON �� ���
void ClrScr (void); // ������� ������ ������� CON
// void LcdPixel (unsigned char x, unsigned char y); // ����� �����
// void LcdPixelErase (unsigned char x, unsigned char y); // �������� �����
// void erase_column (unsigned char x, unsigned char y); // �������� �������