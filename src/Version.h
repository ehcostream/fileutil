#pragma once
#include <stdint.h>
/*
版本号规则
	MAJOR_VERSION 主版本号指新的特性加入，或者修复重大功能或者性能问题，需要向下兼容
	MINOR_VERSION 次版本号指API具体实现发生变化，需要向下兼容
	PATCH_VERSION PATCH指对功能做一般性优化，不影响已经产生的文件内容
*/
namespace Version
{
	static const uint32_t MAJOR_VERSION = 1;
	static const uint32_t MINOR_VERSION = 1;
	static const uint32_t PATCH_VERSION = 0;
}
