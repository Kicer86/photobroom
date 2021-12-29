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

#ifndef COMPLETERFACTORY_HPP
#define COMPLETERFACTORY_HPP

#include <QObject>
#include <map>

#include <core/tag.hpp>
#include <database/database_tools/tag_info_collector.hpp>
#include "icompleter_factory.hpp"
#include "utils/people_list_model.hpp"

class QAbstractItemModel;

namespace Database
{
    struct IDatabase;
}

class IModelCompositorDataSource;
struct ILoggerFactory;
class PeopleListModel;

/**
 * @brief Factory for QCompleters
 *
 * Created QCompleters will be filled with possible values for given tag type
 */
class CompleterFactory: public QObject, public ICompleterFactory
{
    public:
        explicit CompleterFactory(ILoggerFactory &);
        CompleterFactory(const CompleterFactory &) = delete;
        ~CompleterFactory();
        CompleterFactory& operator=(const CompleterFactory &) = delete;

        void set(Database::IDatabase *);

        QCompleter* createCompleter(const Tag::Types &) override;
        QCompleter* createCompleter(const std::set<Tag::Types> &) override;
        QCompleter* createPeopleCompleter() override;
        IModelCompositorDataSource* accessModel(const Tag::Types &) override;
        const IModelCompositorDataSource& accessPeopleModel() override;

    private:
        TagInfoCollector m_tagInfoCollector;
        std::map<std::set<Tag::Types>, std::unique_ptr<IModelCompositorDataSource>> m_tagValueModels;
        PeopleListModel m_peopleListModel;
        ILoggerFactory& m_loggerFactory;

        IModelCompositorDataSource* getModelFor(const std::set<Tag::Types> &);
};

#endif // COMPLETERFACTORY_HPP
