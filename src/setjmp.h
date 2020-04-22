#ifndef _SETJMP_H_
#define _SETJMP_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef long long jmp_buf[16];

#define setjmp _setjmp
#define longjmp _longjmp

int _setjmp(jmp_buf);
void _longjmp(jmp_buf, int);

#ifdef __cplusplus
}
#endif

#endif
