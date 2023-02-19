
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
        if (filter == facesNotAnalysedFilter())
            dbFilter.push_back(FilterFaceAnalysisStatus(FilterFaceAnalysisStatus::NotPerformed));
        else if (filter == validMediaFilter())
            dbFilter.push_back(getValidPhotosFilter());
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


QString QMLFlatModel::facesNotAnalysedFilter() const
{
    return QStringLiteral("faces not analysed");
}


QString QMLFlatModel::validMediaFilter() const
{
    return QStringLiteral("valid entries");
}
