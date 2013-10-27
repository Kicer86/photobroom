/*
    Widget for tags manipulation
    Copyright (C) 2013  Michał Walenciak <MichalWalenciak@gmail.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/


#include "tag_editor_widget.hpp"

#include <assert.h>

#include <vector>
#include <iostream>
#include <set>
#include <algorithm>
#include <iterator>

#include <QString>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QStringListModel>
#include <QDebug>

#include "tag_definition.hpp"

#include "core/types.hpp"

struct TagEntry;

struct EntriesManager: public QObject
{
        explicit EntriesManager(QObject* parent = 0);
    
        TagEntry* constructEntry(const QString& name, QWidget* p);
        void removeAllEntries();
        
        const std::vector<std::unique_ptr<TagEntry>>& getTagEntries() const;
        
        QString getDefaultValue();
        std::set<QString> getDefaultValues();
        
    private:
        std::vector<std::unique_ptr<TagEntry>> m_entries;
        static std::set<QString> m_base_tags;        
        QStringListModel m_combosModel;
        QStringList m_data;
        
        std::set<QString> usedValues() const;
        void registerEntry(std::unique_ptr<TagEntry> &&);
};

struct TagEntry: public TagEntrySignals
{
        friend class EntriesManager;
        
        virtual ~TagEntry();

        TagEntry(const TagEntry &) = delete;
        void operator=(const TagEntry &) = delete;

        //void selectTag(const QString &name);
        void setTagValue(const QString &value);
        void clear();
        
        QString getTagName() const;
        QString getTagValue() const;

    private:
        QLabel    *m_tagName;
        QLineEdit *m_tagValue;
        
        explicit TagEntry(const QString &, QWidget *parent, Qt::WindowFlags f = 0);
};


/**************************************************************************/


