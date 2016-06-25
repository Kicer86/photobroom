
#ifndef ICOMPLETER_FACTORY_HPP
#define ICOMPLETER_FACTORY_HPP

class QCompleter;

class TagNameInfo;

struct ICompleterFactory
{
    virtual ~ICompleterFactory() {}

    virtual QCompleter* createCompleter(const TagNameInfo &);
};

#endif
