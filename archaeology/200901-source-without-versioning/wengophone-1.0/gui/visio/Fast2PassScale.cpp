#include "Fast2PassScale.h"  


LineContribType* CFast2PassScale::AllocContributions (int uLineLength, int uWindowSize)
{
    LineContribType *res = new LineContribType; 
        // Init structure header 
    res->WindowSize = uWindowSize; 
    res->LineLength = uLineLength; 
        // Allocate list of contributions 
    res->ContribRow = new ContributionType[uLineLength];
    for (int u = 0 ; u < uLineLength ; u++) 
    {
        // Allocate contributions for every pixel
        res->ContribRow[u].Weights = new float[uWindowSize];
    }
    return res; 
} 

void CFast2PassScale::FreeContributions (LineContribType * p)
{ 
    for (int u = 0; u < p->LineLength; u++) 
    {
        // Free contribs for every pixel
        delete [] p->ContribRow[u].Weights;
    }
    delete [] p->ContribRow;    // Free list of pixels contribs
    delete p;                   // Free contribs header
} 

 
LineContribType * CFast2PassScale::CalcContributions (int uLineSize, int uSrcSize, float dScale)
{ 

	float dWidth;
    float dFScale = 1.0;
    float dFilterWidth = 1.0;//CurFilter.GetWidth();

    if (dScale < 1.0) 
    {    // Minification
        dWidth = dFilterWidth / dScale; 
        dFScale = dScale; 
    } 
    else
    {    // Magnification
        dWidth= dFilterWidth; 
    }
 
    // Window size is the number of sampled pixels
    int iWindowSize = 2 * (int)ceil(dWidth) + 1; 
 
    // Allocate a new line contributions strucutre
    LineContribType *res = AllocContributions (uLineSize, iWindowSize); 
 
    for (int u = 0; u < uLineSize; u++) 
    {   // Scan through line of contributions
        float dCenter = (float)u / dScale;   // Reverse mapping
        // Find the significant edge points that affect the pixel
        int iLeft = max (0, (int)floor (dCenter - dWidth)); 
       
        int iRight = min ((int)ceil (dCenter + dWidth), int(uSrcSize) - 1); 
        
 
        // Cut edge points to fit in filter window in case of spill-off
        if (iRight - iLeft + 1 > iWindowSize) 
        {
            if (iLeft < (int(uSrcSize) - 1 / 2)) 
            {
                iLeft++; 
            }
            else 
            {
                iRight--; 
            }
        }
		res->ContribRow[u].Left = iLeft; 
		res->ContribRow[u].Right = iRight;
        float dTotalWeight = 0.0;  // Zero sum of weights
		float dVal;
		float filtered;
		int iSrc;

		for (iSrc = iLeft; iSrc <= iRight; iSrc++)
		{   // Calculate weights
			dVal = dFScale * (dCenter - (float)iSrc);
			dVal = float(fabs(dVal)); 
			filtered = (float)(dVal < 1.0 ? 1.0 - dVal : 0.0); 

			dTotalWeight += (res->ContribRow[u].Weights[iSrc-iLeft] =  
				                dFScale * filtered); 
		}
		
		
        assert (dTotalWeight >= 0.0);   // An error in the filter function can cause this 
        if (dTotalWeight > 0.0)
        {   // Normalize weight of neighbouring points
            for (iSrc = iLeft; iSrc <= iRight; iSrc++)
            {   // Normalize point
                res->ContribRow[u].Weights[iSrc-iLeft] /= dTotalWeight; 
            }
        }
   } 
   return res; 
} 