std::set<QString> EntriesManager::m_base_tags( {QObject::tr("Event"), 
                                                QObject::tr("Place"), 
                                                QObject::tr("Date"), 
                                                QObject::tr("Time"),
                                                QObject::tr("People"} );


EntriesManager::EntriesManager(QObject* p): QObject(p), m_entries(), m_combosModel(), m_data()
{

}


TagEntry* EntriesManager::constructEntry(const QString& name, QWidget* p)
{
    std::unique_ptr<TagEntry> tagEntry(new TagEntry(name, p));
    TagEntry* result = tagEntry.get();
    
    registerEntry(std::move(tagEntry));
    
    return result;
}


void EntriesManager::removeAllEntries()
{
    m_entries.clear();
}


const std::vector<std::unique_ptr<TagEntry>>& EntriesManager::getTagEntries() const
{
    return m_entries;
}


void EntriesManager::registerEntry(std::unique_ptr<TagEntry>&& entry)
{
    m_entries.push_back(std::move(entry));
}


QString EntriesManager::getDefaultValue()
{    
    std::set<QString> avail = m_base_tags;
    
    for (const std::unique_ptr<TagEntry>& entry: m_entries)
    {
        const QString n = entry->getTagName();
        
        avail.erase(n);
    }
    
    QString result = "";
    
    if (avail.empty() == false)
        result = *(avail.begin());
    
    return result;
}


std::set<QString> EntriesManager::usedValues() const
{
    std::set<QString> used;
    for (const std::unique_ptr<TagEntry>& entry: m_entries)
    {
        const QString n = entry->getTagName();
        
        used.insert(n);
    }
    
    return used;
}


/**************************************************************************/


TagEntry::TagEntry(const QString &name, QWidget *p, Qt::WindowFlags f):
    TagEntrySignals(p, f),
    m_tagName(nullptr),
    m_tagValue(nullptr)
{
    m_tagName = new QLabel(name, this);
    m_tagValue  = new QLineEdit(this);

    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->addWidget(m_tagName);
    mainLayout->addWidget(m_tagValue);
    
    connect(m_tagValue, SIGNAL(textEdited(QString)), this, SIGNAL(tagEdited()));
}


TagEntry::~TagEntry()
{

}


void TagEntry::setTagValue(const QString &value)
{
    m_tagValue->setText(value);
}


void TagEntry::clear()
{
    m_tagValue->clear();
    m_tagName->clear();

    //for (const QString &tag: m_baseTags)
    //    m_tagsCombo->addItem(tag);
}


QString TagEntry::getTagName() const
{
    const QString result = m_tagName->text();    
    return result;
}


QString TagEntry::getTagValue() const
{   
    const QString result = m_tagValue->text();
    
    return result;
}


/*****************************************************************************/


struct TagEditorWidget::TagsManager: public TagsManagerSlots
{
        TagsManager(TagEditorWidget* tagWidget):
            TagsManagerSlots(tagWidget),
            m_base_tags( {"Event", "Place", "Date", "Time", "People"}),
            m_tagWidget(tagWidget),
            m_tagData(nullptr),
            m_entriesManager(new EntriesManager(this)),
            m_container(nullptr)
        {
            QLayout* layout = new QVBoxLayout(tagWidget);
            TagDefinition* tag = new TagDefinition(tagWidget);
            m_container = new QWidget(tagWidget);
            
            new QVBoxLayout(m_container);
            
            layout->addWidget(tag);
            layout->addWidget(m_container);
            
            connect(tag, SIGNAL(tagChoosen(QString)), this, SLOT(addLine(QString)));
        }

        TagsManager(const TagsManager&) = delete;
        void operator=(const TagsManager&) = delete;

        void setTags(const std::shared_ptr<ITagData>& tagData)
        {
            if (m_tagData.get() != nullptr)
                std::cout << "saving tags: " << (*m_tagData) << std::endl;
            
            m_tagData.reset();
            
            m_entriesManager->removeAllEntries();
            const ITagData::TagsList& tags = tagData->getTags();

            for (auto tagIt: tags)
            {
                ITagData::TagInfo tag(tagIt);
                addLine(tag.name(), tag.valuesString());
            }
                                    
            m_tagData = tagData;
                   
            std::cout << "got tags: " << (*m_tagData) << std::endl;
        }

    private:
        virtual void addLine(const QString& name) override
        {
            addLine(name, "");
        }
        
        
        void addLine(const QString& name, const QString& value)
        {
            TagEntry* tagEntry = m_entriesManager->constructEntry(name, m_tagWidget);
            
            connect(tagEntry, SIGNAL(tagEdited()), this, SLOT(tagEdited()));

            storeTagEntry(tagEntry);
            tagEntry->setTagValue(value);
        }

        void storeTagEntry(TagEntry* tagEntry)
        {
            QLayout* lay = m_container->layout();
            lay->addWidget(tagEntry);
        }
   
        virtual void tagEdited()
        {            
            if (m_tagData != nullptr)
            {
                m_tagData->clear();
                auto& entries = m_entriesManager->getTagEntries();
                for (const std::unique_ptr<TagEntry>& tagEntry: entries)
                {
                    const QString name = tagEntry->getTagName();
                    const QString value = tagEntry->getTagValue();
                    const QStringList values = value.split(";");  //use some constants                
                    const ITagData::ValuesSet vSet(values.begin(), values.end());
                    
                    m_tagData->setTag(name, vSet);
                }
                
                std::cout << *m_tagData << std::endl;
            }
        }
        
        
        TagEntry* getEditedTag() const
        {
            QObject* obj = QObject::sender();
            assert(qobject_cast<QLineEdit *>(obj) != nullptr);
            
            QLineEdit* lineEdit = static_cast<QLineEdit *>(obj);
            QWidget* lineEditParent = lineEdit->parentWidget();
            assert(dynamic_cast<TagEntry *>(lineEditParent) != nullptr);
            
            TagEntry* tagEntry = static_cast<TagEntry *>(lineEditParent);
            
            return tagEntry;
        }

        std::vector<QString> m_base_tags;
        TagEditorWidget* m_tagWidget;
        std::shared_ptr<ITagData> m_tagData;
        EntriesManager* m_entriesManager;
        QWidget* m_container;
};


/*****************************************************************************/


TagEditorWidget::TagEditorWidget(QWidget *p, Qt::WindowFlags f):
    QWidget(p, f),
    m_manager(new TagsManager(this))
{

}


TagEditorWidget::~TagEditorWidget()
{

}


void TagEditorWidget::setTags(const std::shared_ptr<ITagData>& tagData)
{
    m_manager->setTags(tagData);
}
