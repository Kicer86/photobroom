
#ifndef AHEAVY_LIST_MODEL_HPP_INCLUDED
#define AHEAVY_LIST_MODEL_HPP_INCLUDED

#include <stop_token>
#include <QAbstractListModel>

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
        ApplyFinisher(AHeavyListModel* m, int rowsAffected)
        : m_m(m)
        , m_rowsAffected(rowsAffected)
        , m_totalRows(m->rowCount())
        {
            m_m->setState(State::Storing);
        }

        ~ApplyFinisher()
        {
            m_m->setState(State::Loaded);

            // assert model has removed affected rows
            const int currentRowCount = m_m->rowCount();
            const int expectedRowCount = m_totalRows - m_rowsAffected;

            assert(currentRowCount == expectedRowCount);
        }

        void operator()(void *)
        {

        }

        AHeavyListModel* m_m;
        int m_rowsAffected;
        const int m_totalRows;
    };

    using ApplyToken = std::unique_ptr<ApplyFinisher>;

    ~AHeavyListModel()
    {
        m_work.request_stop();
    }

    void reload() override
    {
        using namespace std::placeholders;

        if (state() == State::Idle || state() == State::Loaded)
        {
            clear();

            stoppableTask<T>
            (
                m_work,
                [this](const std::stop_token& stop, StoppableTaskCallback<T> callback) { setState(State::Fetching); loadData(stop, callback); },
                [this](const T& data) { updateData(data); setState(State::Loaded); }
            );
        }
    }

    void apply() override
    {
        const int totalRows = rowCount();

        ApplyToken token = std::make_unique<ApplyFinisher>(this, totalRows);
        applyRows({}, std::move(token));
    }

    void apply(const QList<int>& rows) override
    {
        if (rows.isEmpty() == false)
        {
            ApplyToken token = std::make_unique<ApplyFinisher>(this, rows.count());
            applyRows(rows, std::move(token));
        }
    }

    void clear()
    {
        setState(State::Idle);
        clearData();
    }

    void clear(const QList<int>& rows)
    {
        setState(State::Idle);
        clearData();
    }

    State state() const override
    {
        return m_state;
    }

protected:
    virtual void clearData() = 0;
    virtual void loadData(const std::stop_token& stopToken, StoppableTaskCallback<T>) = 0;
    virtual void updateData(const T &) = 0;
    virtual void applyRows(const QList<int> &, ApplyToken) = 0;

private:
    State m_state = Idle;
    std::stop_source m_work;

    void setState(AHeavyListModelBase::State state)
    {
        m_state = state;

        emit stateChanged();
    }
};

#endif