void CFast2PassScale::SuperScale (
			COLORREF           *pSrc, 
            int                uSrcWidth,
            int                uSrcHeight,
            COLORREF           *pDst, 
            int                uResWidth,
            int                uResHeight)
{

    LineContribType * YContrib = CalcContributions (uResHeight, uSrcHeight, float(uResHeight) / float(uSrcHeight)); 
    LineContribType * XContrib = CalcContributions (uResWidth, uSrcWidth, float(uResWidth) / float(uSrcWidth)); 

    DWORD *ContribPtrX, *ContribTempPtr, *ContribPtrY;        // Compliments YContrib and XContrib above.

	DWORD *Dest = pDst;					                      // Keeps track of where we are to put the 
											                  // next pixels in the destination bitmap.

	float* YWeightPtr;                                        // A pointer to our Y "Wieght" factors.
	float* RGBArray = new float[uSrcWidth * 3];               // Our "temporary" array of pixels 
	float* RGBArrPtr;

	int SourceWidth = 4 * uSrcWidth;                          // This number tells us how many bytes there are 
	                                                          // between 2 vertically adjacent pixels.

	DWORD BVal, GVal, RVal;					                  // Temporary holders for our pixel components
	BVal = GVal = RVal = 0;


	DWORD YDelta, YCounter, XCounter, ColumnCounter; // Some counters

	_asm{

		
			// This is the "Super Optimized" version of the CFast2PassScale scaling function
		    // It "brings the two passes together" (so to speak) reducing memory usage and instructions
		    // Improving on the original C++ source, the temporary bitmap is eliminated (and replaced with 
		    // an array whose size is [12 * uSrcWidth] bytes).  The function will produce identical
		    // bitmaps to the original (corrected) C++ source.  If there are any differences they
		    // should be due to rounding errors (rounding should actually be BETTER with this function
		    // since the float precision decimal numbers are preserved between the Y and X passes.
			//
			//
			// The code achieves speed increases over the original Horizontal and Vertical scaling
		    // functions by calculating the vertical scaling for one horizontal "strip" of the source bitmap
		    // and placing the resulting pixels in an array of float precision R, G, and B values.
			// Then the array is scaled in the "horizontal" direction and the final pixels are placed 
		    // directly into the Destination bitmap.  IOW, each iteration of the VerticalLoop loop 
		    // will produce one line of scaled destination pixels.  This has the advantage of avoiding
		    // "cache thrashing" on most large bitmaps since the bitmap is handled in strips, and the
		    // memory overhead associated with creating and destroying large temporary arrays is eliminated.
		    // Also, several costly "fistp" instructions are avoided (since float RGB values are preserved
		    // between Y and X scaling (as well as not having to parse out RGB values from temporary source 
		    // pixels on the second pass).
		    // 
		    
		
			//  Below is a diagram of how the bitmap is processed. It is mathematically the same 
		    //  as the (corrected) C++ source code.
		
		    //  There is still calculating to be done in both the X and Y direction, but instead of
		    //  doing it all at once, we Vertically Scale each pixel for one line of the source bitmap
		    //  (into an array of float precision R,G,B values) and then do the "Horizontal Scaling"
		    //  on that array (just like the temporary bitmap in the original without the memory waste
		    //  and cache thrashing.  The algorithm is done in these two basic steps for each vertical
		    //  line in the bitmap.


		    //    Bitmap Processing 1 (ColumnLoop):  * = pixel.
		    //
			//    -----------------------
			//   |
			//   | * 1  <- Process Pixels in Y direction to YDelta to get the RBG
			//   | * 2             values of our "Vertically Scaled" pixel.
			//	 | * 3             
		    //   |                 Then repeat for each column in the X direction ->
		    //   |                 building an array of "vertically scaled" pixels 
		    //   |                 for one horizontal line of our temporary bitmap.     	
		

			//    Bitmap Processing 2 (RowLoop):  * = pixel.
			//
			//    -----------------------
			//   |
			//   | ***
			//   | 123 ->    Weight the array values (generated by Part One above) in the 
		    //   |           "X direction" and plot the line of destination pixels. 
		    //   |
		    //   |                 Repeat Parts 1 and 2 for each vertical line in the bitmap.   


			//
			//   Here is a diagram of the loops:
			//
			//
			//   VerticalLoop:          // This loop goes once for each line down destination bitmap.
			//      ColumnLoop: (1)     
			//         YWeightingLoop:  // Does the Y-Weighting for each pixel on the source bitmap
			//
			//      RowLoop:    (2)     
			//         XWeightingLoop:  // Does the X-Weighting for each pixel on the temporary array
			//
			//   * - So VerticalLoop runs once for each line down the bitmap.
			//


		    //  "And now here's something we hope you'll really like......"
	
			mov eax, YContrib;      
			mov eax, [eax];
			sub eax, 12;
			mov ContribPtrY, eax;      // The YContrib (and XContrib) pointers point to arrays
			mov YCounter, 0;           // of scaling weight and Delta information for a row or
									   // column of the source bitmap
	
			ALIGN 16;
			VerticalLoop:              // This loop goes once for each "Y-Pixel" of the
									   // *destination* bitmap.
				mov ebx, XContrib;
				mov ebx, [ebx];
				sub ebx, 12;
				mov ContribPtrX, ebx;  // ContribPtrX and Y point to our weighting structures.
				add ContribPtrY, 12;
				mov edi, ContribPtrY;
				mov ecx, [edi];
				mov esi, [edi + 4];
				
			
				sub esi, ecx;
				inc esi;
				mov YDelta, esi;

				mov eax, SourceWidth;
				imul eax, ecx;
				add eax, pSrc;
				sub eax, 4;
				mov esi, eax;		   // esi is our starting point on any given line
				                       // We use it to hold the place inside the next loop.
				
				mov edi, [edi + 8];
				mov YWeightPtr, edi;   // Points to the pixel weights we will use for Y-Scaling
		
				mov eax, uSrcWidth;
				mov ecx, [RGBArray];
				mov ColumnCounter, eax;
				mov RGBArrPtr, ecx;
				
				ALIGN 16;
				ColumnLoop:			   // This loop goes for each X pixel of the Source bitmap. 
			                           // We calculate the weights of pixels vertically from esi  
				                       // to (esi + YDelta). Then later we scale this line of 
				                       // pixels. Horizintally to get our Destination pixel.
					mov ecx, YDelta;
					mov edi, YWeightPtr;
					add esi, 4;
					mov edx, esi;      // edx points to our source bitmap pixels.
				
					fldz;              // Load up some 0's in our ploat stack for the loop below.
					fldz;
					fldz;									
				

					ALIGN 16;
					YWeightingLoop:    // This loops once for each Pixel in the Y-direction 
					                   // (to YDelta). Just load up each pixel from ecx to 0, 
					                   // and multiply it by it's weighting (pointed to by edi).
	
						fld dword ptr[edi];               // Load up our weighting factor.

						movzx eax, byte ptr [edx];        // Get our RGB value into RVal, BVal, GVal 
						movzx ebx, byte ptr [edx + 1];

						mov BVal, eax;

						movzx eax, byte ptr [edx + 2];
						
						mov GVal, ebx;
						mov RVal, eax;	
						
						fild BVal;
						fmul st(0), st(1);
						fxch;
						add edx, SourceWidth;
						fild GVal;
						fmul st(0), st(1);
						fxch;
						add edi, 4;                      // Go to next weighting factor
						fild RVal;
						fmulp st(1), st(0);
						fxch st(2);                      // Now st(0) thru st(2) are B, G, R....


						faddp st(3), st(0);	
						faddp st(3), st(0);		
						faddp st(3), st(0);	
					
						
						dec ecx;                         // Our counter							
					
					jnz short YWeightingLoop;

				mov ecx, [RGBArrPtr];
									
				fstp dword ptr [ecx];       // B   Move our YScaled pixels into our 
				fstp dword ptr [ecx + 4];   // G   temporary array.
				fstp dword ptr [ecx + 8];   // R
			
				add RGBArrPtr, 12;
				dec ColumnCounter;
		
			jnz short ColumnLoop;			// Process the pixels for the next column...

											// Now we scale our "temporary bitmap" array and 
			                                // plot a line of pixels in our Destination bitmap.
			mov eax, uResWidth;
			mov XCounter, eax;

			mov edx, [Dest];
			mov eax, ContribPtrX;
			mov ContribTempPtr, eax;

			ALIGN 16;
			RowLoop:							 // This loop calculates each destination
												 // X pixel for one vertical line.

				add ContribTempPtr, 12; 
				mov eax, ContribTempPtr;         // Our array of X-weighting factors
				 
				mov ebx, [RGBArray];
				mov edi, eax;
				mov ecx, [eax];
				mov esi, [edi + 4];	
					
				sub esi, ecx;
				mov edi, [edi + 8];
				lea eax, [ecx * 8 + ebx];
				lea ebx, [eax + ecx * 4];
				inc esi;
				mov eax, 4;
		
				fldz;
				fldz;
				fldz;

				ALIGN 16;
				XWeightingLoop:
				
					fld dword ptr[edi];         // Load up weighting factor...
			
					fld dword ptr [ebx];
					fmul st(0), st(1);          // B
					fxch;
					add edi, eax;

					fld dword ptr [ebx + eax];
					fmul st(0), st(1);          // G
					add ebx, 12;
					fxch;

					fld dword ptr [ebx + 2 * eax - 12];
					fmulp st(1), st(0);         // R 
					fxch st(2);

					dec esi;

					faddp st(3), st(0);
					faddp st(3), st(0);		
					faddp st(3), st(0);	
									
				jnz short XWeightingLoop;  // Repeat until all pixels have been wieghted

										   //  Unload pixels into RGB ints and then make them
										   //  into the final RGB quad.

				fistp BVal;				   // Ouch!!!
				fistp GVal;
				fistp RVal;
							
				mov ebx, RVal;		       // We will make our pixel in the ebx register and
				rol ebx, 8;			       // then move it into the address pointed to by 
				or ebx, GVal;              // edx register.
					                 
				rol ebx,8;
				or ebx, BVal;
			
				mov dword ptr [edx], ebx;  // Place the value in Dest
										
				lea edx, [edx + 4];
				dec XCounter;


			jnz short RowLoop;		       // Continue across the array until you have 
								           // processed all horizintal pixels in this "line"
			mov Dest, edx;
			inc YCounter;
			mov eax, YCounter;
			cmp eax, uResHeight;

			
		jb VerticalLoop;			       // Jump down one line and repeat.....

	}

	delete[] RGBArray;

	FreeContributions (YContrib);          // Free contributions structure
	FreeContributions (XContrib);          // Free contributions structure
}

  
void CFast2PassScale::TransparentSuperScale (
			COLORREF           *pSrc, 
            int                uSrcWidth,
            int                uSrcHeight,
            COLORREF           *pDst, 
            int                uResWidth,
            int                uResHeight,
			COLORREF           TransparentColor)
{

    LineContribType * YContrib = CalcContributions (uResHeight, uSrcHeight, float(uResHeight) / float(uSrcHeight)); 
    LineContribType * XContrib = CalcContributions (uResWidth, uSrcWidth, float(uResWidth) / float(uSrcWidth)); 

    DWORD *ContribPtrX, *ContribTempPtr, *ContribPtrY;        // Compliments YContrib and XContrib above.

	DWORD *Dest = pDst;					     // Keeps track of where we are to put the 
											 // next pixels in the destination bitmap.

	float* YWeightPtr;                       // A pointer to our Y "Wieght" factors.
	float* RGBArray = new float[uSrcWidth * 3];  // Our "temporary" array of pixels 
	float* RGBArrPtr;

	int SourceWidth = 4 * uSrcWidth;         // This number tells us how many bytes there are 
	                                         // between 2 vertically adjacent pixels.

	DWORD BVal, GVal, RVal;					 // Temporary holders for our pixel components
	BVal = GVal = RVal = 0;


	DWORD YDelta, YCounter, XCounter, ColumnCounter; // Some counters

	_asm{

		
			// 
			// This is the transparent version of the SuperScale function.  See it's comment block
		    // for details of how SuperScale works.  This code is exactly the same except for a cmpxch
		    // instruction that plots the destination pixel if it is not equal to the "TransparentColor"
		    // variable passed into the function.
			//


			mov eax, YContrib;      
			mov eax, [eax];
			sub eax, 12;
			mov ContribPtrY, eax;      // The YContrib (and XContrib) pointers point to arrays
			mov YCounter, 0;           // of scaling weight and Delta information for a row or
									   // column of the source bitmap
	
			ALIGN 16;
			VerticalLoop:              // This loop goes once for each "Y-Pixel" of the
									   // *destination* bitmap.
				mov ebx, XContrib;
				mov ebx, [ebx];
				sub ebx, 12;
				mov ContribPtrX, ebx;  // ContribPtrX and Y point to our weighting structures.
				add ContribPtrY, 12;
				mov edi, ContribPtrY;
				mov ecx, [edi];
				mov esi, [edi + 4];
			
				sub esi, ecx;
				inc esi;
				mov YDelta, esi;

				mov eax, SourceWidth;
				imul eax, ecx;
				add eax, pSrc;
				sub eax, 4;
				mov esi, eax;		   // esi is our starting point on any given line
				                       // We use it to hold the place inside the next loop.
				
				mov edi, [edi + 8];
				mov YWeightPtr, edi;   // Points to the pixel weights we will use for Y-Scaling
		
				mov eax, uSrcWidth;
				mov ecx, [RGBArray];
				mov ColumnCounter, eax;
				mov RGBArrPtr, ecx;
				
				ALIGN 16;
				ColumnLoop:			   // This loop goes for each X pixel of the Source bitmap. 
			                           // We calculate the weights of pixels vertically from esi  
				                       // to (esi + YDelta). Then later we scale this line of 
				                       // pixels. Horizintally to get our Destination pixel.
					mov ecx, YDelta;
					mov edi, YWeightPtr;
					add esi, 4;
					mov edx, esi;      // edx points to our source bitmap pixels.
				
					fldz;              // Load up some 0's in our ploat stack for the loop below.
					fldz;
					fldz;									
				

					ALIGN 16;
					YWeightingLoop:    // This loops once for each Pixel in the Y-direction 
					                   // (to YDelta). Just load up each pixel from ecx to 0, 
					                   // and multiply it by it's weighting (pointed to by edi).
	
						fld dword ptr[edi];    // Load up our weighting factor.

						movzx eax, byte ptr [edx];        // Get our RGB value into RVal, BVal, GVal 
						movzx ebx, byte ptr [edx + 1];

						mov BVal, eax;

						movzx eax, byte ptr [edx + 2];
						
						mov GVal, ebx;
						mov RVal, eax;	
						
						fild BVal;
						fmul st(0), st(1);
						fxch;
						add edx, SourceWidth;
						fild GVal;
						fmul st(0), st(1);
						fxch;
						add edi, 4;            // Go to next weighting factor
						fild RVal;
						fmulp st(1), st(0);
						fxch st(2);            // Now st(0) thru st(2) are B, G, R....


						faddp st(3), st(0);	
						faddp st(3), st(0);		
						faddp st(3), st(0);	
					
						
						dec ecx;               // Our counter							
					
					jnz short YWeightingLoop;

				mov ecx, [RGBArrPtr];
									
				fstp dword ptr [ecx];          // B   Move our YScaled pixels into our 
				fstp dword ptr [ecx + 4];      // G   temporary array.
				fstp dword ptr [ecx + 8];      // R
			
				add RGBArrPtr, 12;
				dec ColumnCounter;
		
			jnz short ColumnLoop;			// Process the pixels for the next column...

											// Now we scale our temporary bitmap and 
			                                // plot a line of pixels in our Destination bitmap.
			mov eax, uResWidth;
			mov XCounter, eax;

			mov edx, [Dest];
			mov eax, ContribPtrX;
			mov ContribTempPtr, eax;

			ALIGN 16;
			RowLoop:						   // This loop calculates each destination
											   // X pixel for one vertical line.

				add ContribTempPtr, 12; 
				mov eax, ContribTempPtr;       // Our array of X-weighting factors
				 
				mov ebx, [RGBArray];
				mov edi, eax;
				mov ecx, [eax];
				mov esi, [edi + 4];	
					
				sub esi, ecx;
				mov edi, [edi + 8];
				lea eax, [ecx * 8 + ebx];
				lea ebx, [eax + ecx * 4];
				inc esi;
				mov eax, 4;
		
				fldz;
				fldz;
				fldz;

				ALIGN 16;
				XWeightingLoop:
				
					fld dword ptr[edi];         // Load up weighting factor...
			
					fld dword ptr [ebx];
					fmul st(0), st(1);          // B
					fxch;
					add edi, eax;

					fld dword ptr [ebx + eax];
					fmul st(0), st(1);          // G
					add ebx, 12;
					fxch;

					fld dword ptr [ebx + 2 * eax - 12];
					fmulp st(1), st(0);         // R 
					fxch st(2);

					dec esi;

					faddp st(3), st(0);
					faddp st(3), st(0);		
					faddp st(3), st(0);	
									
				jnz short XWeightingLoop;  // Repeat until all pixels have been wieghted

										   // Unload pixels into RGB ints and then make them
										   // into the final RGB quad.

				fistp BVal;				   // Ouch!!!
				fistp GVal;
				fistp RVal;
							
				mov ebx, RVal;		       // We will make our pixel in the ebx registers and
				rol ebx, 8;			       // then move it into the address pointed to by the
				or ebx, GVal;              // edx register.
					                 
				rol ebx,8;
				or ebx, BVal;
			
				mov eax, TransparentColor; 
				mov ecx, [edx];            // We place the destination pixel, if it is not
				cmpxchg  ebx, ecx;         // equal to the transparency color
			
				mov dword ptr [edx], ebx;  // Place the value in Dest
										
				lea edx, [edx + 4];
				dec XCounter;


			jnz short RowLoop;		       // Continue across the array until you have 
								           // processed all horizintal pixels in this "line"
			mov Dest, edx;
			inc YCounter;
			mov eax, YCounter;
			cmp eax, uResHeight;

			
		jb VerticalLoop;			       // Jump down one line and repeat.....

	}

	delete[] RGBArray;

	FreeContributions (YContrib);          // Free contributions structure
	FreeContributions (XContrib);          // Free contributions structure
}

