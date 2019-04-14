# README.md
## 功能介绍
zlibutil是一款使用zlib为压缩算法的c++工具库，此外，还提供了加密/解密的功能。
::主要功能如下::
* 多文件（夹）归档
* 多文件（夹）压缩，解压
* 多线程压缩
* 文件加密，解密

## 如何工作
**压缩的工作流程**
1. 将多个文件进行归档为单个文件
2. 将单个文件读取到ifstream中
3. 将ifstream中的内容输出到zio::ofstream中
4. zio::ofstream申请一个2*size大小的压缩流缓存区in_buffer/out_buffer
5. 初始化in_buffer，读取ifstream中到内容，压缩到out_buffer中去，当out_buffer达到size大小了，将结果输出到文件流中
6. 循环5步骤，直到zlib压缩函数deflate返回Z_STREAM_END,Z_BUF_ERROR或者out_buffer已经没有可写的内容
7. 压缩结束

**多线程压缩**
1. 将多个文件进行归档为单个文件
2. 根据指定的cpu数目n，切割为n子文件
3. 开启n个线程，并根据缓存区大小为每个线程平均分配合适的缓存区大小
4. 每个线程执行**压缩的工作流程**的2，3，4，5，6，7步骤
5. 等待所有线程结束
6. 重组所有线程结果并输出到最终的压缩文件中去
7. 压缩结束

**加密/解谜**
1. 读取单个文件
2. 判断是否为加密文件，如果已经加密则直接退出。
3. 如果为加密，则首先写入加密头信息（加密文件后缀名+原始文件名称）
4. 将文件中的字节流进行取反
5. 输出加密/解谜文件

## 安装
### linux
安装boost
安装zlib
下载源码
在文件目录中cmake .
Make生成执行文件bin/fz即可
### windows
安装boost
安装zlib
配置环境变量
下载源码
cmake .
make生成bin/fz即可
## 如何使用
压缩
```
./bin/fz -c [filename1 filename2 ... n][directory1 directory2... n] -o directory
```

解压
```
./bin/fz -u compressed_filename -o directory
```

加密
```
./bin/fz -e compressed_filename -o directory
```

解密
```
./bin/fz -d encoded_filename -o directory
```
#zlibutil

