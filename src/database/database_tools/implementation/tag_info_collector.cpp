/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2016  Michał Walenciak <MichalWalenciak@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "../tag_info_collector.hpp"

#include <memory>

#include "idatabase.hpp"


TagInfoCollector::TagInfoCollector(): m_tags(), m_database(nullptr)
{

}


TagInfoCollector::~TagInfoCollector()
{

}


void TagInfoCollector::set(Database::IDatabase* db)
{
    m_database = db;

    using namespace std::placeholders;
    auto result = std::bind(&TagInfoCollector::gotTagNames, this, _1);
    m_database->listTagNames(result);
}


const std::set<TagValue>& TagInfoCollector::get(const TagNameInfo& info) const
{
    return m_tags[info];
}


void TagInfoCollector::gotTagNames(const std::deque<TagNameInfo>& names)
{

}

