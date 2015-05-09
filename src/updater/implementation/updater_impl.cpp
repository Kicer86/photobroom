/*
 * Updater's private implementation
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

#include "updater_impl.hpp"

#include "github_api/github_api.hpp"
#include "github_api/aconnection.hpp"

UpdaterImpl::UpdaterImpl(): m_manager(new QNetworkAccessManager), m_connection(nullptr)
{
    GitHubApi api;
    api.set(m_manager.get());

    m_connection = api.connect("8e47abda51d9e3515acf5c22c8278204d5206610");
}


UpdaterImpl::~UpdaterImpl()
{

}


void UpdaterImpl::checkVersion()
{
    connect(m_connection.get(), &GitHub::AConnection::gotReply, this, &UpdaterImpl::gotReply);
    m_connection->get("users/Kicer86");
}


void UpdaterImpl::gotReply(const QJsonDocument&, const QList<QNetworkReply::RawHeaderPair>&)
{

}
