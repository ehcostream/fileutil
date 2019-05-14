#ifndef  __FCONF_H__
#define __FCONF_H__

#ifdef FILEUTIL_DLL
#	if defined(WIN32)
#		ifdef FILEUTIL_INTERNAL
#			define FEXTERN extern __declspec(dllexport)
#		else
#			define FEXTERN extern __declspec(dllimport)
#		endif
#	endif
#endif

#ifndef FEXTERN
#	define FEXTERN extern "C" 
#endif

#endif