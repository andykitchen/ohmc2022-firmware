#ifndef ROCKLING_UTIL_H_
#define ROCKLING_UTIL_H_

#define NOINLINE       __attribute__ ((noinline))
#define ALWAYS_INLINE  __attribute__ ((always_inline))

#ifndef NULL
#define NULL 0
#endif

extern volatile int debug_status;

inline int LO(int x) { return (x & 0x00ff); }
inline int HI(int x) { return (x & 0xff00) >> 8; }

#endif
