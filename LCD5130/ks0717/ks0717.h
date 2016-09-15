// KS0717 Header by (R)soft 8.5.2013
// PORTD - Control, PORTC - DATA
// ������ RESET ����� �� ���������� � �����������, ���������,
// ������� ��� �� +VCC


// �� ����� ����� ����� ������� LCD
#define PIN_RESETN  PORTD_Bit3  // /RESET - ����� ���, ���. ������� ������
#define DDR_RESETN  DDRD_Bit3
#define PIN_RS      PORTD_Bit4  // RS - Register Select: LO - Write to Register
                                //                       HI - DATA Display
#define DDR_RS      DDRD_Bit4
#define PIN_ENABLE  PORTD_Bit5  // ENABLE - ����� ����, ������ ����������
                                // �� ���������� _/
#define DDR_ENABLE  DDRD_Bit5
#define PDATA	PORTC // ���� ������
#define	DDRDATA	DDRC  // ��� ������ � ���� ������, ���� ����� �������������� �������������


// ��� �� ���� -> �������� �������� ����������� � VCC
// ��� �� ����� -> ��������������� 0 (�������� ��� �������������)
#define RESETN_1  DDR_RESETN=0
#define RESETN_0  DDR_RESETN=1
#define RS_1      DDR_RS=0
#define RS_0      DDR_RS=1
#define ENABLE_1  DDR_ENABLE=0
#define ENABLE_0  DDR_ENABLE=1

// ������� ������ (���������� ������������ 100x55)
#define KS0717_PIXELX   (96) // � ����������� ���� �������� 100
#define KS0717_PIXELY   (54) // � ����������� 55
#define KS0717_ROW      (7) // ���������� �����
#define KS0717_COL      (16) // ���������� �������� � ������

// LCD Instructions
#define KS0717_DISPLAY_OFF    (0xAE)
#define KS0717_DISPLAY_ON     (0xAF)
#define KS0717_SETYADDR       (0x40)
#define KS0717_REF_VOLT_MODE  (0x81)
#define KS0717_SET_PAGE_ADDR  (0xB0)
#define KS0717_SETXADDR_LO    (0x00)
#define KS0717_SETXADDR_HI    (0x10)
#define KS0717_ADC_NORMAL     (0xA0) // �������������� �� X
#define KS0717_ADC_REVERSE    (0xA1)
#define KS0717_REVERSE_OFF    (0xA6)
#define KS0717_REVERSE_ON     (0xA7)
#define KS0717_ENTIRE_OFF     (0xA4)
#define KS0717_ENTIRE_ON      (0xA5)
#define KS0717_SELECT_BIAS_18 (0xA2)
#define KS0717_SELECT_BIAS_16 (0xA3)
#define KS0717_SET_MDF_READ   (0xE0)
#define KS0717_RESET_MDF_READ (0xEE)
#define KS0717_RESET_DISPLAY  (0xE2)
#define KS0717_SHL_NORMAL     (0xC0)
#define KS0717_SHL_REVERSE    (0xC8)
#define KS0717_POWER_CONTROL  (0x28)
#define KS0717_RESISTOR_SLCT  (0x20)
#define KS0717_STATIC_IND_OFF (0xAC)
#define KS0717_STATIC_IND_ON  (0xAD)

// 16 �������� � ������, ����� 6 ����� ������
char CON0717 [KS0717_COL*KS0717_ROW+1]; // CONsole buffer ��� ������ �������� sprintf

// ����������� ������ ������ ������ � �������
#define LINEN1 CON0717
#define LINEN2 CON0717+KS0717_COL
#define LINEN3 CON0717+(KS0717_COL*2)
#define LINEN4 CON0717+(KS0717_COL*3)
#define LINEN5 CON0717+(KS0717_COL*4)
#define LINEN6 CON0717+(KS0717_COL*5)
#define LINEN7 CON0717+(KS0717_COL*6)

// function prototypes
/*
void ks0717_write (unsigned char ch); // ������ ����� � ���
void ks0717_cmd(unsigned char cmd); // �������� ������� � ���
void ks0717_data(unsigned char dat); // �������� ������ � ���
void ks0717_set_addr(unsigned char x, unsigned char y); // ��������� ������ � ���
void ks0717_gotoxy(unsigned char x, unsigned char y); // ��������� ������� ������ �� ����������� x,y
void ks0717_putchar(unsigned char ch); // ����� ������� �� ���
void ks0717_cls(void); // ������� ������ - ���������� ��� ������, ��������� ������� ������ � ����
void ks0717_puts (unsigned char *s); // ����� ������ (������ � ���)
void ks0717_set_contrast(unsigned char c); // ������ �������� ���������
void ks0717_set_inverse(unsigned char inv); // ������ �������� ��������
void ks0717_set_power(unsigned char power); // ��������/��������� �������
void ks0717_init (void); // ������������� ����������� AVR
void ks0717_update_console(void); // ������� �� ������ ������� CON �� ���
void ks0717_clear_console (unsigned char byte); // ���������� ������ ������� CON ��������� ������
*/
