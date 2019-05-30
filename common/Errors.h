#ifndef __ERRORS_H__
#define __ERRORS_H__
#define COMMON_INTERNAL

#include "cconf.h"
namespace Errors
{

    CEXTERN const int ERROR_NONE                                      ;  //正常返回

    //-------------------内部错误(50000~60000)-------------------
    CEXTERN const int INTERNAL_ERROR                                  ;  //内部错误
    CEXTERN const int FILESYSTEM_ERROR                                ;  //文件系统错误
    CEXTERN const int ARCHIVE_FILE_UNKNOW_ERROR                       ;  //归档文件未知错误
    CEXTERN const int SPLIT_FILE_UNKNOW_ERROR                         ;  //切割文件位置错误
    //-------------------通用错误(20000~30000)-------------------
    CEXTERN const int INPUT_PATH_NOT_EXIST                            ;  //输入路径不存在
    //-------------------压缩相关错误(10001~10100)-------------------
    CEXTERN const int INVALID_COMPRESS_FILE                           ;  //无效的压缩文件
    CEXTERN const int RPC_COMPRESS_BUFF_ALLOC_FAILED                  ;  //RPC压缩内存申请失败
    CEXTERN const int RPC_COMPRESS_IN_FILE_OPEN_FAILED                ;  //RPC压缩输入文件打开失败
    CEXTERN const int COMPRESS_INPUT_BUFF_NOT_ENOUGH                  ;  //输入缓存区不足
    CEXTERN const int COMPRESS_OUTPUT_BUFF_NOT_ENOUGH                 ;  //输出缓冲区不足
    CEXTERN const int COMPRESS_IN_FILE_OPEN_FAILED                    ;  //压缩时输入文件打开失败
    CEXTERN const int COMPRESS_OUT_FILE_OPEN_FAILED                   ;  //压缩时输出文件打开失败
    CEXTERN const int ZLIB_COMPRESS_FAILED                            ;  //调用zlib压缩失败
    CEXTERN const int COMPRESS_REPEAT                                 ;  //重复压缩

    //-------------------解压缩相关错误(10101~10200)-------------------
    CEXTERN const int UNCOMPRESS_INPUT_BUFF_NOT_ENOUGH                ;  //输入缓存区不足
    CEXTERN const int UNCOMPRESS_OUTPUT_BUFF_NOT_ENOUGH               ;  //输出缓冲区不足
    CEXTERN const int UNCOMPRESS_IN_FILE_OPEN_FAILED                  ;  //压缩时输入文件打开失败
    CEXTERN const int UNCOMPRESS_OUT_FILE_OPEN_FAILED                 ;  //压缩时输出文件打开失败
    CEXTERN const int ZLIB_UNCOMPRESS_FAILED                          ;  //调用zlib压缩失败 
    CEXTERN const int INVALID_UNCOMPRESS_FILE                         ;  //无效的解压文件
    CEXTERN const int RPC_UNCOMPRESS_BUFF_ALLOC_FAILED                ;  //RPC解压缩内存申请失败

    //-------------------归档相关错误(10201~10300)-------------------
    CEXTERN const int ARCHIVE_FILE_SYSTEM_ERROR                       ;  //归档时文件系统错误
    CEXTERN const int ARCHIVE_FILE_NOT_EXIST                          ;  //归档时文件不存在

    //-------------------解档相关错误(10301~10400)-------------------
    CEXTERN const int DEARCHIVE_IN_FILE_OPEN_FAILED                   ;  //解档时输入文件打开失败

    //-------------------文件头校验相关错误(10401~10500)-------------------
    CEXTERN const int INVALID_SIGN                                    ;  //文件签名无效
    CEXTERN const int PARSE_FILE_INVALID                              ;  //解析文件无效

    //-------------------加密/解密相关错误(10501~10600)-------------------
    CEXTERN const int DECODE_FILE_HEAD_PARSE_FAILED                   ;  //解密时文件头校验失败
    CEXTERN const int DECODE_PASSWD_ERROR                             ;  //解密密码错误
    CEXTERN const int ENCODE_REPEAT                                   ;  //重复加密
    CEXTERN const int CRYPTO_OUT_FILE_OPEN_FAILED                     ;  //加密/解密时输出文件打开失败

    //-------------------线程相关相关错误(10601~10700)-------------------
    CEXTERN const int SPLIT_IN_FILE_OPEN_FAILED                       ;  //切割时输入文件打开失败
    CEXTERN const int MISMATACH_BTW_TD_AND_FILEPCS                    ;  //线程数目和切分文件数目不匹配

}
#endif