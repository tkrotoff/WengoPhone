#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "speex/speex_echo.h"
#include "speex/speex_preprocess.h"


#define NN 160

int main()
{
   int echo_fd, ref_fd, e_fd;
   float noise[NN+1];
   short echo_buf[NN], ref_buf[NN], e_buf[NN];
   SpeexEchoState *st;
   SpeexPreprocessState *den;

   echo_fd = open ("play.sw", O_RDONLY);
   ref_fd  = open ("rec.sw",  O_RDONLY);
   e_fd    = open ("echo.sw", O_WRONLY | O_CREAT | O_TRUNC, 0644);

   st = speex_echo_state_init(NN, 8*NN);
   den = speex_preprocess_state_init(NN, 8000);

   while (read(ref_fd, ref_buf, NN*2))
   {
      read(echo_fd, echo_buf, NN*2);
      speex_echo_cancel(st, ref_buf, echo_buf, e_buf, noise);
      speex_preprocess(den, e_buf, noise);
      write(e_fd, e_buf, NN*2);
   }
   speex_echo_state_destroy(st);
   speex_preprocess_state_destroy(den);
   close(e_fd);
   close(echo_fd);
   close(ref_fd);
   return 0;
}
