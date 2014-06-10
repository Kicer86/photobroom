
#include "query_structs.hpp"

#include <assert.h>

#include <QString>
#include <QStringList>

namespace Database
{
    struct InsertQueryData::Data
    {
        QString m_table;
        QStringList m_columns;
        QStringList m_values;
        int m_args;

        Data(): m_table(""), m_columns(), m_values(), m_args(0) {}

        QString quote(const QString& v) const
        {
            return QString("\"%1\"").arg(v);
        }
    };


    InsertQueryData::InsertQueryData(const char* name): m_data(new Data)
    {
        m_data->m_table = name;
    }


    InsertQueryData::~InsertQueryData()
    {

    }


    const QString& InsertQueryData::getName() const
    {
        assert(m_data->m_args == 0);

        return m_data->m_table;
    }


    const QStringList& InsertQueryData::getColumns() const
    {
        assert(m_data->m_args == 0);

        return m_data->m_columns;
    }


    const QStringList& InsertQueryData::getValues() const
    {
        assert(m_data->m_args == 0);

        return m_data->m_values;
    }


    void InsertQueryData::addColumn(const QString& column)
    {
        m_data->m_args++;
        m_data->m_columns.push_back(column);
    }


    void InsertQueryData::addValue(const QString& value)
    {
        m_data->m_args--;
        m_data->m_values.push_back(m_data->quote(value));
    }


    void InsertQueryData::addValue(InsertQueryData::Value value)
    {
        QString v;
        switch(value)
        {
            case Value::CurrentTime: v = "CURRENT_TIMESTAMP"; break;
            case Value::Null:        v = "NULL";              break;

            default: assert(!"???");
        }

        m_data->m_args--;
        m_data->m_values.push_back(v);
    }


    void InsertQueryData::setColumns() {}


    void InsertQueryData::setValues() {}


    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    UpdateQueryData::UpdateQueryData(const char* name): InsertQueryData(name)
    {

    }


    UpdateQueryData::UpdateQueryData(const InsertQueryData& data): InsertQueryData(data)
    {

    }


    void UpdateQueryData::setCondition(const QString& column, const QString& value)
    {
        m_condition = std::make_pair(column, value);
    }


    const std::pair<QString, QString>& UpdateQueryData::getCondition() const
    {
        return m_condition;
    }

}
