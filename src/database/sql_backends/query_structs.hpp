
#ifndef DATABASE_QUERY_STRUCTS_HPP
#define DATABASE_QUERY_STRUCTS_HPP

#include "sql_backend_base_export.h"

#include <memory>
#include <deque>

#include <QString>

#include <utils/data_ptr.hpp>

class QStringList;

namespace Database
{
    struct SQL_BACKEND_BASE_EXPORT InsertQueryData
    {
        InsertQueryData(const char* name);
        InsertQueryData(const InsertQueryData &) = default;
        ~InsertQueryData();

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
        //If value is null, inserts will occur.
        //If value is not null, update or insert will be made (depends if provided value is unique).
        //Important: values added by addInsertOnly() will be used only for null values of key.
        void setKey(const QString &, const QString &);

        //set optional column, value pair for inserts only
        void addInsertOnly(const QString &, const QString &);

        const QString& getName() const;
        const QStringList& getColumns() const;
        const QStringList& getValues() const;

        private:
            struct Data;
            data_ptr<Data> m_data;

            void addColumn(const QString &);
            void addValue(const QString &);

            //finish variadic templates
            void setColumns();
            void setValues();
    };


    struct SQL_BACKEND_BASE_EXPORT UpdateQueryData: public InsertQueryData
    {
        UpdateQueryData(const char* name);
        UpdateQueryData(const InsertQueryData &);

        //update where c == v
        void setCondition(const QString& c, const QString& v);
        const std::pair<QString, QString>& getCondition() const;

        private:
            std::pair<QString, QString> m_condition;
    };
}

#endif
    
