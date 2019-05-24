#include <fz/FileUtilSdk.h>
#include <vector>
#include <string>
#include <iostream>

int main()
{
	CFileUtilGeneratorBase* base = futil::CreateFactory(0, "../conf.json");
	base->Set(2, 1 << 20);
	std::vector<std::string> files;
	std::string strOutFile;

	//压缩
	CFileUtilBase* compresser = base->CreateCompresser();
	files.emplace_back("../../bld/CMakeFiles");
	compresser->Execute(files, ".", nullptr, strOutFile);
	std::cout << strOutFile << std::endl;

	//解压
	CFileUtilBase* uncompresser = base->CreateUncompresser("./CMakeFiles.ar.zb");
	files.clear();
	files.emplace_back("./CMakeFiles.ar.zb");
	uncompresser->Execute(files, "./tmp", nullptr, strOutFile);

	//加密
	/*CFileUtilBase* encoder = base->CreateEncoder();
	files.clear();
	files.emplace_back("./Makefile");
	encoder->Execute(files, "./crypto", (void*)"hello", strOutFile);

	//解密
	CFileUtilBase* decoder = base->CreateDecoder("./crypto/Makefile.spc");
	files.clear();
	files.emplace_back("./crypto/Makefile.spc");
	decoder->Execute(files, "./crypto", (void*)"hello", strOutFile);*/

	// compress with async mode
	// base = futil::CreateFactory(1);
	// compresser = base->CreateCompresser();
	// files.clear();
	// files.emplace_back("./CMakeCache.txt");
	// compresser->Execute(files, ".", nullptr, strOutFile);
	// std::cout << strOutFile << std::endl;
	// futil::WaitAsync();
}