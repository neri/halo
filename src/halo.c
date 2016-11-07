/*

    HALO: HAribote-os LOader for EFI

    Copyright (c) 2016 Nerry, All rights reserved.

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.

*/

#include "stdefi.h"
#include "acpi.h"
#include "x86.h"

// HARIBOTE OS MEMORY MAP
// 00000000-000FFFFF RESERVED
// 00100000-00267FFF floppy image
// 00268000-0026F7FF FREE (30KB)
// 0026F800-0026FFFF IDT
// 00270000-0027FFFF GDT
// 00280000-002FFFFF bootpack.hrb
// 00300000-003FFFFF stack
// 00400000-         FREE

#define MIN_ALLOC   256*3
#define BOOTPACK    0x00280000
#define DISKCACHE   0x00100000

#define HARI_KERNEL_CS  0x10
#define HARI_KERNEL_SS  0x08
#define HARI_KERNEL_DS  HARI_KERNEL_SS
#define HARI_LOADER_CS  0x18
#define HARI_KERNEL_EIP 0x0000001B


EFI_SYSTEM_TABLE* gST;
EFI_BOOT_SERVICES* gBS;
EFI_RUNTIME_SERVICES* gRT;

static EFI_GUID EfiConsoleControlProtocol = EFI_CONSOLE_CONTROL_PROTOCOL_GUID;
static EFI_GUID EfiEdidDiscoveredProtocol = EFI_EDID_DISCOVERED_PROTOCOL_GUID;
static EFI_GUID EfiGraphicsOutputProtocol = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
static EFI_GUID EfiLoadedImageProtocol = EFI_LOADED_IMAGE_PROTOCOL_GUID;
static EFI_GUID EfiSimpleFileSystemProtocol = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;

extern int tek_getsize(const void *p);
extern int tek_decomp(const void *p, void* q, int size);

extern void start_kernel(i386_context_t* regs) __attribute__((noreturn));

typedef struct {
    uint32_t    segsiz;
    char        signature[4];
    uint32_t    reserved1, esp, datsiz, dathrb;
    uint8_t     startup[8];
    uint32_t    malloc_address;
} hari_hrb_file_t;

typedef enum {
    hari_variant_default,
    hari_variant_bitnos,
    hari_variant_chnos,
    hari_vatiant_not_valid = -1,
} hari_variant_t;

//  BOOT_INFO for generic haribote variant
typedef struct {
    uint8_t     CYLS, LEDS, VMODE, Padding;
    uint16_t    SCRNX, SCRNY;
    uint32_t    VRAM;
    uint32_t    Reserved;
} hari_boot_info_t;

// BOOT_INFO for BitNOS
typedef struct {
    uint32_t    CYLS, VMODE, SCRNX, SCRNY, LEDS, VRAM;
} hari_bnos_boot_info_t;


//----------------------------------------------------------------------------

void *memcpy(void *dest, const void *src, size_t n) {
    void *dummy1, *dummy2;
    size_t dummy3;
    __asm__ volatile ("rep movsb;" : "=D"(dummy1),"=S"(dummy2),"=c"(dummy3) : "D"(dest), "S"(src), "c"(n) : "memory");
    return dest;
}

void* malloc(size_t n) {
    void* result = NULL;
    EFI_STATUS status = uefi_call_wrapper(gBS->AllocatePool, 3, EfiLoaderData, n, &result);
    if(EFI_ERROR(status)) {
        return NULL;
    }
    return result;
}

void free(void* p) {
    if(p) {
        uefi_call_wrapper(gBS->FreePool, 1, p);
    }
}

void efi_puts(const char* s) {
    for(; *s; s++) {
        unsigned c = *s;
        if(c == '\n') {
            uefi_call_wrapper(gST->ConOut->OutputString, 2, gST->ConOut, L"\r\n");
        }else{
            uint16_t buff[] = { c, 0 };
            uefi_call_wrapper(gST->ConOut->OutputString, 2, gST->ConOut, buff);
        }
    }
}

