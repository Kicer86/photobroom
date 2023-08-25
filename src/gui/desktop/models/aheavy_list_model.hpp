
#ifndef AHEAVY_LIST_MODEL_HPP_INCLUDED
#define AHEAVY_LIST_MODEL_HPP_INCLUDED

#include <stop_token>
#include <QAbstractListModel>

#include <core/function_wrappers.hpp>


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
    Q_INVOKABLE bool isEmpty() const;
    State state() const;

signals:
    void stateChanged() const;

protected:
    void setState(State);

private:
    State m_state = Idle;
};


template<typename T>
class AHeavyListModel: public AHeavyListModelBase
{
public:
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

    void clear()
    {
        setState(State::Idle);
        clearData();
    }

protected:
    virtual void clearData() = 0;
    virtual void loadData(const std::stop_token& stopToken, StoppableTaskCallback<T>) = 0;
    virtual void updateData(const T &) = 0;

private:
    std::stop_source m_work;
};

#endif
