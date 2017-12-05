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

#ifndef __AMTYPES_H__
#define __AMTYPES_H__

//Platforms: __WIN32__, _WINCE_, _LINUX_, _LINUX_ANDROID_
//32/64 bits

typedef void				MVoid;
typedef unsigned long		MPtrVar;	//a type that indicates the length of a pointer

typedef signed char			MInt8;
typedef unsigned char		MUInt8;
typedef signed short		MInt16;
typedef unsigned short		MUInt16;
typedef signed int			MInt32;
typedef unsigned int		MUInt32;

#if defined(_MSC_VER) || defined(WIN32)	//Visual C++ default definitions
typedef signed		__int64		MInt64;
typedef unsigned	__int64		MUInt64;
#else
typedef signed		long long	MInt64;
typedef unsigned	long long	MUInt64;
#endif

typedef float					MFloat;
typedef double					MDouble;

#if	defined(WINCE) || defined(WIN32)

	#ifndef _WCHAR_T_DEFINED
	typedef MUInt16 wchar_t;
	#define _WCHAR_T_DEFINED
	#endif//#ifndef _WCHAR_T_DEFINED

#elif (defined(EKA2) && defined(__GCCE__))

	#ifndef _STDDEF_H_
	#ifndef __cplusplus
	typedef MUInt16 wchar_t;
	#define __wchar_t_defined
	#endif
	#endif

#elif defined(__GCCE__) || defined(__GCC32__)

	#ifndef _STDDEF_H_
	typedef MUInt16 wchar_t;
	#endif

#endif//#if	defined(WINCE)


#if	defined(__GCC32__)  || defined(__GCCE__) || defined(WINCE) || defined(WIN32) || defined(__WIN32__)
typedef wchar_t			MWChar;
#else
typedef MInt16			MWChar;
#endif

typedef char			MChar;

#endif
