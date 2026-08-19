
#ifndef MISC_HPP
#define MISC_HPP

#include <QString>

#include "core/filesystem.hpp"
#include "project_utils_export.h"


struct ProjectInfo;

Filesystem::Location PROJECT_UTILS_EXPORT includeFileInPrivateMediaLocation(const ProjectInfo &, const Filesystem::Location& path);  // intelligently move, copy or link

#endif
