/*
 * Evaluator of search expressions
 * Copyright (C) 2016  Michał Walenciak <Kicer86@gmail.com>
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

#include "search_expression_evaluator.hpp"

#include <QStringList>


SearchExpressionEvaluator::SearchExpressionEvaluator(const QString& separator):
    m_separator(separator)
{

}


std::vector<SearchExpressionEvaluator::Filter> SearchExpressionEvaluator::evaluate(const QString& input) const
{
    std::vector<Filter> result;

    const QStringList expressions = input.split(m_separator);

    for(const QString& rawExpression: expressions)
    {
        QString expression = rawExpression.trimmed();

        if (expression.isEmpty() == false)
        {
            const bool exact = expression.left(1) == "\"" && expression.right(1) == "\"";

            if (exact)
            {
                expression.chop(1);          // remove last character
                expression.remove(0, 1);     // remove first character
            }

            const Filter filter(expression, exact);
            result.push_back(filter);
        }
    }

    return result;
}
