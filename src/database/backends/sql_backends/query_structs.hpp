
#ifndef DATABASE_QUERY_STRUCTS_HPP
#define DATABASE_QUERY_STRUCTS_HPP

#include <memory>
#include <vector>
#include <QString>
#include <QVariant>

#include <core/data_ptr.hpp>
#include <core/id.hpp>

#include "sql_backend_base_export.h"


namespace Database
{
    class SQL_BACKEND_BASE_EXPORT InsertQueryData
    {
        public:
            enum class Value
            {
                Null,
                CurrentTime,
            };

            InsertQueryData(const char* name);
            InsertQueryData(const InsertQueryData &) = default;
            virtual ~InsertQueryData();

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

            const QString& getName() const;
            const std::vector<QString>& getColumns() const;
            const std::vector<QVariant>& getValues() const;

            void addColumn(const QString &);
            void addValue(const QVariant &);

        private:
            struct Data;
            ol::data_ptr<Data> m_data;

            template<typename T>
            void addValue(const Id<int, T>& id)
            {
                addValue(id.value());
            }
            void addValue(Value);

            //finish variadic templates
            void setColumns();
            void setValues();
    };


    class SQL_BACKEND_BASE_EXPORT UpdateQueryData: public InsertQueryData
    {
        public:
            UpdateQueryData(const char* name);
            explicit UpdateQueryData(const InsertQueryData &);

            //update where c == v
            void addCondition(const QString& c, const QString& v);
            const std::vector<std::pair<QString, QString>>& getCondition() const;

        private:
            std::vector<std::pair<QString, QString>> m_condition;
    };
}

Q_DECLARE_METATYPE(Database::InsertQueryData::Value)

#endif
