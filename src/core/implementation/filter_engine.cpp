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

#define YY_NO_UNISTD_H
#include "filter_engine.tab.hh"
#include "filter_engine.yy.hh"

int filterEngine_parse(IFilterEngineCallback *, yyscan_t);

#if defined YYDEBUG && YYDEBUG==1
extern int filterEngine_debug;
#endif


FilterEngine::FilterEngine()
{

}


FilterEngine::~FilterEngine()
{

}


void FilterEngine::parse(const QString& expression, IFilterEngineCallback* callback) const
{
    //read the html code - each tag will be one field in std::vector
    YY_BUFFER_STATE bp;

#if defined YYDEBUG && YYDEBUG==1
    filterEngine_debug=0;
#endif

    yyscan_t scanner;
    filterEngine_lex_init_extra(callback, &scanner);

    bp=filterEngine__scan_string(expression.toStdString().c_str(), scanner);
    filterEngine__switch_to_buffer(bp, scanner);
    filterEngine_parse(callback, scanner);

    filterEngine__delete_buffer(bp, scanner);
    filterEngine_lex_destroy(scanner);
}
