
#ifndef DATABASE_QUERY_STRUCTS_HPP
#define DATABASE_QUERY_STRUCTS_HPP

#include "sql_backend_base_export.h"

#include <memory>
#include <deque>


class QString;
class QStringList;

namespace Database
{
    struct SQL_BACKEND_BASE_EXPORT TableUpdateData
    {
        TableUpdateData(const char* name);
        ~TableUpdateData();

        //define common columns
        template<typename Column, typename... V>
        void setColumns(const Column& column, const V&... columns)
        {
            addColumn(column);
            setColumns(columns...);
        }

        //set values for common columns
        template<typename Value, typename... V>
        void setValues(const Value& value, const V&... values)
        {
            addValue(value);
            setValues(values...);
        }

        //set key column and its value.
        //If particular id is provided, and update will be made. Use "" for inserts (new values)
        void setKey(const QString &, const QString &);

        //set optional column, value pair for inserts only
        void addInsertOnly(const QString &, const QString &);

        const QString& getName() const;
        const QStringList& getColumns() const;
        const QStringList& getValues() const;
        const std::pair<QString, QString>& getKey() const;
        const std::deque<std::pair<QString, QString>>& getInsertOnly() const;

        private:
            struct Data;
            std::unique_ptr<Data> m_data;

            void addColumn(const QString &);
            void addValue(const QString &);

            //finish variadic templates
            void setColumns();
            void setValues();
    };
}

#endif
    
