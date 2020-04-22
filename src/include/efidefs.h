#ifndef INCLUDED_EFIDEFS_H
#define INCLUDED_EFIDEFS_H

#include <stdint.h>

typedef uint16_t		wchar_t;

#define	EFIAPI
#define OUT
#define IN
#define OPTIONAL
#define CONST			const

typedef	void			VOID;
typedef	intptr_t		INTN;
typedef uintptr_t		UINTN;
typedef	int64_t			INT64;
typedef uint64_t		UINT64;
typedef int32_t			INT32;
typedef uint32_t		UINT32;
typedef int16_t			INT16;
typedef uint16_t		UINT16;
typedef int8_t			INT8;
typedef uint8_t			UINT8;

typedef INTN			EFI_STATUS;
typedef wchar_t			CHAR16;

typedef UINTN			EFI_TPL;
typedef VOID			*EFI_EVENT;


typedef UINT8			BOOLEAN;
#define	TRUE			1
#define FALSE			0

typedef struct _EFI_GUID {
	UINT32	Data1;
	UINT16	Data2;
	UINT16	Data3;
	UINT8	Data4[8];
} EFI_GUID;

#if defined(__x86_64__) || defined(__aarch64__)
#define EFIERR(a)		(0x8000000000000000 | a)
#elif defined(__i386__) || defined(__arm__)
#define EFIERR(a)		(0x80000000 | a)
#endif

#endif
