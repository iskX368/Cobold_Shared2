/////////////////////////////////////////////////////////////////////////////
// "HighResTimer.h" 
//
// (c) 1997 RoentDek
// by Klaus Ullmann-Pfleger
/////////////////////////////////////////////////////////////////////////////

// for _Win32 definition
//	  time unit is in us
// for not _Win32 definition
//    time unit is in us but will be reduced to ms
//	  not all functions are available

#ifdef _WIN32	// if Win9x, WinNT or W2K

__declspec (dllexport) double HighResTimerFrequency(void);
__declspec (dllimport) void HighResTimerInit(void);
__declspec (dllimport) void HighResTimerSetFinal(int uSeconds);
__declspec (dllimport) void HighResTimerSetFinal(int uSeconds, LARGE_INTEGER &HRTFinal, DWORD &LRTFinal);
__declspec (dllimport) bool HighResTimerTestFinal(LARGE_INTEGER &HRTFinal, DWORD &LRTFinal);
__declspec (dllimport) bool HighResTimerTestFinal(void);
__declspec (dllimport) void HighResTimerWait(int uSeconds);

#else			// if Windows 3.x or DOS (no GeneralIO.Dll available)

#define HighResTimerWait(uSeconds)			sleep(uSeconds/1000+1);

#endif
