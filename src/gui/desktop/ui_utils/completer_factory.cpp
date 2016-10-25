/*
 * Factory for QCompleter for particular tag type
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

#include "completer_factory.hpp"

#include <QCompleter>

#include <database/database_tools/tag_info_collector.hpp>
#include <utils/tag_value_model.hpp>


CompleterFactory::CompleterFactory(): m_tagInfoCollector(), m_tagValueModels(), m_loggerFactory(nullptr)
{

}


CompleterFactory::~CompleterFactory()
{

}


void CompleterFactory::set(Database::IDatabase* db)
{
    m_tagInfoCollector.set(db);
}


void CompleterFactory::set(ILoggerFactory* lf)
{
    m_loggerFactory = lf;
}


QCompleter* CompleterFactory::createCompleter(const TagNameInfo& info)
{
    return createCompleter( std::set<TagNameInfo>({info}) );
}


QCompleter* CompleterFactory::createCompleter(const std::set<TagNameInfo>& infos)
{
    TagValueModel* model = getModelFor(infos);

    QCompleter* result = new QCompleter(model);
    return result;
}


TagValueModel* CompleterFactory::getModelFor(const std::set<TagNameInfo>& infos)
{
    auto it = m_tagValueModels.find(infos);

    if (it == m_tagValueModels.end())
    {
        assert(m_loggerFactory != nullptr);

        auto model = std::make_unique<TagValueModel>(infos);
        model->set(m_loggerFactory);
        model->set(&m_tagInfoCollector);

        auto insert_it = m_tagValueModels.insert( std::make_pair(infos, std::move(model)) );

        it = insert_it.first;
    }

    return it->second.get();
}
