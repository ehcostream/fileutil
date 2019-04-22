#pragma once
#include <ostream>
#include <cstring>

struct FileHead
{
	//版本信息 eg.1.2.33
	char szVersion[32+1];
	//私钥不能超过32位
	char szKey[32+1];
	//签名信息
	char szSign[32+1];
	//时间戳
	uint32_t dwTimeStamp;
	//后缀名不能20位
	char szExt[20+1];
	//文件名
	char szFilename[256+1];
};


class CFileUtilHead
{
public:
	CFileUtilHead();
	explicit CFileUtilHead(const FileHead& rstHead);
	~CFileUtilHead();

	void Init(const FileHead& rstHead);

public:
	//将头信息附加到文件头部,调用时机：在文件刚打开时调用，切记！！！
	static int Attach(std::ostream& rstOut, const std::string& rstrOutFile, std::string strUserKey = std::string());

	//返回明文key
	static std::string Parse(std::istream& rstIn, int& nError, FileHead& rstHead);

	static int GetData(const std::string& rstrFile, FileHead& rstHead);
private:
	//MD5(zlibutil_head_key)
	static const std::string ENCODE_KEY;

private:
	std::string m_strVersion;
	std::string m_strKey;
	std::string m_strSign;
	uint32_t m_dwTimeStamp;
};