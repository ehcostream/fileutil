#include "SymCrypto.h"


int CSymCrypto::SymEncodeOld(const std::string& rstrSource, const std::string& rstrOut, const std::string& rstrKey, bool bEncode, std::string& rstrOutFile)
{
	//判断输出文件夹是否存在如果不存在，则创建
    fs::path filePath(rstrOut);
    if(!fs::exists(filePath))
    {
        fs::create_directories(filePath);
    }
    
    std::ifstream in(rstrSource, std::ifstream::in | std::ifstream::binary);
    std::ofstream out;
    std::string strExt(".spc");
    int nError = 0;
    do
    {
        if(!in.is_open())
        {
            nError = 1;
            break;
        }

        std::string strOutFile;
        EncodeHeaderInfo ehInfo;
        //首先读取加密文件头信息
        in.read((char*)&ehInfo, sizeof(ehInfo));

        if(bEncode)
        {
            //
            
            if(std::string(ehInfo.szExt) == strExt)
            {
                //已经加密过
                nError = 3;
                break;
            }
            else
            {
                //恢复文件指针到文件头部
                strOutFile = fs::path(rstrSource).filename().string();
                strOutFile.append(strExt);
                strOutFile = (filePath / strOutFile).string();
                in.seekg(0, in.beg);
            }
        }
        else
        {
            if(std::string(ehInfo.szExt) != strExt)
            {
                nError = 4;
                break;
            }
            strOutFile = (filePath / ehInfo.szFilename).string();
        }
        std::cout << "source:" << rstrSource << "out:" << strOutFile << std::endl;
        out.open(strOutFile, std::ofstream::out | std::ofstream::trunc);
        if(!out.is_open())
        {
            nError = 2;
            break;
        }

        if(bEncode)
        {
            strncpy(&ehInfo.szFilename[0], fs::path(rstrSource).filename().string().c_str(), rstrSource.length());
            strncpy(&ehInfo.szFilename[0] + fs::path(rstrSource).filename().string().length(), "\0", 1);
            strncpy(&ehInfo.szExt[0], strExt.c_str(), strExt.length());
            strncpy(&ehInfo.szExt[0] + strExt.length(), "\0", 1);
            out.write((char*)&ehInfo, (size_t)sizeof(EncodeHeaderInfo));
        }
        
        //open file normally
        char c;
        while(in.peek() != EOF)
        {
            in.get(c);
            out << (char)(~c);
        }
        rstrOutFile = strOutFile;
    }while(false);

    if(nError == 0 )
    {
        std::cout << (bEncode ? "encode" : "decode") << " file successfully" << std::endl;
    }
    else
    {
        std::cout << (bEncode ? "encode" : "decode") << " file fail, error code: "  << nError << std::endl;
    }
    in.close();
    out.close();
    return nError;
}

int CSymCrypto::SymEncode(const std::string& rstrSource, const std::string& rstrOut, const std::string& rstrKey, bool bEncode, std::string& rstrOutFile)
{
    //判断输出文件夹是否存在如果不存在，则创建
    std::cout << __FILE__ << " " << __FUNCTION__ << " " << rstrKey << std::endl;
    fs::path filePath(rstrOut);
    if(!fs::exists(filePath))
    {
        fs::create_directories(filePath);
    }
    
    std::ifstream in(rstrSource, std::ifstream::in | std::ifstream::binary);
    std::ofstream out;
    std::string strExtension(".spc");
    int nError = 0;
    do
    {
        if(!bEncode)
        {
            //如果是解密，首先解析头信息，取出真实密码并判断密码与文件本身使用的密码是否一致
            //如果不一致，直接返回，密码错误
            //如果一致，使用密码对文件内容进行解密
            int nParseResult = 0;
            std::string strExt;
            std::string strFilename;
            std::string strRealKey = CFileUtilHead::Parse(in, nParseResult, strExt, strFilename);
            if(nParseResult != 0 || strExt != strExtension)
            {
                //文件格式无效
                nError = 1;
                break;
            }
            std::cout << strRealKey << "<= =>" << rstrKey << std::endl;
            if(strRealKey != rstrKey)
            {
                //密码错误
                nError = 2;
                break;
            }
            rstrOutFile = (filePath / strFilename).string();
        }
        else
        {
            
            int nParseResult = 0;
            std::string strExt;
            std::string strFilename;
            std::string strRealKey = CFileUtilHead::Parse(in, nParseResult, strExt, strFilename);
            if(strExt == strExtension)
            {
                //重复加密
                nError = 3;
                break;
            }
            else
            {
                //恢复文件指针
                rstrOutFile = fs::path(rstrSource).filename().string();
                rstrOutFile.append(strExtension);
                rstrOutFile = (filePath / rstrOutFile).string();
                in.seekg(0, in.beg);
            }

        }

        std::cout << "source:" << rstrSource << "out:" << rstrOutFile << std::endl;
        out.open(rstrOutFile, std::ofstream::out | std::ofstream::trunc);
        if(!out.is_open())
        {
            nError = 4;
            break;
        }
        if(bEncode)
        {
            //附加文件头信息
            CFileUtilHead::Attach(out, rstrOutFile, rstrKey);
        }
        uint64_t dwPos = 0;
        char c;
        //开始进行解密/解密
        while(in.peek() != EOF)
        {
            char szXor = rstrKey.at((dwPos++) % rstrKey.length());
            in.get(c);
            char tC = szXor ^ (char)c;
            out << tC;
        }

    }while(false);
    in.close();
    out.close();
    return nError;
}