/*
 * Photo Broom - photos management tool.
 * Copyright (C) 2013  Michał Walenciak <MichalWalenciak@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef ICONFIGURATION_HPP
#define ICONFIGURATION_HPP

#include <functional>

class QString;
class QVariant;

#include "core_export.h"


struct CORE_EXPORT IConfiguration
{
    typedef std::function<void(const QString &, const QVariant &)> Watcher;

    virtual ~IConfiguration() = default;

    virtual QVariant getEntry(const QString &) = 0;
    virtual void setEntry(const QString &, const QVariant &) = 0;

    virtual void setDefaultValue(const QString &, const QVariant &) = 0;

    virtual void watchFor(const QString& key, const Watcher &) = 0;
};

#endif  //ICONFIGURATION_HPP
