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

#ifndef FILTERENGINE_HPP
#define FILTERENGINE_HPP

#include "core_export.h"

class QString;
class QStringList;


struct IFilterEngineCallback
{
    virtual ~IFilterEngineCallback() = default;

    virtual void filterPhotos() = 0;

    virtual void photoFlag(const QString& name, const QString& value) = 0;
    virtual void photoTag(const QString& name, const QString& value) = 0;
    virtual void photoChecksum(const QString &) = 0;
};


class CORE_EXPORT FilterEngine final
{
    public:
        FilterEngine();
        ~FilterEngine();

        void parse(const QString &, IFilterEngineCallback *) const;

    private:
        void forPhotos(const QStringList& expression, IFilterEngineCallback *) const;
};

#endif // FILTERENGINE_H
