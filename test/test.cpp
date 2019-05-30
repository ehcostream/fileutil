#include <FileUtilSdk.h>
#include <vector>
#include <string>
#include <iostream>

void OnFinished(int err)
{
	std::cout << __FUNCTION__ << ", error: " << err << std::endl;
}

int main()
{
	CFileUtilGeneratorBase* base = futil::CreateFactory(0, "../../conf.json");
	base->Set(1, 1 << 20);
	std::vector<std::string> files;
	std::string strOutFile;
//同步
	//压缩
	CFileUtilBase* compresser = base->CreateCompresser();
	files.emplace_back("../../../bld");
	compresser->Execute(files, ".", nullptr, strOutFile, OnFinished);
	std::cout << strOutFile << std::endl;
	std::cout << "---------------------COMPRESS FINISHED--------------------" << std::endl;

	//解压
	CFileUtilBase* uncompresser = base->CreateUncompresser("./bld.ar.zb");
	assert(uncompresser != nullptr);
	files.clear();
	files.emplace_back("./bld.ar.zb");
	uncompresser->Execute(files, "./tmp", nullptr, strOutFile, OnFinished);
	std::cout << "---------------------UNCOMPRESS FINISHED--------------------" << std::endl;

	//加密
	CFileUtilBase* encoder = base->CreateEncoder();
	files.clear();
	files.emplace_back("../CMakeCache.txt");
	encoder->Execute(files, "./crypto", (void*)"hello", strOutFile, OnFinished);
	std::cout << "---------------------ENCODE FINISHED--------------------" << std::endl;

	//解密
	CFileUtilBase* decoder = base->CreateDecoder("./crypto/CMakeCache.txt.spc");
	assert(decoder != nullptr);
	files.clear();
	files.emplace_back("./crypto/CMakeCache.txt.spc");
	decoder->Execute(files, "./crypto", (void*)"hello", strOutFile, OnFinished);
	std::cout << "---------------------DECODE FINISHED--------------------" << std::endl;

	futil::ReleaseUtil(compresser);
	futil::ReleaseUtil(uncompresser);
	futil::ReleaseUtil(encoder);
	futil::ReleaseUtil(decoder);

//异步
	base = futil::CreateFactory(1, "../../conf.json");
	//压缩
	compresser = base->CreateCompresser();
	files.clear();
	files.emplace_back("../../../bld");
	compresser->Execute(files, ".", nullptr, strOutFile, OnFinished);
	std::cout << strOutFile << std::endl;
	futil::SleepInAWhile(500);
	std::cout << "---------------------ASYNC COMPRESS FINISHED--------------------" << std::endl;

	//解压
	uncompresser = base->CreateUncompresser("./bld.ar.zb");
	files.clear();
	files.emplace_back("./bld.ar.zb");
	uncompresser->Execute(files, "./tmp1", nullptr, strOutFile, OnFinished);
	futil::SleepInAWhile(500);
	std::cout << "---------------------ASYNC UNCOMPRESS FINISHED--------------------" << std::endl;

	//加密
	encoder = base->CreateEncoder();
	files.clear();
	files.emplace_back("./CMakeLists.txt");
	encoder->Execute(files, "./crypto1", (void*)"hello", strOutFile, OnFinished);
	futil::SleepInAWhile(500);
	std::cout << "---------------------ASYNC ENCODE FINISHED--------------------" << std::endl;
	
	//解密
	decoder = base->CreateDecoder("./crypto1/CMakeLists.txt.spc");
	assert(decoder != nullptr);
	files.clear();
	files.emplace_back("./crypto1/CMakeLists.txt.spc");
	decoder->Execute(files, "./crypto1", (void*)"hello", strOutFile, OnFinished);
	std::cout << "---------------------ASYNC DECODE FINISHED--------------------" << std::endl;

	futil::WaitAsync();
	futil::ReleaseUtil(compresser);
	futil::ReleaseUtil(uncompresser);
	futil::ReleaseUtil(encoder);
	futil::ReleaseUtil(decoder);
}