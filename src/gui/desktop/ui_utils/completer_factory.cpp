/*
 * Photo Broom - photos management tool.
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
#include <QStringListModel>

#include <core/ilogger.hpp>
#include <core/model_compositor.hpp>
#include <database/database_tools/tag_info_collector.hpp>

#include "utils/tag_value_model.hpp"


namespace
{
    QCompleter* buildCompleterFor(IModelCompositorDataSource* source)
    {
        QCompleter* completer = new QCompleter;

        ModelCompositor* model_compositor = new ModelCompositor(completer);
        model_compositor->add(source);

        completer->setModel(model_compositor);

        return completer;
    }
}


CompleterFactory::CompleterFactory(const ILogger& logger):
    m_logger(logger.subLogger("CompleterFactory")),
    m_tagInfoCollector(*m_logger->subLogger("CompleterFactory")),
    m_tagValueModels(),
    m_peopleListModel()
{

}


CompleterFactory::~CompleterFactory()
{

}


void CompleterFactory::set(Database::IDatabase* db)
{
    m_tagInfoCollector.set(db);
    m_peopleListModel.setDB(db);
}


QCompleter* CompleterFactory::createCompleter(const Tag::Types& info)
{
    return createCompleter( std::set<Tag::Types>({info}) );
}


QCompleter* CompleterFactory::createCompleter(const std::set<Tag::Types>& infos)
{
    IModelCompositorDataSource* model = getModelFor(infos);

    return buildCompleterFor(model);
}


QCompleter* CompleterFactory::createPeopleCompleter()
{
    return buildCompleterFor(&m_peopleListModel);
}


IModelCompositorDataSource* CompleterFactory::accessModel(const Tag::Types& tagType)
{
    return getModelFor({tagType});
}


const IModelCompositorDataSource& CompleterFactory::accessPeopleModel()
{
    return m_peopleListModel;
}


IModelCompositorDataSource* CompleterFactory::getModelFor(const std::set<Tag::Types>& infos)
{
    auto it = m_tagValueModels.find(infos);

    if (it == m_tagValueModels.end())
    {
        auto tags_model = std::make_unique<TagValueModel>(infos, &m_tagInfoCollector, *m_logger);

        std::tie(it, std::ignore) = m_tagValueModels.emplace(infos, std::move(tags_model));
    }

    return it->second.get();
}
