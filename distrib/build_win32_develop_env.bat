@rem Copyright 2019 spectrum.
@rem Download the boost_1_70_0 library originally installed from https://dl.bintray.com/boostorg/release/1.70.0/source/boost_1_70_0.zip

@rem Download the boost,and it will take serveral minutes,please be patient.
powershell -Command "(New-Object Net.WebClient).DownloadFile('http://dl.bintray.com/boostorg/release/1.70.0/source/boost_1_70_0.zip', 'boost_1_70_0.zip')"
powershell -Command "Add-Type -Assembly 'System.IO.Compression.FileSystem'; [System.IO.Compression.ZipFile]::ExtractToDirectory('boost_1_70_0.zip', '.')"

@rem set absolute path to boost_1_70_0 with forward slashes
set BOOST_DIR=%cd:\=/%/boost_1_70_0/
cd %BOOST_DIR%

@rem build the boost with required libraries include thread,filesystem,date_time,and install them into c:\boost
call bootstrap.bat --prefix=c:\boost
@rem default compile the boost with msvc14.0(VS2015) in x64 mode
.\b2.exe toolset=msvc-14.0 address-model=64 architecture=x86 threading=multi --build-type=minimal --with-date_time --with-filesystem --with-thread link=static runtime-link=static -j8 install
@rem return the original position
cd /d %~dp0
@rem Clone the gRPC repository (including submodules)
powershell git clone --recursive -b ((New-Object System.Net.WebClient).DownloadString(\"https://grpc.io/release\").Trim()) https://github.com/grpc/grpc
set gRPC_DIR=%cd:\=/%/grpc

cd %gRPC_DIR%\test\distrib\cpp

@rem build all the dependent libraries thoese are needed by gRPC
call run_distrib_test_cmake_as_externalproject.bat