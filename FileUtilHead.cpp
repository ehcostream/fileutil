#include "FileUtilhead.h"
#include "VersionInfo.h"
#include "Md5.h"

struct FileHead
{
	//版本信息 eg.1.2.33
	char szVersion[32];
	//32位密钥(密文)
	char szKey[32];
	//签名信息
	char szSign[32];
	//时间戳
	uint32_t dwTimeStamp;
};

CFileUtilHead::CFileUtilHead()
{

}

CFileUtilHead::~CFileUtilHead()
{
	
}

CFileUtilHead::CFileUtilHead(const FileHead& rstHead)
	:m_strVersion(rstHead.szVersion),
	 m_strKey(rstHead.szKey),
	 m_strSign(rstHead.szSign),
	 m_dwTimeStamp(rstHead.dwTimeStamp)
{
}

void CFileUtilHead::Init(const FileHead& rstHead)
{
	m_strVersion = std::string(rstHead.szVersion);
	m_strSign = std::string(rstHead.szSign);
	m_strKey = std::string(rstHead.szKey);
	m_dwTimeStamp = rstHead.dwTimeStamp;
}

//将头信息附加到文件头部,调用时机：在文件刚打开时调用，切记！！！
int CFileUtilHead::Attach(std::ostream& rstOut, std::string rstrUserKey)
{
	//只支持32字节以内的字符
	assert(rstrUserKey.length() <= 32);

	FileHead stHead;
	memset(&stHead, '\0', sizeof(stHead));
	uint32_t dwPos = 0;
	std::string strEncodeKey;
	for(const auto c : rstrUserKey)
	{
		char szXor = ENCODE_KEY.at((dwPos++) % ENCODE_KEY.length());
		strEncodeKey.append(szXor ^ (char)c);
	}
	stHead.dwTimeStamp = time(nullptr);
	strncpy(stHead.szKey, strEncodeKey.c_str(), strEncodeKey.length());
	strncpy(stHead.szVersion, CVersionInfo::String().c_str(), VersionInfo::String().length());
	std::ostringstream oss;
	oss << CVersionInfo::String() << strEncodeKey << stHead.dwTimeStamp;
	std::string strMd5Sign(Md5::Encode(oss.str()));
	strncpy(stHead.szSign, strMd5Sign.c_str(), strMd5Sign.length());
	//初始化stHead完成

	rstOut.write(&stHead, sizeof(FileHead));
	return 0;
}

std::string CFileUtilHead::Parse(std::istream& rstIn, int& nError)
{
	//读取FileHead
	//判断是否有效
	//解析实际的key
	nError = 0;
	std::string strRealKey;
	do
	{
		FileHead stHead;
		rstIn.read(&stHead, sizeof(FileHead));
		std::ostringstream oss;
		oss << std::string(stHead.szVersion) << std::string(stHead.szKey) << stHead.dwTimeStamp;
		std::string strSign(Md5::Encode(oss.str()));
		if(std::string(stHead.szSign) != strSign)
		{
			//文件无效
			nError = 1;
			break;
		}
		std::string strEncodeKey(stHead.szKey);

		uint32_t dwPos = 0;
		for(const auto c : strEncodeKey)
		{
			char szXor = ENCODE_KEY.at((dwPos++) % ENCODE_KEY.length());
			strRealKey.append(szXor ^ (char)c);
		}

	}while(false);
	return strRealKey;
}
