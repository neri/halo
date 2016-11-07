// missing definitions

#ifndef INCLUDED_EFIADD_H
#define INCLUDED_EFIADD_H

//----------------------------------------------------------------------------

#ifndef EFI_LOADED_IMAGE_PROTOCOL_GUID

#define EFI_LOADED_IMAGE_PROTOCOL_GUID \
	{ 0x5B1B31A1, 0x9562, 0x11d2, { 0x8E, 0x3F, 0x00, 0xA0, 0xC9, 0x69, 0x72, 0x3B } }

#endif

#ifndef EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID

#define EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID \
	{ 0x0964e5b22, 0x6459, 0x11d2, { 0x8e, 0x39, 0x00, 0xa0, 0xc9, 0x69, 0x72, 0x3b } }

#endif

//----------------------------------------------------------------------------

#ifndef EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL_GUID

#define EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL_GUID \
	{ 0xdd9e7534, 0x7762, 0x4698, {0x8c, 0x14, 0xf5, 0x85, 0x17, 0xa6, 0x25, 0xaa } }

typedef struct _EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL;

#define EFI_SHIFT_STATE_VALID		0x80000000
#define EFI_RIGHT_SHIFT_PRESSED		0x00000001
#define EFI_LEFT_SHIFT_PRESSED		0x00000002
#define EFI_RIGHT_CONTROL_PRESSED	0x00000004
#define EFI_LEFT_CONTROL_PRESSED	0x00000008
#define EFI_RIGHT_ALT_PRESSED		0x00000010
#define EFI_LEFT_ALT_PRESSED		0x00000020
#define EFI_RIGHT_LOGO_PRESSED		0x00000040
#define EFI_LEFT_LOGO_PRESSED		0x00000080
#define EFI_MENU_KEY_PRESSED		0x00000100
#define EFI_SYS_REQ_PRESSED			0x00000200

typedef UINT8 EFI_KEY_TOGGLE_STATE;
#define EFI_TOGGLE_STATE_VALID		0x80
#define EFI_KEY_STATE_EXPOSED		0x40
#define EFI_SCROLL_LOCK_ACTIVE		0x01
#define EFI_NUM_LOCK_ACTIVE			0x02
#define EFI_CAPS_LOCK_ACTIVE		0x04

typedef struct EFI_KEY_STATE {
	UINT32 KeyShiftState;
	EFI_KEY_TOGGLE_STATE KeyToggleState;
} EFI_KEY_STATE;

typedef struct {
	EFI_INPUT_KEY Key;
	EFI_KEY_STATE KeyState;
} EFI_KEY_DATA;

typedef
EFI_STATUS
(EFIAPI *EFI_KEY_NOTIFY_FUNCTION) (
IN EFI_KEY_DATA *KeyData
);

typedef
EFI_STATUS
(EFIAPI *EFI_INPUT_RESET_EX) (
IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *This,
IN BOOLEAN ExtendedVerification
);

typedef
EFI_STATUS
(EFIAPI *EFI_INPUT_READ_KEY_EX) (
IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *This,
OUT EFI_KEY_DATA *KeyData
);

typedef
EFI_STATUS
(EFIAPI *EFI_SET_STATE) (
IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *This,
IN EFI_KEY_TOGGLE_STATE *KeyToggleState
);

typedef
EFI_STATUS
(EFIAPI *EFI_REGISTER_KEYSTROKE_NOTIFY) (
IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *This,
IN EFI_KEY_DATA *KeyData,
IN EFI_KEY_NOTIFY_FUNCTION KeyNotificationFunction,
OUT VOID **NotifyHandle
);

typedef
EFI_STATUS
(EFIAPI *EFI_UNREGISTER_KEYSTROKE_NOTIFY) (
IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *This,
IN VOID *NotificationHandle
);

struct _EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL{
	EFI_INPUT_RESET_EX Reset;
	EFI_INPUT_READ_KEY_EX ReadKeyStrokeEx;
	EFI_EVENT WaitForKeyEx;
	EFI_SET_STATE SetState;
	EFI_REGISTER_KEYSTROKE_NOTIFY RegisterKeyNotify;
	EFI_UNREGISTER_KEYSTROKE_NOTIFY UnregisterKeyNotify;
};

#endif

//----------------------------------------------------------------------------

