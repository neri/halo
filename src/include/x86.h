



typedef	struct _i386_context_t {
    /* 00 */ uint32_t	cr2;
    /* 04 */ union { uint32_t edi; uint16_t di; uint8_t dil; };
    /* 08 */ union { uint32_t esi; uint16_t si; uint8_t sil; };
    /* 0C */ union { uint32_t ebp; uint16_t bp; uint8_t bpl; };
    /* 10 */ uint32_t	_DUMMY_esp;
    /* 14 */ union { uint32_t ebx; uint16_t bx; struct { uint8_t bl, bh; }; };
    /* 18 */ union { uint32_t edx; uint16_t dx; struct { uint8_t dl, dh; }; };
    /* 1C */ union { uint32_t ecx; uint16_t cx; struct { uint8_t cl, ch; }; };
    /* 20 */ union { uint32_t eax; uint16_t ax; struct { uint8_t al, ah; }; };
    /* 24 */ uint32_t	gs;
    /* 28 */ uint32_t	fs;
    /* 2C */ uint32_t	es;
    /* 30 */ uint32_t	ds;
    /* 34 */ uint32_t	intnum;
    /* 38 */ uint32_t	err;
    /* 3C */ uint32_t	eip;
    /* 40 */ uint32_t	cs;
    /* 44 */ uint32_t	eflags;
    /* USER MODE */
    /* 48 */ uint32_t	esp;
    /* 4C */ uint32_t	ss;
    /* VIRTUAL 8086 MODE */
    /* 50 */ uint32_t	vmes;
    /* 54 */ uint32_t	vmds;
    /* 58 */ uint32_t	vmfs;
    /* 5C */ uint32_t	vmgs;
} i386_context_t;


#define	EFLAGS_CF						0x00000001
#define	EFLAGS_PF						0x00000004
#define	EFLAGS_AF						0x00000010
#define	EFLAGS_ZF						0x00000040
#define	EFLAGS_SF						0x00000080
#define	EFLAGS_TF						0x00000100
#define	EFLAGS_IF						0x00000200
#define	EFLAGS_DF						0x00000400
#define	EFLAGS_OF						0x00000800
#define	EFLAGS_IOPLMASK					0x00003000
#define	EFLAGS_IOPL3					EFLAGS_IOPLMASK
#define	EFLAGS_NT						0x00004000
#define	EFLAGS_RF						0x00010000
#define	EFLAGS_VM						0x00020000
#define	EFLAGS_AC						0x00040000
#define	EFLAGS_VIF						0x00080000
#define	EFLAGS_VIP						0x00100000
#define	EFLAGS_ID						0x00200000

#define	ERROR_EXT						0x0001
#define	ERROR_IDT						0x0002
#define	ERROR_LDT						0x0004

#define	PTE_PRESENT					0x0000000000000001
#define	PTE_WRITE					0x0000000000000002
#define	PTE_USER					0x0000000000000004
#define	PTE_NOCACHE					0x0000000000000000
#define	PTE_ACCESS					0x0000000000000020
#define	PTE_DIRTY					0x0000000000000040
#define	PTE_LARGE2M					0x0000000000000080		// PAE
#define	PTE_GLOBAL					0x0000000000000100		// PGE
#define	PTE_AVL						0x0000000000000E00
#define	PTE_NOT_EXECUTE				0x8000000000000000LL	// NXE

#define	PTE_AVL_NOT_RESERVED		0x0000000000000000
#define	PTE_AVL_RESERVED			0x0000000000000200
