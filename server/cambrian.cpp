#include <iostream>
#include <memory>
#include <string>
#include <sstream>
#include <vector>
#include <gflags/gflags.h>
#include <dlfcn.h>
#include <unistd.h>
#include <dirent.h>
#include <cstring>

#include <grpcpp/grpcpp.h>
#include <grpcpp/impl/codegen/async_generic_service.h>
#include <grpcpp/impl/codegen/async_stream.h>
#include <grpcpp/impl/codegen/async_unary_call.h>
#include <grpcpp/impl/codegen/client_callback.h>
#include <grpcpp/impl/codegen/method_handler_impl.h>
#include <grpcpp/impl/codegen/proto_utils.h>
#include <grpcpp/impl/codegen/rpc_method.h>
#include <grpcpp/impl/codegen/server_callback.h>
#include <grpcpp/impl/codegen/service_type.h>
#include <grpcpp/impl/codegen/status.h>
#include <grpcpp/impl/codegen/stub_options.h>
#include <grpcpp/impl/codegen/sync_stream.h>


using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReaderWriter;
using grpc::Status;

typedef void* SERVICE_SO_HANDLE;
typedef ::grpc::Service** (*CreateServicesFunc)(int*);


//启动参数使用gflags
DEFINE_string(server, "0.0.0.0:8000", "IP Address of server");
DEFINE_string(lib_dir, "", "The directory of dynamic library");
DEFINE_bool(load_balancer, false, "Whether open the load banlance or not");


bool GetAllSoFiles(const std::string& rstrPath, std::vector<std::string>& rstSoFilesPath)
{
	DIR *dir;
	struct dirent *ptr;

	if(rstrPath.length() > 1000)
	{
		std::cout << "File path is too long, must be low than 1000" << std::endl;
		return false;
	}

	if ((dir = opendir(rstrPath.c_str())) == NULL)
	{
		std::cout << "Failed to open the dir :" << rstrPath << std::endl;
		return false;
	}

	while ((ptr = readdir(dir)) != NULL)
	{
		if (strcmp(ptr->d_name, ".") == 0 || strcmp(ptr->d_name, "..") == 0)
		{
			continue;
		}
		else if (ptr->d_type == 8)
		{
			// file
			std::ostringstream oss;
			oss << rstrPath << "/" << std::string(ptr->d_name);

			rstSoFilesPath.push_back(oss.str());
		}
		else if (ptr->d_type == 4)
		{
			// dir
			std::ostringstream oss;
			oss << rstrPath << "/" << std::string(ptr->d_name);
			GetAllSoFiles(oss.str(), rstSoFilesPath);
		}
	}
	closedir(dir);
	return true;
}

int RunServer()
{
	int nError = 0;
	std::unique_ptr<Server> server;
	std::unique_ptr<ServerBuilder> builder;
	do
	{
		std::string server_address(FLAGS_server);
		//读取指定目录的so
		builder = std::unique_ptr<ServerBuilder>(new ServerBuilder());
		builder->AddListeningPort(server_address, grpc::InsecureServerCredentials());

		std::vector<std::string> stSoFilesPath;
		bool bDone = GetAllSoFiles(FLAGS_lib_dir, stSoFilesPath);
		if(!bDone)
		{
			nError = 1;
			break;
		}

		for(auto it = stSoFilesPath.begin(); it != stSoFilesPath.end(); ++it)
		{
			SERVICE_SO_HANDLE pstHandle = dlopen((*it).c_str(), RTLD_LAZY);
			if(pstHandle == nullptr)
			{
				nError = 1;
				std::cout << "Load service so failed, so: " << *it << ", Error: " << dlerror() << std::endl;
				break;
			}

			CreateServicesFunc pstCreateServicesFunc = (CreateServicesFunc)dlsym(pstHandle, "CreateServices");
			if(pstCreateServicesFunc == nullptr)
			{
				nError = 2;
				std::cout << "CreateFactory failed, Invalid factory create function" << std::endl;
				break;
			}
		
			int nServiceCount = 0;
			::grpc::Service** pstServices = pstCreateServicesFunc(&nServiceCount);
			if(pstServices == nullptr || nServiceCount == 0)
			{
				nError = 3;
				std::cout << "SO occured a unknown error" << std::endl;
				break;
			}
			std::cout << "So load successfully, count: " << nServiceCount << std::endl;
			
			for(int n = 0; n < nServiceCount; ++n)
			{
				builder->RegisterService(pstServices[n]);
			}

		}

		if(nError != 0)
		{
			break;
		}
		server = std::unique_ptr<Server>(builder->BuildAndStart());		

	}while(false);

	if(nError != 0)
	{
		std::cout << "Failed to run the server :" << FLAGS_server << ", Error: " << nError << std::endl;	
	}
	else
	{
		std::cout << "Server Listening On " << FLAGS_server << std::endl;
		server->Wait();
	}

	server->Wait();

	return nError;
}

int main(int argc, char** argv)
{
	GFLAGS_NS::ParseCommandLineFlags(&argc, &argv, true);
	RunServer();
	return 0;
}
