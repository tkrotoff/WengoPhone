/***************************************************************
A.2 aec.cxx
***************************************************************/
/* aec.cxx
 * Acoustic Echo Cancellation NLMS-pw algorithm
 * Author: Andre Adrian, DFS Deutsche Flugsicherung
 * <Andre.Adrian@dfs.de>
 *
 * Version 1.1
 * Copyright (C) DFS Deutsche Flugsicherung (2004). All Rights Reserved.
 *           (C) Mbdsys SARL (2004).
 * 
 * You are allowed to use this source code in any open source or closed source
 * software you want. You are allowed to use the algorithms for a hardware
 * solution. You are allowed to modify the source code.
 * You are not allowed to remove the name of the author from this memo or from
 * the source code files. You are not allowed to monopolize the source code or
 * the algorithms behind the source code as your intellectual property.
 * This source code is free of royalty and comes with no warranty.
 */

#include <stdio.h>
#include <stdlib.h>
# ifdef WIN32
#  define _USE_MATH_DEFINES
#  include <math.h>
#  define roundf(x) floorf((x) + 0.5f) 
#  ifndef M_PI
#    define M_PI 3.14159265358979323846
#  endif /* !M_PI */
# else /* !WIN32 */
#  include <math.h>
# endif /* !WIN32 */
#include <string.h>
#include "aec.h"



IIR_HP6::IIR_HP6(int sampleFreq)
{
  const float AlphaHp6 = 0.075f; /* controls Transfer Frequency */

  alpha = AlphaHp6 / (sampleFreq/8000);

  memset(this, 0, sizeof(IIR_HP6));
}


/* Vector Dot Product */
inline float dotp(register float a[], register float b[]) {
  register float sum0 = 0.0, sum1 = 0.0, sum2 = 0.0, sum3 = 0.0;
  register float *aend =  a + NLMS_LEN;
  
  for(; a < aend; a += 4, b += 4) {
    // optimize: partial loop unrolling
    sum0 += a[0]  * b[0];
	sum1 += a[1]  * b[1];
    sum2 += a[2]  * b[2];
    sum3 += a[3]  * b[3];
  }
  return sum0+sum1+sum2+sum3;
}


/*
 * Algorithm:  Recursive single pole FIR high-pass filter
 *
 * Reference: The Scientist and Engineer's Guide to Digital Processing
 */

FIR1::FIR1()
{
}

void FIR1::init(float preWhiteTransferAlpha)
{
  float x = expf(-2.0 * M_PI * preWhiteTransferAlpha);
  
  a0 = (1.0f + x) / 2.0f;
  a1 = -(1.0f + x) / 2.0f;
  b1 = x;
  last_in = 0.0f;
  last_out = 0.0f;
}

AEC::AEC(int micboost, int samplingFreq) : hp0(samplingFreq), hp2(samplingFreq)
{
  hp1.init(0.01f / (samplingFreq/8000));  /* 10Hz */
  hp01.init(0.01f / (samplingFreq/8000));  /* 10Hz */
 
  Fx.init(PreWhiteAlphaTF);
  Fe.init(PreWhiteAlphaTF);
  need300HPF = samplingFreq == 8000;

  max_max_x = 0.0f;
  hangover = 0;
  memset(max_x, 0, sizeof(max_x));
  dtdCnt = dtdNdx = 0;
  
  memset(x, 0, sizeof(x));
  memset(xf, 0, sizeof(xf));
  memset(w, 0, sizeof(w));
  j = NLMS_EXT;
  lastupdate = 0;
  dotp_xf_xf = 0.0f;
  boost = micboost;
}

