#include <cassert>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include "FileUtilHead.h"
#include "VersionInfo.h"
#include "MD5.h"

const std::string CFileUtilHead::ENCODE_KEY = "4393d121109b3abf1c94d1bd749d9c05";
namespace fs = boost::filesystem;

struct FileHead
{
	//版本信息 eg.1.2.33
	char szVersion[32+1];
	//私钥(密文)不能超过32位
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
int CFileUtilHead::Attach(std::ostream& rstOut, const std::string& rstrOutFile, std::string rstrUserKey)
{
	std::cout << __FILE__ << "\t" << __FUNCTION__ << std::endl;
	//只支持32字节以内的字符
	FileHead stHead;
	assert(rstrUserKey.length() <= sizeof(stHead.szKey));
	fs::path path(rstrOutFile);

	
	memset(&stHead, '\0', sizeof(stHead));
	uint32_t dwPos = 0;
	std::string strEncodeKey;
	for(const auto c : rstrUserKey)
	{
		char szXor = ENCODE_KEY.at((dwPos++) % ENCODE_KEY.length());
		char tC = szXor ^ (char)c;
		strEncodeKey.append(&tC);
		std::cout << "xor:" << (int)szXor << ",c:" << (int)c << ",result:" << (int)tC << std::endl;
	}
	std::cout << "encode size:" << dwPos << std::endl;
	stHead.dwTimeStamp = time(nullptr);
	strncpy(stHead.szKey, strEncodeKey.c_str(), strEncodeKey.length());
	strncpy(stHead.szVersion, CVersionInfo::String().c_str(), CVersionInfo::String().length());
	std::ostringstream oss;
	oss << CVersionInfo::String() << strEncodeKey << stHead.dwTimeStamp << path.extension().string() << path.stem().string();
	std::string strMd5Sign(MD5::Encode(oss.str()));
	strncpy(stHead.szSign, strMd5Sign.c_str(), strMd5Sign.length());
	//获取文件名称和后缀名称
	strncpy(stHead.szFilename, path.stem().string().c_str(), path.stem().string().length());
	strncpy(stHead.szExt, path.extension().string().c_str(), path.extension().string().length());
	stHead.szKey[strEncodeKey.length()] = '\0';
	stHead.szFilename[path.stem().string().length()] = '\0';
	stHead.szExt[path.extension().string().length()] = '\0';
	stHead.szSign[strMd5Sign.length()] = '\0';
	stHead.szVersion[CVersionInfo::String().length()] = '\0';

	//初始化stHead完成

	std::cout << "Attach Before--->\n" << std::string(stHead.szVersion) << " ->"
		          << strEncodeKey << "<- "
		          << stHead.dwTimeStamp << " "
		          << path.extension().string() << " "
		          << path.stem().string() << " "
		          << strMd5Sign << std::endl;

	std::cout << "Attach After--->\n" << std::string(stHead.szVersion) << " ->"
		          << std::string(stHead.szKey) << "<- "
		          << stHead.dwTimeStamp << " "
		          << std::string(stHead.szExt) << " "
		          << std::string(stHead.szFilename) << " "
		          << std::string(stHead.szSign) << std::endl;

	for(const auto c : strEncodeKey)
	{
		std::cout << (int)c << std::endl;
	}

	rstOut.write((char*)&stHead, sizeof(FileHead));
	return 0;
}

std::string CFileUtilHead::Parse(std::istream& rstIn, int& nError,  std::string& rstrExt, std::string& rstrFilename)
{
	//读取FileHead
	//判断是否有效
	//解析实际的key

	nError = 0;
	std::string strRealKey;
	do
	{
		FileHead stHead;
		rstIn.read((char*)&stHead, sizeof(FileHead));
		std::ostringstream oss;
		oss << std::string(stHead.szVersion) << std::string(stHead.szKey) << stHead.dwTimeStamp << std::string(stHead.szExt) << std::string(stHead.szFilename);
		std::cout << "Parse--->\n" << std::string(stHead.szVersion) << " "
		          << std::string(stHead.szKey) << " "
		          << stHead.dwTimeStamp << " "
		          << std::string(stHead.szExt) << " "
		          << std::string(stHead.szFilename) << " "
		          << std::string(stHead.szSign) << std::endl;

		std::string strSign(MD5::Encode(oss.str()));
		if(std::string(stHead.szSign) != strSign)
		{
			//文件无效
			nError = 1;
			break;
		}
		std::string strEncodeKey(stHead.szKey, 5);
		std::cout << "encodekeysource:" << strEncodeKey << ",encodekey length:" << strEncodeKey.length() << std::endl;
		for(const auto c : strEncodeKey)
		{
			std::cout << (int)c << std::endl;
		}

		uint32_t dwPos = 0;
		for(const auto c : strEncodeKey)
		{
			char szXor = ENCODE_KEY.at((dwPos++) % ENCODE_KEY.length());
			char tC = szXor ^ (char)c;
			strRealKey.append(&tC);
			std::cout << "xor:" << (int)szXor << ",c:" << (int)c << ",result:" << (int)tC << std::endl;
		}
		std::cout << "parse size:" << dwPos << std::endl;

		rstrExt = std::string(stHead.szExt);
		rstrFilename = std::string(stHead.szFilename);

	}while(false);
	std::cout << __FILE__ << "\t" << __FUNCTION__ << "\t" << nError << std::endl;

	return strRealKey;
}
