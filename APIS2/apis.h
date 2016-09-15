// APIS Washer board stupid header by (R)soft 2015
// APIS Errors:
// Error1 - ������ ����������� ������� (�������� ��� 0x000)
// Error2 - ������ ����������� ��������� (������� ��� 0x3FF)
// Error3 - ������ ������ ����������� ��� � ������ (LEVEL1=LEVEL2=LEVEL3=1)
// Error4 - ������������ ������� ��������� ��� ���� ���������.
// Error9 - ��������� � ����������� ������������ ��� ��������/���������� ���� I2C

//---------------------------------------------
// ������ ������ (��������):
// ������������ ��������� - 12, 14, 11, 16
// ������� 11 COM - �����.
// ������� 12 LEVEL1 - ��� ������.
// ������� 14 LEVEL2 - ��� �������� (������� ������������ ������ �� ������� ����� ��� ��������).
//          ���� ������� ������������� � LEVEL1 �� LEVEL2 � ����������� �������.
// ������� 16 LEVEL3 - ��� ���������� (������� ������������ ������ �� ������� (�������)).
//          ���� ������� ����������, ��������� � �����, � ��� ������.
//          ����� ��� ����������, �� ������ ���� LEVEL1=1, LEVEL2=0, LEVEL3=0


#define SECRET_CODE 0xFA    // ��������� ��� ��� ��������� ���������� ������

// 0x4B - LINE OFF
// 0x10 - LINE ON (��������� �������, ������� � ������ ������;)
#define LINE_ON_TRESHOLD  (0x40) // ����� ������������ ��� ���������� ����

#define APIS_QUARTZ       (8000000L)
#define I2C_TIMER_CONST   (100)       // 50 ms

//---------------------------------------------------------------
// �������� ����������� ������

//#define XTAL2    PORTB_Bit7
//#define XTAL1    PORTB_Bit6
#define SCK       PORTB_Bit5
#define MISO      PORTB_Bit4
#define MOSI      PORTB_Bit3
#define ROTOR     PORTB_Bit2  // ���������� ���������� ������
#define REVERSE   PORTB_Bit1  // ���������� �������� (���� 2)
#define FORWARD   PORTB_Bit0  // ���������� �������� (���� 1)

#define LOCK      PORTC_Bit0  // ���������� ������ ����
#define BUZZER    PORTC_Bit1
#define RESERVED  PORTC_Bit2
// PC3 for input (LEVEL3)
// PC4 - SDA I2C
// PC5 - SCL I2C
// PC6 - ADC6 (LINE)
// PC7 - ADC7 (Termo)

#define WATER2     PORTD_Bit7   // ���������� �������� ������ ���� (�������� ������)
#define WATER1     PORTD_Bit6   // ���������� �������� ������ ���� (��������������� ������)
#define POMP       PORTD_Bit5   // ���������� ������
#define HEATING    PORTD_Bit4   // ������
// PD3(INT1) for input - ZeroCrossDetect
// PD2 (INT0) for input - IR
// PD1 (LEVEL2) for input
// PD0 (LEVEL1) for input


//------------------------------------
// ����� (������ ������)

#define LEVEL1  PIND_Bit0
#define LEVEL2  PIND_Bit1
#define LEVEL3  PINC_Bit3

//===================================================================
void InitPorts (void)
{
  //----------------- Port B
  DDRB=0xFF;
  PORTB=0x04; // PB2 ������ ���� ���������� 1 (ROTOR TRIAC)
  
  //----------------- Port C
  DDRC=0xF7;  // PC3 (LEVEL3) for input
  PORTC=0x09; // pullup for PC3, PC0 ������ ���� ���������� 1 (LOCK TRIAC)
  
  //----------------- Port D
  // PD0 (LEVEL1) for input
  // PD1 (LEVEL2) for input
  // PD2 (INT0) for input - IR
  // PD3(INT1) for input - ZeroCrossDetect
  DDRD=0xF0;  // LEVEL1 & LEVEL2 & IR & ZCD for input
  PORTD=0xEF; // IR & ZCD input pullup, PD5 & PD6 & PD7 ������ ���� ���������� 1 (POMP & WATER1 & WATER2 TRIACs)
}

// ���������� ����������
unsigned char Timer_I2C;    // ������ ������ I2C
unsigned char Timer_Buzzer; // ������ �����
unsigned char SecretCode;   // �������� ���������� ���� ��� ���������� �������� �������
unsigned char SecretMode; // ��� ������� ��������� ������, ��������������� ��������� �����, ����������� ������ ��������� ���

//-------- �������� �������� ��������� ����������������� ������� ------------------
unsigned char HeatingState = 0; // Heating Relay state
unsigned char RotorState = 0;   // Rotor triac 0..0xFF state
unsigned char ForwardState = 0; // Forward Relay State
unsigned char ReverseState = 0; // Reverse Relay State
unsigned char PompState = 0;    // Pomp triac state
unsigned char WaterState1 = 0;  // Water1 triac state (��������������� ������)
unsigned char WaterState2 = 0;  // Water2 triac state (�������� ������)
unsigned char LockState = 0;    // Lock triac state

//------- ��������������� ������ -----------------------------------
unsigned char ClockMode = 0;  // ����� ��������� ����� ������/����
unsigned char TempMode = 0;   // ����� ��������� ����������� �������/���
unsigned char SpinMode = 0;   // ����� ��������� ��������������� ��������

unsigned int adcLine;                   // ��� ������� ���� (LINE) ����� ��������� ����� ����������
unsigned int adcTemperature;            // �������� ��� �������� �����������
unsigned char temperature;              // �������� �������� ����������� � ��������
volatile unsigned char SetTemperature;  // �������� ������������� ����������� ��� ������� ����
volatile unsigned char SetSpin;         // �������� ������������� ������� �������� ��� ������
volatile unsigned char SpinRpm;         // �������� ������� �������� ��� PWM (����������� �� �������)

unsigned char PWMCount;                 // ������� ��� PWM ������ ���������� �������
unsigned int SpinCounter;               // ������������ ��� �������� ������� � ���������� �������
unsigned short BeepCounter;             // ������������ ��� �������� �������

//------ ���������� Timer'�
volatile unsigned char msec;
volatile unsigned char sec;     // ������� ������
volatile unsigned char minute;  // ������� �����
volatile unsigned char hour;    // ������� �����
volatile unsigned char timer;   // ��������� ������ ������� ������ (max 255 �)

volatile unsigned char ProgramNumber;   // ����� ����������� ��������� (0 - ��������� �� �����������)
volatile unsigned char ProgramCounter;  // ������� ������
volatile unsigned char SubState;        // �������� ������������ �����
volatile unsigned char __flash *zprg;   // ��������� �� ������� �������� ������
volatile unsigned char cmd;             // ������� ����������� ������� ��������� ������
volatile unsigned char t;               // ����� ���������� �������
volatile unsigned char p;               // �������� �������
volatile unsigned char __flash *retp;   // Return Pointer ��� ����� Do While
unsigned char cycles;          // ���������� ������ ���������� Do While
unsigned char cnt;             // ������� ������ Do While


//===================================================
//--------- Slave memory I/O registers (4+3+4+9+3 = 23)
// ������� ������, ������� ������������ �� �������
// 20 ����
typedef union
{
  unsigned char Buf[20];
  struct
  {
    unsigned char Seg1; // Bit0 - SEGA ... Bit6 - SEGG
    unsigned char Seg2;
    unsigned char Seg3;
    unsigned char Seg4;

    unsigned char Red1;  // RED LED 1: zero - off, 0xFF - on
    unsigned char Red2;  // RED LED 2
    unsigned char Red3;  // RED LED 3

    unsigned char Ris1;  // zero - off, 0xFF - on
    unsigned char Ris2;
    unsigned char Ris3;
    unsigned char Ris4;

    unsigned char Modes;  // 0, 0xFF
    unsigned char Spin;   // 0, 0x80, 0xC0, 0xE0, 0xFF
    unsigned char Termo;  // 0, 0x80, 0xC0, 0xE0, 0xFF
    unsigned char Timer;  // 0, 0x80, 0xC0, 0xE0, 0xFF
    unsigned char Lock;   // 0, 0x80, 0xC0, 0xE0, 0xFF
    unsigned char Key;    // 0, 0x80, 0xC0, 0xE0, 0xFF
    unsigned char Points; // 0, 0x80, 0xC0, 0xE0, 0xFF
    unsigned char Buzzer; // 0, 0xFF
    unsigned char Gradus; // 0, 0xFF
   };
} regspackage;

