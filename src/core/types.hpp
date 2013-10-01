
#ifndef TYPES_HPP
#define TYPES_HPP

#include <vector>
#include <map>
#include <set>

#include <QString>

struct ITagData
{
    typedef QString ValueType;
    typedef std::set<ValueType> TagValues;
    typedef std::map<QString, TagValues> TagsList;
    
    struct TagInfo
    {        
        TagInfo(const TagsList::const_iterator &it): m_name(it->first), m_values(it->second) {}
        TagInfo(const std::pair<QString, std::set<QString>> &data): m_name(data.first), m_values(data.second) {}
        
        TagInfo& operator=(const std::pair<QString, std::set<QString>> &data)
        {
             m_name = data.first; 
             m_values = data.second;
             
             return *this;
        }
        
        const QString& name() const
        {
            return m_name;
        }
        
        const TagValues& values() const
        {
            return m_values;
        }
        
        QString valuesString() const
        {
            QString result;
            
            for(const QString &str: m_values)
            {
                result += str + " ";                //TODO: temporary
            }
            
            return result.simplified();
        }
        
        private:            
            QString m_name;
            TagValues m_values;
    };
    
    virtual ~ITagData();
    
    //get list of tags
    virtual TagsList getTags() const = 0;
    
    //set tag and its values. Overvrite existing tags
    virtual void setTag(const QString& name, const TagValues& values) = 0;
    virtual void setTag(const QString& name, const ValueType& value) = 0;
};

class TagDataBase: public ITagData
{
    public:
        TagDataBase();
        virtual ~TagDataBase();
        
        using ITagData::setTag;
        virtual void setTag(const QString &, const ValueType &) override;
};

class TagData: public TagDataBase
{
    public:
        TagData();
        virtual ~TagData();
        
        virtual TagsList getTags() const override;
        
        using TagDataBase::setTag;
        virtual void setTag(const QString &, const TagValues &) override;
        
    private:
        TagsList m_tags;
};


class TagDataComposite: public TagDataBase
{
    public:
        TagDataComposite();
        virtual ~TagDataComposite();
        
        void setTagDatas(const std::vector<ITagData *> &);
        
        TagsList getTags() const override;
        
        using TagDataBase::setTag;
        virtual void setTag(const QString& name, const TagValues& values) override;
        
    private:
        std::vector<ITagData*> m_tags;
};


#endif
