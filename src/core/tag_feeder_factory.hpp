
#ifndef TAG_FEEDER_FACTORY
#define TAG_FEEDER_FACTORY

#include <memory>

#include "itagfeeder.hpp"

#include "core_export.h"

class CORE_EXPORT TagFeederFactory: public ITagFeederFactory
{
	public:
		TagFeederFactory();

		std::shared_ptr<ITagFeeder> get() override;
};

#endif
