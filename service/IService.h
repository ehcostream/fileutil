#pragma once 

#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace grpc
{
	class Service;
};

class ServicePool
{
public:
	static ServicePool& Instance()
	{
		static ServicePool pool;
		return pool;
	};

	template<class T>
	bool Alloc()
	{
		T* t = new T();
		if(t != nullptr)
		{
			m_servicePool.emplace_back(t);
			return true;
		}
		else
		{
			return false;
		}
	}

	const std::vector<::grpc::Service*>& GetPoolData() { return m_servicePool; }

	uint32_t GetPoolSize() { return m_servicePool.size(); }

private:
	ServicePool() = default;
	~ServicePool() = default;
private:
	std::vector<::grpc::Service*> m_servicePool;
};



extern "C"
{
	extern ::grpc::Service** CreateServices(int* count);
}