
#include "qml_flat_model.hpp"

using namespace Database;

QMLFlatModel::QMLFlatModel()
{

}


void QMLFlatModel::setTextFilters(const QStringList& filters)
{
    m_filters = filters;
    std::vector<Filter> dbFilter;

    for (const auto& filter: filters)
    {
        if (filter == "faces not analysed")
            dbFilter.push_back(FilterFaceAnalysisStatus(FilterFaceAnalysisStatus::NotPerformed));
        else
        {
            // log warning
        }
    }

    FlatModel::setFilter(dbFilter);
}


const QStringList& QMLFlatModel::textFilters() const
{
    return m_filters;
}