void efi_wait_any_key () {
    uefi_call_wrapper(gST->ConIn->Reset, 2, gST->ConIn, TRUE);
    for(;;) {
        EFI_INPUT_KEY key;
        EFI_STATUS status = uefi_call_wrapper(gST->ConIn->ReadKeyStroke, 2, gST->ConIn, &key);
        if(status == EFI_SUCCESS){
            if ((key.ScanCode != 0) || (key.UnicodeChar != 0) ) {
                break;
            }
        }
    }
}

EFI_STATUS efi_get_file_content(IN EFI_FILE_HANDLE fs, IN wchar_t* filename, OUT void* _buffer, OUT uintptr_t* _filesize OPTIONAL) {
    EFI_STATUS status;
    EFI_FILE_HANDLE handle = NULL;
    void* buffer = NULL;
    uint64_t filesize = 0xFFFFFFFFFFFFFFFF;
    uint64_t filemode = EFI_FILE_MODE_READ;

    status = uefi_call_wrapper(fs->Open, 5, fs, &handle, filename, filemode, 0);
    if(EFI_ERROR(status)) return status;
    status = uefi_call_wrapper(handle->SetPosition, 2, handle, filesize);
    if(EFI_ERROR(status)) goto error;
    status = uefi_call_wrapper(handle->GetPosition, 2, handle, &filesize);
    if(EFI_ERROR(status)) goto error;
    status = uefi_call_wrapper(handle->SetPosition, 2, handle, 0);
    if(EFI_ERROR(status)) goto error;
    if(sizeof(UINTN) < 8 && filesize > 0x100000000) {
        status = EFI_OUT_OF_RESOURCES;
        goto error;
    }
    buffer = malloc(filesize);
    if(!buffer){
        status = EFI_OUT_OF_RESOURCES;
        goto error;
    }
    status = uefi_call_wrapper(handle->Read, 3, handle, (UINTN*)&filesize, buffer);
    if(EFI_ERROR(status)) goto error;
    status = uefi_call_wrapper(handle->Close, 1, handle);
    if(EFI_ERROR(status)) goto error;

    int size_tek = tek_getsize(buffer);
    if(size_tek > 0) {
        tek_decomp(buffer, _buffer, size_tek);
        filesize = size_tek;
    }else{
        memcpy(_buffer, buffer, filesize);
    }

    free(buffer);
    if(_filesize) {
        *_filesize = filesize;
    }
    return EFI_SUCCESS;

error:
    if(buffer) {
        free(buffer);
    }
    if(handle) {
        uefi_call_wrapper(handle->Close, 1, handle);
    }
    return status;
}


void efi_set_textmode(BOOLEAN is_text_mode) {
    EFI_STATUS status;
    EFI_CONSOLE_CONTROL_PROTOCOL* efi_cc = NULL;
    EFI_CONSOLE_CONTROL_SCREEN_MODE mode_to_be = is_text_mode ? EfiConsoleControlScreenText : EfiConsoleControlScreenGraphics;
    status = uefi_call_wrapper(gBS->LocateProtocol, 3, &EfiConsoleControlProtocol, NULL, (void**)&efi_cc);
    if (!EFI_ERROR(status)) {
        EFI_CONSOLE_CONTROL_SCREEN_MODE mode;
        uefi_call_wrapper(efi_cc->GetMode, 4, efi_cc, &mode, NULL, NULL);
        if (mode != mode_to_be) {
            uefi_call_wrapper(efi_cc->SetMode, 2, efi_cc, mode_to_be);
        }
    }
    uefi_call_wrapper(gST->ConOut->EnableCursor, 2, gST->ConOut, is_text_mode);
}

static inline int guid_is_equal_to(EFI_GUID* guid1, EFI_GUID* guid2) {
    uint32_t* p = (uint32_t*)guid1;
    uint32_t* q = (uint32_t*)guid2;
    return (p[0] == q[0]) && (p[1] == q[1]) && (p[2] == q[2]) && (p[3] == q[3]);
}

