
#ifndef ICOMPLETER_FACTORY_HPP
#define ICOMPLETER_FACTORY_HPP

class QCompleter;

struct TagTypeInfo;

struct ICompleterFactory
{
    virtual ~ICompleterFactory() {}

    virtual QCompleter* createCompleter(const TagTypeInfo &) = 0;
    virtual QCompleter* createCompleter(const std::set<TagTypeInfo> &) = 0;

    virtual QCompleter* createPeopleCompleter() = 0;
};

#endif
