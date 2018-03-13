/*
 * Expressions parser
 * Copyright (C) 2018  Michał Walenciak <Kicer86@gmail.com>
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
 */

#include "filter_engine.hpp"

#include <cassert>

#include <QStringList>


FilterEngine::FilterEngine()
{

}


FilterEngine::~FilterEngine()
{

}


void FilterEngine::parse(const QString& expression, IFilterEngineCallback* callback) const
{
    QStringList divided = expression.split(" ");

                         divided.takeFirst(); // scope
    const QString item = divided.takeFirst(); // item type

    if (item == "photos")
    {
        callback->filterPhotos();
        forPhotos(divided, callback);
    }
    else
        assert(!"unknown item to fetch");
}


void FilterEngine::forPhotos(const QStringList& expression, IFilterEngineCallback* callback) const
{
    QStringList to_process = expression;
    if (to_process.isEmpty())
        return;

    const QString operand = to_process.takeFirst();

    if (operand == "with")
    {
        const QString filter = to_process.takeFirst();

        if (filter == "flag")
        {
            const QString name  = to_process.takeFirst();
                                  to_process.takeFirst(); // operand
            const QString value = to_process.takeFirst();

            callback->photoFlag(name, value);
        }
        else if (filter == "tag")
        {
            const QString name  = to_process.takeFirst();
                                  to_process.takeFirst(); // operand
            const QString value = to_process.takeFirst();

            callback->photoTag(name, value);
        }
        else if (filter == "sha")
        {
                                  to_process.takeFirst(); // operand
            const QString value = to_process.takeFirst();

            callback->photoChecksum(value);
        }
        else
            assert(!"unknown filter");
    }
    else if (operand.isEmpty() == false)
        assert(!"unknown operator");
}