void CFast2PassScale::SSESuperScale (
			COLORREF           *pSrc, 
            int                uSrcWidth,
            int                uSrcHeight,
            COLORREF           *pDst, 
            int                uResWidth,
            int                uResHeight)
{

    LineContribType * YContrib = CalcContributions (uResHeight, uSrcHeight, float(uResHeight) / float(uSrcHeight)); 
    LineContribType * XContrib = CalcContributions (uResWidth, uSrcWidth, float(uResWidth) / float(uSrcWidth)); 

	DWORD YDelta, YCounter, XCounter, ColumnCounter;    // Some counters
    DWORD *ContribPtrX, *ContribTempPtr, *ContribPtrY;  // Compliments YContrib and XContrib above.

	DWORD *Dest = pDst;					                // Keeps track of where we are to put the 
											            // next pixels in the destination bitmap.

	float* YWeightPtr;                                  // A pointer to our Y "Wieght" factors.


	int SourceWidth = 4 * uSrcWidth;                    // This number tells us how many bytes there are 
	                                                    // between 2 vertically adjacent pixels.

	__m128* SSETempStoreOrig = new __m128[uSrcWidth + 1];
	__m128* SSETempStore;
	_asm{

		mov eax, SSETempStoreOrig;   // This little trick makes sure that we are
		add eax, 16;                 // always loading up 128bit packed floats that
		shr eax, 4;                  // are 16-bit aligned.
		shl eax, 4;
		mov SSETempStore, eax; 

	}


	__m128* SSETempStorePtr;



	_asm{

			// This is the SSE optimized version of the CFast2PassScale scaling function.
		    // See the comment block that accompanies the SuperScale function for an
		    // explanation of how the scaling is actually done vs the original C++ code.
			//
		    // Also note that the temporary array size is [16 * uSrcWidth] bytes for this function
			//
	
			mov eax, YContrib;      
			mov eax, [eax];
			sub eax, 12;
			mov ContribPtrY, eax;      // The YContrib (and XContrib) pointers point to arrays
			mov YCounter, 0;           // of scaling weight and Delta information for a row or
									   // column of the source bitmap
	
			ALIGN 16;
			VerticalLoop:              // This loop goes once for each "Y-Pixel" of the
									   // *destination* bitmap.
				mov ebx, XContrib;
				mov ebx, [ebx];
				sub ebx, 12;
				mov ContribPtrX, ebx;  // ContribPtrX and Y point to our weighting structures.
				add ContribPtrY, 12;
				mov edi, ContribPtrY;
				mov ecx, [edi];
				mov esi, [edi + 4];
			
				sub esi, ecx;
				inc esi;
				mov YDelta, esi;

				mov eax, SourceWidth;
				imul eax, ecx;
				add eax, pSrc;
				

				sub eax, 4;
				mov esi, eax;		   // esi is our starting point on any given line
				                       // We use it to hold the place inside the next loop.
				
				mov edi, [edi + 8];
				mov YWeightPtr, edi;   // Points to the pixel weights we will use for Y-Scaling
		
				mov eax, uSrcWidth;
				mov ecx, [SSETempStore];
				mov ColumnCounter, eax;
				mov SSETempStorePtr, ecx;
				
				ALIGN 16;
				ColumnLoop:			   // This loop goes for each X pixel of the Source bitmap. 
			                           // We calculate the weights of pixels vertically from esi  
				                       // to (esi + YDelta). Then later we scale this line of 
				                       // pixels. Horizintally to get our Destination pixel.

					xorps xmm2, xmm2;  // Zero out our XMM accumulator.
					pxor mm0, mm0;     // Zero out an MMX register

					mov eax, 4;
					mov ebx, SourceWidth;

					mov ecx, YDelta;
					mov edi, YWeightPtr;
					add esi, eax;
					mov edx, esi;      // edx points to our source bitmap pixels.
				
			
					ALIGN 16;
					YWeightingLoop:    // This loops once for each Pixel in the Y-direction 
					                   // (to YDelta). Just load up each pixel from ecx to 0, 
					                   // and multiply it by it's weighting (pointed to by edi).
	
						movss xmm1, [edi];    // Load up our Weighting multiplier
						movd mm1, [edx];      // load up the pixel values into an MMX register
						punpcklbw mm1, mm0;   // and begin unpacking them into 2 MMX regiters
						
						shufps xmm1, xmm1, 0; // Copy the multiplier into all 4 DWORDS
						movq mm2, mm1;
						punpcklbw mm1, mm0;
						punpckhbw mm2, mm0;
					
						cvtpi2ps xmm0, mm2;   // now load up the mmx regiters onto the
						movlhps xmm0, xmm0;   // SSE regiters and multiply by the weighting
						add edx, ebx;
						cvtpi2ps xmm0, mm1;
						
						
					    add edi, eax;         // Go to next weighting factor
						mulps xmm0, xmm1;
						
						dec ecx;              // Our counter		
						addps xmm2, xmm0;     // Add to our accumulator
			
					jnz short YWeightingLoop;

					mov ecx, [SSETempStorePtr];
					movaps [ecx], xmm2;       // Move our scaled value into a temporary array


				add SSETempStorePtr, 16;
				dec ColumnCounter;
		
			jnz short ColumnLoop;			// Process the pixels for the next column...

											// Now we scale our temporary bitmap and 
			                                // plot a line of pixels in our Destination bitmap.
			mov eax, uResWidth;
			mov XCounter, eax;

			mov edx, [Dest];
			mov eax, ContribPtrX;
			mov ContribTempPtr, eax;

			ALIGN 16;
			RowLoop:					    // This loop calculates each destination
											// X pixel for one vertical line.

				add ContribTempPtr, 12; 
				mov eax, ContribTempPtr;    // Our array of X-weighting factors
				 
				mov ebx, [SSETempStore];
				mov edi, eax;
				mov ecx, [eax];
				mov esi, [edi + 4];		

				xorps xmm2, xmm2;           // Zero out our accumulator

				sub esi, ecx;
				mov edi, [edi + 8];
				lea eax, [ecx * 8 + ebx];
				inc esi;
				lea ebx, [eax + ecx * 8];
					

				ALIGN 16;
				XWeightingLoop:

				    movss xmm1, [edi];      // Load up our weights and 128bit packed
					movaps xmm0, [ebx];     // floating point pixels.  Multiply and add...
					
					shufps xmm1, xmm1, 0;
					add edi, 4;
					
				    add ebx, 16;
					dec esi;
					mulps xmm0, xmm1;
					addps xmm2, xmm0;

				jnz short XWeightingLoop;   // Repeat until all pixels have been wieghted

										    //  Unload pixels into RGB ints and then make them
										    //  into the final RGB quad.

				CVTPS2PI  mm0, xmm2;
				movhlps xmm2, xmm2;
				CVTPS2PI  mm1, xmm2;

				movd ebx, mm0;
				movd eax, mm1;
				psrlq mm0, 24;
				shl eax, 16;
				or ebx, eax;
				movd eax, mm0;
				or ebx, eax;

			
				mov dword ptr [edx], ebx;   // Place the value in Dest
										
				lea edx, [edx + 4];
				dec XCounter;


			jnz short RowLoop;		        // Continue across the array until you have 
								            // processed all horizintal pixels in this "line"
			mov Dest, edx;
			inc YCounter;
			mov eax, YCounter;
			cmp eax, uResHeight;

			
		jb VerticalLoop;			        // Jump down one line and repeat.....

		emms;                               // Get out of MMX mode

	}
	delete[] SSETempStoreOrig;

	FreeContributions (YContrib);           // Free contributions structure
	FreeContributions (XContrib);           // Free contributions structure
}

