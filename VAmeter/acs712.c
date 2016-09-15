#define ISENSE    5   // ����� ������ ��������� ����
#define USENSE    7
#define VREF      3.22  // ������� ���������� (���������� �������)
#define VIN_ZERO  2.5 // ���������� �� ��� ��� ������� ���� (�����)
#define ADC_REF   0x0316  // �������� ��� ��� ������� ���� (������ ����� ��������,
                          // ����� ������� ������ �� ������)

//-----------------------------------------------------------------
// ������������� ������ � ��� ������ ��������� ���� � ����������

void acs712_init (void)
{
  // Disable Digital Pins
  // Disable ADC5 Pin
  DIDR0=(1<<ADC5D);

  // AVCC=AREF
  // MUX=ADC5
  ADMUX=(0<<REFS1)|(1<<REFS0)|(0<<ADLAR)|(0<<MUX3)|(1<<MUX2)|(0<<MUX1)|(1<<MUX0);

  // Maximum prescaller
  // ADC Enable
  // Start Firtst One Conversion
  ADCSRA=(1<<ADEN)|(1<<ADSC)|(0<<ADATE)|(0<<ADIF)|(0<<ADIE)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);


}

//---------------------------------------------
// ������ 10-������� �������� �� ��� �� ��������� ������
// 5 - ������� �����
// 7 - ����� ����������

unsigned int adc_read (unsigned char channel)
{
  unsigned char tmpL, tmpH;

  PRR&=~(1<<PRADC); // ����� ���� ����������� ������������������ ���
  ADMUX&=0xF8;
  ADMUX|=channel; // ��������� ������ ������
  ADCSRA|=(1<<ADSC); // ������ ������ ��������������
  while (ADCSRA&(1<<ADSC)); // ���� ����� ��������������

  tmpL=ADCL;
  tmpH=ADCH;
  
  return (tmpH<<8|tmpL);
}
