
#ifndef MOCK_CORE_FACTORY_ACCESSOR_HPP_INCLUDED
#define MOCK_CORE_FACTORY_ACCESSOR_HPP_INCLUDED

#include <gmock/gmock.h>

#include <core/icore_factory_accessor.hpp>

class ICoreFactoryAccessorMock: public ICoreFactoryAccessor
{
    public:
        MOCK_METHOD(ILoggerFactory&, getLoggerFactory, (), (override));
        MOCK_METHOD(IExifReaderFactory&, getExifReaderFactory, (), (override));
        MOCK_METHOD(IConfiguration&, getConfiguration, (), (override));
        MOCK_METHOD(ITaskExecutor&, getTaskExecutor, (), (override));
};

#endif
