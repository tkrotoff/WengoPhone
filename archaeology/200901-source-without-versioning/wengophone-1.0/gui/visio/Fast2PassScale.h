
///////////////////////////////////////////////////////////////////////////////////////////

// CFast2PassScale.h



///////////////////////////////////////////////////////////////////////////////////////////////
//
//   The CFast2PassScale Class!!!
//
//
//   The code in this class is based on code originally "borrowed" from the codeguru site article:
//   "2 Pass Scaling using Filters" by Eran Yariv. 
//
//   I have re-written the scaling portion with inline assembler, bringing the two passes "together".
//   Being machine level, it probably has a few restrictions I haven't even realized yet. I do know that:
//
//   - It works with arrays of COLORREFs (meaning pixels are DWORD aligned values - RGB QUADS).  The 
//     algorithm puts in a "0" for the alignment byte (throwing out whatever happens to be in there 
//     in the original bitmap.  So a pixel that is 02RRGGBB will have the 02 stripped out of it when it
//     is scaled (to 00RRGGBB).
//    
//   - You now pass in "int" instead of UINT to the scaling functions so you are now limited to only 
//     2^31 pixels in any dimension.
//
//   - I have done my best to make the algorithm "mathematically identical" to the original one. Rounding
//     issues are probably a little different since I only use floats for the pixel weights in the 
//     ContributionType structure, and I keep the vertically scaled "pixels" in floating point form
//     between "passes" to save time.  Note that when I speak of the "original" code, I mean the corrected
//     code (corrections suggested by myself and others on codeguru.com).  If you haven't made ANY changes 
//     to the code given to you, you should notice *blazing* speed improvements as well as better quality.
//
//   - The temporary bitmap in the original code whose size was XSource * YDest * 4Bytes per pixel has
//     been replaced by an array whose size is 12 bytes * XSource.  So very large bitmaps could use 
//     significantly less memory and scaling processes that can now fit entirely into the cache should 
//     also benefit.
//
//   - I wrote the Bilinear filtering right into CalcContributions().  Since filtering is done before the
//     scaling and because I only re-wrote the scaling in assembler, you should be able to add the other
//     filtering functions back in if you want.  Just make sure you use *floats* for the weights and not
//     doubles or the assembler code will screw it up.
//
//   I have taken out the templates as well as deleting the filter class altogether since I only have
//   need for the Bilinear filter type. If you need to you should be able to add templates back in using  
//   Eran's code as a reference. The CallBack function is gone too, though it could be re-added.
//   Note that every "UINT" in all function declarations has been changed to an "int"
//
//   In addition, I have added a TransparentScale function allowing you to set a transparent color
//   so that you can scale "onto" a destination bitmap with transparency.
// 
//   This way if you want to scale a bitmap and put it over another one with transparency, you can do
//   it in one step, by creating your background image and passing it into TransparentScale() as the 
//   Destination bitmap.  Then all the colors in the scaled image that equal the value passed into
//   TransparentScale will be transparent.
//
//   The assembler was optimized for PPro/P2 chips.  By accessing the source array memory in BYTES or WORDS
//   instead of DWORDS, you could probably speed up a regular Pentium chip by about %10-%15 but it could 
//   slow down the PII/III chips by around %40!  So this code is a "balance" of the 486DX/P5/P6 based chips
//   with a tendency towards 32 bit code.  There are also private versions of the regular and transparent
//   scaling functions that use the SSE instructions on the P3 yielding another 25% improvement.  I don't
//   have a CPUID function yet, so they will not be called unless you edit the Scale and TransparentScale
//   functions to call them yourself.
//
//   If you can improve these assembler functions even more, please share your improvements at
//   codeguru.com!





#ifndef _2_PASS_SCALE_H_
#define _2_PASS_SCALE_H_

#include <math.h> 
#include <windows.h>
#include <windef.h>
#include <assert.h>
#include <fvec.h>
 

typedef struct 
{ 
   int Left,Right;   // Bounds of source pixels window
   float *Weights;  // Normalized weights of neighboring pixels (changed from double* in Eran's code)
   
} ContributionType;  // Contirbution information for a single pixel

typedef struct 
{ 
   ContributionType *ContribRow; // Row (or column) of contribution weights 
   int WindowSize,              // Filter window size (of affecting source pixels) 
        LineLength;              // Length of line (no. or rows / cols) 
} LineContribType;               // Contribution information for an entire line (row or column)



 
class CFast2PassScale 
{
public:

    CFast2PassScale()  {};

    ~CFast2PassScale() {};

    COLORREF * AllocAndScale (  
        COLORREF   *pOrigImage, 
        int        uOrigWidth, 
        int        uOrigHeight, 
        int        uNewWidth, 
        int        uNewHeight);

    COLORREF * Scale (  
        COLORREF   *pOrigImage, 
        int        uOrigWidth, 
        int        uOrigHeight, 
        COLORREF   *pDstImage,
        int        uNewWidth, 
        int        uNewHeight);

	COLORREF * TransparentScale (  
        COLORREF   *pOrigImage, 
        int        uOrigWidth, 
        int        uOrigHeight, 
        COLORREF   *pDstImage,
        int        uNewWidth, 
        int        uNewHeight,
		COLORREF   TransparentColor);


private:

	    
    LineContribType *AllocContributions (   int uLineLength, 
                                            int uWindowSize);

    void FreeContributions (LineContribType * p);

    LineContribType *CalcContributions (    int    uLineSize, 
                                            int    uSrcSize, 
                                            float  dScale);

	void SuperScale (          // This replaces the other VerticalScale and HorizontalScale functions.
			COLORREF           *pSrc, 
            int                uSrcWidth,
            int                uSrcHeight,
            COLORREF           *pDst, 
            int                uResWidth,
            int                uResHeight);

	void TransparentSuperScale (          // This replaces the other VerticalScale and HorizontalScale functions.
			COLORREF           *pSrc,     // It also adds a COLORREF 
            int                uSrcWidth,
            int                uSrcHeight,
            COLORREF           *pDst, 
            int                uResWidth,
            int                uResHeight,
			COLORREF           TransParentColor);

	void SSESuperScale (          // The SSE optimized version of SuperScale
			COLORREF           *pSrc, 
            int                uSrcWidth,
            int                uSrcHeight,
            COLORREF           *pDst, 
            int                uResWidth,
            int                uResHeight);

	void SSETransparentSuperScale (// The SSE optimized version of TransparentSuperScale
			COLORREF           *pSrc,    
            int                uSrcWidth,
            int                uSrcHeight,
            COLORREF           *pDst, 
            int                uResWidth,
            int                uResHeight,
			COLORREF           TransParentColor);
};

 

#endif //   _2_PASS_SCALE_H_




