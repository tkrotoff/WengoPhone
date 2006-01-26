#ifndef WIN32
#include <sys/types.h>
#include <unistd.h>
#define OPENMODE O_RDONLY
#else
#include <io.h>
#define OPENMODE (O_RDONLY|O_BINARY)
typedef long off_t;
#define SEEK_END 2
#define SEEK_SET 0
#endif

#include <fcntl.h>
#include <stdlib.h>

#include "phmbuf.h"

#define DONT(x)
#define DO(x) x


ph_mediabuf_t *
ph_mediabuf_new(int size)
{
  ph_mediabuf_t  *mb;

  mb = (ph_mediabuf_t *) malloc(sizeof(*mb) + (size + 1) );

  if (!mb)
    return 0;

  DONT(printf("allocated mediabuf = %08x size=%d\n", mb, size));
  
  mb->buf = (short *) (mb + 1);
  mb->size = size/2;
  mb->next = 0;
  return mb;
}


void ph_mediabuf_free(ph_mediabuf_t *mb)
{
  DONT(printf("ph_mediabuf_free(%08x)\n",mb));
  if (mb)
    free(mb);
}


int ph_mediabuf_mixaudio(ph_mediabuf_t *mb, short *mix, int samples)
{
  short *src = mb->buf + mb->next;
  int  len;
  int  xlen;
  int avail = mb->size - mb->next;

  if (samples > avail)
    len = avail;
  else
    len = samples;

  xlen = len;

  while(len--)
    {
      *mix = (*mix + *src) >> 1;
      mix++;
      src++;
    }

  
  mb->next += xlen;

  return xlen;

}


ph_mediabuf_t *
ph_mediabuf_load(const char *filename, int samplerate)
{
  int  fd;
  off_t flen;
  int  xlen;
  ph_mediabuf_t *mb;


  if (16000 != samplerate && samplerate != 8000)
    return 0;

  fd = open(filename, OPENMODE);
  
  if (fd == -1)
    return 0;

  flen = lseek(fd, 0, SEEK_END);

  lseek(fd, 0, SEEK_SET);



  if (samplerate == PH_MEDIA_NATIVE_FILE_RATE)
    {
      mb = ph_mediabuf_new((int) flen);
      if (mb)
	read(fd, (char *) (mb->buf),  flen);
      close(fd);
      return mb;
    }

  
  if (PH_MEDIA_NATIVE_FILE_RATE == 16000 && samplerate == 8000)
  {
      short buf[256];
      int blen;
      short *dst;
      
      flen /= 2;
      mb = ph_mediabuf_new((int) flen);

      if (!mb)
	{
	  close(fd);
	  return 0;
	}

      dst = mb->buf;

      while(flen > 0)
	{
	  blen = read(fd, (char *) buf, sizeof(buf));
	  if (blen >= 0)
	    {
	      int i;
	      blen /= 2;

	      for(i = 0; i < blen; i += 2)
		*dst++ = buf[i];
	    }
	  else
	    break;

	  flen -= blen;
	}

      close(fd);
      return mb;
  }

  return mb;

}