#ifndef EFI_SHELL_PARAMETERS_PROTOCOL_GUID

#define EFI_SHELL_PARAMETERS_PROTOCOL_GUID \
   { 0x752f3136, 0x4e16, 0x4fdc, { 0xa2, 0x2a, 0xe5, 0xf4, 0x68, 0x12, 0xf4, 0xca } }
typedef void* SHELL_FILE_HANDLE;

typedef struct _EFI_SHELL_PARAMETERS_PROTOCOL {
	CHAR16             **Argv;
	UINTN              Argc;
	SHELL_FILE_HANDLE  StdIn;
	SHELL_FILE_HANDLE  StdOut;
	SHELL_FILE_HANDLE  StdErr;
} EFI_SHELL_PARAMETERS_PROTOCOL;

#endif

//----------------------------------------------------------------------------

#ifndef EFI_TIMESTAMP_PROTOCOL_GUID
#define EFI_TIMESTAMP_PROTOCOL_GUID \
	{ 0xafbfde41, 0x2e6e, 0x4262, { 0xba, 0x65, 0x62, 0xb9, 0x23, 0x6e, 0x54, 0x95 } }

typedef struct {
	UINT64 Frequency;
	UINT64 EndValue;
} EFI_TIMESTAMP_PROPERTIES;

typedef EFI_STATUS(EFIAPI *TIMESTAMP_GET_PROPERTIES) ( OUT EFI_TIMESTAMP_PROPERTIES *Properties );
typedef UINT64(EFIAPI *TIMESTAMP_GET) (VOID);

typedef struct _EFI_TIMESTAMP_PROTOCOL {
	TIMESTAMP_GET GetTimestamp;
	TIMESTAMP_GET_PROPERTIES GetProperties;
} EFI_TIMESTAMP_PROTOCOL;

#endif

//----------------------------------------------------------------------------

#ifndef EFI_CONSOLE_CONTROL_PROTOCOL_GUID

#define EFI_CONSOLE_CONTROL_PROTOCOL_GUID \
 	{ 0xf42f7782, 0x12e, 0x4c12, { 0x99, 0x56, 0x49, 0xf9, 0x43, 0x4, 0xf7, 0x21 } }

typedef struct _EFI_CONSOLE_CONTROL_PROTOCOL EFI_CONSOLE_CONTROL_PROTOCOL;

typedef enum {
        EfiConsoleControlScreenText,
        EfiConsoleControlScreenGraphics,
        EfiConsoleControlScreenMaxValue,
} EFI_CONSOLE_CONTROL_SCREEN_MODE;

typedef EFI_STATUS (EFIAPI *EFI_CONSOLE_CONTROL_PROTOCOL_GET_MODE)(
        struct _EFI_CONSOLE_CONTROL_PROTOCOL *This,
        EFI_CONSOLE_CONTROL_SCREEN_MODE *Mode,
        BOOLEAN *UgaExists,
        BOOLEAN *StdInLocked
);

typedef EFI_STATUS (EFIAPI *EFI_CONSOLE_CONTROL_PROTOCOL_SET_MODE)(
        struct _EFI_CONSOLE_CONTROL_PROTOCOL *This,
        EFI_CONSOLE_CONTROL_SCREEN_MODE Mode
);

typedef EFI_STATUS (EFIAPI *EFI_CONSOLE_CONTROL_PROTOCOL_LOCK_STD_IN)(
        struct _EFI_CONSOLE_CONTROL_PROTOCOL *This,
        CHAR16 *Password
);

struct _EFI_CONSOLE_CONTROL_PROTOCOL {
        EFI_CONSOLE_CONTROL_PROTOCOL_GET_MODE GetMode;
        EFI_CONSOLE_CONTROL_PROTOCOL_SET_MODE SetMode;
        EFI_CONSOLE_CONTROL_PROTOCOL_LOCK_STD_IN LockStdIn;
};

#endif

//----------------------------------------------------------------------------

#ifndef EFI_SIMPLE_POINTER_PROTOCOL_GUID

#define	EFI_SIMPLE_POINTER_PROTOCOL_GUID  \
	{ 0x31878c87, 0xb75, 0x11d5, {0x9a, 0x4f, 0x0, 0x90, 0x27, 0x3f, 0xc1, 0x4d } }

