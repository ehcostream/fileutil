#include "Errors.h"


CEXTERN const int ERROR_NONE                                      = 0;  //正常返回

//-------------------内部错误(50000~60000)-------------------
CEXTERN const int INTERNAL_ERROR                                  = 50000;  //内部错误
CEXTERN const int FILESYSTEM_ERROR                                = 50001;  //文件系统错误
CEXTERN const int ARCHIVE_FILE_UNKNOW_ERROR                       = 50002;  //归档文件未知错误
CEXTERN const int SPLIT_FILE_UNKNOW_ERROR                         = 50003;  //切割文件位置错误

//-------------------压缩相关错误(10001~10100)-------------------
CEXTERN const int INVALID_COMPRESS_FILE                           = 10001;  //无效的压缩文件
CEXTERN const int RPC_COMPRESS_BUFF_ALLOC_FAILED                  = 10002;  //RPC压缩内存申请失败
CEXTERN const int RPC_COMPRESS_IN_FILE_OPEN_FAILED                = 10003;  //RPC压缩输入文件打开失败
CEXTERN const int COMPRESS_INPUT_BUFF_NOT_ENOUGH                  = 10004;  //输入缓存区不足
CEXTERN const int COMPRESS_OUTPUT_BUFF_NOT_ENOUGH                 = 10005;  //输出缓冲区不足
CEXTERN const int COMPRESS_IN_FILE_OPEN_FAILED                    = 10006;  //压缩时输入文件打开失败
CEXTERN const int COMPRESS_OUT_FILE_OPEN_FAILED                   = 10007;  //压缩时输出文件打开失败
CEXTERN const int ZLIB_COMPRESS_FAILED                            = 10008;  //调用zlib压缩失败
CEXTERN const int COMPRESS_REPEAT                                 = 10009;  //重复压缩

//-------------------解压缩相关错误(10101~10200)-------------------
CEXTERN const int UNCOMPRESS_INPUT_BUFF_NOT_ENOUGH                = 10101;  //输入缓存区不足
CEXTERN const int UNCOMPRESS_OUTPUT_BUFF_NOT_ENOUGH               = 10102;  //输出缓冲区不足
CEXTERN const int UNCOMPRESS_IN_FILE_OPEN_FAILED                  = 10103;  //压缩时输入文件打开失败
CEXTERN const int UNCOMPRESS_OUT_FILE_OPEN_FAILED                 = 10104;  //压缩时输出文件打开失败
CEXTERN const int ZLIB_UNCOMPRESS_FAILED                          = 10105;  //调用zlib压缩失败 
CEXTERN const int INVALID_UNCOMPRESS_FILE                         = 10106;  //无效的解压文件
CEXTERN const int RPC_UNCOMPRESS_BUFF_ALLOC_FAILED                = 10107;  //RPC解压缩内存申请失败

//-------------------归档相关错误(10201~10300)-------------------
CEXTERN const int ARCHIVE_FILE_SYSTEM_ERROR                       = 10201;  //归档时文件系统错误
CEXTERN const int ARCHIVE_FILE_NOT_EXIST                          = 10202;  //归档时文件不存在

//-------------------解档相关错误(10301~10400)-------------------
CEXTERN const int DEARCHIVE_IN_FILE_OPEN_FAILED                   = 10301;  //解档时输入文件打开失败

//-------------------文件头校验相关错误(10401~10500)-------------------
CEXTERN const int INVALID_SIGN                                    = 10401;  //文件签名无效
CEXTERN const int PARSE_FILE_INVALID                              = 10402;  //解析文件无效

//-------------------加密/解密相关错误(10501~10600)-------------------
CEXTERN const int DECODE_FILE_HEAD_PARSE_FAILED                   = 10501;  //解密时文件头校验失败
CEXTERN const int DECODE_PASSWD_ERROR                             = 10502;  //解密密码错误
CEXTERN const int ENCODE_REPEAT                                   = 10503;  //重复加密
CEXTERN const int CRYPTO_OUT_FILE_OPEN_FAILED                     = 10504;  //加密/解密时输出文件打开失败

//-------------------线程相关相关错误(10601~10700)-------------------
CEXTERN const int SPLIT_IN_FILE_OPEN_FAILED                       = 10601;  //切割时输入文件打开失败
CEXTERN const int MISMATACH_BTW_TD_AND_FILEPCS                    = 10602;  //线程数目和切分文件数目不匹配