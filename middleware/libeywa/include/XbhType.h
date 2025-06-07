#ifndef XBH_TYPE_H
#define XBH_TYPE_H

typedef void                XBH_VOID;
typedef signed char         XBH_S8;
typedef char                XBH_CHAR;
typedef signed short        XBH_S16;
typedef signed int          XBH_S32;
typedef signed long         XBH_LONG;
typedef float               XBH_FLOAT;
typedef double              XBH_DOUBLE;
typedef long long           XBH_S64;

typedef unsigned char       XBH_BOOL;
typedef unsigned char       XBH_U8;
typedef unsigned char       XBH_UCHAR;
typedef unsigned short      XBH_U16;
typedef unsigned int        XBH_U32;
typedef unsigned long       XBH_ULONG;
typedef unsigned long long  XBH_U64;

#define XBH_SUCCESS         0
#define XBH_FAILURE         (-1)
/*
 *部分IIC设备在进行大量数据读写的时候，如果在过程中这一组IIC进行了其他芯片的读写，可能会影响此设备的操作
 * 因此需要设置一个状态，在lock状态下其他的IIC操作需要等待此操作完成
 */
#define XBH_LOCKSTATUS      1


#define XBH_FALSE           0
#define XBH_TRUE            1

#define XBH_NULL          0L
#define XBH_NULL_PTR      0L

#ifndef XBH_UNUSED
#define XBH_UNUSED(x)     ((void)x)
#endif


#endif //XBH_TYPE_H
