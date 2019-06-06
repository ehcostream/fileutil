#ifndef __UN_COPYABLE__
#define __UN_COPYABLE__
class CUncopyable 
{
protected:
	CUncopyable() = default;
	~CUncopyable() = default;
private:
	CUncopyable(const CUncopyable&);
	CUncopyable& operator=(const CUncopyable&);
};
#endif