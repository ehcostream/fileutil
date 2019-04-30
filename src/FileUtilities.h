#pragma once
#ifdef SYNC
	#include "FileCompresser.h"
	#include "FileUncompresser.h"
	#include "FileEncoder.h"
	#include "FileDecoder.h"
#endif
#ifdef ASYNC
	#include "FileCompresserAsync.h"
	#include "FileUncompresserAsync.h"
	#include "FileEncoderAsync.h"
	#include "FileDecoderAsync.h"
#endif