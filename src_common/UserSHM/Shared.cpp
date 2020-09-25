// SharedClass.cpp source
//

#include "StdAfx.h"
#include "UserSHM.h"
#include "memory.h"

//SharedClass::SharedClass():pCoboldParserInstance(nullptr)
SharedClass::SharedClass()
{
	csDAqNameAndPath.Empty();
	csDAnNameAndPath.Empty();
	bDAqIsRunningOnline = false;
	bDAqIsRunningOffline = false;
	bDAnIsRunning = false;
	adc_peak_infos_array = nullptr;
	adc_peak_infos_array_max_nof_hits = 0;

	for (__int32 ch = 0;ch<SHAREDCLASS_ARRAYSIZE;ch++) 
	{
		adc_trace[ch] = new __int16[15000];
		memset(adc_trace[ch],0,15000*sizeof(__int16));
		adc_trace_length[ch] = 0;
	}
}

SharedClass::~SharedClass()
{
	for (__int32 ch = 0;ch<SHAREDCLASS_ARRAYSIZE;ch++) 
	{
		if (adc_trace[ch]) 
			delete[] adc_trace[ch];
		adc_trace[ch] = nullptr;
	}
}