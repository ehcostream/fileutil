set THIRD_PARTY_DIR=%cd:\=/%/distrib/grpc/example_build
set OPENSSL_DIR=%cd:\=/%/distrib/grpc/OpenSSL-Win32

mkdir build
cd build

cmake .. -G"Visual Studio 14 2015" -DProtobuf_DIR:PATH=%THIRD_PARTY_DIR%/protobuf/cmake -Dc-ares_DIR:PATH=%THIRD_PARTY_DIR%/c-ares/lib/cmake/c-ares -DZLIB_ROOT:STRING=%THIRD_PARTY_DIR%/zlib -DOPENSSL_ROOT_DIR:PATH=%OPENSSL_DIR%/OpenSSL-Win32 -DgRPC_DIR:PATH=%THIRD_PARTY_DIR%/grpc/lib/cmake/grpc -DBOOST_ROOT=c:/Boost -DgRPC_ROOT:PATH=%THIRD_PARTY_DIR%/grpc -DCMAKE_INSTALL_PREFIX=c:/futil
cmake --build . --config Release --target install
echo Project has been built successfully!