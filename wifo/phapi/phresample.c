#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define PH_OPTIMIZE_RESAMPLE 1

#ifdef _MSC_VER
#define INLINE __inline
#else
#define INLINE __inline__
#endif

#if defined(PH_OPTIMIZE_RESAMPLE)
struct phresamplectx
{
  double     upsamplebuf[4];
  double     downsamplebuf[4];

};


// Filter descriptions:
//   LpBu4/3600 == Lowpass Butterworth filter, order 4, -3.01dB frequency
//     3600
//
static INLINE double
dofilter(register double val, double buf[4]) {
   register double tmp, fir, iir;
   tmp= buf[0]; memmove(buf, buf+1, 3*sizeof(double));
   iir= val * 0.06750480601637321;
   iir -= 0.4514083390923062*tmp; fir= tmp;
   iir -= -0.2270502870808351*buf[0]; fir += buf[0]+buf[0];
   fir += iir;
   tmp= buf[1]; buf[1]= iir; val= fir;
   iir= val;
   iir -= 0.04574887683193848*tmp; fir= tmp;
   iir -= -0.1635911661136266*buf[2]; fir += buf[2]+buf[2];
   fir += iir;
   buf[3]= iir; val= fir;
   return val;
}


void *ph_resample_init()
{
  struct phresamplectx *ctx = calloc(sizeof(struct phresamplectx), 1);
  
  return ctx;
}



void ph_resample_cleanup(void *p)
{
  struct phresamplectx *ctx = (struct phresamplectx *)p;
  free(ctx);

}


/**
 * @brief in-place downsample of a buffer by a factor of 2
 *         we first pass the signal through low-pass filter with cutoff freq 3700 Hz
 *         and then decimate by factot of 2 
 */
void ph_downsample(void *rctx, void *framebuf, int framesize)
{
  short *sp = (short *) framebuf; // 'narrow' buffer
  short *dp = (short *) framebuf; // 'wide' buffer
  double tmp;
  struct phresamplectx *ctx = (struct phresamplectx *)rctx;
  double *fbuf = ctx->downsamplebuf;

  
  framesize = (framesize / sizeof(short)) / 2;

  while( framesize-- )
    {
      tmp = dofilter((double) *sp++, fbuf);
      *dp++ = (short) (tmp + 0.5);
      dofilter((double) *sp++, fbuf);
    }
}


/**
 * @brief upsample of a buffer by a factor of 2
 *        we insert 0 between each sample and then pass  the signal through low-pass filter with cutoff freq 3700 Hz
 */
void ph_upsample(void *rctx, void *dbuf, void *framebuf, int framesize)
{
  short *sp = (short *) framebuf; // 'narrow' buffer
  short *dp = (short *) dbuf; // 'wide' buffer
  double tmp;
  struct phresamplectx *ctx = (struct phresamplectx *)rctx;
  double *fbuf = ctx->upsamplebuf;
  
  framesize = framesize / sizeof(short);

  while( framesize-- )
    {
      tmp = dofilter((double) *sp++, fbuf);
      *dp++ = (short) (tmp + 0.5);
      tmp = dofilter(0.0, fbuf);
      *dp++ = (short) (tmp + 0.5);
    }
}




#else /* PH_OPTIMZE_RESAMPLE */
#include "fidlib.h"

struct phresamplectx
{
  FidFilter *filt;
  FidFunc    *funcp;
  FidRun     *run;
  void       *upsamplebuf;
  void       *downsamplebuf;

};


void *ph_resample_init()
{
  struct phresamplectx *ctx = calloc(sizeof(struct phresamplectx), 1);
  

  ctx->filt = fid_design("LpBu4", 16000, 3600, 0, 0, 0);
  ctx->run = fid_run_new(ctx->filt, &ctx->funcp);
  ctx->upsamplebuf = fid_run_newbuf(ctx->run);
  ctx->downsamplebuf = fid_run_newbuf(ctx->run);

  return ctx;
}



void ph_resample_cleanup(void *p)
{
  struct phresamplectx *ctx = (struct phresamplectx *)p;

  if (!p)
    return;

  fid_run_freebuf(ctx->downsamplebuf);
  fid_run_freebuf(ctx->upsamplebuf);
  fid_run_free(ctx->run);
  free(ctx->filt);
  free(ctx);

}

  
/**
 * @brief in-place downsample of a buffer by a factor of 2
 *         we first pass the signal through low-pass filter with cutoff freq 3700 Hz
 *         and then decimate by factot of 2 
 */
void ph_downsample(void *rctx, void *framebuf, int framesize)
{
  short *sp = (short *) framebuf; // 'narrow' buffer
  short *dp = (short *) framebuf; // 'wide' buffer
  double tmp;
  struct phresamplectx *ctx = (struct phresamplectx *)rctx;
  void *fbuf = ctx->downsamplebuf;
  FidRun *run = ctx->run;
  FidFunc *funcp = ctx->funcp;

  
  framesize = (framesize / sizeof(short)) / 2;

  while( framesize-- )
    {
      tmp = funcp(fbuf, (double) *sp++);
      *dp++ = (short) (tmp + 0.5);
      funcp(fbuf, (double) *sp++);
    }
}


/**
 * @brief upsample of a buffer by a factor of 2
 *        we insert 0 between each sample and then pass  the signal through low-pass filter with cutoff freq 3700 Hz
 */
void ph_upsample(void *rctx, void *dbuf, void *framebuf, int framesize)
{
  short *sp = (short *) framebuf; // 'narrow' buffer
  short *dp = (short *) dbuf; // 'wide' buffer
  double tmp;
  struct phresamplectx *ctx = (struct phresamplectx *)rctx;
  void *fbuf = ctx->upsamplebuf;
  FidRun *run = ctx->run;
  FidFunc *funcp = ctx->funcp;

  
  framesize = framesize / sizeof(short);

  while( framesize-- )
    {
      tmp = funcp(fbuf, (double) *sp++);
      *dp++ = (short) (tmp + 0.5);
      tmp = funcp(fbuf, 0.0);
      *dp++ = (short) (tmp + 0.5);
    }
}

#endif
