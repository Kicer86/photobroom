
#ifndef TYPES_HPP
#define TYPES_HPP

#include <string>
#include <vector>
#include <map>

struct ITagData
{
    struct TagInfo
    {
        std::string name;
        std::string values;
    };
    
    virtual ~ITagData();
    
    //get list of tags
    virtual std::vector<TagInfo> getTags() const = 0;
    
    //set tag and its values. Overvrite existing tags
    virtual void setTag(const std::string &name, const std::string &values) = 0;
};


class TagData: public ITagData
{
    public:
        TagData();
        virtual ~TagData();
        
        virtual std::vector<TagInfo> getTags() const;
        virtual void setTag(const std::string& name, const std::string& values);
        
    private:
        std::map<std::string, std::string> m_tags;
};


class TagDataComposite: public ITagData
{
    public:
        TagDataComposite();
        virtual ~TagDataComposite();
        
        void setTagDatas(std::vector<ITagData *>);
        
        virtual std::vector< TagInfo > getTags() const;
        virtual void setTag(const std::string& name, const std::string& values);
};


#endif
