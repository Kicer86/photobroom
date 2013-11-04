/*
    Manager for TagEntries
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


#ifndef ENTRIES_MANAGER_HPP
#define ENTRIES_MANAGER_HPP

#include <memory>
#include <set>

#include <QObject>
#include <QStringListModel>

#include "tag_entry.hpp"

class TagEntry;

class EntriesManager: public QObject
{
    public:
        explicit EntriesManager(QObject* parent = 0);
    
        TagEntry* constructEntry(const TagInfo& name, QWidget* p);
        void removeAllEntries();
        
        const std::vector<std::unique_ptr<TagEntry>>& getTagEntries() const;
        
        QString getDefaultValue();
        std::set< TagInfo > getDefaultValues();
        
    private:
        std::vector<std::unique_ptr<TagEntry>> m_entries;
        static std::set<TagInfo> m_base_tags;        
        QStringListModel m_combosModel;
        QStringList m_data;
        
        std::set<QString> usedValues() const;
        void registerEntry(std::unique_ptr<TagEntry> &&);
};

#endif // ENTRIES_MANAGER_HPP
