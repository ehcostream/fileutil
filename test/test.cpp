#include <fz/FileUtilSdk.h>
#include <vector>
#include <string>
#include <iostream>

int main()
{
	CFileUtilGeneratorBase* base = futil::CreateFactory(0);
	CFileUtilBase* compresser = base->CreateCompresser();
	std::vector<std::string> files;
	files.emplace_back("./Makefile");
	std::string strOutFile;
	compresser->Execute(files, ".", nullptr, strOutFile);

	base = futil::CreateFactory(1);
	compresser = base->CreateCompresser();
	files.clear();
	files.emplace_back("./CMakeCache.txt");
	compresser->Execute(files, ".", nullptr, strOutFile);
	std::cout << strOutFile << std::endl;
	futil::WaitAsync();
}