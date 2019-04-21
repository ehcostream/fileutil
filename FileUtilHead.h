#pragma once
#include <ostream>
#include <cstring>

class FileHead;

class CFileUtilHead
{
public:
	CFileUtilHead();
	explicit CFileUtilHead(const FileHead& rstHead);
	~CFileUtilHead();

	void Init(const FileHead& rstHead);

	//将头信息附加到文件头部,调用时机：在文件刚打开时调用，切记！！！
	int Attach(std::ostream& rstOut, std::string strUserKey = std::string());

public:
	//返回明文key
	static std::string Parse(std::istream& rstIn, int& nError);
private:
	//MD5(zlibutil_head_key)
	static const std::string ENCODE_KEY;

private:
	std::string m_strVersion;
	std::string m_strKey;
	std::string m_strSign;
	uint32_t m_dwTimeStamp;
};