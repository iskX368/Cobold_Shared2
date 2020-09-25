#include "stdafx.h" 
#include "CoboldUtilities.h"
#include "Delay.h"

// constructor
DelayClass::DelayClass():bPleaseTerminateThread(false),
						delay_seconds(0.),
						bThreadIsRunning(false),
						wtpDelayCThread(nullptr)
{
}

// destructor
DelayClass::~DelayClass() 
{
	if(this->bThreadIsRunning) 
		TerminateThread();
	if(this->bThreadIsRunning)
	{
		this->bPleaseTerminateThread = true;
		Sleep(50);
	}
}

void DelayClass::StartThread() 
{
	this->bPleaseTerminateThread = false;
	this->wtpDelayCThread = AfxBeginThread(&DelayThread ,(LPVOID)this,THREAD_PRIORITY_NORMAL,0,0,NULL);
}

void DelayClass::TerminateThread() 
{
	this->bPleaseTerminateThread = true; 
	if (this->bThreadIsRunning)
	{
		Sleep(10);
		//while (this->bThreadIsRunning) {this->bPleaseTerminateThread = true; Sleep(10);} // XXX not yet working because thread is alread dead. But why?
	}
}

UINT DelayThread(LPVOID vpClassPointer)
{
	DelayClass *DelayInstance = (DelayClass*)vpClassPointer;
	DelayInstance->bThreadIsRunning = true;

	__int32 n = __int32(DelayInstance->delay_seconds*10.);
	for (__int32 i=0; i < n;++i)
	{
		if (DelayInstance->bPleaseTerminateThread) 
			break;
		Sleep(100);
	}
	n = n*100 - __int32(DELAYCLASS_COMMANDSIZE*DelayInstance->delay_seconds);
	if (n>0 && !DelayInstance->bPleaseTerminateThread)
		Sleep(n);

	CString comm = CString((char*)DelayInstance->command);
	if (!DelayInstance->bPleaseTerminateThread)
		ExecuteCommand(comm);
	DelayInstance->bThreadIsRunning = false;
	AfxEndThread(1,TRUE);
	return TRUE;
}

