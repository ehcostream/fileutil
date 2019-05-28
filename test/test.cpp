#include <fz/FileUtilSdk.h>
#include <vector>
#include <string>
#include <iostream>

void OnFinished(int err)
{
	std::cout << __FUNCTION__ << ", error: " << err << std::endl;
}

int main()
{
	CFileUtilGeneratorBase* base = futil::CreateFactory(0, "../conf.json");
	base->Set(1, 1 << 20);
	std::vector<std::string> files;
	std::string strOutFile;
//同步
	//压缩
	CFileUtilBase* compresser = base->CreateCompresser();
	files.emplace_back("../../bld/CMakeFiles");
	compresser->Execute(files, ".", nullptr, strOutFile, OnFinished);
	std::cout << strOutFile << std::endl;

	//解压
	CFileUtilBase* uncompresser = base->CreateUncompresser("./CMakeFiles.ar.zb");
	files.clear();
	files.emplace_back("./CMakeFiles.ar.zb");
	uncompresser->Execute(files, "./tmp", nullptr, strOutFile, OnFinished);

	//加密
	CFileUtilBase* encoder = base->CreateEncoder();
	files.clear();
	files.emplace_back("./Makefile");
	encoder->Execute(files, "./crypto", (void*)"hello", strOutFile, OnFinished);

	//解密
	CFileUtilBase* decoder = base->CreateDecoder("./crypto/Makefile.spc");
	files.clear();
	files.emplace_back("./crypto/Makefile.spc");
	decoder->Execute(files, "./crypto", (void*)"hello", strOutFile, OnFinished);

	futil::ReleaseUtil(compresser);
	futil::ReleaseUtil(uncompresser);
	futil::ReleaseUtil(encoder);
	futil::ReleaseUtil(decoder);

//异步
	base = futil::CreateFactory(1, "../conf.json");
	//压缩
	compresser = base->CreateCompresser();
	files.emplace_back("../../bld/CMakeFiles");
	compresser->Execute(files, ".", nullptr, strOutFile, OnFinished);
	std::cout << strOutFile << std::endl;
	futil::Sleep(100);

	//解压
	uncompresser = base->CreateUncompresser("./CMakeFiles.ar.zb");
	files.clear();
	files.emplace_back("./CMakeFiles.ar.zb");
	uncompresser->Execute(files, "./tmp", nullptr, strOutFile, OnFinished);
	futil::Sleep(100);

	//加密
	encoder = base->CreateEncoder();
	files.clear();
	files.emplace_back("./Makefile");
	encoder->Execute(files, "./crypto", (void*)"hello", strOutFile, OnFinished);
	futil::Sleep(100);
	
	//解密
	decoder = base->CreateDecoder("./crypto/Makefile.spc");
	files.clear();
	files.emplace_back("./crypto/Makefile.spc");
	decoder->Execute(files, "./crypto", (void*)"hello", strOutFile, OnFinished);

	futil::WaitAsync();

	futil::ReleaseUtil(compresser);
	futil::ReleaseUtil(uncompresser);
	futil::ReleaseUtil(encoder);
	futil::ReleaseUtil(decoder);
}