
#ifndef ICORE_FACTORY_ACCESSOR_HPP
#define ICORE_FACTORY_ACCESSOR_HPP

struct IConfiguration;
struct IExifReaderFactory;
struct ILoggerFactory;
struct ITaskExecutor;

struct ICoreFactoryAccessor
{
    virtual ~ICoreFactoryAccessor() = default;

    virtual ILoggerFactory& getLoggerFactory() = 0;
    virtual IExifReaderFactory& getExifReaderFactory() = 0;
    virtual IConfiguration& getConfiguration() = 0;
    virtual ITaskExecutor& getTaskExecutor() = 0;
};

#endif
