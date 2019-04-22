#pragma once
#include <sstream>
#include "Version.h"
class CVersionInfo
{
public:
	CVersionInfo(uint32_t dwMajorVer, uint32_t dwMinorVer, uint32_t dwPatchVer)
		:m_dwMajorVer(dwMajorVer),
		 m_dwMinorVer(dwMinorVer),
		 m_dwPatchVer(dwPatchVer) {}

	~CVersionInfo() = default;
	CVersionInfo& operator= (const CVersionInfo& version)
	{
		if(this != &version)
		{
			m_dwMajorVer = version.m_dwMajorVer;
			m_dwMinorVer = version.m_dwMinorVer;
			m_dwPatchVer = version.m_dwPatchVer;
		}
		return *this;
	}
	
	CVersionInfo(const CVersionInfo& version)
	{
		m_dwMajorVer = version.m_dwMajorVer;
		m_dwMinorVer = version.m_dwMinorVer;
		m_dwPatchVer = version.m_dwPatchVer;
	}

	uint32_t GetMajorVer() { return m_dwMajorVer; }
	uint32_t GetMinorVer() { return m_dwMinorVer; }
	uint32_t GetPatchVer() { return m_dwPatchVer; }

	std::string Print()
	{
		std::ostringstream oss;
	    oss << m_dwMajorVer << "." 
	    << m_dwMinorVer << "." 
	    << m_dwPatchVer;
		return oss.str();
	}

	static std::string String()
	{
		std::ostringstream oss;
	    oss << Version::MAJOR_VERSION << "." 
	    << Version::MINOR_VERSION << "." 
	    << Version::PATCH_VERSION;

		return oss.str();
	}

private:
	uint32_t m_dwMajorVer;
	uint32_t m_dwMinorVer;
	uint32_t m_dwPatchVer;
};