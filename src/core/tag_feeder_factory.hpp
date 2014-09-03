
#ifndef TAG_FEEDER_FACTORY
#define TAG_FEEDER_FACTORY

#include <memory>
#include <map>
#include <thread>

#include "itagfeeder.hpp"

#include "core_export.h"

class CORE_EXPORT TagFeederFactory: public ITagFeederFactory
{
	public:
		TagFeederFactory();

		std::shared_ptr<ITagFeeder> get() override;

	private:
		std::map<std::thread::id, std::shared_ptr<ITagFeeder>> m_feeders;
};

#endif