inline float AEC::nlms_pw(float mic, float spk, int update)
{
  float d = mic;      	        // desired signal
  x[j] = spk;
  xf[j] = Fx.highpass(spk);     // pre-whitening of x

  // calculate error value (mic signal - estimated mic signal from spk signal)
  float e = d - dotp(w, x + j);
  float ef = Fe.highpass(e);    // pre-whitening of e
 
  dotp_xf_xf += (xf[j]*xf[j] - xf[j+NLMS_LEN-1]*xf[j+NLMS_LEN-1]);
 
  if (update) {    
      
    // calculate variable step size
    float mikro_ef = 0.5f * ef / dotp_xf_xf;
    
    // update tap weights (filter learning)
   
	float *wp = w, *xfp = xf+j, *wpend = w + NLMS_LEN;
    for (; wp < wpend; wp += 4, xfp += 4) {
      *wp += mikro_ef * *xfp;   
      wp[1] += mikro_ef * xfp[1];   
      wp[2] += mikro_ef * xfp[2];   
      wp[3] += mikro_ef * xfp[3];   
    }
  }
  lastupdate = update;
  
  if (--j < 0) {
    // optimize: decrease number of memory copies
    j = NLMS_EXT;
    memmove(x+j+1, x, (NLMS_LEN-1)*sizeof(float));    
    memmove(xf+j+1, xf, (NLMS_LEN-1)*sizeof(float));    
  }
  
  return e;
}


inline int AEC::dtd(float micf, float spkf)
{
  // optimized implementation of max(|x[0]|, |x[1]|, .., |x[L-1]|):
  // calculate max of block (DTD_LEN values)
  spkf = fabsf(spkf);
  if (spkf > max_x[dtdNdx]) {
    max_x[dtdNdx] = spkf;
    if (spkf > max_max_x) {
      max_max_x = spkf;
    }
  }
  if (++dtdCnt >= DTD_LEN) {
    dtdCnt = 0;
    // calculate max of max
    max_max_x = 0.0f;
    for (int i = 0; i < DTD_SIZE; ++i) {
      if (max_x[i] > max_max_x) {
        max_max_x = max_x[i];
      }
    }
    // rotate Ndx
    if (++dtdNdx >= DTD_SIZE) dtdNdx = 0;
    max_x[dtdNdx] = 0.0f;
  }

  // The Geigel DTD algorithm with Hangover timer Thold
  if (fabsf(micf) >= GeigelThreshold * max_max_x) {
    hangover = Thold;
  }
    
  if (hangover) --hangover;
  
  if (max_max_x < UpdateThreshold) {
    // avoid update with silence or noise
    return 1;
  } else {
    return (hangover > 0);
  }
}


inline int AEC::doAEC(int mic, int spk) 
{
  float micf = (float)mic;
  float spkf = (float)spk;
  int res;

  // Mic Highpass Filter - telephone users are used to 300Hz cut-off
 if (need300HPF)
	 micf = hp0.highpass(micf);
 else
	 micf = hp01.highpass(micf);

  // Spk Highpass Filter - to remove DC
 spkf = hp1.highpass(spkf);

  // Double Talk Detector
  int update = !dtd(micf, spkf);
//  printf("%s", update?"!":"x");


  // Acoustic Echo Cancellation
  micf = nlms_pw(micf, spkf, update);

  // Acoustic Echo Suppression
  if (update) {
    // Non Linear Processor (NLP): attenuate low volumes
    micf *= NLPAttenuation;
  }


  res = (int) (micf + 0.5f);
  // Try to replace Mic Boost
 
 if(boost)
    res <<= 3; 
  // Saturation
  if (res > (int)MAXPCM) {
    res = (int)MAXPCM;
  } else if (res < (int) -MAXPCM) {
    res = (int)-MAXPCM;
  }
 return(res);
}

//class AEC *gAec;


extern "C" {

void *create_AEC(int boost, int samplingfreq){
  return (void *)new AEC(boost, samplingfreq);
}

short do_AEC(void *ec, short ref, short mic){
  if(ec)
    return((short)(((class AEC *)ec)->doAEC((int)mic, (int)ref)));

  return mic;
}

void kill_AEC(void *ec){
  delete (class AEC *)ec;
}

}
