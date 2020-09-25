/////////////////////////////////////////////////////////////////////////////
// "GeneralIO.h" 
//
// (c) 1997 RoentDek
// by Klaus Ullmann-Pfleger
/////////////////////////////////////////////////////////////////////////////

#define GIO_NOTTESTED	0
#define GIO_OK			1
#define GIO_ERROR		2

#ifdef _WIN32	// if Win9x, WinNT or W2K

__declspec (dllimport) int OpenPortsForNT(bool bDisplayError);

__declspec (dllimport) unsigned char InputPort(short Port);
__declspec (dllimport) unsigned short InputPortWord(short Port);
__declspec (dllimport) unsigned long InputPortDWord(short Port);

__declspec (dllimport) void OutputPort(short Port, unsigned char Data);
__declspec (dllimport) void OutputPortWord(short Port, unsigned short Data);
__declspec (dllimport) void OutputPortDWord(short Port, unsigned long Data);

#else			// if Windows 3.x or DOS (no GeneralIO.Dll available)

#define OpenPortsForNT(bDisplayError)	(GIO_OK)

#define InputPort(Port)					_inp(Port)
#define InputPortWord(Port)				_inpw(Port)
#define InputPortDWord(Port)			_inpd(Port)

#define OutputPort(Port,Data)			_outp(Port,Data)
#define OutputPortWord(Port,Data);		_outpw(Port,Data)
#define OutputPortDWord(Port,Data);		_outpd(Port,Data)

#endif