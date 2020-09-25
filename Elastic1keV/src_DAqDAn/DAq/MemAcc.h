/*
MemAccess Library
Copyright (c) 1999-2004 Hai Li, Zeal SoftStudio.
Web: http://www.zealsoft.com (English)
	 http://zealsoft.yeah.net (Chinese)
E-Mail: support@zealsoft.com
*/
#ifndef MemAccess

#define MemAccess

#ifdef __cplusplus
extern "C" {
#endif

#include <windows.h>

#define V86_To_PhyAddress(seg,ofs)	((ULONG) \
			   (((unsigned long)(seg) << 4) | (unsigned)(ofs)))

// Determine whether the program is running under WinNT.
BOOL APIENTRY maIsWinNT();

// Open the diver.
BOOL APIENTRY maOpenLibrary();

// Close the driver.
BOOL APIENTRY maCloseLibrary();

// Return a byte at memory location nAddress.
BYTE APIENTRY maPeekB(DWORD nAddress);
// Return a word at memory location nAddress.
WORD APIENTRY maPeekW(DWORD nAddress);
// Return a double word at memory location nAddress.
DWORD APIENTRY maPeekD(DWORD nAddress);

// Store a byte at memory location nAddress.
BOOL APIENTRY maPokeB(DWORD nAddress, WORD nData);
// Store a word at memory location nAddress.
BOOL APIENTRY maPokeW(DWORD nAddress, WORD nData);
// Store a double word at memory location nAddress.
BOOL APIENTRY maPokeD(DWORD nAddress, DWORD dwData);

// Set the registration information.
void APIENTRY maLicenseInfo(LPSTR sUser, long dwKey);

// Error constants
const int ERROR_MA_DRIVER = 2;
const int ERROR_MA_NO_DEVICE_HANDLE = 9996;
const int ERROR_MA_SCM_CANT_CONNECT = 9998;

// Returns a value indicating the last operation is successful or not.
int APIENTRY maGetLastState(char *s);

// Returns the version of MemAccess Library.
WORD APIENTRY maGetVersion();

// Install the service under Windows NT/2000/XP
BOOL APIENTRY maInstallService();
// Remove the service under Windows NT/2000/XP
BOOL APIENTRY maRemoveService();
// Control the service under Windows NT/2000/XP
BOOL APIENTRY maControlService();

// Map the physical address to process address
void* APIENTRY maMapPhysToLinear(DWORD PhsAddr, DWORD dwSize, HANDLE* hMem);

// Unmap the address
BOOL APIENTRY maUnmapPhysicalMemory(HANDLE hMem, PVOID LinearAddr);

typedef struct {
	ULONG BaseAddress;
	ULONG Size;
	ULONG IOType;
} BADDR;

// Get the base address of a PCI device
LONG APIENTRY maGetDeviceBaseAddress(USHORT* VendorID, USHORT* DeviceID, LONG nIndex, BADDR baddrBuffer[6]);

#ifdef __cplusplus
}
#endif

#endif