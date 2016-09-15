// --- LCD ��������� ��� ������������� Z-��������� ���� �����
// --- �.�. �������� �������� ������ �������. ������ � ����
// --- LCD ���������� ���� � ����� ������� ���������� �����������
// --- ����� (�� ����� - 0, �� ���� - 1).
// --- ������ ����������� ����� ������ �� ����� � ����������
// --- LCD �������������� ���������� ����� �����.
// --- ����������� ��� LCD Nokia 1200 c ����������� ������� 3,3 �


// �� ����� ����� ����� ������� LCD

#define PIN_CSN     PORTB_Bit4 // Chip Select Bit
#define DDR_CSN     DDRB_Bit4
#define PIN_SCL     PORTB_Bit6 // Serial Clock
#define DDR_SCL     DDRB_Bit6
#define PIN_SI      PORTB_Bit7 // Serial Data Input
#define DDR_SI      DDRB_Bit7
#define PIN_RESN    PORTB_Bit5 // /Reset LCD Driver
#define DDR_RESN    DDRB_Bit5

#define CSN_1  PIN_CSN=1
#define CSN_0  PIN_CSN=0
#define SCL_1  PIN_SCL=1
#define SCL_0  PIN_SCL=0
#define SI_1   PIN_SI=1
#define SI_0   PIN_SI=0
#define RESN_1 PIN_RESN=1
#define RESN_0 PIN_RESN=0

// ������� ������ (���������� ������������ 132x65)
#define SED15B1_PIXELX  (96) // � ����������� ���� �������� 132
#define SED15B1_PIXELY  (65)
#define SED15B1_ROW     (8) // ���������� �����
#define SED15B1_COL     (16) // ���������� �������� � ������

// ����� ������ X ��-�� �������������� ��������� � ������
// �.�. ���������� ����� ����������� �� �=132 � ��� ��� ����� 96 ��������, ��
// ����� ��������� �� ������ ��������� �����������, ����� ��� ������� �� 6 ���� � ������
// ��� ������� �� 6 ���� �� ����������, ������� ��������� ����� ����� ������
// ������� �� ��� ������� �.�. � ����� � ��� ����, ����� �������
// 18 in HEX 0x12   36 in HEX 0x24
#define SED15B1_ADD_XHI   0x00
#define SED15B1_ADD_XLO   0x00

// ������� ���
#define SED15B1_DISPLAY_ON          (0xAF)
#define SED15B1_DISPLAY_OFF         (0xAE) // Display OFF ��� �������� �����������������
#define SED15B1_DISPLAY_NORMAL      (0xA6)
#define SED15B1_DISPLAY_REVERSE     (0xA7)
#define SED15B1_SEGMENT_DIR_NORMAL  (0xA0)
#define SED15B1_SEGMENT_DIR_REVERSE (0xA1)
#define SED15B1_COMMON_DIR_NORMAL   (0xC0)
#define SED15B1_COMMON_DIR_REVERSE  (0xC8)
#define SED15B1_SET_BIAS_19         (0xA2) // Set BIAS 1/9
#define SED15B1_SET_BIAS_17         (0xA3) // Set BIAS 1/7
#define SED15B1_RESISTOR_RATIO      (0x20) // 0...6
#define SED15B1_ELECTRONIC_VOLUME   (0x80) // | 0...1F (1F - low, 0 - high)
#define SED15B1_POWER_CTRL_SET      (0x28) // | 0x04 booster | 0x02 voltage regulator | 0x01 booster ON/OFF
#define SED15B1_ALL_POINTS_ON       (0xA5) // �������� ��� �������
#define SED15B1_ALL_POINTS_OFF      (0xA4) // ���������� �����������

#define SED15B1_START_LINE_ADDR    (0x40) // � ������ ������ �������� 0...3F

// 0 <= Y <= 8
#define SED15B1_SETYADDR           (0xB0) // ��������� ������ ������ - ��������� 4 ���� 0...8

// 0 <= X <= 131
#define SED15B1_SETXADDR_LOWER     (0x00) // Set A3...A0 (Low Tetrade)
#define SED15B1_SETXADDR_UPPER     (0x10) // Set A7...A4 (High Tetrade)

// �������� ���������� (volatile) ���������� LCD
//volatile unsigned char sed_x = 0; // ������� X, ������� ����������� ��� ������ ������
//volatile unsigned char sed_y = 0;
//volatile unsigned char sed_contrast = 0; // �������� �������������
//volatile unsigned char sed_inverse = 0; // �������� ������ (0 - ���������)

// 16 �������� � ������, ����� 8 ����� ������

char CON15B1 [(SED15B1_COL*SED15B1_ROW)+1]; // CONsole buffer ��� ������ �������� sprintf (128 ��������)

// ����������� ������ ������ ������ � �������
#define LIN1 CON15B1
#define LIN2 CON15B1+SED15B1_COL
#define LIN3 CON15B1+(SED15B1_COL*2)
#define LIN4 CON15B1+(SED15B1_COL*3)
#define LIN5 CON15B1+(SED15B1_COL*4)
#define LIN6 CON15B1+(SED15B1_COL*5)
#define LIN7 CON15B1+(SED15B1_COL*6)
#define LIN8 CON15B1+(SED15B1_COL*7)

// function prototypes

void sed15b1_write (unsigned char ch); // ������ ����� � ���
void sed15b1_cmd(unsigned char cmd); // �������� ������� � ���
void sed15b1_data(unsigned char dat); // �������� ������ � ���
//void pcf8812_set_addr(unsigned char x, unsigned char y); // ��������� ������ � ���
//void pcf8812_gotoxy(unsigned char x, unsigned char y); // ��������� ������� ������ �� ����������� x,y
//void pcf8812_putchar(unsigned char ch); // ����� ������� �� ���
//void pcf8812_cls(void); // ������� ������ - ���������� ��� ������, ��������� ������� ������ � ����
//void pcf8812_puts (unsigned char *s); // ����� ������ (������ � ���)
//void pcf8812_set_contrast(unsigned char c); // ������ �������� ���������
//void pcf8812_set_inverse(unsigned char inv); // ������ �������� ��������
//void pcf8812_set_power(unsigned char power); // ��������/��������� �������
//void pcf8812_init (void); // ������������� ����������� AVR
//void pcf8812_update_console(void); // ������� �� ������ ������� CON �� ���
void sed15b1_clear_console (unsigned char byte); // ���������� ������ ������� CON ��������� ������

