#pragma once
#include <boost/asio/io_service.hpp>
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>
#include "Singleton.h"

class CThreadPool : public CSingleton<CThreadPool>
{
public:
	~CThreadPool()
	{
		m_ioService.stop();
		m_threadPool.join_all();
	}

	void Init(int n)
	{
		m_threadPool.join_all();
		m_pWorkGurad = std::shared_ptr<boost::asio::io_service::work>(new boost::asio::io_service::work(m_ioService));
		for(int i = 0; i < n; ++i)
		{
			m_threadPool.create_thread(boost::bind(&boost::asio::io_service::run, &m_ioService));
		}
	}

	void JoinAll()
	{
		m_pWorkGurad = nullptr;
		m_threadPool.join_all();
	}
	
	template<class CompletionHandler>
	void Post(CompletionHandler handler)
	{
		m_ioService.post(handler);
	}

private:
	boost::asio::io_service m_ioService;
	boost::thread_group m_threadPool;
	std::shared_ptr<boost::asio::io_service::work> m_pWorkGurad;
};