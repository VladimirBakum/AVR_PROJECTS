/*************************************************************************
**
**  ��������� ���������� ������������ ���
**
*************************************************************************/

#ifndef DSPTOOLS_H
#define DSPTOOLS_H

#include <mathlib.h>


// ��������� ���������� ��������� �������� �� �������
template <typename el_t, typename res_t>
void recursiveMultiple(el_t *pArr, res_t  *pResult, int count)
{
    for (int i = 1; i < count; i++)
        pResult[i-1] = (res_t)pArr[i]*(res_t)pArr[i-1];
}


// ���������� ��������, ������������ �� ���� ������
template <typename sample_t, typename sum_t>
void calcSamplesSumPerSymbol
                 (  sample_t *pData        // ��������� �� ������� ������
                  , int symbolsNumber      // ����� �������� ����������
                  , float samplesPerSymbol // ����� �������� �� ������
                  , sum_t *pSumResult       // ��������� �� �������� ������ ���� ������� � �������� �������
                  )
{
    int i,j;
    for (i = 0; i < symbolsNumber; i++) {
        pSumResult[i] = 0;
        int offset = i*samplesPerSymbol;
        for (j = 0; j < samplesPerSymbol; j++)
            pSumResult[i] += pData[offset + j];
    };
}


// �������������� �������
template <typename Type>
void integral(Type *pVector, int count, int dt)
{
    Type sum;
    int j, i;
    for(i = 0; i < count/dt; i++){
        sum = 0;
        for (j = 0; j < dt; j++){
            sum += pVector[i*dt + j];}
            sum /= dt;
        for (j = 0; j < dt; j++)
            pVector[i*dt + j] = sum;
    }
}


// ���������� ������� ����������� ��������
template <typename sig_t, typename res_t>
void movingAverage (sig_t *pVector, res_t *pResult, int count, int avgLength)
{
    int i, j;
    res_t avg;
    for (i = 0; i < count; i++) {
        avg = 0;
        int indexBegin = (i - avgLength/2 ) < 0 ? 0 : ( i - avgLength/2);
        int indexEnd =  (i + avgLength/2) > count ? count : i + avgLength/2;
        for (j = indexBegin; j < indexEnd; j ++)
            avg += (res_t)pVector[j];
        pResult[i] = avg/avgLength;

     }
}

// ��� - ������
template <typename sig_t, typename res_t, typename h_t>
void firFilter (sig_t *pVector, res_t *pResult, int count, h_t *pH, int hLength)
{
    res_t sum;
    int i, j;
    for (i = 0; i < hLength; i++) {
        sum = 0;
        for (j = 0; j < i; j++)
            sum += (res_t)(pVector[i - j]*pH[j]);
        pResult[i] = sum;
    }

    for (i = hLength; i < count; i++) {
        sum = 0;
        for (j = 0; j < hLength; j++)
            sum += pVector[i - j]*pH[j];
        pResult[i] = sum;
    }
}

// ������������ ��������
template <typename vec1_t, typename vec2_t, typename res_t>
void vecMul(const vec1_t * pVec1, const vec2_t * pVec2, res_t * pRes, int count)
{
        int i;
        for(i = 0; i < count; i++)
                pRes[i] = (res_t)pVec1[i] * (res_t)pVec2[i];
}


// ��������� ������������ ��������
template <typename vec1_t, typename vec2_t>
double dotProd(const vec1_t *x, const vec2_t *y, const int nx)
{
    int i;
    double sum = 0;
    for (i=0; i < nx; i++) {
            sum += x[i] * y[i];
    }
    return sum;
}

// ������������ ��������
template <typename sig1_t, typename sig2_t, typename res_t>
void sum (sig1_t *pFirstSignal, sig2_t *pSecondSignal, res_t *pResult, int count)
{
    int i;
    for (i = 0; i < count; i++) {
        pResult[i] = (res_t)pFirstSignal[i] + (res_t)pSecondSignal[i];
    }
}


// �������� ��������
template <typename sig1_t, typename sig2_t, typename res_t>
void sub (sig1_t *pFirstSignal, sig2_t *pSecondSignal, res_t *pResult, int count)
{
    int i;
    for (i = 0; i < count; i++) {
        pResult[i] = (res_t)pFirstSignal[i] - (res_t)pSecondSignal[i];
    }
}


// ����������� ����� ��������� ��������, �������� �� ������������� � �������������� ��� ��������,
// �������������� ���������� ������� � ����������� �����������
template <typename source_t, typename dest_t>
    void convert (source_t *pSource, dest_t * pDest, double coeff, int sampleNumber, int resampleCoeff)
{
    int i;
    int n = (int)(sampleNumber/resampleCoeff);
    for (i = 0; i < n; i++) {
            pDest[i] = (dest_t)pSource[i*resampleCoeff]*coeff;
        }
}


// �������� ������ �������������� ����� � �������������� �����
// ������� �����������, ������ ����� ����������� ������
template <typename real_t, typename complex_t>
void realToComplex (real_t *pRealBuf,     // ��������� �� ������ ������. �����
                    complex_t *pCplxBuf,  // ��������� �� ������ �����������. �����
                    int length		  // ����� ��������� � �������
                    )
{
    int i;
    for (i=0; i < length; i++) {
        pCplxBuf[2*i] = (complex_t)pRealBuf[i];
        pCplxBuf[2*i + 1] = 0;
    }
}

// ����� �������
template <typename Type>
Type elementsSum(Type *pArray , int length)
{
    Type result = 0;
    for (int i = 0; i < length; i++)
      result += pArray[i];
    return result;
}

// ���������� ������ ������ �� ������ �������, ������� �������������, ������� � ��������� ����
inline double sinSample(int i, double freq, double phase, double sampleRate)
{
    return sin( (2*PI*freq*i/sampleRate)  + phase);
}

// ��������� ������ ����
template <typename sig_t>
void noiseGen(sig_t *pRes, int count, double amp,  double constLevel)
{
    int i;
    for (i = 0; i < count; i++) {
        pRes[i] = (sig_t)randomEx(-amp + constLevel, amp + constLevel);
    }
}

// ��������� ��������������� �������
template <typename sig_t>
void sinGen (sig_t *pResult, int numSamples, double freq,
             double  amp, double  phase, double sampleRate )
{
    int i;
    for (i = 0; i < numSamples; i++)
            pResult[i] = amp*sinSample(i, freq, phase, sampleRate);
}

// �������� � ������� ������ ����
template <typename sig_t>
void addNoise(sig_t *pSignal, int numSamples, double noizeAmp)
{
    int i;
    for (i = 0; i < numSamples; i++) {
        pSignal[i] += (sig_t)randomEx(-noizeAmp, noizeAmp);
    }
}


#endif // DSPTOOLS_H
