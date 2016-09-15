#define VERTICAL    0
#define HORIZONTAL  1

unsigned char jpeg_type=VERTICAL;

typedef struct 
{
  const __hugeflash unsigned char * jpic; // �������� �� ������ � JPEG ���������
  WORD jsize;                         // ������ �������
  WORD joffset;                       // ������� ������� ��� ������ �������
} IODEV;

//===========================================================
// ������� ������������� ������� JPEG � �����

unsigned int input_func (JDEC * jd, unsigned char * buff, unsigned int ndata) 
{
  IODEV * dev = (IODEV *) jd->device;
  // ��������� ����� �� ������� �������
  ndata = dev->jsize - dev->joffset  > ndata ? ndata : dev->jsize - dev->joffset;
  // �������� �������� �� ������� � �����
  if(buff) 
    {
//      memcpy_P(buff, dev->jpic + dev->joffset, ndata);
      unsigned int i;
      for (i=0;i<ndata;i++)
        {
          // ����������� �� ����� � ����� RAM
          ((unsigned char*)buff)[i] = ((dev->jpic + dev->joffset)[i]);
        }    
    }
  // ����������� ������� ��������
  dev->joffset += ndata;
  // ���������� �������� ������ ������������� ������
  return ndata;
}

//=================================================================
// ������� ������ ��������������� bmp �� ������ �� LCD
// ���������� jpeg_type ���������� ���������� ��� ��������� ����

unsigned int output_func(JDEC * jd, void * bitmap, JRECT * rect) 
{

  unsigned short * bmp = (unsigned short *) bitmap;

  unsigned int x;
  unsigned int y;
  unsigned int i = 0;
//  unsigned int color;

  // ���� ������ ������� �����������
  for(y = rect->top; y <= rect->bottom; y++) 
    {
      for(x = rect->left; x <= rect->right; x++) 
        {
          //LCD_SetPoint(y, x, bmp[i++]);
          if (jpeg_type==VERTICAL)
            {
              ili9481_putpixel16(x,y,bmp[i++]);
            }
          else
            {
              ili9481_putpixel16(y,x,bmp[i++]);
            }
        }
    }

  return 1;	// Continue to decompress
}


#define SCALE   0     // Output scaling 0: 1/1, 1: 1/2, 2: 1/4 or 3: 1/8

unsigned char jdwork[3100];




