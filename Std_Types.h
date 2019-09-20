#ifndef INCLUDES_STD_TYPES_H_
#define INCLUDES_STD_TYPES_H_

typedef unsigned char           bool;
typedef unsigned char           uint8_t;          /*           0 .. 255             */
typedef unsigned short          uint16_t;         /*           0 .. 65535           */
typedef unsigned long           uint32_t;         /*           0 .. 4294967295      */
typedef signed char             sint8_t;          /*        -128 .. +127            */
typedef signed short            sint16_t;         /*      -32768 .. +32767          */
typedef signed long             sint32_t;         /* -2147483648 .. +2147483647     */
typedef signed long long        sint64_t;
typedef unsigned long long      uint64_t;         /*       0..18446744073709551615  */
typedef float                   float32_t;
typedef double                  float64_t;


typedef uint8_t           Std_ReturnType;
#define E_OK			((Std_ReturnType)0x00u)
#define E_NOT_OK		((Std_ReturnType)0x01u)
#define STD_ON       	0x01u
#define STD_OFF       	0x00u


#endif
