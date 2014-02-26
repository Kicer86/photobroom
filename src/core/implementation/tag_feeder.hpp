
#ifndef TAG_FEEDER_HPP
#define TAG_FEEDER_HPP

#include <memory>
#include <string>

#include "core_export.h"
#include "itagfeeder.hpp"

struct ITagData;

class CORE_EXPORT TagFeeder: public ITagFeeder
{
    public:
        TagFeeder();

        virtual std::unique_ptr<ITagData> getTagsFor(const std::string& path) override;
        virtual void update(ITagData *, const std::string& path) override;
        
    private:
        void feed(const std::string& path, ITagData *);
};

#endif
