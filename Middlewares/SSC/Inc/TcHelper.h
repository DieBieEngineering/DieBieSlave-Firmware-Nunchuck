
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the TCHELPER_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// TCHELPER_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#include <windows.h>

#ifdef __cplusplus
#define EXPORT extern "C" __declspec (dllexport)
#else
#define EXPORT __declspec (dllexport)
#endif




EXPORT long	 __stdcall TcHelperInstall();
EXPORT long	 __stdcall TcHelperRemove();
EXPORT unsigned short __stdcall GetNumberOfEscCards();
EXPORT void* __stdcall MapESCPointer(unsigned short cardInstance);
EXPORT void  __stdcall UnmapESCPointer();