regspackage Reg;

// ������� ������, ������� �������� ������� ��� ������ ������ � ��������� ������ ��������
// 3 �����
volatile unsigned char Reg_Prog;  // ������� ������ ��������� ��������� � �����. � ���
volatile unsigned char Reg_But1;  // Buttons Register 1
volatile unsigned char Reg_But2;  // Buttons Register 2

unsigned char MaskProg = 0xFF;    // ���� ����� ��� ��������� �������. 0xFF - ��������� ��������; 0 - �� �������� 
unsigned char BufRead[3];         // ��������� ����� ��� ������ I2C

//----------------------------
// Indication Symbols

#define DIG_0 0xC0
#define DIG_1 0xF9
#define DIG_2 0xA4
#define DIG_3 0xB0
#define DIG_4 0x99
#define DIG_5 0x92
#define DIG_6 0x82
#define DIG_7 0xF8
#define DIG_8 0x80
#define DIG_9 0x90
#define DIG_A 0x88
#define DIG_B 0x83
#define DIG_C 0xC6
#define DIG_D 0xA1
#define DIG_E 0x86
#define DIG_F 0x8E
#define DIG_P 0x8C
#define DIG_I 0xCF
#define DIG_S 0x92
#define DIG_U 0xC1
#define DIG_L 0xC7
#define DIG_H 0x89
#define DIG_SPACE  0xFF
#define DIG_c 0xA7
#define DIG_d 0xA1
#define DIG_n 0xAB
#define DIG_o 0xA3
#define DIG_r 0xAF
#define DIG_t 0x87
#define DIG__ 0xF7
#define DIG_MINUS 0xBF

// ������� ��c��� �������� ��������
static __flash char SPIN[9]=
{
  // 0  100   200   300   400   500   600   700   800 prm
     0, 0x36, 0x39, 0x3F, 0x48, 0x51, 0x60, 0x6F, 0x7E
};

static __flash char HEX[16]=
{
  DIG_0, DIG_1, DIG_2, DIG_3, DIG_4, DIG_5, DIG_6, DIG_7, DIG_8, DIG_9,
  DIG_A, DIG_B, DIG_C, DIG_D, DIG_E, DIG_F
};

static __flash unsigned int DEC[256]=
{
  // 00 - 99
  DIG_0<<8|DIG_0, DIG_0<<8|DIG_1, DIG_0<<8|DIG_2, DIG_0<<8|DIG_3, DIG_0<<8|DIG_4, DIG_0<<8|DIG_5,
  DIG_0<<8|DIG_6, DIG_0<<8|DIG_7, DIG_0<<8|DIG_8, DIG_0<<8|DIG_9, DIG_1<<8|DIG_0, DIG_1<<8|DIG_1,
  DIG_1<<8|DIG_2, DIG_1<<8|DIG_3, DIG_1<<8|DIG_4, DIG_1<<8|DIG_5, DIG_1<<8|DIG_6, DIG_1<<8|DIG_7, 
  DIG_1<<8|DIG_8, DIG_1<<8|DIG_9, DIG_2<<8|DIG_0, DIG_2<<8|DIG_1, DIG_2<<8|DIG_2, DIG_2<<8|DIG_3, 
  DIG_2<<8|DIG_4, DIG_2<<8|DIG_5, DIG_2<<8|DIG_6, DIG_2<<8|DIG_7, DIG_2<<8|DIG_8, DIG_2<<8|DIG_9, 
  DIG_3<<8|DIG_0, DIG_3<<8|DIG_1, DIG_3<<8|DIG_2, DIG_3<<8|DIG_3, DIG_3<<8|DIG_4, DIG_3<<8|DIG_5, 
  DIG_3<<8|DIG_6, DIG_3<<8|DIG_7, DIG_3<<8|DIG_8, DIG_3<<8|DIG_9, DIG_4<<8|DIG_0, DIG_4<<8|DIG_1, 
  DIG_4<<8|DIG_2, DIG_4<<8|DIG_3, DIG_4<<8|DIG_4, DIG_4<<8|DIG_5, DIG_4<<8|DIG_6, DIG_4<<8|DIG_7, 
  DIG_4<<8|DIG_8, DIG_4<<8|DIG_9, DIG_5<<8|DIG_0, DIG_5<<8|DIG_1, DIG_5<<8|DIG_2, DIG_5<<8|DIG_3, 
  DIG_5<<8|DIG_4, DIG_5<<8|DIG_5, DIG_5<<8|DIG_6, DIG_5<<8|DIG_7, DIG_5<<8|DIG_8, DIG_5<<8|DIG_9, 
  DIG_6<<8|DIG_0, DIG_6<<8|DIG_1, DIG_6<<8|DIG_2, DIG_6<<8|DIG_3, DIG_6<<8|DIG_4, DIG_6<<8|DIG_5, 
  DIG_6<<8|DIG_6, DIG_6<<8|DIG_7, DIG_6<<8|DIG_8, DIG_6<<8|DIG_9, DIG_7<<8|DIG_0, DIG_7<<8|DIG_1, 
  DIG_7<<8|DIG_2, DIG_7<<8|DIG_3, DIG_7<<8|DIG_4, DIG_7<<8|DIG_5, DIG_7<<8|DIG_6, DIG_7<<8|DIG_7, 
  DIG_7<<8|DIG_8, DIG_7<<8|DIG_9, DIG_8<<8|DIG_0, DIG_8<<8|DIG_1, DIG_8<<8|DIG_2, DIG_8<<8|DIG_3, 
  DIG_8<<8|DIG_4, DIG_8<<8|DIG_5, DIG_8<<8|DIG_6, DIG_8<<8|DIG_7, DIG_8<<8|DIG_8, DIG_8<<8|DIG_9, 
  DIG_9<<8|DIG_0, DIG_9<<8|DIG_1, DIG_9<<8|DIG_2, DIG_9<<8|DIG_3, DIG_9<<8|DIG_4, DIG_9<<8|DIG_5, 
  DIG_9<<8|DIG_6, DIG_9<<8|DIG_7, DIG_9<<8|DIG_8, DIG_9<<8|DIG_9,
  // 100 - 199
  DIG_0<<8|DIG_0, DIG_0<<8|DIG_1, DIG_0<<8|DIG_2, DIG_0<<8|DIG_3, DIG_0<<8|DIG_4, DIG_0<<8|DIG_5,
  DIG_0<<8|DIG_6, DIG_0<<8|DIG_7, DIG_0<<8|DIG_8, DIG_0<<8|DIG_9, DIG_1<<8|DIG_0, DIG_1<<8|DIG_1,
  DIG_1<<8|DIG_2, DIG_1<<8|DIG_3, DIG_1<<8|DIG_4, DIG_1<<8|DIG_5, DIG_1<<8|DIG_6, DIG_1<<8|DIG_7, 
  DIG_1<<8|DIG_8, DIG_1<<8|DIG_9, DIG_2<<8|DIG_0, DIG_2<<8|DIG_1, DIG_2<<8|DIG_2, DIG_2<<8|DIG_3, 
  DIG_2<<8|DIG_4, DIG_2<<8|DIG_5, DIG_2<<8|DIG_6, DIG_2<<8|DIG_7, DIG_2<<8|DIG_8, DIG_2<<8|DIG_9, 
  DIG_3<<8|DIG_0, DIG_3<<8|DIG_1, DIG_3<<8|DIG_2, DIG_3<<8|DIG_3, DIG_3<<8|DIG_4, DIG_3<<8|DIG_5, 
  DIG_3<<8|DIG_6, DIG_3<<8|DIG_7, DIG_3<<8|DIG_8, DIG_3<<8|DIG_9, DIG_4<<8|DIG_0, DIG_4<<8|DIG_1, 
  DIG_4<<8|DIG_2, DIG_4<<8|DIG_3, DIG_4<<8|DIG_4, DIG_4<<8|DIG_5, DIG_4<<8|DIG_6, DIG_4<<8|DIG_7, 
  DIG_4<<8|DIG_8, DIG_4<<8|DIG_9, DIG_5<<8|DIG_0, DIG_5<<8|DIG_1, DIG_5<<8|DIG_2, DIG_5<<8|DIG_3, 
  DIG_5<<8|DIG_4, DIG_5<<8|DIG_5, DIG_5<<8|DIG_6, DIG_5<<8|DIG_7, DIG_5<<8|DIG_8, DIG_5<<8|DIG_9, 
  DIG_6<<8|DIG_0, DIG_6<<8|DIG_1, DIG_6<<8|DIG_2, DIG_6<<8|DIG_3, DIG_6<<8|DIG_4, DIG_6<<8|DIG_5, 
  DIG_6<<8|DIG_6, DIG_6<<8|DIG_7, DIG_6<<8|DIG_8, DIG_6<<8|DIG_9, DIG_7<<8|DIG_0, DIG_7<<8|DIG_1, 
  DIG_7<<8|DIG_2, DIG_7<<8|DIG_3, DIG_7<<8|DIG_4, DIG_7<<8|DIG_5, DIG_7<<8|DIG_6, DIG_7<<8|DIG_7, 
  DIG_7<<8|DIG_8, DIG_7<<8|DIG_9, DIG_8<<8|DIG_0, DIG_8<<8|DIG_1, DIG_8<<8|DIG_2, DIG_8<<8|DIG_3, 
  DIG_8<<8|DIG_4, DIG_8<<8|DIG_5, DIG_8<<8|DIG_6, DIG_8<<8|DIG_7, DIG_8<<8|DIG_8, DIG_8<<8|DIG_9, 
  DIG_9<<8|DIG_0, DIG_9<<8|DIG_1, DIG_9<<8|DIG_2, DIG_9<<8|DIG_3, DIG_9<<8|DIG_4, DIG_9<<8|DIG_5, 
  DIG_9<<8|DIG_6, DIG_9<<8|DIG_7, DIG_9<<8|DIG_8, DIG_9<<8|DIG_9,
  // 200 - 255
  DIG_0<<8|DIG_0, DIG_0<<8|DIG_1, DIG_0<<8|DIG_2, DIG_0<<8|DIG_3, DIG_0<<8|DIG_4, DIG_0<<8|DIG_5,
  DIG_0<<8|DIG_6, DIG_0<<8|DIG_7, DIG_0<<8|DIG_8, DIG_0<<8|DIG_9, DIG_1<<8|DIG_0, DIG_1<<8|DIG_1,
  DIG_1<<8|DIG_2, DIG_1<<8|DIG_3, DIG_1<<8|DIG_4, DIG_1<<8|DIG_5, DIG_1<<8|DIG_6, DIG_1<<8|DIG_7, 
  DIG_1<<8|DIG_8, DIG_1<<8|DIG_9, DIG_2<<8|DIG_0, DIG_2<<8|DIG_1, DIG_2<<8|DIG_2, DIG_2<<8|DIG_3, 
  DIG_2<<8|DIG_4, DIG_2<<8|DIG_5, DIG_2<<8|DIG_6, DIG_2<<8|DIG_7, DIG_2<<8|DIG_8, DIG_2<<8|DIG_9, 
  DIG_3<<8|DIG_0, DIG_3<<8|DIG_1, DIG_3<<8|DIG_2, DIG_3<<8|DIG_3, DIG_3<<8|DIG_4, DIG_3<<8|DIG_5, 
  DIG_3<<8|DIG_6, DIG_3<<8|DIG_7, DIG_3<<8|DIG_8, DIG_3<<8|DIG_9, DIG_4<<8|DIG_0, DIG_4<<8|DIG_1, 
  DIG_4<<8|DIG_2, DIG_4<<8|DIG_3, DIG_4<<8|DIG_4, DIG_4<<8|DIG_5, DIG_4<<8|DIG_6, DIG_4<<8|DIG_7, 
  DIG_4<<8|DIG_8, DIG_4<<8|DIG_9, DIG_5<<8|DIG_0, DIG_5<<8|DIG_1, DIG_5<<8|DIG_2, DIG_5<<8|DIG_3, 
  DIG_5<<8|DIG_4, DIG_5<<8|DIG_5

};

