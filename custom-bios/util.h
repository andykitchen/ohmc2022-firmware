#ifndef ROCKLING_UTIL_H_
#define ROCKLING_UTIL_H_

#define NOINLINE __attribute__ ((noinline))

extern volatile int debug_status;

inline int BYTE0(int x) { return (x & 0x00ff); }
inline int BYTE1(int x) { return (x & 0xff00) >> 8; }

#endif
