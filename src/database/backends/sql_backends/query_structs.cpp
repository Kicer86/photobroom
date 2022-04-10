
#include "query_structs.hpp"

#include <assert.h>

#include <QString>
#include <QStringList>

namespace Database
{
    struct InsertQueryData::Data
    {
        QString m_table;
        std::vector<QString> m_columns;
        std::vector<QVariant> m_values;
        int m_args;

        Data(): m_table(""), m_columns(), m_values(), m_args(0) {}
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


    const std::vector<QString>& InsertQueryData::getColumns() const
    {
        assert(m_data->m_args == 0);

        return m_data->m_columns;
    }


    const std::vector<QVariant>& InsertQueryData::getValues() const
    {
        assert(m_data->m_args == 0);

        return m_data->m_values;
    }


    void InsertQueryData::addColumn(const QString& column)
    {
        m_data->m_args++;
        m_data->m_columns.push_back(column);
    }


    void InsertQueryData::addValue(const QVariant& value)
    {
        m_data->m_args--;
        m_data->m_values.push_back(value);
    }


    void InsertQueryData::addValue(InsertQueryData::Value value)
    {
        m_data->m_args--;
        m_data->m_values.push_back(QVariant::fromValue(value));
    }


    void InsertQueryData::setColumns() {}


    void InsertQueryData::setValues() {}


    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    UpdateQueryData::UpdateQueryData(const char* name): InsertQueryData(name), m_condition()
    {

    }


    UpdateQueryData::UpdateQueryData(const InsertQueryData& data): InsertQueryData(data), m_condition()
    {

    }


    void UpdateQueryData::addCondition (const QString& column, const QString& value)
    {
        m_condition.emplace_back(column, value);
    }


    const std::vector<std::pair<QString, QString>>& UpdateQueryData::getCondition() const
    {
        return m_condition;
    }

}
