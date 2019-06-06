#ifndef __CCONF_H__
#define __CCONF_H__

#ifdef COMMON_DLL
#	if defined(WIN32)
#		ifdef COMMON_INTERNAL
#			define CEXTERN extern "C" __declspec(dllexport)
#		else
#			define CEXTERN extern "C" __declspec(dllimport)
#		endif
#	endif
#endif

#ifndef CEXTERN
#	define CEXTERN extern "C" 
#endif

#endif