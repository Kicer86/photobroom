/*
 * ConfigurableTreeItemDelegate - extension of TreeItemDelegate
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


#include <configuration/iconfiguration.hpp>

#include "photos_item_delegate.hpp"
#include "config_keys.hpp"
#include "utils/config_tools.hpp"


PhotosItemDelegate::PhotosItemDelegate(ImagesTreeView* view, IConfiguration* config):
    TreeItemDelegate(view),
    m_config(config)
{
    readConfig();
}


PhotosItemDelegate::~PhotosItemDelegate()
{

}


void PhotosItemDelegate::set(IConfiguration* config)
{
    m_config = config;
    m_config->registerObserver(this);

    readConfig();
}


void PhotosItemDelegate::readConfig()
{
    if (m_config != nullptr)
    {
        setupEvenColor(m_config->getEntry(ViewConfigKeys::bkg_color_even));
        setupOddColor(m_config->getEntry(ViewConfigKeys::bkg_color_odd));
    }
}


void PhotosItemDelegate::setupEvenColor(const QVariant& v)
{
    const uint32_t evenColor = v.toUInt();
    const QColor evenQColor = ConfigTools::intToColor(evenColor);

    setNodeBackgroundEvenColor(evenQColor);
}


void PhotosItemDelegate::setupOddColor(const QVariant& v)
{
    const uint32_t oddColor = v.toUInt();
    const QColor oddQColor = ConfigTools::intToColor(oddColor);

    setNodeBackgroundOddColor(oddQColor);
}



void PhotosItemDelegate::configChanged(const QString& entry, const QVariant& value)
{
    if (entry == ViewConfigKeys::bkg_color_even)
        setupEvenColor(value);
    else if (entry == ViewConfigKeys::bkg_color_odd)
        setupOddColor(value);
}