// ������� �������� ����������� 0...99 ��������
static __flash unsigned int TEMP[100]=
{
  (45), 46, 47, 48, 49, (50), 51, 53, 54, 56, (58), 59, 60, 62, 65,
  (68), 70, 72, 75, 78, (81), 84, 86, 90, 94, (98), 102, 105, 109, 112, 
  (117), 121, 125, 130, 135, (142), 146, 150, 155, 160, (164), 170, 176, 182, 188, 
  (196), 202, 207, 213, 220, (227), 233, 238, 244, 250, (258), 266, 273, 280, 288, 
  (296), 302, 312, 319, 326, (334), 342, 348, 355, 363, (372), 380, 388, 395, 403, 
  (411), 420, 428, 436, 443, (451), 461, 469, 475, 483, (492), 500, 508, 516, 522, 
  (531), 540, 548, 554, 560, (568), 580, 590, 602, 616
};



//---------------------------------------
// �������� �� �������
#define KEY_DELAY (-20)
// �������� �� ������� �������
#define KEY_REPDEL (100)
// �������� �� ����������
#define KEY_REPPER (78)

// ���� ������
#define kSuper  0x01
#define kEasy   0x02
#define kIron   0x03
#define kSpin   0x04
#define kTermo  0x05
#define kTimer  0x06
#define kLock   0x07
#define kOn     0x08
#define kStart  0x09

#define KEY_PRESSED_SHORT (1) // 1 ������������� 5 ��
#define KEY_PRESSED_LONG  (10)

volatile unsigned char Key = 0x00;  // ��� ������� �������, 0 - ��� �������
unsigned char OldKey = 0x00;        // ������ ��� �������, 0 - ��� ������ ��������
signed char KeyCounter = KEY_DELAY; // ������� ��� �������� ����������
volatile unsigned char KeyFlag = 0;
volatile unsigned char RepeatFlag;


//-------------------------------------------------------------------
// ��������� ���������� ����������� � ��������
// �� ����� - �������� ���
// �� ������ - ���������� � 0..99 �������� ������� ��� 0xFF - ��� ���������
//
unsigned char TemperatureConvert (unsigned int tempvalue)
{
  if (tempvalue <45 || tempvalue>616) return 0xFF;

  unsigned char i;
  for (i=0; i<100; i++)
  {
    if (tempvalue <= TEMP[i]) return i;
  }

  return i;
}



//-------------------------------------------------
// ����� �� ������� ���������� (� ��������)
void delay_timer (unsigned char time)
{
  timer=time; // timer �������������� � �����������
  while (timer);
}


//-----------------------------------------------------------------------------
// ���������/������������� ��������� (������� Start/Stop)
// � ��������� ���������� ����� ��������� � ��������� �� ������� ���������.
// ����� ������������ Start/Stop ��������� �� ����� ������, � �����
// �������� �������� ������. �� �������� ������������ �������� ���������
// �����������������. 

