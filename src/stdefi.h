
#include <efi.h>
#include <efiprot.h>
#include <wchar.h>
#include <stdint.h>
#include <string.h>
#include "efiadd.h"

#ifndef INCLUDED_STDEFI_H
#define INCLUDED_STDEFI_H


extern EFI_SYSTEM_TABLE* gST;
extern EFI_BOOT_SERVICES* gBS;
extern EFI_RUNTIME_SERVICES* gRT;


void efi_puts(const char* s);
void efi_puts_err(const char* s);

#endif
