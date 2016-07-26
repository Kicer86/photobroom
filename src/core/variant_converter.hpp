/*
 * Tool for TagValue to QString conversion.
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

#ifndef VARIANTCONVERTER_HPP
#define VARIANTCONVERTER_HPP

#include <QVariant>

#include "tag.hpp"

#include "core_export.h"

class CORE_EXPORT [[deprecated]] VariantConverter
{
    public:
        VariantConverter();
        VariantConverter(const VariantConverter &) = delete;
        ~VariantConverter();

        QString operator()(const QVariant &) const;
        QString operator()(const QString &) const = delete;
        QVariant operator()(const TagType &, const QString &) const;

        VariantConverter& operator=(const VariantConverter &) = delete;
};

#endif // VARIANTCONVERTER_HPP
