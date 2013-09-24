
#ifndef TYPES_HPP
#define TYPES_HPP

#include <vector>
#include <map>

#include <QString>

struct ITagData
{
    typedef std::map<QString, QString> TagsList;
    
    struct TagInfo
    {
        QString name;
        QString values;
    };
    
    virtual ~ITagData();
    
    //get list of tags
    virtual const TagsList& getTags() const = 0;
    
    //set tag and its values. Overvrite existing tags
    virtual void setTag(const QString &name, const QString &values) = 0;
};


class TagData: public ITagData
{
    public:
        TagData();
        virtual ~TagData();
        
        virtual const TagsList& getTags() const override;
        virtual void setTag(const QString& name, const QString& values) override;
        
    private:
        TagsList m_tags;
};


class TagDataComposite: public ITagData
{
    public:
        TagDataComposite();
        virtual ~TagDataComposite();
        
        void setTagDatas(const std::vector< ITagData* >&);
        
        virtual const TagsList& getTags() const override;
        virtual void setTag(const QString& name, const QString& values) override;
        
    private:
        std::vector<ITagData*> m_tags;
};


#endif
