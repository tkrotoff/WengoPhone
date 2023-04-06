struct piximage {
%TypeHeaderCode
#include <pixertool/pixertool.h>
%End

        unsigned char * data;
        unsigned width;
        unsigned height;
        pixosi palette;

};

enum pixosi {
        PIX_OSI_UNSUPPORTED = -1,
        PIX_OSI_YUV420P = 0, /* this value: 0 should not be modified. It is currently used as 0 in phmedia-video.c */
        PIX_OSI_I420,
        PIX_OSI_YUV422,
        PIX_OSI_YUV411,
        PIX_OSI_YUV422P,
        PIX_OSI_YUV444P,
        PIX_OSI_YUV424P,
        PIX_OSI_YUV41P,
        PIX_OSI_YUY2,
        PIX_OSI_YUYV,
        PIX_OSI_YVYU,
        PIX_OSI_UYVY,
        PIX_OSI_YV12,
        PIX_OSI_RGB555,
        PIX_OSI_RGB565,
        PIX_OSI_RGB1,
        PIX_OSI_RGB4,
        PIX_OSI_RGB8,
        PIX_OSI_RGBA32, //18
        PIX_OSI_RGB32, //19
        PIX_OSI_ARGB32, //20
        PIX_OSI_RGB24, //21
        PIX_OSI_BGR24, //22
        PIX_OSI_NV12 //23
};

enum pixerrorcode {
        PIX_OK,
        PIX_NOK
};

enum {
        PIX_NO_FLAG,
        PIX_FLIP_HORIZONTALLY
};


piximage * pix_alloc(pixosi pix, unsigned width, unsigned height) /Factory/;

void pix_free(piximage * ptr);

unsigned pix_size(pixosi pix, unsigned width, unsigned height);

pixerrorcode pix_convert(int flags, piximage * img_dst, piximage * img_src);

piximage * pix_copy(piximage * src);

