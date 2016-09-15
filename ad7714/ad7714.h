#define delay_us(V) __delay_cycles(V*8L)

void delay_ms (int c)
{
  do
  {
    delay_us (1000); // 1 ms
  }
  while (--c);
}

//================================
// Port Definitions
#define AD7714_PORT PORTB
#define AD7714_DIR  DDRB
#define AD7714_PIN  PINB
#define DD_SCK      PB1
#define DD_MOSI     PB2
#define DD_MISO     PB3  
#define DRDY_7714   PB4   // ���������� ������ ���
#define CS_7714     PB6   // ����� ��������� ���


unsigned char ErrorFlag_7714; // ���� ������ ��� (������ ������� ��� ������ ����������) 0 - ��, 0xFF - Error
unsigned char ADC7714_Channel; // ������� �������� ������ ������ ��� [0..7]
unsigned char ADC7714_A; // ������� ������� ����������
unsigned char ADC7714_B;
unsigned char ADC7714_C; // ������� ������� ����������