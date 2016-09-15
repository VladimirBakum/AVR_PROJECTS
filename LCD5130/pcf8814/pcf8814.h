// ������� ���������� � STE2007

// �� ����� ����� ����� ������� LCD

#define PIN_SCLK    PORTA_Bit6 // Clock Data
#define DDR_SCLK    DDRA_Bit6
#define PIN_SDIN    PORTA_Bit7 // Serial Data Input
#define DDR_SDIN    DDRA_Bit7
#define PIN_RST     PORTA_Bit5 // Reset LCD ���� ��� ����� ���������� � �����������
#define DDR_RST     DDRA_Bit5
#define PIN_PORT    PINA

// ��� �� ���� -> �������� �������� ����������� � VCC
// ��� �� ����� -> ��������������� 0 (�������� ��� �������������)
#define SCLK_1  PIN_SCLK=1
#define SCLK_0  PIN_SCLK=0
#define SDIN_1  PIN_SDIN=1
#define SDIN_0  PIN_SDIN=0
#define RST_1   PIN_RST=1
#define RST_0   PIN_RST=0

// ������� ������
#define PCF8814_PIXELX  (96)
#define PCF8814_PIXELY  (65)
#define PCF8814_ROW     (8) // ���������� �����
#define PCF8814_COL     (16) // ���������� �������� � ������

#define PCF8814_DELAY   (1) // �������� � ���

// ������� ���

#define PCF8814_SET_BIAS_19         (0xA2) // Set BIAS 1/9
#define PCF8814_SET_BIAS_17         (0xA3) // Set BIAS 1/7

#define PCF8814_ALL_POINTS_OFF      (0xA4) // ���������� �����������
#define PCF8814_ALL_POINTS_ON       (0xA5) // �������� ��� �������

#define PCF8814_DISPLAY_NORMAL      (0xA6)
#define PCF8814_DISPLAY_REVERSE     (0xA7)

#define PCF8814_DATA_ORDER_OFF      (0xA8)
#define PCF8814_DATA_ORDER_ON       (0xA9)

#define PCF8814_RAM_ADDR_MODE_MSB   (0xAA)
#define PCF8814_RAM_ADDR_MODE_LSB   (0xAB)

#define PCF8814_PARTIAL_POSITION    (0xAC) // And second byte - 0...7

#define PCF8814_NLINE_INVERSION     (0xAD)  // And second byte - Value

#define PCF8814_DISPLAY_OFF         (0xAE) // Display OFF ��� �������� �����������������
#define PCF8814_DISPLAY_ON          (0xAF)

#define PCF8814_VERTICAL_MIRROR_OFF (0xC0)
#define PCF8814_VERTICAL_MIRROR_ON  (0xC8)

#define PCF8814_SET_PARTIAL_DISPLAY (0xD0) // 0...7
#define PCF8814_READ_ID             (0xDB)

#define PCF8814_ROW_MIRROR_OFF      (0xE0)
#define PCF8814_ROW_MIRROR_ON       (0xE1)
#define PCF8814_SOFTWARE_RESET      (0xE2)
#define PCF8814_NOP                 (0xE3)
#define PCF8814_REFRESH_RATE        (0xEF) // see ste2007

#define PCF8814_RESISTOR_RATIO      (0x20) // 0...6

#define PCF8814_POWER_CTRL_SET_OFF  (0x28)
#define PCF8814_POWER_CTRL_SET_ON   (0x2F) // ??? 0x2C �� �����
#define PCF8814_POWER_CTRL_SET      (0x2C)
#define PCF8814_CHARGE_FACTOR       (0x3D)
#define PCF8814_ELECTRONIC_VOLUME   (0x80) // | 0...1F (1F - low, 0 - high)

#define PCF8814_START_LINE_ADDR    (0x40) // � ������ ������ �������� 0...3F

// 0 <= Y <= 8
#define PCF8814_SETYADDR           (0xB0) // ��������� ������ ������ - ��������� 4 ���� 0...8

// 0 <= X <= 131
#define PCF8814_SETXADDR_LOWER     (0x00) // Set A3...A0 (Low Tetrade)
#define PCF8814_SETXADDR_UPPER     (0x10) // Set A7...A4 (High Tetrade)

// Bias Defines
#define PCF8814_BIAS_100  (0x00) // 1:100
#define PCF8814_BIAS_80   (0x01) // 1:80
#define PCF8814_BIAS_65   (0x02)
#define PCF8814_BIAS_48   (0x03)
#define PCF8814_BIAS_40   (0x04)
#define PCF8814_BIAS_24   (0x05)
#define PCF8814_BIAS_18   (0x06)
#define PCF8814_BIAS_10   (0x07)

// �������� ���������� (volatile) ���������� LCD
//volatile unsigned char lcd8814_x = 0; // ������� X, ������� ����������� ��� ������ ������
//volatile unsigned char lcd8814_y = 0;
//volatile unsigned char lcd8814_contrast = 0; // �������� �������������
//volatile unsigned char lcd8814_inverse = 0; // �������� ������ (0 - ���������)

// 16 �������� � ������, ����� 8 ����� ������

char CON8814 [PCF8814_COL*PCF8814_ROW+1]; // CONsole buffer ��� ������ �������� sprintf (128 ��������)

// ����������� ������ ������ ������ � �������
#define LN1 CON8814
#define LN2 CON8814+PCF8814_COL
#define LN3 CON8814+(PCF8814_COL*2)
#define LN4 CON8814+(PCF8814_COL*3)
#define LN5 CON8814+(PCF8814_COL*4)
#define LN6 CON8814+(PCF8814_COL*5)
#define LN7 CON8814+(PCF8814_COL*6)
#define LN8 CON8814+(PCF8814_COL*7)

// function prototypes

void pcf8814_write (unsigned char ch); // ������ ����� � ���
void pcf8814_cmd(unsigned char cmd); // �������� ������� � ���
void pcf8814_data(unsigned char dat); // �������� ������ � ���
void pcf8814_set_addr(unsigned char x, unsigned char y); // ��������� ������ � ���
void pcf8814_gotoxy(unsigned char x, unsigned char y); // ��������� ������� ������ �� ����������� x,y
void pcf8814_putchar(unsigned char ch); // ����� ������� �� ���
void pcf8814_cls(void); // ������� ������ - ���������� ��� ������, ��������� ������� ������ � ����
void pcf8814_puts (unsigned char *s); // ����� ������ (������ � ���)
void pcf8814_set_contrast(unsigned char c); // ������ �������� ���������
void pcf8814_set_inverse(unsigned char inv); // ������ �������� ��������
void pcf8814_set_power(unsigned char power); // ��������/��������� �������
void pcf8814_init (void); // ������������� ����������� AVR
void pcf8814_update_console(void); // ������� �� ������ ������� CON �� ���
void pcf8814_clear_console (unsigned char byte); // ���������� ������ ������� CON ��������� ������

