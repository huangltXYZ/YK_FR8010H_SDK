#ifndef __COMMON_H
#define __COMMON_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

//#include "cmsis_armcc.h"

typedef unsigned char                   BOOL;
typedef char                             CHAR;
typedef signed char                     S8;
typedef unsigned char                   U8;
typedef signed short                    S16;
typedef unsigned short                  U16;
typedef signed int                      S32;
typedef unsigned int                    U32;
typedef signed long long int           S64;
typedef unsigned long long int         U64;

typedef float                          F32;
typedef double                         F64;



#ifndef  PACKED_STRUCT
	#define PACKED_STRUCT __packed typedef struct
#endif



#ifndef  _false
	#define  _false	0
#endif

#ifndef  _true
	#define  _true	1
#endif

#ifndef NULL
	#define NULL 	 (void *)0
#endif

#ifndef _INLINE_
	#define _INLINE_ __inline
#endif

#ifndef PNULL
	#define PNULL (void *)0
#endif

#ifndef TRUE
  #define TRUE     true
#endif

#ifndef FALSE
  #define FALSE    false
#endif

#define INFINITE 0xFFFFFFFF

//#ifndef MAX
//	#define MAX(a, b)    ((a) > (b) ? (a) : (b))
//#endif
//
//#ifndef MIN
//	#define MIN(a, b)    ((a) > (b) ? (b) : (a))
//#endif

#define SHIELD_WARNING(X) (void)X 

#ifndef SET_BIT
	#define SET_BIT(REG, BIT)     ((REG) |= (BIT))
#endif

#ifndef CLEAR_BIT
	#define CLEAR_BIT(REG, BIT)   ((REG) &= ~(BIT))
#endif

#ifndef READ_BIT
	#define READ_BIT(REG, BIT)    ((REG) & (BIT))
#endif

#ifndef CLEAR_REG
	#define CLEAR_REG(REG)        ((REG) = (0x0))
#endif

#ifndef WRITE_REG
	#define WRITE_REG(REG, VAL)   ((REG) = (VAL))
#endif

#ifndef READ_REG
	#define READ_REG(REG)         ((REG))
#endif


#ifndef ENABLE_IRQ
	#define ENABLE_IRQ()         __enable_irq()
#endif

#ifndef DISABLE_IRQ
	#define DISABLE_IRQ()        __disable_irq()
#endif

#ifndef NOP
	#define NOP()                  __nop()//__NOP()
#endif

#ifndef SYSTEM_RESET
	#define SYSTEM_RESET()       platform_reset_patch(0)//NVIC_SystemReset()
#endif

#define ALIGN_512B(Addr)    ((Addr + 0x200 - 1) & ~(0x200 - 1))
#define ALIGN_4K(Addr)      ((Addr + 0x1000 - 1) & ~(0x1000 - 1))


/*
 * \brief   Typecast the macro argument into milliseconds
 */
#define TmrMilliseconds( n )    ( (U32) (n) * 1 )
   
   
/*
 * \brief   Converts the macro argument from seconds to Milliseconds
 */
#define TmrSecondsToMilliseconds( n )   ( (U32) ((n) * 1000) )

/*
 * \brief   Converts the macro argument from microseconds to seconds
 */
#define TmrMicrosecondsToSeconds( n )   ( ((n) + 500000) / 1000000 )

/*
 * \brief   Converts the macro argument (i.e. seconds) into milliseconds
 */
#define TmrSeconds( n )         ( (U32) (TmrMilliseconds(n) * 1000) )

/*
 * \brief   Converts the macro argument (i.e. minutes) into milliseconds
 */
#define TmrMinutes( n )         ( (U32) (TmrSeconds(n) * 60) )

/* Compute the number of elements of an array */
#define NumberOfElements(x) (sizeof(x)/sizeof((x)[0]))

/* Compute the size of a string initialized with quotation marks */
#define SizeOfString(string) (sizeof(string) - 1)

#define GetRelAddr(strct, member) ((uint32_t)&(((strct*)(void *)0)->member))
#define GetSizeOfMember(strct, member) sizeof(((strct*)(void *)0)->member)

#define BigtoLittle16(A)              (( ((U16)(A) & 0xff00) >> 8)| (( (U16)(A) & 0x00ff) << 8))  

#define BigtoLittle32(A)              ((( (U32)(A) & 0xff000000) >> 24) | \
                                       (( (U32)(A) & 0x00ff0000) >> 8)   | \
                                       (( (U32)(A) & 0x0000ff00) << 8)   | \
                                       (( (U32)(A) & 0x000000ff) << 24))  
 

/**@brief Macro for performing integer division, making sure the result is rounded up.
 *
 * @details One typical use for this is to compute the number of objects with size B is needed to
 *          hold A number of bytes.
 *
 * @param[in]   A   Numerator.
 * @param[in]   B   Denominator.
 *
 * @return      Integer result of dividing A by B, rounded up.
 */
#define CEIL_DIV(A, B)      \
    (((A) + (B) - 1) / (B))
	
	

extern const unsigned int CRC32_Table[256];

extern void Delay_us(U32 ulCnt);
extern void Delay_ms(U32 ulCnt);

extern inline U16 co_read16p(void const *ptr16);

extern inline U32 co_read32p(void const *ptr32);

U32 Get_CRC32(U32 crc, void* input, U32 len);

void ble_delay_100us(U16 us_100);



#endif
