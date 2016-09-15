// --- ����������� ��� LCD Nokia 6100/6610 c ����������� ������� 3,3 �

// Font sizes
#define SMALL  0
#define MEDIUM 1
#define LARGE  2

// �� ����� ����� ����� ������� LCD
#define PIN_CLK     PORTB_Bit5 // Serial Clock
#define DDR_CLK     DDRB_Bit5
#define PIN_DAT     PORTB_Bit6 // Serial Data Input
#define DDR_DAT     DDRB_Bit6
#define PIN_RESX    PORTB_Bit7 // /Reset LCD Driver
#define DDR_RESX    DDRB_Bit7

#define CLK_1  PIN_CLK=1
#define CLK_0  PIN_CLK=0
#define DAT_1  PIN_DAT=1
#define DAT_0  PIN_DAT=0
#define RESX_1 PIN_RESX=1
#define RESX_0 PIN_RESX=0

// ������� ������
#define PCF8833_PIXELX  (132)
#define PCF8833_PIXELY  (132)
#define PCF8833_ROW     (16) // ���������� �����
#define PCF8833_COL     (22) // ���������� �������� � ������

//-------------------------------------------------
// Commandset
// Philips PCF8833 LCD controller commandset

#define P_NOP       0x00 // nop 
#define P_SWRESET   0x01 // software reset 
#define P_BSTROFF   0x02 // booster voltage OFF 
#define P_BSTRON    0x03 // booster voltage ON 
#define P_RDDIDIF   0x04 // read display identification 
#define P_RDDST     0x09 // read display status 
#define P_SLEEPIN   0x10 // sleep in 
#define P_SLEEPOUT  0x11 // sleep out 
#define P_PTLON     0x12 // partial display mode 
#define P_NORON     0x13 // display normal mode 
#define P_INVOFF    0x20 // inversion OFF 
#define P_INVON     0x21 // inversion ON 
#define P_DALO      0x22 // all pixel OFF 
#define P_DAL       0x23 // all pixel ON 
#define P_SETCON    0x25 // write contrast 
#define P_DISPOFF   0x28 // display OFF 
#define P_DISPON    0x29 // display ON 
#define P_CASET     0x2A // column address set 
#define P_PASET     0x2B // page address set 
#define P_RAMWR     0x2C // memory write 
#define P_RGBSET    0x2D // colour set 
#define P_PTLAR     0x30 // partial area 
#define P_VSCRDEF   0x33 // vertical scrolling definition 
#define P_TEOFF     0x34 // test mode 
#define P_TEON      0x35 // test mode 
#define P_MADCTL    0x36 // memory access control 
#define P_SEP       0x37 // vertical scrolling start address 
#define P_IDMOFF    0x38 // idle mode OFF 
#define P_IDMON     0x39 // idle mode ON 
#define P_COLMOD    0x3A // interface pixel format 
#define P_SETVOP    0xB0 // set Vop 
#define P_BRS       0xB4 // bottom row swap 
#define P_TRS       0xB6 // top row swap 
#define P_DISCTR    0xB9 // display control 
#define P_DOR       0xBA // data order 
#define P_TCDFE     0xBD // enable/disable DF temperature compensation 
#define P_TCVOPE    0xBF // enable/disable Vop temp comp 
#define P_EC        0xC0 // internal or external oscillator 
#define P_SETMUL    0xC2 // set multiplication factor 
#define P_TCVOPAB   0xC3 // set TCVOP slopes A and B 
#define P_TCVOPCD   0xC4 // set TCVOP slopes c and d 
#define P_TCDF      0xC5 // set divider frequency 
#define P_DF8COLOR  0xC6 // set divider frequency 8-color mode 
#define P_SETBS     0xC7 // set bias system 
#define P_RDTEMP    0xC8 // temperature read back 
#define P_NLI       0xC9 // n-line inversion 
#define P_RDID1     0xDA // read ID1 
#define P_RDID2     0xDB // read ID2 
#define P_RDID3     0xDC // read ID3 

//=====================================================
// 12-bit color definitions 
#define WHITE   0xFFF //0x00
#define BLACK   0x000 
#define RED     0xF00 
#define GREEN   0x0F0 
#define BLUE    0x00F 
#define CYAN    0x0FF 
#define MAGENTA 0xF0F 
#define YELLOW  0xFF0 
#define BROWN   0xB22 
#define ORANGE  0xFA0 
#define PINK    0xF6A  //0x0A

