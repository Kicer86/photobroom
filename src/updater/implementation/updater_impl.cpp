/*
 * Photo Broom - photos management tool.
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

#include <cassert>
#include <set>

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

#include "github_api/github_api_qt.hpp"
#include "github_api/iconnection.hpp"
#include "github_api/request.hpp"

#include "config.hpp"

#include "version.hpp"


UpdaterImpl::UpdaterImpl(): m_request(nullptr)
{
    GitHub::QtBackend::Api api(m_manager);

    m_request.reset(new GitHub::Request(api.connect()));
}


UpdaterImpl::~UpdaterImpl()
{

}


void UpdaterImpl::checkVersion(const IUpdater::StatusCallback& callback)
{
    const Version currentVersion = Version::fromString(PHOTO_BROOM_VERSION);
    const std::string json = m_request->getReleases("Kicer86", "photobroom");
    const QJsonDocument doc = QJsonDocument::fromJson(json.c_str());

    std::map<Version, int> versions;
    IUpdater::OnlineVersion versionInfo;

    QJsonArray releases = doc.array();
    for(const QJsonValueRef release_ref: releases)
    {
        assert(release_ref.isObject());
        const QJsonObject release = release_ref.toObject();

        const auto releaseVer = releaseVersion(release);
        const Version ver = Version::fromTagName(releaseVer.first);

        versions[ver] = releaseVer.second;
    }

    if (versions.empty() == false)
    {
        auto newestVersion = versions.rbegin();
        const Version& onlineVersion = newestVersion->first;
        if (onlineVersion > currentVersion)
        {
            versionInfo.status = IUpdater::OnlineVersion::NewVersionAvailable;
            versionInfo.url = getReleaseUrl(newestVersion->second);
        }
        else
            versionInfo.status = IUpdater::OnlineVersion::UpToDate;
    }

    callback(versionInfo);
}


std::pair<QString, int> UpdaterImpl::releaseVersion(const QJsonObject& release) const
{
    std::pair<QString, int> result;

    auto draft_it = release.find("draft");
    const bool is_draft = draft_it->toBool(false);

    auto prerelease_it = release.find("prerelease");
    const bool is_prerelease = prerelease_it->toBool(false);

    if (is_draft == false && is_prerelease == false)
    {
        auto tag_name_it = release.find("tag_name");
        const QString tag_name = tag_name_it->toString();

        auto id_it = release.find("id");
        const int id = id_it->toInt();

        result.first = tag_name;
        result.second = id;
    }

    return result;
}


QString UpdaterImpl::getReleaseUrl(int id) const
{
    const std::string json = m_request->getRelease("Kicer86", "photobroom", id);
    const QJsonDocument doc = QJsonDocument::fromJson(json.c_str());
    const QJsonObject release = doc.object();

    auto url_it = release.find("html_url");
    const QString url = url_it->toString();;

    return url;
}