void ProgramStartStop (unsigned char PrgNum, unsigned char __flash* TableProg)
{
  Timer_Buzzer = KEY_PRESSED_LONG;  // ��������

  if (ProgramNumber) // ���� ��������� ��������, �� ������ STOP � ���������� PC
  {
    MaskProg = 0xFF; // ��������������� ���������, ���� ��� ���� ��������� (� ������ �������� door)
    ProgramNumber = 0;
//    StoreEEPROM(&EE_nProgram, ProgramNumber);

    // ����� ���������� ��� �������� ����������������� ����� ������� ������.
    // � ����� �������� ���� ����������� �.�. �� ��������� �� ��������� � delay_timer.
    // ��� ���� �������, �� ��������.
    HeatingState=0; HEATING=0;
    RotorState=0;
    ForwardState=0; FORWARD=0;
    ReverseState=0; REVERSE=0;
    PompState=0; POMP=1;        // � ���� ������ ���������� ����������� ���������
    WaterState1=0; WATER1=1;    // � ���� ������ ���������� ����������� ���������
    WaterState2=0; WATER2=1;    // � ���� ������ ���������� ����������� ���������
    LockState=0; LOCK=1;        // � ���� ������ ���������� ����������� ���������
    Reg.Seg1 = DIG_S;
    Reg.Seg2 = DIG_t;
    Reg.Seg3 = DIG_o;    
    Reg.Seg4 = DIG_P;
    Reg.Lock = 0; // �������� ��������� ������ ��������� (�������)
    i2cMasterSendNI(0x40, sizeof (Reg.Buf), Reg.Buf);  // �������� �������� ���������
    delay_timer(2); // ������� �� ��� ������� Stop
  }
  else  // ���� ��������� �� ��������, �� ������ Good � ������������� PC
  {
    // ���� ��������� � ������ ��������������, �� ���������� ��� ������ � �������� ���������
    if (SpinMode!=0)  { SpinMode = 0; Reg.Spin = 0; }
    if (TempMode!=0)  { TempMode = 0; Reg.Termo = 0; Reg.Gradus = 0; }

    SpinRpm = SPIN[SetSpin]; // ������������� ������� ��� �������� ������, ������������ �� �������
    ProgramNumber = PrgNum; // ����� ��������� (����� ����������, ����� ����� ��������� �������� ��� ���)
    zprg = TableProg; // ��������� �� ����������� ���������
    ProgramCounter = 0;
    SubState = 0;
    Reg.Seg1 = DIG_6;
    Reg.Seg2 = DIG_o;
    Reg.Seg3 = DIG_o;    
    Reg.Seg4 = DIG_d;
    Reg.Lock = 0xE0; // ���������� ��������� ������ ��������� (�������� �������)
    i2cMasterSendNI(0x40, sizeof (Reg.Buf), Reg.Buf);  // �������� �������� ���������
    delay_timer(2);                     // ������� �� 2 ������� "Good"
//    StoreEEPROM(&EE_nProgram, ProgramNumber);
  }  
}

