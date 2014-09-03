
#ifndef TAG_FEEDER_FACTORY
#define TAG_FEEDER_FACTORY

#include <memory>

#include "core_export.h"

struct ITagFeeder;

struct CORE_EXPORT TagFeederFactory
{
	TagFeederFactory() = delete;

	static std::shared_ptr<ITagFeeder> get();
};

#endif