void CFast2PassScale::SSETransparentSuperScale (
			COLORREF           *pSrc, 
            int                uSrcWidth,
            int                uSrcHeight,
            COLORREF           *pDst, 
            int                uResWidth,
            int                uResHeight,
			COLORREF           TransparentColor)
{

    LineContribType * YContrib = CalcContributions (uResHeight, uSrcHeight, float(uResHeight) / float(uSrcHeight)); 
    LineContribType * XContrib = CalcContributions (uResWidth, uSrcWidth, float(uResWidth) / float(uSrcWidth)); 

	DWORD YDelta, YCounter, XCounter, ColumnCounter;          // Some counters
    DWORD *ContribPtrX, *ContribTempPtr, *ContribPtrY;        // Compliments YContrib and XContrib above.

	DWORD *Dest = pDst;					     // Keeps track of where we are to put the 
											 // next pixels in the destination bitmap.

	float* YWeightPtr;                       // A pointer to our Y "Wieght" factors.


	int SourceWidth = 4 * uSrcWidth;         // This number tells us how many bytes there are 
	                                         // between 2 vertically adjacent pixels.

	__m128* SSETempStoreOrig = new __m128[uSrcWidth + 1];
	__m128* SSETempStore;
	_asm{

		mov eax, SSETempStoreOrig;   // This little trick makes sure that we are
		add eax, 16;                 // always loading up 128bit packed floats that
		shr eax, 4;                  // are 16-bit aligned.
		shl eax, 4;
		mov SSETempStore, eax; 

	}


	__m128* SSETempStorePtr;



	_asm{

		
			// This is the SSE optimized version of the CFast2PassScale scaling function.
		    // See the comment block that accompanies the SuperScale function for an
		    // explanation of how the scaling is actually done vs the original C++ code.
			//
			// This function is just like SSESuperScale except that it scales transparently onto
			// the destination bitmap.
		    //
	
			mov eax, YContrib;      
			mov eax, [eax];
			sub eax, 12;
			mov ContribPtrY, eax;      // The YContrib (and XContrib) pointers point to arrays
			mov YCounter, 0;           // of scaling weight and Delta information for a row or
									   // column of the source bitmap
	
			ALIGN 16;
			VerticalLoop:              // This loop goes once for each "Y-Pixel" of the
									   // *destination* bitmap.
				mov ebx, XContrib;
				mov ebx, [ebx];
				sub ebx, 12;
				mov ContribPtrX, ebx;  // ContribPtrX and Y point to our weighting structures.
				add ContribPtrY, 12;
				mov edi, ContribPtrY;
				mov ecx, [edi];
				mov esi, [edi + 4];
			
				sub esi, ecx;
				inc esi;
				mov YDelta, esi;

				mov eax, SourceWidth;
				imul eax, ecx;
				add eax, pSrc;
				

				sub eax, 4;
				mov esi, eax;		   // esi is our starting point on any given line
				                       // We use it to hold the place inside the next loop.
				
				mov edi, [edi + 8];
				mov YWeightPtr, edi;   // Points to the pixel weights we will use for Y-Scaling
		
				mov eax, uSrcWidth;
				mov ecx, [SSETempStore];
				mov ColumnCounter, eax;
				mov SSETempStorePtr, ecx;
				
				ALIGN 16;
				ColumnLoop:			   // This loop goes for each X pixel of the Source bitmap. 
			                           // We calculate the weights of pixels vertically from esi  
				                       // to (esi + YDelta). Then later we scale this line of 
				                       // pixels. Horizintally to get our Destination pixel.

					xorps xmm2, xmm2;  // Zero out our XMM accumulator.
					pxor mm0, mm0;     // Zero out an MMX register

					mov eax, 4;
					mov ebx, SourceWidth;

					mov ecx, YDelta;
					mov edi, YWeightPtr;
					add esi, eax;
					mov edx, esi;      // edx points to our source bitmap pixels.
				
			
					ALIGN 16;
					YWeightingLoop:    // This loops once for each Pixel in the Y-direction 
					                   // (to YDelta). Just load up each pixel from ecx to 0, 
					                   // and multiply it by it's weighting (pointed to by edi).
	
						movss xmm1, [edi];    // Load up our Weighting multiplier
						movd mm1, [edx];      // load up the pixel values into an MMX register
						punpcklbw mm1, mm0;   // and begin unpacking them into 2 MMX regiters
						
						shufps xmm1, xmm1, 0; // Copy the multiplier into all 4 DWORDS
						movq mm2, mm1;
						punpcklbw mm1, mm0;
						punpckhbw mm2, mm0;
					
						cvtpi2ps xmm0, mm2;   // now load up the mmx regiters onto the
						movlhps xmm0, xmm0;   // SSE regiters and multiply by the weighting
						add edx, ebx;
						cvtpi2ps xmm0, mm1;
						
						
					    add edi, eax;         // Go to next weighting factor
						mulps xmm0, xmm1;
						
						dec ecx;              // Our counter		
						addps xmm2, xmm0;     // Add to our accumulator
			
					jnz short YWeightingLoop;

					mov ecx, [SSETempStorePtr];
					movaps [ecx], xmm2;       // Move our scaled value into a temporary array


				add SSETempStorePtr, 16;
				dec ColumnCounter;
		
			jnz short ColumnLoop;			  // Process the pixels for the next column...
 
										      // Now we scale our temporary bitmap and 
			                                  // plot a line of pixels in our Destination bitmap.
			mov eax, uResWidth;
			mov XCounter, eax;

			mov edx, [Dest];
			mov eax, ContribPtrX;
			mov ContribTempPtr, eax;

			ALIGN 16;
			RowLoop:						  // This loop calculates each destination
											  // X pixel for one vertical line.

				add ContribTempPtr, 12; 
				mov eax, ContribTempPtr;      // Our array of X-weighting factors
				 
				mov ebx, [SSETempStore];
				mov edi, eax;
				mov ecx, [eax];
				mov esi, [edi + 4];		

				xorps xmm2, xmm2;             // Zero out our accumulator

				sub esi, ecx;
				mov edi, [edi + 8];
				lea eax, [ecx * 8 + ebx];
				inc esi;
				lea ebx, [eax + ecx * 8];
					

				ALIGN 16;
				XWeightingLoop:

				    movss xmm1, [edi];       // Load up our weights and 128bit packed
					movaps xmm0, [ebx];      // floating point pixels.  Multiply and add...
					
					shufps xmm1, xmm1, 0;
					add edi, 4;
					
				    add ebx, 16;
					dec esi;
					mulps xmm0, xmm1;
					addps xmm2, xmm0;

				jnz short XWeightingLoop;  // Repeat until all pixels have been wieghted

										   //  Unload pixels into RGB ints and then make them
										   //  into the final RGB quad.

				CVTPS2PI  mm0, xmm2;
				movhlps xmm2, xmm2;
				CVTPS2PI  mm1, xmm2;

				movd ebx, mm0;
				movd eax, mm1;
				psrlq mm0, 24;
				shl eax, 16;
				or ebx, eax;
				movd eax, mm0;
				or ebx, eax;

				mov eax, TransparentColor; 
				mov ecx, [edx];            // We place the destination pixel, if it is not
				cmpxchg  ebx, ecx;         // equal to the transparency color
			
				mov dword ptr [edx], ebx;  // Place the value in Dest
										
				lea edx, [edx + 4];
				dec XCounter;


			jnz short RowLoop;		       // Continue across the array until you have 
								           // processed all horizintal pixels in this "line"
			mov Dest, edx;
			inc YCounter;
			mov eax, YCounter;
			cmp eax, uResHeight;

			
		jb VerticalLoop;			       // Jump down one line and repeat.....

		emms;                              // Get out of MMX mode

	}
	delete[] SSETempStoreOrig;

	FreeContributions (YContrib);          // Free contributions structure
	FreeContributions (XContrib);          // Free contributions structure
}


