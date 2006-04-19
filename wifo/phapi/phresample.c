#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
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

