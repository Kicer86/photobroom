
#ifndef ICORE_FACTORY_HPP
#define ICORE_FACTORY_HPP

struct ILoggerFactory;

struct ICoreFactory
{
    virtual ~ICoreFactory() = default;

    virtual ILoggerFactory* getLoggerFactory() = 0;
};

#endif
