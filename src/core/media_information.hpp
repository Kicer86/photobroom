/*
 * Photo Broom - photos management tool.
 * Copyright (C) 2017  Michał Walenciak <Kicer86@gmail.com>
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

#ifndef MEDIAINFORMATION_HPP
#define MEDIAINFORMATION_HPP

#include <memory>

#include "imedia_information.hpp"

#include "core_export.h"

struct ICoreFactoryAccessor;


class CORE_EXPORT MediaInformation : public IMediaInformation
{
    public:
        explicit MediaInformation(ICoreFactoryAccessor* coreFactory);
        MediaInformation(const MediaInformation &) = delete;
        MediaInformation(MediaInformation &&) = delete;

        ~MediaInformation();

        MediaInformation& operator=(const MediaInformation &) = delete;
        MediaInformation& operator=(MediaInformation &&) = delete;

        FileInformation getInformation(const QString &) const override;

    private:
        struct Impl;
        std::unique_ptr<Impl> m_impl;
};

#endif // MEDIAINFORMATION_HPP
