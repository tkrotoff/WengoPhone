#ifndef __PHMBUF_H__
#define __PHMBUF_H__ 1



struct ph_mediabuf
{
  short  *buf;
  int    next;
  int    size;
};

typedef struct ph_mediabuf ph_mediabuf_t;



ph_mediabuf_t *ph_mediabuf_new(int size);
void ph_mediabuf_init(ph_mediabuf_t *mb, void *buf, int size);
void ph_mediabuf_cleanup(ph_mediabuf_t *mb);
void ph_mediabuf_free(ph_mediabuf_t *mb);
int ph_mediabuf_mixaudio(ph_mediabuf_t *mb, short *mix, int samples);
ph_mediabuf_t *ph_mediabuf_load(const char *filename, int samplerate);


#ifndef PH_MEDIA_NATIVE_FILE_RATE
#define PH_MEDIA_NATIVE_FILE_RATE 16000
#endif

#endif