//--------------------------------------------------------------
// ����� ������� �������� � �������������� ���������� ������
// ����� ��������� �� ��������.
void ManualMode (void)
{
      //------------ ������ 8 ��������� - ��������������� ������ ------------------
      if (Reg_Prog==8 && Key==kStart && !RepeatFlag)
      {
        SetTemperature = LoadEEPROM(&EE_tProgram8);  // ������ ��������� ����������� ������� �� EEPROM
        SetSpin = LoadEEPROM(&EE_rpmProgram8);      // ������ �� EEPROM ��� ��������
        ProgramStartStop(8, PROGRAM8);                  // ������ ��������� �8
      }

      //------------ ������ 9 ��������� - ������� ������ ---------------------
      if (Reg_Prog==9 && Key==kStart && !RepeatFlag)
      {
        SetTemperature = LoadEEPROM(&EE_tProgram9);
        SetSpin = LoadEEPROM(&EE_rpmProgram9);
        ProgramStartStop(9, PROGRAM9);
      }

      //------------ ������ 12 ��������� - ����������� ����������, ���� � ���������� ����� ------------
      if (Reg_Prog==12 && Key==kStart && !RepeatFlag)
      {
        SetSpin = LoadEEPROM(&EE_rpmProgram12);
        ProgramStartStop(12, PROGRAM12);
      }

      //------------ ������ 13 ��������� - ������ ����������, ���� � ���������� ����� -----------
      if (Reg_Prog==13 && Key==kStart && !RepeatFlag)
      {
        SetSpin = LoadEEPROM(&EE_rpmProgram13);
        ProgramStartStop(13, PROGRAM13);
      }
      
      //------------ ������ 14 ��������� - ����������� ����� -------------------
      if (Reg_Prog==14 && Key==kStart && !RepeatFlag)
      {
        SetSpin = LoadEEPROM(&EE_rpmProgram14);
        ProgramStartStop(14, PROGRAM14);
      }

      //------------ ������ 15 ��������� - ������ ����� ----------------------
      if (Reg_Prog==15 && Key==kStart && !RepeatFlag)
      {
        SetSpin = LoadEEPROM(&EE_rpmProgram15);
        ProgramStartStop(15, PROGRAM15);
      }

      //------------ ������ 16 ��������� - ���� ���� ----------------------
      if (Reg_Prog==16 && Key==kStart && !RepeatFlag)
      {
        ProgramStartStop(16, PROGRAM16);
      }

      // Timer mode minute/hour
      if (Reg_Prog==11 && Key==kTimer && !RepeatFlag)
      {
        ClockMode = ~ClockMode;
        Timer_Buzzer=KEY_PRESSED_SHORT;
      }

      // Timer Reset
      if (Reg_Prog==11 && Key==2 && !RepeatFlag)
      {
        hour = 0;
        minute = 0;
        sec = 0;
        Timer_Buzzer=KEY_PRESSED_SHORT;
      }
      //------------ ���������� ��������� ����� ��� ���������� ������ --------------------
      // � ������ 7 �������� ������ ����� - ��������� ���� �������,
      // ������ ����� �������� + � - ��������� ��������� ���.
      // ����� ����� ���� ����� �������� ������ ���������� ����� ���������� �������� �������.
      if (Reg_Prog==7 && Key==kLock && !RepeatFlag)
      {
        if (SecretMode==0)  SecretMode = 0xFF; 
        else                SecretMode = 0; 
        Timer_Buzzer = KEY_PRESSED_SHORT;
      }
      // SecretCode Increase
      if (Reg_Prog==7 && Key==1 && SecretMode==0xFF)
      {
        SecretCode++;
        Timer_Buzzer = KEY_PRESSED_SHORT;
      }
      // SecretCode Decrease
      if (Reg_Prog==7 && Key==3 && SecretMode==0xFF)
      {
        SecretCode--;
        Timer_Buzzer = KEY_PRESSED_SHORT;
      }
      
      //----------------------------------------------------------------------------------

      // ----------- Set Spin mode for program 8 -------------------
      // ������� ������ 8 ��������� ����� ��������, ����� ������ ��������.
      // ����� ������� �������� ����� ���� �������� � ����� �����
      // �������� + � - ���������� �������� �������� � ��������/���, ������� 100.
      // ������������� �������� ����������� � EEPROM.
      if (Reg_Prog==8 && Key==kSpin && !RepeatFlag)
      {
        if (SpinMode==0)
        { 
          SpinMode = 8;
          TempMode = 0; // ����� ������ ����� ����������� ���� ������ ��������� ��������
          Reg.Termo = 0;
          Reg.Spin = 0xE0; // �������� �������� ������ ��������
          SetSpin = LoadEEPROM(&EE_rpmProgram8); // �������� �������� �� EEPROM
        }
        else              
        {
          SpinMode = 0; 
          Reg.Spin = 0; 
        }
        Timer_Buzzer=KEY_PRESSED_SHORT;
      }
      // Spin Increase for program 8
      if (Reg_Prog==8 && Key==1 && SpinMode==8)
      {
        SetSpin = LoadEEPROM(&EE_rpmProgram8);
        if (SetSpin <= MAX_SPIN-1)
        {
          SetSpin++;
          StoreEEPROM(&EE_rpmProgram8, SetSpin);
          Timer_Buzzer=KEY_PRESSED_SHORT;
        }
      }
      // Spin Decrease for program 8
      if (Reg_Prog==8 && Key==3 && SpinMode==8)
      {
        SetSpin = LoadEEPROM(&EE_rpmProgram8);
        if (SetSpin >= MIN_SPIN+1)
        {
          SetSpin--;
          StoreEEPROM(&EE_rpmProgram8, SetSpin);
          Timer_Buzzer=KEY_PRESSED_SHORT;
        }
      }
      //----------------------------------------------------------------------------------

      // ----------- Set Spin mode for program 9 -------------------
      // ������� ������ 9 ��������� ����� ��������, ����� ������ ��������.
      // ����� ������� �������� ����� ���� �������� � ����� �����
      // �������� + � - ���������� �������� �������� � ��������/���, ������� 100.
      // ������������� �������� ����������� � EEPROM.
      if (Reg_Prog==9 && Key==kSpin && !RepeatFlag)
      {
        if (SpinMode==0)
        { 
          SpinMode = 9;
          TempMode = 0;
          Reg.Termo = 0;
          Reg.Spin = 0xE0; // �������� �������� ������ ��������
          SetSpin = LoadEEPROM(&EE_rpmProgram9); // �������� �������� �� EEPROM
        }
        else              
        {
          SpinMode = 0; 
          Reg.Spin = 0; 
        }
        Timer_Buzzer=KEY_PRESSED_SHORT;
      }
      // Spin Increase for program 9
      if (Reg_Prog==9 && Key==1 && SpinMode==9)
      {
        SetSpin = LoadEEPROM(&EE_rpmProgram9);
        if (SetSpin <= MAX_SPIN-1)
        {
          SetSpin++;
          StoreEEPROM(&EE_rpmProgram9, SetSpin);
          Timer_Buzzer=KEY_PRESSED_SHORT;
        }
      }
      // Spin Decrease for program 9
      if (Reg_Prog==9 && Key==3 && SpinMode==9)
      {
        SetSpin = LoadEEPROM(&EE_rpmProgram9);
        if (SetSpin >= MIN_SPIN+1)
        {
          SetSpin--;
          StoreEEPROM(&EE_rpmProgram9, SetSpin);
          Timer_Buzzer=KEY_PRESSED_SHORT;
        }
      }
      //----------------------------------------------------------------------------------

      // ----------- Set Spin mode for program 12 -------------------
      // ������� ������ 12 ��������� ����� ��������, ����� ������ ��������.
      // ����� ������� �������� ����� ���� �������� � ����� �����
      // �������� + � - ���������� �������� �������� � ��������/���, ������� 100.
      // ������������� �������� ����������� � EEPROM.
      if (Reg_Prog==12 && Key==kSpin && !RepeatFlag)
      {
        if (SpinMode==0)
        { 
          SpinMode = 12;
          Reg.Spin = 0xE0; // �������� �������� ������ ��������
          SetSpin = LoadEEPROM(&EE_rpmProgram12); // �������� �������� �� EEPROM
        }
        else              
        {
          SpinMode = 0; 
          Reg.Spin = 0; 
        }
        Timer_Buzzer=KEY_PRESSED_SHORT;
      }
      // Spin Increase for program 12
      if (Reg_Prog==12 && Key==1 && SpinMode==12)
      {
        SetSpin = LoadEEPROM(&EE_rpmProgram12);
        if (SetSpin <= MAX_SPIN-1)
        {
          SetSpin++;
          StoreEEPROM(&EE_rpmProgram12, SetSpin);
          Timer_Buzzer=KEY_PRESSED_SHORT;
        }
      }
      // Spin Decrease for program 12
      if (Reg_Prog==12 && Key==3 && SpinMode==12)
      {
        SetSpin = LoadEEPROM(&EE_rpmProgram12);
        if (SetSpin >= MIN_SPIN+1)
        {
          SetSpin--;
          StoreEEPROM(&EE_rpmProgram12, SetSpin);
          Timer_Buzzer=KEY_PRESSED_SHORT;
        }
      }
      //----------------------------------------------------------------------------------

      // ----------- Set Spin mode for program 13 -------------------
      // ������� ������ 13 ��������� ����� ��������, ����� ������ ��������.
      // ����� ������� �������� ����� ���� �������� � ����� �����
      // �������� + � - ���������� �������� �������� � ��������/���, ������� 100.
      // ������������� �������� ����������� � EEPROM.
      if (Reg_Prog==13 && Key==kSpin && !RepeatFlag)
      {
        if (SpinMode==0)
        { 
          SpinMode = 13;
          Reg.Spin = 0xE0; // �������� �������� ������ ��������
          SetSpin = LoadEEPROM(&EE_rpmProgram13); // �������� �������� �� EEPROM
        }
        else              
        {
          SpinMode = 0; 
          Reg.Spin = 0; 
        }
        Timer_Buzzer=KEY_PRESSED_SHORT;
      }
      // Spin Increase for program 13
      if (Reg_Prog==13 && Key==1 && SpinMode==13)
      {
        SetSpin = LoadEEPROM(&EE_rpmProgram13);
        if (SetSpin <= MAX_SPIN-1)
        {
          SetSpin++;
          StoreEEPROM(&EE_rpmProgram13, SetSpin);
          Timer_Buzzer=KEY_PRESSED_SHORT;
        }
      }
      // Spin Decrease for program 13
      if (Reg_Prog==13 && Key==3 && SpinMode==13)
      {
        SetSpin = LoadEEPROM(&EE_rpmProgram13);
        if (SetSpin >= MIN_SPIN+1)
        {
          SetSpin--;
          StoreEEPROM(&EE_rpmProgram13, SetSpin);
          Timer_Buzzer=KEY_PRESSED_SHORT;
        }
      }
      //----------------------------------------------------------------------------------

      // ----------- Set Spin mode for program 14 -------------------
      // ������� ������ 14 ��������� ����� ��������, ����� ������ ��������.
      // ����� ������� �������� ����� ���� �������� � ����� �����
      // �������� + � - ���������� �������� �������� � ��������/���, ������� 100.
      // ������������� �������� ����������� � EEPROM.
      if (Reg_Prog==14 && Key==kSpin && !RepeatFlag)
      {
        if (SpinMode==0)
        { 
          SpinMode = 14; 
          Reg.Spin = 0xE0; // �������� �������� ������ ��������
          SetSpin = LoadEEPROM(&EE_rpmProgram14); // �������� �������� �� EEPROM
        }
        else              
        {
          SpinMode = 0; 
          Reg.Spin = 0; 
        }
        Timer_Buzzer=KEY_PRESSED_SHORT;
      }
      // Spin Increase for program 14
      if (Reg_Prog==14 && Key==1 && SpinMode==14)
      {
        SetSpin = LoadEEPROM(&EE_rpmProgram14);
        if (SetSpin <= MAX_SPIN-1)
        {
          SetSpin++;
          StoreEEPROM(&EE_rpmProgram14, SetSpin);
          Timer_Buzzer=KEY_PRESSED_SHORT;
        }
      }
      // Spin Decrease for program 14
      if (Reg_Prog==14 && Key==3 && SpinMode==14)
      {
        SetSpin = LoadEEPROM(&EE_rpmProgram14);
        if (SetSpin >= MIN_SPIN+1)
        {
          SetSpin--;
          StoreEEPROM(&EE_rpmProgram14, SetSpin);
          Timer_Buzzer=KEY_PRESSED_SHORT;
        }
      }
      //----------------------------------------------------------------------------------
      
      // ----------- Set Spin mode for program 15 -------------------
      // ������� ������ 15 ��������� ����� ��������, ����� ������ ��������.
      // ����� ������� �������� ����� ���� �������� � ����� �����
      // �������� + � - ���������� �������� �������� � ��������/���, ������� 100.
      // ������������� �������� ����������� � EEPROM.
      if (Reg_Prog==15 && Key==kSpin && !RepeatFlag)
      {
        if (SpinMode==0)
        { 
          SpinMode = 15; 
          Reg.Spin = 0xE0; // �������� �������� ������ ��������
          SetSpin = LoadEEPROM(&EE_rpmProgram15); // �������� �������� �� EEPROM
        }
        else              
        {
          SpinMode = 0; 
          Reg.Spin = 0; 
        }
        Timer_Buzzer=KEY_PRESSED_SHORT;
      }
      // Spin Increase for program 15
      if (Reg_Prog==15 && Key==1 && SpinMode==15)
      {
        SetSpin = LoadEEPROM(&EE_rpmProgram15);
        if (SetSpin <= MAX_SPIN-1)
        {
          SetSpin++;
          StoreEEPROM(&EE_rpmProgram15, SetSpin);
          Timer_Buzzer=KEY_PRESSED_SHORT;
        }
      }
      // Spin Decrease for program 15
      if (Reg_Prog==15 && Key==3 && SpinMode==15)
      {
        SetSpin = LoadEEPROM(&EE_rpmProgram15);
        if (SetSpin >= MIN_SPIN+1)
        {
          SetSpin--;
          StoreEEPROM(&EE_rpmProgram15, SetSpin);
          Timer_Buzzer=KEY_PRESSED_SHORT;
        }
      }
      //----------------------------------------------------------------------------------

      // ----------- Set Temperature mode for program 8 -------------------
      // ����������� 8 ��������� ����� ��������, ����� ������ ����������.
      // ����� ������� �������� ������� ���� ���������� � ����� �����
      // �������� + � - ���������� �������� �������� � ��������, ������� 5.
      // ������������� ����������� ����������� � EEPROM.
      if (Reg_Prog==8 && Key==kTermo && !RepeatFlag)
      {
        if (TempMode==0)  
        { 
          TempMode = 8;
          SpinMode = 0;
          Reg.Spin = 0;
          Reg.Termo = 0xE0; 
          SetTemperature = LoadEEPROM(&EE_tProgram8); 
        }
        else              
        {
          TempMode = 0; 
          Reg.Termo = 0; 
        }
        Timer_Buzzer=KEY_PRESSED_SHORT;
      }
      // Temperature Increase for program 8
      if (Reg_Prog==8 && Key==1 && TempMode==8)
      {
        SetTemperature = LoadEEPROM(&EE_tProgram8);
        if (SetTemperature <= MAX_TEMPERATURE-5)
        {
          SetTemperature = SetTemperature + 5;
          StoreEEPROM(&EE_tProgram8, SetTemperature);
          Timer_Buzzer=KEY_PRESSED_SHORT;
        }
      }
      // Temperature Decrease for program 8
      if (Reg_Prog==8 && Key==3 && TempMode==8)
      {
        SetTemperature = LoadEEPROM(&EE_tProgram8);
        if (SetTemperature >= MIN_TEMPERATURE+5)
        {
          SetTemperature = SetTemperature - 5;
          StoreEEPROM(&EE_tProgram8, SetTemperature);
          Timer_Buzzer=KEY_PRESSED_SHORT;
        }
      }
      //----------------------------------------------------------------------------------
      
      // ------------- Temperature mode for program 9 -------------------------
      // ����������� 9 ��������� ����� ��������, ����� ������ ����������.
      // ����� ������� �������� ������� ���� ���������� � ����� �����
      // �������� + � - ���������� �������� �������� � ��������, ������� 5.
      // ������������� ����������� ����������� � EEPROM.
      if (Reg_Prog==9 && Key==kTermo && !RepeatFlag)
      {
        if (TempMode==0)  
        { 
          TempMode = 9;
          SpinMode =0;
          Reg.Spin = 0;
          Reg.Termo = 0xE0; 
          SetTemperature = LoadEEPROM(&EE_tProgram9);
        }
        else              
        {
          TempMode = 0; 
          Reg.Termo = 0; 
        }
        Timer_Buzzer=KEY_PRESSED_SHORT;
      }
      // Temperature Increase for program 9
      if (Reg_Prog==9 && Key==1 && TempMode==9)
      {
        SetTemperature = LoadEEPROM(&EE_tProgram9);
        if (SetTemperature <= MAX_TEMPERATURE-5)
        {
          SetTemperature = SetTemperature + 5;
          StoreEEPROM(&EE_tProgram9, SetTemperature);
          Timer_Buzzer=KEY_PRESSED_SHORT;
        }
      }
      // Temperature Decrease for program 9
      if (Reg_Prog==9 && Key==3 && TempMode==9)
      {
        SetTemperature = LoadEEPROM(&EE_tProgram9);
        if (SetTemperature >= MIN_TEMPERATURE+5)
        {
          SetTemperature = SetTemperature - 5;
          StoreEEPROM(&EE_tProgram9, SetTemperature);
          Timer_Buzzer=KEY_PRESSED_SHORT;
        }
      }
      //---------------------------------------------------------------------------------

      //-------------- ��������� ������� ���������� ------------------------------
      // ��� ������� �������� ����� ����� ���������� ����
      if (SecretCode==SECRET_CODE && SecretMode==0) // ���� ��������� ����� ������� � ��������� ��������� ���
      {
        //Temperature mode gradus/ADC
        if (Reg_Prog==10 && Key==kTermo && !RepeatFlag)
        {
          if (TempMode==0)  TempMode = 0xFF;
          else              TempMode = 0;
          Timer_Buzzer=KEY_PRESSED_SHORT;
        }

        // Lock ON/OFF and Line ON/OFF
        if (Reg_Prog==1 && Key==kStart && !RepeatFlag)
        {
          LockState = ~LockState;
          Timer_Buzzer=KEY_PRESSED_SHORT;
        }
      
        // Forward ON/OFF     
        if (Reg_Prog==2 && Key==kStart && !RepeatFlag)
        {
          // ���� ����� ��������� � ��� ����������� ������, �� ����������� ������
          if (!ReverseState)  { ForwardState = ~ForwardState; Timer_Buzzer=KEY_PRESSED_SHORT; }
        }

        // Reverse ON/OFF
        if (Reg_Prog==2 && Key==8 && !RepeatFlag)
        {
          if (!ForwardState)  { ReverseState = ~ReverseState; Timer_Buzzer=KEY_PRESSED_SHORT; }
        }

        // Speed Increase
        if (Reg_Prog==2 && Key==1)
        {
          if (RotorState<=0xFF-3) { RotorState = RotorState + 3; Timer_Buzzer=KEY_PRESSED_SHORT; }
        }

        // Speed Decrease
        if (Reg_Prog==2 && Key==3)
        {
          if (RotorState >= 3)  { RotorState = RotorState - 3; Timer_Buzzer=KEY_PRESSED_SHORT; }
        }

        // Speed Reset to Zero
        // middle button
        if (Reg_Prog==2 && Key==2)  
        {
          if (RotorState) { RotorState=0; Timer_Buzzer=KEY_PRESSED_SHORT; }// ���� �� ����� ����, �� ��������
        }

        // Speed Set to minimal rotation
        // SPIN button
        if (Reg_Prog==2 && Key==4)  
        {
          if (RotorState!=0x3C) { RotorState=0x3C; Timer_Buzzer=KEY_PRESSED_SHORT; }
        }
      
        // Speed Set to speed rotation 2
        // TERMO button
        if (Reg_Prog==2 && Key==5)  
        {
          if (RotorState!=0x50) { RotorState=0x50; Timer_Buzzer=KEY_PRESSED_SHORT; }
        }
      
        // Pomp ON/OFF
        if (Reg_Prog==4 && Key==kStart && !RepeatFlag)
        {
         PompState = ~PompState; 
         Timer_Buzzer=KEY_PRESSED_SHORT;
        }

        // Water ON/OFF
        if ( Reg_Prog==3 && Key==kStart && !RepeatFlag) 
        {
         WaterState1 = ~WaterState1;
         WaterState2 = WaterState1;
         Timer_Buzzer=KEY_PRESSED_SHORT;
        }

        // Heater ON/OFF
        if (Reg_Prog==5 && Key==kStart && !RepeatFlag)
        { // ���������� �������, ���� ��� ������. �������� ������ ����� �������2=0
          if ( LEVEL1==1 && LEVEL2==0 ) HeatingState = ~HeatingState;
          else HeatingState=0;  // ���� ������� ��������� �� �����������, �� �������� ������
          if (HeatingState)  Timer_Buzzer=50; // ������������� � ��������� ����������� ������� ������
        }
      }
    
}

