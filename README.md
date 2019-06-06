FILEUTIL - 使用指南
===========================

# 准备工作

## Linux
输入以下命令安装依赖环境
```sh
 $ [sudo] apt-get install wget
 $ [sudo] apt-get install build-essential autoconf libtool pkg-config
 $ [sudo] apt-get install python3 python3-dev golang
```
```sh
 $ [sudo] apt-get install libgflags-dev libgtest-dev
 $ [sudo] apt-get install clang libc++-dev
```

## Windows

项目使用cmake + msvc14.0进行编译，安装以下应用
- 安装 [Visual Studio 2015]()  (依赖 Visual C++ 编译器).
- 安装 [Git](https://git-scm.com/) 添加到 `PATH` 环境变量中
- 安装 [CMake](https://cmake.org/download/). 选择自动添加到 `PATH` 环境变量中
- 安装 [Active State Perl](https://www.activestate.com/activeperl/)
- 安装 [Go](https://golang.org/dl/) 
- 安装 [yasm](http://yasm.tortall.net/) 添加到 `PATH` 环境变量中


# 从Git拉取源代码 
从git上拉取 `fileutil`源代码 (windows打开命令行工具，或者powershell)
```sh
$ git clone https://github.com/ehcostream/fileutil.git
```

# 部署开发环境

## Docker
可以直接拉取已经配置完成的镜像文件，进行**编译**和**使用**
- 需要安装docker(自行安装)，windows请在powershell或者CMD命令行工具中执行
```sh
docker login
docker pull ehcostream/ubuntu:fileutil
docker run -it ehcostream/ubuntu:fileutil
```
这样就进入生成的容器中了，已经安装所有依赖项，可以直接进行**编译**和**使用**
```sh
$ cd /home/spectrum/Work/fileutil
$ git pull --rebase
```
编译步骤参考Linux

## Linux
### 安装依赖库
```sh
$ [sudo] ./fileutil/distrib/build_linux_develop_env.sh
```
这个脚本将会编译和安装**boost**，**grpc**和它所依赖的第三方库cares，zlib，openssl，protobuf.
等待脚本执行完成，项目所依赖的开发环境就构建完成，由于**GFW**的原因，建议使用**VPN**加速，正常下载比较缓慢
### 编译
```sh
$ [sudo] ./build.sh
```
编译完成后头生成文件和库文件
- /usr/local/include/futil
- /usr/local/lib/libfutil.so

## Windows
### 安装依赖库(建议使用powershell，使用Administrator启动)
```sh
PS .\fileutil\distrib\build_win32_develop_env.bat
```
### 编译
```sh
PS .\win32_build.bat
```

编译完成后头生成文件和库文件
- c:\futil\include
- c:\futil\lib

# 使用
主要有以下功能

- 压缩
- 解压
- 加密
- 解密
- 异步压缩
- 异步解压
- 异步加密
- 异步解密

详细使用方法参考test/test.cpp源码

## Linux
### 编译example
进入test目录
```sh
$ cd test
$ mkdir build && cd build && cmake .. && make
```
```sh
./test
```
### 依赖项
linux上运行依赖**libcares.so**,**libz.so**，移植时需要一并移植

## Windows
### 编译example
打开powershell，进入项目所在目录
```sh
PS cd test
PS mkdir build && cd build
PS cmake .. -G"Visual Stuido 14 2015"
PS cmake --build --config Release
```
执行
```sh
.\Release\test.exe
```

### 依赖项
windwos运行时仅依赖**futil.dll**，需要拷贝到程序运行目录