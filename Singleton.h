#pragma once
template<class T>
class CSingleton
{
public:
	static T& Instance()
	{
		static T t;
		return t;
	}
};