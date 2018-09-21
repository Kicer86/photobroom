
#ifndef ICOMPLETER_FACTORY_HPP
#define ICOMPLETER_FACTORY_HPP

class QCompleter;

struct TagNameInfo;

struct ICompleterFactory
{
    virtual ~ICompleterFactory() {}

    virtual QCompleter* createCompleter(const TagNameInfo &) = 0;
    virtual QCompleter* createCompleter(const std::set<TagNameInfo> &) = 0;

    virtual QCompleter* createPeopleCompleter() = 0;
};

#endif
