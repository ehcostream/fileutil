#include <cassert>
#include <memory>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <Constants.h>
#include <Errors.h>
#include "FileUtilHead.h"
#include "VersionInfo.h"
#include "MD5.h"

namespace fs = boost::filesystem;


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
	int nError = Errors::ERROR_NONE;
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
		char szXor = Constants::ENCODE_KEY.at((dwPos++) % Constants::ENCODE_KEY.length());
		char tC = szXor ^ (char)c;
		strEncodeKey.append(&tC, 1);
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

	rstOut.write((char*)&stHead, sizeof(FileHead));
	return nError;
}

std::string CFileUtilHead::Parse(std::istream& rstIn, int& nError, FileHead& rstHead)
{
	//读取FileHead
	//判断是否有效
	//解析实际的key

	nError = Errors::ERROR_NONE;
	std::string strRealKey;
	do
	{
		rstIn.read((char*)&rstHead, sizeof(FileHead));
		std::ostringstream oss;
		oss << std::string(rstHead.szVersion) << std::string(rstHead.szKey) << rstHead.dwTimeStamp << std::string(rstHead.szExt) << std::string(rstHead.szFilename);
		std::string strSign(MD5::Encode(oss.str()));
		if(std::string(rstHead.szSign) != strSign)
		{
			//文件无效
			nError = Errors::INVALID_SIGN;
			break;
		}
		std::string strEncodeKey(rstHead.szKey);
		std::cout << "encodekeysource:" << strEncodeKey << ",encodekey length:" << strEncodeKey.length() << std::endl;

		uint32_t dwPos = 0;
		for(const auto c : strEncodeKey)
		{
			char szXor = Constants::ENCODE_KEY.at((dwPos++) % Constants::ENCODE_KEY.length());
			char tC = szXor ^ (char)c;
			strRealKey.append(&tC, 1);
			std::cout << "xor:" << (int)szXor << ",c:" << (int)c << ",result:" << (int)tC << std::endl;
		}
		std::cout << "parse size:" << dwPos << std::endl;
	}while(false);
	std::cout << __FILE__ << "\t" << __FUNCTION__ << "\t" << nError << std::endl;

	return strRealKey;
}

int CFileUtilHead::GetData(const std::string& rstrFile, FileHead& rstHead)
{
	int nError = Errors::ERROR_NONE;
	do
	{
		if(fs::is_directory(fs::path(rstrFile)))
		{
			nError = Errors::PARSE_FILE_INVALID;
			break;
		}
		std::unique_ptr< std::ifstream > ifsp = std::unique_ptr< std::ifstream >(new std::ifstream(rstrFile));
		std::istream * isp = ifsp.get();
		(*isp).read((char*)&rstHead, sizeof(FileHead));
		std::ostringstream oss;
		oss << std::string(rstHead.szVersion) << std::string(rstHead.szKey) << rstHead.dwTimeStamp << std::string(rstHead.szExt) << std::string(rstHead.szFilename);
		ifsp->close();
		std::string strSign(MD5::Encode(oss.str()));
		if(std::string(rstHead.szSign) != strSign)
		{
			nError = Errors::INVALID_SIGN;
			break;
		}
	}while(false);

	return nError;
}