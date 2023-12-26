
#ifndef BATCH_FACE_RECOGNITION_MODEL_HPP_INCLUDED
#define BATCH_FACE_RECOGNITION_MODEL_HPP_INCLUDED

#include "flat_model.hpp"


class QMLFlatModel: public FlatModel
{
    Q_OBJECT

    Q_PROPERTY(Database::IDatabase* database WRITE setDatabase READ database REQUIRED)
    Q_PROPERTY(QStringList text_filters WRITE setTextFilters READ textFilters)

    // filters
    Q_PROPERTY(QString facesNotAnalysed READ facesNotAnalysedFilter CONSTANT)
    Q_PROPERTY(QString validMedia READ validMediaFilter CONSTANT)

public:
    QMLFlatModel();

    void setTextFilters(const QStringList& filters);
    const QStringList& textFilters() const;

    QString facesNotAnalysedFilter() const;
    QString validMediaFilter() const;

private:
    QStringList m_filters;
};

#endif
