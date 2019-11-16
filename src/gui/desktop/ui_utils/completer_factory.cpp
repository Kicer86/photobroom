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
#include <QLocale>

#include <database/database_tools/tag_info_collector.hpp>

#include "utils/people_list_model.hpp"
#include "utils/tag_value_model.hpp"
#include "utils/variant_display.hpp"


namespace
{
    class VariantToStringModelProxy final: public QAbstractListModel
    {
        public:
            VariantToStringModelProxy(QAbstractItemModel* model):
                QAbstractListModel(),
                m_model(model)
            {
            }

            ~VariantToStringModelProxy() = default;

            QVariant data(const QModelIndex& index, int role) const override
            {
                const QLocale locale;
                const QVariant d = m_model->data(index, role);

                const QVariant result = role == Qt::DisplayRole || role == Qt::EditRole?
                    localize(d, locale):
                    d;

                return result;
            }

            int rowCount(const QModelIndex& parent) const override
            {
                return m_model->rowCount(parent);
            }

        private:
            QAbstractItemModel* m_model;
    };
}


CompleterFactory::CompleterFactory():
    m_tagInfoCollector(),
    m_tagValueModels(),
    m_peopleListModel(new PeopleListModel),
    m_loggerFactory(nullptr)
{

}


CompleterFactory::~CompleterFactory()
{
    delete m_peopleListModel, m_peopleListModel = nullptr;
}


void CompleterFactory::set(Database::IDatabase* db)
{
    m_tagInfoCollector.set(db);

    m_peopleListModel->setDB(db);
}


void CompleterFactory::set(ILoggerFactory* lf)
{
    m_loggerFactory = lf;
}


QCompleter* CompleterFactory::createCompleter(const TagTypes& info)
{
    return createCompleter( std::set<TagTypes>({info}) );
}


QCompleter* CompleterFactory::createCompleter(const std::set<TagTypes>& infos)
{
    QAbstractItemModel* model = getModelFor(infos);

    QCompleter* result = new QCompleter(model);
    return result;
}


QCompleter* CompleterFactory::createPeopleCompleter()
{
    return new QCompleter(m_peopleListModel);
}


QAbstractItemModel* CompleterFactory::accessModel(const TagTypes& tagType)
{
    return getModelFor({tagType});
}


QAbstractItemModel* CompleterFactory::getModelFor(const std::set<TagTypes>& infos)
{
    auto it = m_tagValueModels.find(infos);

    if (it == m_tagValueModels.end())
    {
        assert(m_loggerFactory != nullptr);

        auto tags_model = std::make_unique<TagValueModel>(infos);
        tags_model->set(m_loggerFactory);
        tags_model->set(&m_tagInfoCollector);

        auto proxy_model = std::make_unique<VariantToStringModelProxy>(tags_model.get());

        ModelPair models = ModelPair( std::move(proxy_model), std::move(tags_model) );
        std::tie(it, std::ignore) = m_tagValueModels.insert( std::make_pair(infos, std::move(models)) );
    }

    return it->second.first.get();
}


QAbstractItemModel* CompleterFactory::getModelForPeople()
{
    assert(m_peopleListModel != nullptr);

    return m_peopleListModel;
}