//--------------------------------------------------------------
// === ��������� ����������� ��������� ������ ===
// ��� ������� ����� ��������� ������� ����� DO � WHILE.
// ��������: PRG_Do, 10,  // ���� 10 ���
//           PRG_Buzzer, 5, 
//           PRG_While,   // ��������� ���� - ��������� 10 ���

void WashingProcess (void)
{
  //------------- Program Counter ----------------------------
  // ���������� ��������� ������.
  // ���� ������ �� ����������, �� �� ������� � ���������� ������.

  if (ProgramNumber && timer==0)  // ���� ��������� �������� � ������ �������� �� ����� ��������� ����.
  {
    cmd = *zprg;  // ����� �������

    // ����� ����� ��������� ������, ���� � ����������� ������ �� ����� �������� ��������� �� ��������� �������
    switch (cmd)
    {

     // 2 bytes - ���� Do While (Do TWhile). ������ Do.
      // ��� ������� ���� ���������� ����� �������� � �����
      // �������������� � ��������� TWhile � While.
     // ����� �������� ����� �� ���� ��������, ����� ������ ��� �� ����� ���������
    case PRG_Do:
      retp = zprg + 2;  // ���������� ��������� �������� ����� Do While - ����� �� ���� ��������
      zprg++;           // ��������� �� ��������
      cycles = *zprg++; // ����� �������� ���������� ������
      ProgramCounter++; // ������� ������ ���������
      break;

    // 1 byte - ���� Do While. ������ While - ������� �� ����. ����� While �� �������.
    case PRG_While:
      cnt++;  // ����������� ������� ������
      if (cnt==cycles)  // ���� ������� ������ ���������� ���������� ������, �� ���� ������
      {
        cnt = 0; // �������� ������� ������ �� ������
        zprg++;  // ����������� ��������� ������ - ��������� ������������
      }
      else  // ����� ���������� ��������� ������ �� ������ ����� Do
      {
        zprg = retp;      // ����� �������� ����� Do While
      }
      break;

      // 2 bytes
    case PRG_Pause:
      zprg++; // ��������� ���������
      t = *zprg++;
      if (t!=0) timer = t;  // ���������, ���� �� ����, �� ������������� ������
      ProgramCounter++;
      break;

      // 2 bytes
    case PRG_LockState:
      zprg++;
      LockState = *zprg++;  // �������� �������� ���������
      ProgramCounter++; // ����������� ������� ������
      break;

      // 3 bytes (with pause)
      // ������ ������ ���� ��������������� ������
    case PRG_WaterState1:
      zprg++;
      WaterState1 = *zprg++;
      t = *zprg++;
      if (t!=0) timer = t;
      ProgramCounter++;
      break;

      // 3 bytes (with pause)
      // ������ ������ ���� �������� ������
    case PRG_WaterState2:
      zprg++;
      WaterState2 = *zprg++;
      t = *zprg++;
      if (t!=0) timer = t;
      ProgramCounter++;
      break;

      // 3 bytes (with pause)
    case PRG_PompState:
      zprg++;
      PompState = *zprg++;
      t = *zprg++;
      if (t!=0) timer = t;
      ProgramCounter++;
      break;

      // 3 bytes (with pause)
    case PRG_HeatingState:
      zprg++;
      HeatingState = *zprg++;
      t = *zprg++;
      if (t!=0) timer = t;
      ProgramCounter++;
      break;

      // 3 bytes (with pause)
    case PRG_RotorState:
      zprg++;
      RotorState = *zprg++;
      t = *zprg++;
      if (t!=0) timer = t;
      ProgramCounter++;
      break;

      // 3 bytes (with pause)
    case PRG_ForwardState:
      zprg++;
      ForwardState = *zprg++;
      t = *zprg++;
      if (t!=0) timer = t;
      ProgramCounter++;
      break;

      // 3 bytes (with pause)
    case PRG_ReverseState:
      zprg++;
      ReverseState = *zprg++;
      t = *zprg++;
      if (t!=0) timer = t;
      ProgramCounter++;
      break;

      // 2 bytes
      // ������ ���� - �������� t=(n*5 ms)
    case PRG_Buzzer:
      zprg++;
      Timer_Buzzer = *zprg++;
      ProgramCounter++;
      break;

      // 1 byte
      // ��������� ������������.
      // ������� ����������� � ���������� ������
    case PRG_MotorBrake:
      {
        switch (SubState)
        {
        case 0:
          SpinCounter = RotorState<<8; // ���������� ��������
          SubState++;
          break;
        case 1:
          SpinCounter--; // ������� ���������� ��������
          // ���� ������� ������ ����,
          if ((SpinCounter>>8) > 0)   RotorState = SpinCounter>>8; // �� ��������� �������
          else
          {
            ForwardState = 0x00; // ��������� ����� ������
            SubState=0;
            ProgramCounter++;
            zprg++; // ��������� �� ��������� �������
          }
        }
        break;
      }

      // 2 bytes
      // ��������� ������������.
      // ��������� ������ ������ � ������� ������ �� ��������, ��������� � ���������
    case PRG_MotorStart:
      {
        switch (SubState)
        {
        case 0:
          zprg++;
          p = *zprg;      // ����� �������� ��������
          zprg--;      // ������� ��������� �� �������, ����� ���������� ������������
          // ���� �������� <= 0x28 �� ����� ���������� ������� - ���� ������
          if (p <= 0x28) { zprg = zprg+2; break; }
          SpinCounter = 0; // ���������� �������� - ���������
          ForwardState = 0xFF; // �������� ����� ������
          SubState++;
          break;
        case 1:
          SpinCounter++;  // ������ ������� - ������ ����������� �������
          // ���� ������� ������ ������������� ��������
          if ((SpinCounter>>8) < p)   RotorState = SpinCounter>>8; // �� ����������� ������� ������
          else
          {
            RotorState = SpinCounter>>8;
            SubState=0;
            ProgramCounter++;          
            zprg = zprg + 2; // ��������� �� ��������� �������
          }
        }
        break;
      }

      // 3 bytes
      // ��������� ������������.
      // ��������� ������ ������,
      // ������� ������ ������ �� �������� n (������ ����)
      // ������ ���� - ����� ������ � �������� �� �������� ����������.
      // ���������� PRG_MotorSetSpin, ������ �� ����� ���������� ��������
    case PRG_MotorSpin:
      {
        switch (SubState)
        {
        case 0:
          zprg++;
          p = *zprg++;      // ����� �������� ��������
          t = *zprg;        // ����� �������� ������� (�������� 255 �)
          zprg = zprg - 2;      // ������� ��������� �� �������, ����� ���������� ������������
          // ���� �������� <= 0x28 �� ����� ���������� ������� - ���� ������
          if (p <= 0x28) { zprg = zprg+3; break; }
          SpinCounter = 0; // ���������� �������� - ���������
          ForwardState = 0xFF; // �������� ����� ������
          SubState++;
          break;
        case 1:
          SpinCounter++;  // ������ ������� - ������ ����������� �������
          // ���� ������� ������ ������������� ��������
          if ((SpinCounter>>8) < p)   RotorState = SpinCounter>>8; // �� ����������� ������� ������
          else                        
          {
            RotorState = SpinCounter>>8;
            SubState++; // ����� ���� ������
          }
          break;
        case 2:
          timer = t; // ����� ������ � ��������� t (�������� 255 �)
          SubState++;
          break;
        // �������� ����� ��� � �����������
        case 3:
          SpinCounter--; // ������� ���������� ��������
          // ���� ������� ������ ����,
          if ((SpinCounter>>8) > 0)   RotorState = SpinCounter>>8; // �� ��������� �������
          else                        SubState++; // ����� ���� ������
          break;
        case 4:
          ForwardState = 0x00; // ��������� ����� ������
          SubState=0;
          ProgramCounter++;          
          zprg = zprg + 3; // ��������� �� ��������� �������
        }
        break;
      }

      // 2 bytes
      // ��������� ������������.
      // ��������� ������ ������,
      // ������� ������ ������ �� ��������, ������������� � SpinRpm � ������� ����������.
      // ������ ���� - ����� ������ � �������� �� �������� ����������.
      // ������������ ��� ������ �����.
    case PRG_MotorSetSpin:
      {
        switch (SubState)
        {
        case 0:
          // ���� �������� <= 0x28 �� ����� ���������� ������� - ���� ������
          if (SpinRpm <= 0x28) { zprg = zprg+2; break; }
          SpinCounter = 0; // ���������� �������� - ���������
          ForwardState = 0xFF; // �������� ����� ������
          SubState++;
          break;
        case 1:
          SpinCounter++;  // ������ ������� - ������ ����������� �������
          // ���� ������� ������ ������������� ��������
          if ((SpinCounter>>8) < SpinRpm)   RotorState = SpinCounter>>8; // �� ����������� ������� ������
          else                              
          {
            RotorState = SpinCounter>>8;
            SubState++; // ����� ���� ������
          }
          break;
        case 2:
          zprg++;
          timer = *zprg; // ����� ������ (�������� 255 �)
          zprg--;      // ������� ��������� �� �������, ����� ���������� ������������
          SubState++;
          break;
        // �������� ����� ��� � �����������
        case 3:
          SpinCounter--; // ������� ���������� ��������
          // ���� ������� ������ ����,
          if ((SpinCounter>>8) > 0)   RotorState = SpinCounter>>8; // �� ��������� �������
          else                        SubState++; // ����� ���� ������
          break;
        case 4:
          ForwardState = 0x00; // ��������� ����� ������
          SubState=0;
          ProgramCounter++;          
          zprg = zprg + 2; // ��������� �� ��������� �������
        }
        break;
      }
      
      // 3 bytes
      // ��������� ������������ ��� ������ ������.
      // ZigZag - ����-����.
      // ������� ������ ������ ����� �� �������� � �������� 0x3C...0x41 (������ ����), �����,
      // ������� ����������, �����. ������ ����� � ���. ������ ������.
      // �.�. ������� � ���� �������, ����� � ������.
      // ������ ���� - ����� ������ � ����������� ����� �������� � �����������, � ����� �����.
      //  ____      ____
      // /    \____/    \____
      // ������������ ��� ������ "������� ������" � "����������� ������".
      // � ���������� ��������: 0x3C - ������� ������, 0x41 - ����������� ������.
    case PRG_MotorZigZag:
      {
        switch (SubState)
        {
        case 0: 
          ForwardState = 0xFF;
          SubState++;
          break;
        case 1:
          RotorState = 0x00;
          timer=1;
          SubState++;
          break;
        case 2:
          RotorState = 0x37;  // ����� �������
          timer=2;
          SubState++;
          break;
        case 3:
          zprg++;
          p = *zprg++;
          t = *zprg;
          zprg = zprg - 2;  // ������� ��������� �� �������, ����� ���������� ������������
          RotorState = p;
          timer=t;
          SubState++;
          break;
        case 4:
          RotorState = 0x37;
          timer=2;
          SubState++;
          break;
        case 5:
          RotorState = 0x00;
          timer=3;          // 3 ������� ����� �� ������� ��������
          SubState++;
          break;
        case 6:
          ForwardState = 0x00;  // ���������� ������
          timer=1;              // pause
          SubState++;
          break;
        case 7:
          ReverseState = 0xFF;  // �������� �����
          timer=1;
          SubState++;
          break;
        case 8:
          RotorState = 0x37;
          timer=2;
          SubState++;
          break;
        case 9:
          RotorState = p;
          timer=t;
          SubState++;
          break;
        case 10:
          RotorState = 0x37;
          timer=2;
          SubState++;
          break;
        case 11:
          RotorState = 0x00;
          timer=3;          // 3 ������� ����� �� ������� ��������
          SubState++;
          break;
        case 12:
          ReverseState = 0x00;  // ��������� �����
          timer=1;
          SubState=0;
          ProgramCounter++;
          zprg = zprg + 3; // ��������� �� ��������� �������
        }
        break;
      }
      
      // 1 byte
      // �������� ������������� ����������� � ��������� ����, ���� �����
      // (���������������� ��������� ����)
    case PRG_HeatingTest:
      {
        ProgramCounter++;
        zprg++;  // ��������� �� ��������� �������
        // ���� ������� t ������ ���������, �� �������� ��� � ���� ������
        if (temperature < SetTemperature)   HeatingState = 0xFF;
        break;
      }
      
      // 1 byte
      // ���� ������� ������� ���� �� �������� �����������.
      // ������������ ������ � �������� Do, ������� ���������� ����������� ��������� �����.
      // ���� ����������� �������� �������� ������, �� ���������� ��������� ������;
      // ���� ����������� �� �������� ��������, �� ��������� �� ��������� ������� ��������������� �
      // ������������ � �������� Do. ��� ���� �������� ����� Do �� �����.
      // ����� ���� �������� ����� �������� ���, ����� ����������� temperature
      // �������������. ����� ����� ��������� ������� ���� � ���� (����� ��� �� ���������).
      // ��� ������� ���� ����� �������� ��������� �������� ���� ��� ������������� ���� 
      // (������ ������ �����).
      // � ������ ��������� ���������, ���� ������� ���, �� ����� ������������ ����������� ����
      // � ������� ������ ����������.
    case PRG_TWhile:
      {
        if (temperature >= SetTemperature) // ���� ������� t ������ ��� ����� ���������, �� ��, ���� ������
        {
          ProgramCounter++;
          zprg++;  // ��������� �� ��������� �������
        }
        else  // ����� ���������� ��������� ������ �� ������ ����� Do
        {
          zprg = retp;      // ����� �������� ����� Do TWhile
        }
        break;
      }
        
      // 1 byte
      // �������� ������ LEVEL2=0 ( � ���� ���� ����).
      // ����� ����� �� �������������, ��� ���� �� ������ ����/������.
      // ���������� ������� ��������� �������� ������� ���� � ���� - H2O.
    case PRG_L2Wait:
      MaskProg=0; // ��������� ��������� ������� (�������� ��������� H2O)
      Reg.Seg1 = DIG_H; // ������ H2O - �������� ����������� � ������ � �������� �����
      Reg.Seg2 = DIG_2;
      Reg.Seg3 = DIG_o;
      Reg.Seg4 = DIG_SPACE;
        
      if (LEVEL1==1 && LEVEL2==0) // ���� ������ ��������, �� 
      {
        MaskProg = 0xFF; // �������� ��������� ������� (������� ��������� H2O)
        ProgramCounter++;
        zprg++;  // ����������� ��������� ������
      }
      else
      {
        BeepCounter++;
        // ����������� ������, �������, ��� ���� ��� � �������� ��������
        if (BeepCounter == 0 || BeepCounter == 0x100)     Timer_Buzzer = 10;
      }
      break;

      // 1 byte
      // �������� ������ LEVEL1=0 (��� ������).
      // ����� ����� �� �������������, ��� ���� �� ������ ����/������.
      // ���������� ������� ��������� �������� ������� ���� - FULL.
      // �������, ����� ����� ����������������� ����������� ���� ����� ��������� �����.
    case PRG_L1Wait:
      MaskProg=0; // ��������� ��������� ������� (�������� ��������� FULL)
      Reg.Seg1 = DIG_F; // ������ FULL - �������� ����������� � ������ � �������� �����
      Reg.Seg2 = DIG_U;
      Reg.Seg3 = DIG_L;
      Reg.Seg4 = DIG_L;
        
      if (LEVEL1==0 && LEVEL2==1) // ���� ������ ��������, �� 
      {
        MaskProg = 0xFF; // �������� ��������� ������� (������� ��������� FULL)
        ProgramCounter++;
        zprg++;  // ����������� ��������� ������
      }
      break;


      // 1 byte
      // �������� �������� ������ ���� � ��������� ������� � ��� �������� ����.
      // ����� ����� �� �������������, ��� ���� �� ������ ����/������.
      // ���������� ������� ��������� �������� �������� ���� - door.
    case PRG_LockWait:
      MaskProg=0; // ��������� ��������� ������� (�������� ��������� door)
      Reg.Seg1 = DIG_d; // ������ door - �������� �������� ����
      Reg.Seg2 = DIG_o; // (�������� ����������� � ������ � �������� �����)
      Reg.Seg3 = DIG_o;
      Reg.Seg4 = DIG_r;
        
      if (adcLine < LINE_ON_TRESHOLD) // ���� ������ ��������, �� 
      {
        MaskProg = 0xFF; // �������� ��������� ������� (������� ��������� door)
        ProgramCounter++;
        zprg++;  // ���������� ��������� ������
      }
      else
      {
        BeepCounter++;
        // ����������� ������, �������, ��� ���� ��� � �������� �������� - ������� ���
        if (BeepCounter == 0 || BeepCounter == 0x100) Timer_Buzzer = 10;
      }
      break;
      
      // 1 byte
    case PRG_End:       
      ProgramNumber=0;
//        StoreEEPROM(0x00, ProgramNumber);
      Timer_Buzzer = 50;  // ������ ����
      // ��������� ����� ���������� ���������
      Reg.Seg1 = DIG_E;
      Reg.Seg2 = DIG_n;
      Reg.Seg3 = DIG_d;
      Reg.Seg4 = DIG_SPACE;
      Reg.Lock = 0; // �������� ��������� ������ ��������� (�������)
      i2cMasterSendNI(0x40, 4, Reg.Buf);  // �������� �������� ���������
      delay_timer(2);
      Timer_Buzzer = 25;  // ������ ����
      break;

    // ������������ �������
    default:
      ProgramNumber=0;
      Reg.Seg1 = DIG_E; // Error4 - ������������ ������� ��� ���� ��������� ������.
      Reg.Seg2 = DIG_r;
      Reg.Seg3 = DIG_r;
      Reg.Seg4 = DIG_4;
      i2cMasterSendNI(0x40, 4, Reg.Buf);  // �������� �������� ���������
      while (1) { __no_operation(); } // ������ ����� ������ � ��������
     }
  }
  
  
  
}



