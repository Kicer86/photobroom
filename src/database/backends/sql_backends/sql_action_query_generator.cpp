/*
 * Generator for IAction.
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

#include "sql_action_query_generator.hpp"

#include "tables.hpp"

namespace Database
{

    struct ActionsVisitor final: Database::IActionVisitor
    {
        ActionsVisitor(): m_result() {}
        virtual ~ActionsVisitor() {}

        QString visit(const IAction::Ptr& action)
        {
            m_result.clear();
            action->visitMe(this);

            return m_result;
        }

        private:
            QString m_result;

            virtual void visit(ModifyFlagAction* action) override
            {
                QStringList setActions;

                for(auto entry: action->flags)
                {
                    const QString name = getFlagName(entry.first);
                    const int value = entry.second;
                    const QString setAction = QString("%1 = %2").arg(name).arg(value);

                    setActions.append(setAction);
                }

                const char* table = TAB_FLAGS;
                const QString assignments = setActions.join(", ");

                m_result = QString("UPDATE %1 SET %2 WHERE photo_id IN (%3)").arg(table).arg(assignments);
            }

            QString getFlagName(Photo::FlagsE flag) const
            {
                QString result;

                switch(flag)
                {
                    case Photo::FlagsE::StagingArea:     result = FLAG_STAGING_AREA;  break;
                    case Photo::FlagsE::ExifLoaded:      result = FLAG_TAGS_LOADED;   break;
                    case Photo::FlagsE::Sha256Loaded:    result = FLAG_SHA256_LOADED; break;
                    case Photo::FlagsE::ThumbnailLoaded: result = FLAG_THUMB_LOADED;  break;
                    case Photo::FlagsE::GeometryLoaded:  result = FLAG_GEOM_LOADED;   break;
                }

                return result;
            }
    };


    ////////////////////////////////////////////////////////////////////////////


    SqlActionQueryGenerator::SqlActionQueryGenerator()
    {

    }


    SqlActionQueryGenerator::~SqlActionQueryGenerator()
    {

    }


    QString SqlActionQueryGenerator::generate(const IAction::Ptr& action) const
    {
        ActionsVisitor visitor;
        const QString actionQuery = visitor.visit(action);

        return actionQuery;
    }

}
