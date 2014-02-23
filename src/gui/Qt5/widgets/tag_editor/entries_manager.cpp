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


#include "entries_manager.hpp"

#include "tag_entry.hpp"
#include "configuration/constants.hpp"





EntriesManager::EntriesManager(QObject* p): QObject(p), m_entries(), m_combosModel(), m_data()
{

}


TagEntry* EntriesManager::constructEntry(const TagNameInfo& info, QWidget* p)
{
    std::unique_ptr<TagEntry> tagEntry(new TagEntry(info, p));
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
    const std::set<TagNameInfo> avail = getDefaultValues();

    QString result = "";

    if (avail.empty() == false)
        result = avail.begin()->getName();

    return result;
}


std::set<TagNameInfo> EntriesManager::getDefaultValues()
{
    std::set<TagNameInfo> avail = m_base_tags;

    for (const std::unique_ptr<TagEntry>& entry: m_entries)
    {
        const TagNameInfo& tagInfo = entry->getTagInfo();

        avail.erase(tagInfo);
    }

    return avail;
}


std::set<TagNameInfo> EntriesManager::usedValues() const
{
    std::set<TagNameInfo> used;
    for (const std::unique_ptr<TagEntry>& entry: m_entries)
    {
        const TagNameInfo& n = entry->getTagInfo();

        used.insert(n);
    }

    return used;
}
