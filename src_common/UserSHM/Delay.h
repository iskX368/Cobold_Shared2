#include "stdafx.h"

#define DELAYCLASS_COMMANDSIZE	1000

UINT DelayThread(LPVOID vpClassPointer);

/////////////////////////////////////////////////////////
class DelayClass
{
public:
	volatile CWinThread *wtpDelayCThread;
	volatile bool bPleaseTerminateThread;
	volatile bool bThreadIsRunning;
	volatile double delay_seconds;
	volatile char command[DELAYCLASS_COMMANDSIZE];

	// constructor
	DelayClass();

	// destructor
	~DelayClass();

	void StartThread();

	void TerminateThread();
};