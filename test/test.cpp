#include <fz/FileUtilSdk.h>
#include <vector>
#include <string>
#include <iostream>

int main()
{
	CFileUtilGeneratorBase* base = futil::CreateFactory(0);
	CFileUtilBase* compresser = base->CreateCompresser();
	std::vector<std::string> files;
	std::string strOutFile;
	files.emplace_back("../../bld/CMakeFiles");
	compresser->Execute(files, ".", nullptr, strOutFile);
	std::cout << strOutFile << std::endl;

	CFileUtilBase* uncompresser = base->CreateUncompresser("./CMakeFiles.ar.zb");
	files.clear();
	files.emplace_back("./CMakeFiles.ar.zb");
	uncompresser->Execute(files, "./tmp", nullptr, strOutFile);

	// compress with async mode
	// base = futil::CreateFactory(1);
	// compresser = base->CreateCompresser();
	// files.clear();
	// files.emplace_back("./CMakeCache.txt");
	// compresser->Execute(files, ".", nullptr, strOutFile);
	// std::cout << strOutFile << std::endl;
	// futil::WaitAsync();
}