COLORREF * CFast2PassScale::AllocAndScale ( 
    COLORREF   *pOrigImage, 
    int        uOrigWidth, 
    int        uOrigHeight, 
    int        uNewWidth, 
    int        uNewHeight)
{ 
    COLORREF *pRes = new COLORREF [uNewWidth * uNewHeight];

    if (uOrigWidth != uNewWidth || uOrigHeight != uNewHeight)
	{
		SuperScale ( pOrigImage, uOrigWidth, uOrigHeight, pRes, uNewWidth, uNewHeight);
		//SSESuperScale ( pOrigImage, uOrigWidth, uOrigHeight, pRes, uNewWidth, uNewHeight);
	}


	else if (uOrigWidth == uNewWidth && uOrigHeight == uNewHeight)
		memcpy (pRes, pOrigImage, sizeof (COLORREF) * uOrigWidth * uOrigHeight);

   
    return pRes;
} 

 
COLORREF * CFast2PassScale:: Scale ( 
    COLORREF   *pOrigImage, 
    int        uOrigWidth, 
    int        uOrigHeight, 
    COLORREF   *pDstImage, 
    int        uNewWidth, 
    int        uNewHeight)
{ 

	// Make sure we have legal values...

	assert(uOrigWidth  > 0);
	assert(uOrigHeight > 0);
	assert(uNewWidth   > 0);
	assert(uNewHeight  > 0);
	assert(pOrigImage  != NULL);
	assert(pDstImage   != NULL);


	// There are two possibilities here.  If either the NewX and NewY sizes are
	// different from the originals, then call the SuperScale Algorythm.
    // If neither X or Y scaling is required, just memcopy from source to destination.


	if (uOrigWidth != uNewWidth || uOrigHeight != uNewHeight)
	{
		SuperScale ( pOrigImage, uOrigWidth, uOrigHeight, pDstImage, uNewWidth, uNewHeight);
		//SSESuperScale ( pOrigImage, uOrigWidth, uOrigHeight, pDstImage, uNewWidth, uNewHeight);
	}


	else if (uOrigWidth == uNewWidth && uOrigHeight == uNewHeight)
		memcpy (pDstImage, pOrigImage, sizeof (COLORREF) * uOrigWidth * uOrigHeight);


    return pDstImage;
} 