typedef struct _EFI_SIMPLE_POINTER_PROTOCOL EFI_SIMPLE_POINTER_PROTOCOL;

typedef struct {
	INT32   RelativeMovementX;
	INT32   RelativeMovementY;
	INT32   RelativeMovementZ;
	BOOLEAN LeftButton;
	BOOLEAN RightButton;
} EFI_SIMPLE_POINTER_STATE;

typedef struct {
	UINT64  ResolutionX;
	UINT64  ResolutionY;
	UINT64  ResolutionZ;
	BOOLEAN LeftButton;
	BOOLEAN RightButton;
} EFI_SIMPLE_POINTER_MODE;

typedef
EFI_STATUS
(EFIAPI *EFI_SIMPLE_POINTER_RESET)(
	struct _EFI_SIMPLE_POINTER_PROTOCOL	*This,
	BOOLEAN	ExtendedVerification
);

typedef
EFI_STATUS
(EFIAPI *EFI_SIMPLE_POINTER_GET_STATE)(
	struct _EFI_SIMPLE_POINTER_PROTOCOL	*This,
	EFI_SIMPLE_POINTER_STATE	*State
);

struct _EFI_SIMPLE_POINTER_PROTOCOL {
	EFI_SIMPLE_POINTER_RESET      Reset;
	EFI_SIMPLE_POINTER_GET_STATE  GetState;
	EFI_EVENT                     WaitForInput;
	EFI_SIMPLE_POINTER_MODE       *Mode;
};

#endif

//----------------------------------------------------------------------------

#ifndef EFI_ABSOLUTE_POINTER_PROTOCOL_GUID

#define EFI_ABSOLUTE_POINTER_PROTOCOL_GUID \
	{ 0x8D59D32B, 0xC655, 0x4AE9, { 0x9B, 0x15, 0xF2, 0x59, 0x04, 0x99, 0x2A, 0x43 } }

typedef struct _EFI_ABSOLUTE_POINTER_PROTOCOL EFI_ABSOLUTE_POINTER_PROTOCOL;

typedef struct {
	UINT64 AbsoluteMinX;
	UINT64 AbsoluteMinY;
	UINT64 AbsoluteMinZ;
	UINT64 AbsoluteMaxX;
	UINT64 AbsoluteMaxY;
	UINT64 AbsoluteMaxZ;
	UINT32 Attributes;
} EFI_ABSOLUTE_POINTER_MODE;

#define EFI_ABSP_SupportsAltActive    0x00000001
#define EFI_ABSP_SupportsPressureAsZ  0x00000002

typedef
EFI_STATUS
(EFIAPI *EFI_ABSOLUTE_POINTER_RESET)(
	EFI_ABSOLUTE_POINTER_PROTOCOL *This,
	BOOLEAN                       ExtendedVerification
);

#define EFI_ABSP_TouchActive	0x00000001
#define EFI_ABS_AltActive		0x00000002

typedef struct {
	UINT64 CurrentX;
	UINT64 CurrentY;
	UINT64 CurrentZ;
	UINT32 ActiveButtons;
} EFI_ABSOLUTE_POINTER_STATE;

typedef
EFI_STATUS
(EFIAPI *EFI_ABSOLUTE_POINTER_GET_STATE)(
	EFI_ABSOLUTE_POINTER_PROTOCOL  *This,
	EFI_ABSOLUTE_POINTER_STATE     *State
);

struct _EFI_ABSOLUTE_POINTER_PROTOCOL {
	EFI_ABSOLUTE_POINTER_RESET      Reset;
	EFI_ABSOLUTE_POINTER_GET_STATE  GetState;
	EFI_EVENT                       WaitForInput;
	EFI_ABSOLUTE_POINTER_MODE       *Mode;
};

#endif

//----------------------------------------------------------------------------

#ifndef EFI_EDID_DISCOVERED_PROTOCOL_GUID

#define EFI_EDID_DISCOVERED_PROTOCOL_GUID \
	{ 0x1c0c34f6, 0xd380, 0x41fa, { 0xa0, 0x49, 0x8a, 0xd0, 0x6c, 0x1a, 0x66, 0xaa } }

typedef struct {
	UINT32 SizeOfEdid;
	UINT8 *Edid;
} EFI_EDID_DISCOVERED_PROTOCOL;

#endif
//----------------------------------------------------------------------------

#endif
