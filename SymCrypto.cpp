#include "SymCrypto.h"


int CSymCrypto::SymEncode(const std::string& rstrSource, const std::string& rstrOut, bool bEncode, std::string& rstrOutFile)
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