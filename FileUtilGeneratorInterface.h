#ifndef __FILE_UTIL_GENERATOR_INTERFACE__
#define __FILE_UTIL_GENERATOR_INTERFACE__

#include "FileUtilBase.h"

class CFileUtilGeneratorInterface
{
public:
	virtual CFileUtilBase* CreateCompresser() = 0;
	virtual CFileUtilBase* CreateUncompresser(const std::string& rstrInFile) = 0;
	virtual CFileUtilBase* CreateEncoder() = 0;
	virtual CFileUtilBase* CreateDecoder(const std::string& rstrInFile) = 0;
};
#endif