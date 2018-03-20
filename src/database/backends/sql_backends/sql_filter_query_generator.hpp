/*
 * Generator of SELECT queries.
 * Copyright (C) 2015  Michał Walenciak <MichalWalenciak@gmail.com>
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

#ifndef SQLFILTERQUERYGENERATOR_HPP
#define SQLFILTERQUERYGENERATOR_HPP

#include <vector>
#include <set>

#include <QString>
#include <QStack>

#include <core/filter_engine.hpp>
#include <database/filter.hpp>


namespace Database
{

    class SqlFilterQueryGenerator: IFilterEngineCallback
    {
        public:
            SqlFilterQueryGenerator();
            SqlFilterQueryGenerator(const SqlFilterQueryGenerator &) = delete;
            ~SqlFilterQueryGenerator();

            SqlFilterQueryGenerator& operator=(const SqlFilterQueryGenerator &) = delete;

            [[deprecated]] QString generate(const std::vector<IFilter::Ptr> &) const;
            QString generate(const QString& expression);

        private:
            struct ScopeData
            {
                std::set<QString> to_join;
                std::set<QString> where_conditions;
            };

            QStack<ScopeData> m_scopeData;
            QString m_scope;

            void filterPhotos() override;
            void photoFlag(const QString& name, const QString& value) override;
            void photoTag(const QString& name, const QString& value) override;
            void photoTag(const QString& name) override;
            void photoChecksum(const QString &) override;
            void photoID(const QString & ) override;
            void negate() override;

            QString flush();
    };

}

#endif // SQLFILTERQUERYGENERATOR_HPP
