#pragma once
template<class T>
class CSingleton
{
public:
	inline static T& Instance()
	{
		static T t;
		return t;
	}
};