COLORREF * CFast2PassScale:: TransparentScale ( 
    COLORREF   *pOrigImage, 
    int        uOrigWidth, 
    int        uOrigHeight, 
    COLORREF   *pDstImage, 
    int        uNewWidth, 
    int        uNewHeight,
	COLORREF   TransparentColor)
{ 

    // Make sure we have legal values...

	assert(uOrigWidth  > 0);
	assert(uOrigHeight > 0);
	assert(uNewWidth   > 0);
	assert(uNewHeight  > 0);
	assert(pOrigImage  != NULL);
	assert(pDstImage   != NULL);

	// There are two possibilities here.  If either the NewX and NewY sizes are
	// different from the originals, then call the SuperScale Algorythm.
    // If neither X or Y scaling is required, just memcopy from source to destination.


	COLORREF TColor = RGB(GetBValue(TransparentColor), GetGValue(TransparentColor) , GetRValue(TransparentColor));

	if (uOrigWidth != uNewWidth || uOrigHeight != uNewHeight)
	{
		TransparentSuperScale ( pOrigImage, uOrigWidth, uOrigHeight, pDstImage, uNewWidth, uNewHeight, TColor);
	//	SSETransparentSuperScale ( pOrigImage, uOrigWidth, uOrigHeight, pDstImage, uNewWidth, uNewHeight, TColor);
	}


	else if (uOrigWidth == uNewWidth && uOrigHeight == uNewHeight)
		memcpy (pDstImage, pOrigImage, sizeof (COLORREF) * uOrigWidth * uOrigHeight);


    return pDstImage;
} 
