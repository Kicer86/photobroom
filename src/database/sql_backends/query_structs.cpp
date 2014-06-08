
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
        std::pair<QString, QString> m_key;
        std::deque<std::pair<QString, QString>> m_insertOnly;

        Data(): m_table(""), m_columns(), m_values(), m_args(0), m_key(), m_insertOnly() {}

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


    void InsertQueryData::setKey(const QString& key, const QString& value)
    {
        m_data->m_key = std::make_pair(key, m_data->quote(value));
    }


    void InsertQueryData::addInsertOnly(const QString& name, const QString& value)
    {
        m_data->m_insertOnly.push_back(std::make_pair(name, value));
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


    const std::pair<QString, QString>& InsertQueryData::getKey() const
    {
        return m_data->m_key;
    }


    const std::deque< std::pair<QString, QString> >& InsertQueryData::getInsertOnly() const
    {
        return m_data->m_insertOnly;
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


    void InsertQueryData::setColumns() {}


    void InsertQueryData::setValues() {}

}