static EFI_STATUS efi_get_configuration_table(EFI_GUID* guid, void** table) {
    for(int i=0; i<gST->NumberOfTableEntries; i++) {
        EFI_CONFIGURATION_TABLE* tab = gST->ConfigurationTable + i;
        if(guid_is_equal_to(&tab->VendorGuid, guid)) {
            *table = tab->VendorTable;
            return EFI_SUCCESS;
        }
    }
    return EFI_NOT_FOUND;
}

static void* acpi_find_descriptor_table(acpi_rsd_ptr_t* acpi_rsd, char* name) {
    if(acpi_rsd->revision >= 2) {
        uint8_t* xsdt = (uint8_t*)(intptr_t)acpi_rsd->xsdtaddr;
        int n_tables = (*((uint32_t*)(xsdt+4))-36)/8;
        uint64_t* tables = (uint64_t*)(xsdt+36);
        for(int i=0; i<n_tables; i++) {
            uint32_t* p = (uint32_t*)(intptr_t)(tables[i]);
            uint32_t* q = (uint32_t*)name;
            if(*p == *q) {
                return p;
            }
        }
    } else {
        uint8_t* rsdt = (uint8_t*)(intptr_t)acpi_rsd->rsdtaddr;
        int n_tables = (*((uint32_t*)(rsdt+4))-36)/4;
        uint32_t* tables = (uint32_t*)(rsdt+36);
        for(int i=0; i<n_tables; i++) {
            uint32_t* p = (uint32_t*)(intptr_t)(tables[i]);
            uint32_t* q = (uint32_t*)name;
            if(*p == *q) {
                return p;
            }
        }
    }
    return NULL;
}

//----------------------------------------------------------------------------

EFI_STATUS init_gop (IN EFI_HANDLE image, OUT EFI_GRAPHICS_OUTPUT_PROTOCOL** _gop) {

    EFI_STATUS status;
    EFI_GRAPHICS_OUTPUT_PROTOCOL* gop;

    //  Get native display resolution from EDID
    int edid_res_x = 0, edid_res_y = 0;
    EFI_EDID_DISCOVERED_PROTOCOL* edid;
    status = uefi_call_wrapper(gBS->LocateProtocol, 3, &EfiEdidDiscoveredProtocol, NULL, (void**)&edid);
    if(!EFI_ERROR(status)) {
        edid_res_x = ((edid->Edid[58]&0xF0)<<4) + edid->Edid[56];
        edid_res_y = ((edid->Edid[61]&0xF0)<<4) + edid->Edid[59];
    }

    //  Init GOP
    UINTN handleCount = 0;
    EFI_HANDLE* handleBuffer = NULL;
    status = uefi_call_wrapper(gBS->LocateHandleBuffer, 5, ByProtocol, &EfiGraphicsOutputProtocol, NULL, &handleCount, &handleBuffer);
    if(EFI_ERROR(status)){
        efi_puts("Error: GOP NOT FOUND\n");
        return status;
    }else{
        status = uefi_call_wrapper(gBS->OpenProtocol, 6, handleBuffer[0], &EfiGraphicsOutputProtocol, (void**)&gop, image, NULL, EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL);
        if(EFI_ERROR(status)) {
            efi_puts("Error: GOP NOT FOUND\n");
            return status;
        } else {
            uint32_t mode_to_be = 0;
            EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE* mode = gop->Mode;
            EFI_GRAPHICS_OUTPUT_MODE_INFORMATION* info = mode->Info;
            for(int i=0; i<mode->MaxMode; i++) {
                UINTN sizeOfInfo;
                EFI_GRAPHICS_OUTPUT_MODE_INFORMATION* info;
                uefi_call_wrapper(gop->QueryMode, 4, gop, i, &sizeOfInfo, &info);
                if((info->HorizontalResolution == edid_res_x) && (info->VerticalResolution == edid_res_y)) {
                    mode_to_be = i;
                    break;
                }
            }
            if(mode->Mode != mode_to_be) {
                uefi_call_wrapper(gop->SetMode, 2, gop, mode_to_be);
            }
        }
    }

    *_gop = gop;
    return EFI_SUCCESS;
}


