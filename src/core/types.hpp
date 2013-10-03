
#ifndef TYPES_HPP
#define TYPES_HPP

#include <assert.h>

#include <vector>
#include <map>
#include <set>
#include <memory>

#include <QString>

struct TagNameInfo
{
    QString name;
    char separator;
    
    TagNameInfo(const QString& n, char s = ';'): name(n), separator(s) {}
    
    operator QString() const
    {
        return name;
    }
    
    bool operator==(const TagNameInfo& other) const
    {
        const bool result = name == other.name;
        
        assert(result == false || separator == other.separator);  //if result is true, then separators must be equal
        
        return result;
    }
    
    bool operator<(const TagNameInfo& other) const
    {
        const bool result = name < other.name;
        
        return result;
    }
};


struct ValueTypeInfo
{
    QString m_value;
    
    ValueTypeInfo(const QString& v): m_value(v) {}
    
    operator QString() const
    {
        return m_value;
    }
    
    bool operator<(const ValueTypeInfo& other) const
    {
        const bool result = m_value < other.m_value;
        
        return result;
    }
    
    const QString& value() const
    {
        return m_value;
    }
};


struct ITagData
{
    typedef TagNameInfo NameType;
    typedef ValueTypeInfo ValueType;
    typedef std::set<ValueType> ValuesSet;
    typedef std::map<NameType, ValuesSet> TagsList;
    
    struct TagInfo
    {        
        TagInfo(const TagsList::const_iterator &it): m_name(it->first), m_values(it->second) {}
        TagInfo(const std::pair<NameType, ValuesSet> &data): m_name(data.first), m_values(data.second) {}
        
        TagInfo& operator=(const std::pair<NameType, ValuesSet> &data)
        {
             m_name = data.first; 
             m_values = data.second;
             
             return *this;
        }
        
        QString name() const
        {
            return m_name;
        }
        
        const ValuesSet& values() const
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
            NameType m_name;
            ValuesSet m_values;
    };
    
    virtual ~ITagData();
    
    //get list of tags
    virtual TagsList getTags() const = 0;
    
    //set tag and its values. Overvrite existing tags
    virtual void setTag(const NameType& name, const ValuesSet& values) = 0;
    virtual void setTag(const NameType& name, const ValueType& value) = 0;
};

class TagDataBase: public ITagData
{
    public:
        TagDataBase();
        virtual ~TagDataBase();
        
        using ITagData::setTag;
        virtual void setTag(const NameType &, const ValueType &) override;
};

class TagData: public TagDataBase
{
    public:
        TagData();
        virtual ~TagData();
        
        virtual TagsList getTags() const override;
        
        using TagDataBase::setTag;
        virtual void setTag(const NameType &, const ValuesSet &) override;
        
    private:
        TagsList m_tags;
};


class TagDataComposite: public TagDataBase
{
    public:
        TagDataComposite();
        virtual ~TagDataComposite();
        
        void setTagDatas(const std::vector<std::shared_ptr<ITagData>> &);
        
        TagsList getTags() const override;
        
        using TagDataBase::setTag;
        virtual void setTag(const NameType& name, const ValuesSet& values) override;
        
    private:
        std::vector<std::shared_ptr<ITagData>> m_tags;
};


#endif
