
#ifndef AHEAVY_LIST_MODEL_HPP_INCLUDED
#define AHEAVY_LIST_MODEL_HPP_INCLUDED


#include <QAbstractListModel>
#include <QFuture>
#include <QPromise>

#include <core/function_wrappers.hpp>

/**
 * @brief Qt interface of AHeavyListModel
 *
 * @ref AHeavyListModel is a template. Template class cannot be moced, so types and methods used by views
 * are extracted to this class
 */

class AHeavyListModelBase: public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(State state READ state NOTIFY stateChanged)

public:
    ~AHeavyListModelBase() = default;

    enum State
    {
        Idle,
        Fetching,
        Loaded,
        Storing,
    };
    Q_ENUMS(State)

    virtual Q_INVOKABLE void reload() = 0;
    virtual Q_INVOKABLE void apply() = 0;
    virtual Q_INVOKABLE void apply(const QList<int> &) = 0;
    Q_INVOKABLE bool isEmpty() const
    {
        return rowCount() == 0;
    }

    virtual State state() const = 0;

signals:
    void stateChanged() const;
};


template<typename T>
class AHeavyListModel: public AHeavyListModelBase
{
public:
    friend struct ApplyFinisher;
    struct ApplyFinisher
    {
        ApplyFinisher(AHeavyListModel* m, const std::optional<QList<int>>& rowsAffected)
        : m_m(m)
        , m_rowsAffected(rowsAffected)
        , m_totalRows(m->rowCount())
        {
            m_m->setState(State::Storing);
        }

        ~ApplyFinisher()
        {
            m_rowsAffected? m_m->clearData(*m_rowsAffected) : m_m->clearData();
            m_m->setState(State::Loaded);

            if (m_rowsAffected)
            {
                // assert model has removed affected rows
                const int currentRowCount = m_m->rowCount();
                const qsizetype expectedRowCount = m_totalRows - m_rowsAffected->size();

                assert(currentRowCount == expectedRowCount);
            }
        }

        void operator()(void *)
        {

        }

        AHeavyListModel* m_m;
        const std::optional<QList<int>> m_rowsAffected;
        const qsizetype m_totalRows;
    };

    using DataVector = std::vector<T>;
    using ApplyToken = std::unique_ptr<ApplyFinisher>;

    ~AHeavyListModel()
    {
        m_loadDataFuture.cancel();
        m_loadDataFuture.waitForFinished();
    }

    int rowCount(const QModelIndex& parent = {}) const override
    {
        return parent.isValid()? 0: static_cast<int>(m_data.size());
    }

    void reload() override
    {
        using namespace std::placeholders;

        if (state() == State::Idle || state() == State::Loaded)
        {
            clear();

            QPromise<DataVector> promise;
            m_loadDataFuture = promise.future();

            m_loadDataFuture.then(this, [this](const DataVector& data)
            {
                updateData(data);
                setState(State::Loaded);
            });

            setState(State::Fetching);
            loadData(std::move(promise));
        }
    }

    void apply() override
    {
        ApplyToken token = std::make_unique<ApplyFinisher>(this, std::optional<QList<int>>());
        applyRows({}, std::move(token));
    }

    void apply(const QList<int>& rows) override
    {
        if (rows.isEmpty() == false)
        {
            ApplyToken token = std::make_unique<ApplyFinisher>(this, rows);
            applyRows(rows, std::move(token));
        }
    }

    void clear()
    {
        setState(State::Idle);
        clearData();
    }

    State state() const override
    {
        return m_state;
    }

protected:
    virtual void loadData(QPromise<std::vector<T>> &&) = 0;
    virtual void applyRows(const QList<int> &, ApplyToken) = 0;

    const DataVector& internalData() const
    {
        return m_data;
    }

private:
    std::vector<T> m_data;
    QFuture<DataVector> m_loadDataFuture;
    State m_state = Idle;

    void setState(AHeavyListModelBase::State state)
    {
        m_state = state;

        emit stateChanged();
    }

    void clearData()
    {
        beginResetModel();
        m_data.clear();
        endResetModel();
    }

    void clearData(const QList<int>& rows)
    {
        beginResetModel();
        QList<int> mutRows = rows;
        std::sort(mutRows.rbegin(), mutRows.rend());

        for(const int row: mutRows)
            m_data.erase(m_data.begin() + row);

        endResetModel();
    }

    void updateData(const std::vector<T>& data)
    {
        const int count = static_cast<int>(data.size());
        beginInsertRows({}, 0, count - 1);
        m_data = data;
        endInsertRows();
    }
};

#endif