EFI_STATUS EFIAPI efi_main(EFI_HANDLE image, EFI_SYSTEM_TABLE *systab) {
    EFI_STATUS status;
    EFI_FILE_HANDLE efi_fs = NULL;
    EFI_GRAPHICS_OUTPUT_PROTOCOL* gop = NULL;

    BOOLEAN bootpack_alloc_ok = FALSE;

    gST = systab;
    gBS = systab->BootServices;
    gRT = systab->RuntimeServices;

    efi_set_textmode(TRUE);

//    efi_puts("Starting Haribote OS...\n\n");

    //  Prepare filesystem
    {
        EFI_FILE_IO_INTERFACE *efi_fio;
        EFI_LOADED_IMAGE *efi_li;
        status = uefi_call_wrapper(gST->BootServices->HandleProtocol, 3, image, &EfiLoadedImageProtocol, (void**)&efi_li);
        if (EFI_ERROR(status)) return 0;
        status = uefi_call_wrapper(gST->BootServices->HandleProtocol, 3, efi_li->DeviceHandle, &EfiSimpleFileSystemProtocol, (void**)&efi_fio);
        if (EFI_ERROR(status)) return 0;
        status = uefi_call_wrapper(efi_fio->OpenVolume, 2, efi_fio, &efi_fs);
        if (EFI_ERROR(status)) return 0;
    }

    //  Allocate memory for haribote
    UINTN sz_bootpack = MIN_ALLOC;
    EFI_PHYSICAL_ADDRESS pa_bootpack = 0x00100000;
    status = uefi_call_wrapper(gBS->AllocatePages, 4, AllocateAddress, EfiLoaderData, sz_bootpack, &pa_bootpack);
    if(EFI_ERROR(status)) {
        efi_puts("Error: Can't allocate memory\n");
        goto error;
    }
    bootpack_alloc_ok = TRUE;

    //  load haribote.bhs
    hari_hrb_file_t* bhs = (hari_hrb_file_t*)BOOTPACK;
    status = efi_get_file_content(efi_fs, L"EFI\\halo\\haribote.bhs", bhs, NULL);
    if(EFI_ERROR(status)) {
        efi_puts("Error: Can't read haribote.bhs\n");
        goto error;
    }
    hari_variant_t hari_variant = hari_vatiant_not_valid;
    if(strncmp(bhs->signature, "Hari", 4) == 0) {
        hari_variant = hari_variant_default;
    }else if(strncmp(bhs->signature, "BNos", 4) == 0) { // BitNOS
        hari_variant = hari_variant_bitnos;
    // }else if(strncmp(bhs->signature, "CHNP", 4) == 0) { // CHNOS - currently can't boot
    //     hari_variant = hari_variant_chnos;
    }else{
        efi_puts("Error: Bad kernel signature found\n");
        goto error;
    }

    //  load haribote.img
    status = efi_get_file_content(efi_fs, L"EFI\\halo\\haribote.img", (void*)DISKCACHE, NULL);
    if(EFI_ERROR(status)) {
        efi_puts("Error: Can't read haribote.img\n");
        goto error;
    }

    //  ps2 check
    // {
    //     acpi_rsd_ptr_t* acpi_rsd_ptr;
    //     EFI_GUID efi_acpi_20_table_guid = ACPI_20_TABLE_GUID;
    //     status = efi_get_configuration_table(&efi_acpi_20_table_guid, (void**)&acpi_rsd_ptr);
    //     if(EFI_ERROR(status)) {
    //         EFI_GUID efi_acpi_table_guid = ACPI_TABLE_GUID;
    //         status = efi_get_configuration_table(&efi_acpi_table_guid, (void**)&acpi_rsd_ptr);
    //     }
    //     if(EFI_ERROR(status)) {
    //         efi_puts("Error: ACPI NOT FOUND\n");
    //         goto error;
    //     }
    //
    //     acpi_fadt_t* fadt = acpi_find_descriptor_table(acpi_rsd_ptr, "FACP");
    //     Print(L"ACPI Info: SMI %04x %02x/%02x IAPC %04x Flags %08x\n", fadt->SMI_CMD, fadt->ACPI_ENABLE, fadt->ACPI_DISABLE, fadt->IAPC_BOOT_ARCH, fadt->Flags);
    //
    //     if((fadt->IAPC_BOOT_ARCH & ACPI_FADT_IAPC_8042) == 0) {
    //         efi_puts("Error: PS2 NOT PRESENT\n");
    //         goto error;
    //     }
    // }

    //  Init graphics
    status = init_gop(image, &gop);
    if(EFI_ERROR(status)) goto error;

    //  Prepare to exit boot service
    UINTN memorymapsize = sizeof(EFI_MEMORY_DESCRIPTOR), mapkey, descriptorsize;
    EFI_MEMORY_DESCRIPTOR* memorymap = malloc(memorymapsize);
    UINT32 descriptorversion;
    status = uefi_call_wrapper(gBS->GetMemoryMap, 5, &memorymapsize, memorymap, &mapkey, &descriptorsize, &descriptorversion);

    while(status == EFI_BUFFER_TOO_SMALL) {
        free(memorymap);
        memorymap = malloc(memorymapsize);
        status = uefi_call_wrapper(gBS->GetMemoryMap, 5, &memorymapsize, memorymap, &mapkey, &descriptorsize, &descriptorversion);
    }

    if(EFI_ERROR(status)) {
        efi_puts("Error: GetMemoryMap() failed\n");
        goto error;
    }

    //  Exit BootServices
    status = uefi_call_wrapper(gBS->ExitBootServices, 2, image, mapkey);
    if(EFI_ERROR(status)) {
        efi_puts("Error: ExitBootServices() failed\n");
        goto error;
    }

    //  Fill BOOT_INFO
    switch(hari_variant) {
    default:
        {
            hari_boot_info_t* boot_info = (hari_boot_info_t*)(0x0FF0);
            EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE* mode = gop->Mode;
            EFI_GRAPHICS_OUTPUT_MODE_INFORMATION* info = mode->Info;
            boot_info->CYLS = 80;
            boot_info->LEDS = 0;
            boot_info->VMODE = 32;
            boot_info->SCRNX = info->HorizontalResolution;
            boot_info->SCRNY = info->VerticalResolution;
            boot_info->VRAM = mode->FrameBufferBase;
            //boot_info->Reserved = (uint32_t)(intptr_t)gRT;
        }
        break;

    case hari_variant_bitnos:
        {
            hari_bnos_boot_info_t* boot_info = (hari_bnos_boot_info_t*)(0x0900);
            EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE* mode = gop->Mode;
            EFI_GRAPHICS_OUTPUT_MODE_INFORMATION* info = mode->Info;
            boot_info->CYLS = 80;
            boot_info->LEDS = 0;
            boot_info->VMODE = 32;
            boot_info->SCRNX = info->HorizontalResolution;
            boot_info->SCRNY = info->VerticalResolution;
            boot_info->VRAM = mode->FrameBufferBase;
        }
        break;
    }

    //  relocate data
    memcpy((void*)(uintptr_t)(bhs->esp), ((uint8_t*)bhs)+bhs->dathrb, bhs->datsiz);

    //  Boot haribote
    i386_context_t regs;
    memset(&regs, 0, sizeof(regs));
    regs.cs = HARI_KERNEL_CS;
    regs.eip = HARI_KERNEL_EIP;
    regs.ss = HARI_KERNEL_SS;
    regs.esp = bhs->esp;
    regs.ds = HARI_KERNEL_DS;
    regs.es = HARI_KERNEL_DS;

    start_kernel(&regs);

error:
    if(bootpack_alloc_ok) {
        uefi_call_wrapper(gBS->FreePages, 2, pa_bootpack, sz_bootpack);
    }

    efi_puts("    Press any key to exit...\n");
    efi_wait_any_key();

    return EFI_LOAD_ERROR;
}
