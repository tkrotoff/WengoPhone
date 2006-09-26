#ifndef __PHCODEC_H__
#define __PHCODEC_H__



#ifdef __cplusplus
extern "C"
{
#endif

enum phcodec_types 
{
  PH_CODEC_TYPE_AUDIO=0,
  PH_CODEC_TYPE_VIDEO,
  PH_CODEC_TYPE_OTHER
};

struct phcodec
{
  const char *mime;                  /** mime string describing the codec */
  int   clockrate; /** sampling rate of the decoded signal */
  int   encoded_framesize;
  int   decoded_framesize; /** size in bytes of a 20ms frame of samples */
  void  *(*encoder_init)(void *);
  void  *(*decoder_init)(void *);
  void  (*encoder_cleanup)(void *ctx);
  void  (*decoder_cleanup)(void *ctx);
  int   (*encode)(void *ctx, const void *src, int srcsize, void *dst, int dstsize);
  int   (*decode)(void *ctx, const void *src, int srcsize, void *dst, int dstsize);
  void (*tie)(void *enc, void *dec); 

  int   codec_type;
  int   priority;
  struct phcodec *next;
};

typedef struct phcodec phcodec_t;


extern phcodec_t *ph_codec_list;

void ph_media_codecs_init(const char *pluginpath);

typedef   int (*ph_codec_plugin_init_type)(void (*codec_register_function)(phcodec_t *));


#ifdef __cplusplus
}
#endif

#endif
