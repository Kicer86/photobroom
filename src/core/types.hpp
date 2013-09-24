
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
        TagInfo(const TagsList::const_iterator &it): m_name(&it->first), m_values(&it->second) {}
        TagInfo(const std::pair<QString, QString> &data): m_name(&data.first), m_values(&data.second) {}
        
        TagInfo operator=(const std::pair<QString, QString> &data)
        {
             m_name = &data.first; 
             m_values = &data.second; 
        }
        
        const QString& name() const
        {
            return *m_name;
        }
        
        const QString& values() const
        {
            return *m_values;
        }
        
        private:            
            const QString *m_name;
            const QString *m_values;
    };
    
    virtual ~ITagData();
    
    //get list of tags
    virtual TagsList getTags() const = 0;
    
    //set tag and its values. Overvrite existing tags
    virtual void setTag(const QString &name, const QString &values) = 0;
};


class TagData: public ITagData
{
    public:
        TagData();
        virtual ~TagData();
        
        virtual TagsList getTags() const override;
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
        
        TagsList getTags() const override;
        virtual void setTag(const QString& name, const QString& values) override;
        
    private:
        std::vector<ITagData*> m_tags;
};


#endif
