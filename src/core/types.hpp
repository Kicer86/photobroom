
#ifndef TYPES_HPP
#define TYPES_HPP

#include <vector>
#include <map>

#include <QString>

struct ITagData
{
    struct TagInfo
    {
        QString name;
        QString values;
    };
    
    virtual ~ITagData();
    
    //get list of tags
    virtual std::vector<TagInfo> getTags() const = 0;
    
    //set tag and its values. Overvrite existing tags
    virtual void setTag(const QString &name, const QString &values) = 0;
};


class TagData: public ITagData
{
    public:
        TagData();
        virtual ~TagData();
        
        virtual std::vector<TagInfo> getTags() const;
        virtual void setTag(const QString& name, const QString& values);
        
    private:
        std::map<QString, QString> m_tags;
};


class TagDataComposite: public ITagData
{
    public:
        TagDataComposite();
        virtual ~TagDataComposite();
        
        void setTagDatas(const std::vector< ITagData* >&);
        
        virtual std::vector< TagInfo > getTags() const;
        virtual void setTag(const QString& name, const QString& values);
};


#endif
