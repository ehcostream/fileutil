#pragma once
#include <ostream>

class CVersionInfo
{
public:
	CVersionInfo(uint32_t dwMajorVer, uint32_t dwMinorVer, uint32_t dwPatchVer)
		:m_dwMajorVer(dwMajorVer),
		 m_dwMinorVer(dwMinorVer),
		 m_dwPatchVer(dwPatchVer) {}

	~CVersionInfo() = default;
	CVersionInfo& operator= (const CVersionInfo&) = default;
	CVersionInfo(const CVersionInfo&) = default;

	uint32_t GetMajorVer() { return m_dwMajorVer; }
	uint32_t GetMinorVer() { return m_dwMinorVer; }
	uint32_t GetPatchVer() { return m_dwPatchVer; }


	static std::string String()
	{
		std::ostringstream oss;
	    << m_dwMajorVer << "." 
	    << m_dwMinorVer << "." 
	    << m_dwPatchVer;

		return oss.str();
	}

private:
	uint32_t m_dwMajorVer;
	uint32_t m_dwMinorVer;
	uint32_t m_dwPatchVer;
};