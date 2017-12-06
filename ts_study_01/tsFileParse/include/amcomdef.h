/*----------------------------------------------------------------------------------------------
*
* This file is Arcvideo's property. It contains Arcvideo's trade secret, proprietary and 		
* confidential information. 
* 
* The information and code contained in this file is only for authorized Arcvideo employees 
* to design, create, modify, or review.
* 
* DO NOT DISTRIBUTE, DO NOT DUPLICATE OR TRANSMIT IN ANY FORM WITHOUT PROPER AUTHORIZATION.
* 
* If you are not an intended recipient of this file, you must not copy, distribute, modify, 
* or take any action in reliance on it. 
* 
* If you have received this file in error, please immediately notify Arcvideo and 
* permanently delete the original and any copy of any file and any printout thereof.
*
*-------------------------------------------------------------------------------------------------*/

#ifndef __AMCOMDEF_H__
#define __AMCOMDEF_H__

#include "amtypes.h"

typedef MInt32					MLong;	//As the size of long is various depending on compilers, we'd better use fixed-size integer type instead
typedef MUInt8					MByte;
typedef MUInt8*					MPByte;
typedef MUInt16					MWord;
typedef MUInt32					MDWord;
typedef MVoid*					MHandle;
typedef MInt32					MBool;
typedef MVoid*					MPVoid;
typedef MChar*					MPChar;
typedef MInt16					MShort;
typedef const MChar*			MPCChar;
typedef	MInt32					MRESULT;
typedef MDWord					MCOLORREF; 

typedef struct __tag_rect
{
	MInt32 left;
	MInt32 top;
	MInt32 right;
	MInt32 bottom;
} MRECT, *PMRECT;

typedef struct __tag_point
{ 
	MInt32 x; 
	MInt32 y; 
} MPOINT, *PMPOINT;


#define MNull		0
#define MFalse		0
#define MTrue		1

#ifndef MAX_PATH

#ifdef WINCE
#define MAX_PATH	260
#else
#define MAX_PATH	256
#endif

#endif

#ifdef M_WIDE_CHAR
#define MTChar MWChar
#else 
#define MTChar MChar
#endif

#endif
