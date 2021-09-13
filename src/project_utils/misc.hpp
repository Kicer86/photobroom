
#ifndef MISC_HPP
#define MISC_HPP

#include "project_utils_export.h"

#include <QString>

struct ProjectInfo;

QString PROJECT_UTILS_EXPORT moveFileToPrivateMediaLocation(const ProjectInfo &, const QString& path);
QString PROJECT_UTILS_EXPORT linkFileToPrivateMediaLocation(const ProjectInfo &, const QString& path);
QString PROJECT_UTILS_EXPORT includeFileToPrivateMediaLocation(const ProjectInfo &, const QString& path);  // intelligently move, copy or link

#endif
