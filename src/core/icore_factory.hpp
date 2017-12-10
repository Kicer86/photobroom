
#ifndef ICORE_FACTORY_HPP
#define ICORE_FACTORY_HPP

struct IConfiguration;
struct IExifReaderFactory;
struct ILoggerFactory;
struct ITaskExecutor;

struct ICoreFactory
{
    virtual ~ICoreFactory() = default;

    virtual ILoggerFactory* getLoggerFactory() = 0;
    virtual IExifReaderFactory* getExifReaderFactory() = 0;
    virtual IConfiguration* getConfiguration() = 0;
    virtual ITaskExecutor* getTaskExecutor() = 0;
};

#endif
