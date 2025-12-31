
#include <cassert>

#include <QAbstractListModel>
#include <QHash>
#include <QVector>

#include <core/function_wrappers.hpp>

#include "observable_executor.hpp"
#include "observables_registry.hpp"


class ObservableExecutor::TasksModel final: public QAbstractListModel
{
    public:
        enum Role
        {
            NameRole = Qt::UserRole + 1,
            CountRole,
        };

        explicit TasksModel(QObject* parent = nullptr)
            : QAbstractListModel(parent)
        {

        }

        int rowCount(const QModelIndex& parent = QModelIndex()) const override
        {
            return parent.isValid()? 0 : m_entries.size();
        }

        QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override
        {
            if (!index.isValid())
                return {};

            const int row = index.row();
            if (row < 0 || row >= m_entries.size())
                return {};

            const Entry& entry = m_entries[row];

            switch (role)
            {
                case NameRole:  return entry.name;
                case CountRole: return entry.count;
                default:        return {};
            }
        }

        QHash<int, QByteArray> roleNames() const override
        {
            return {
                { NameRole,  "name"  },
                { CountRole, "count" },
            };
        }

        void adjust(const QString& name, int delta)
        {
            const auto it = m_rowForText.constFind(name);

            if (it == m_rowForText.constEnd())
            {
                assert(delta >= 0);
                const int row = m_entries.size();

                beginInsertRows(QModelIndex(), row, row);
                m_entries.push_back(Entry{.name = name, .count = delta});
                m_rowForText.insert(name, row);
                endInsertRows();
            }
            else
            {
                const int row = *it;
                assert(row >= 0 && row < m_entries.size());

                Entry& entry = m_entries[row];
                entry.count += delta;
                assert(entry.count >= 0);

                emit dataChanged(index(row), index(row), {CountRole});
            }
        }

    private:
        struct Entry
        {
            QString name;
            int count = 0;
        };

        QVector<Entry> m_entries;
        QHash<QString, int> m_rowForText;
};


ObservableExecutor::ObservableExecutor()
    : m_tasksModel(std::make_unique<TasksModel>(this))
{
    ObservablesRegistry::instance().add(this);

    m_executionSpeedBuffer.fill(0);

    connect(&m_executionSpeedTimer, &QTimer::timeout, this, &ObservableExecutor::updateExecutionSpeed);
    m_executionSpeedTimer.start(std::chrono::seconds(1));
}


ObservableExecutor::~ObservableExecutor()
{
    ObservablesRegistry::instance().remove(this);
}


int ObservableExecutor::awaitingTasks() const
{
    return m_awaitingTasks;
}


int ObservableExecutor::tasksExecuted() const
{
    return m_tasksExecuted;
}


double ObservableExecutor::executionSpeed() const
{
    return m_executionSpeed;
}


QAbstractItemModel* ObservableExecutor::tasks() const
{
    return m_tasksModel.get();
}


void ObservableExecutor::newTaskInQueue(const std::string& name)
{
    const QString qname = QString::fromStdString(name);

    m_awaitingTasks++;

    emit awaitingTasksChanged(m_awaitingTasks);

    auto* model = m_tasksModel.get();
    invokeMethod(model, &ObservableExecutor::TasksModel::adjust, qname, 1);
}


void ObservableExecutor::taskMovedToExecution(const std::string &)
{
    m_awaitingTasks--;
    m_tasksExecuted++;

    assert(m_awaitingTasks >= 0);

    emit awaitingTasksChanged(m_awaitingTasks);
    emit tasksExecutedChanged(m_tasksExecuted);
}


void ObservableExecutor::taskExecuted(const std::string& name)
{
    const QString qname = QString::fromStdString(name);

    m_tasksExecuted--;
    m_executionSpeedCounter++;

    assert(m_tasksExecuted >= 0);

    emit tasksExecutedChanged(m_tasksExecuted);

    auto* model = m_tasksModel.get();
    invokeMethod(model, &ObservableExecutor::TasksModel::adjust, qname, -1);
}


void ObservableExecutor::updateExecutionSpeed()
{
    const std::size_t bufferSize = m_executionSpeedBuffer.size();
    m_executionSpeedBuffer[m_executionSpeedBufferPos] = m_executionSpeedCounter;
    m_executionSpeedBufferPos = (m_executionSpeedBufferPos + 1) % bufferSize;
    m_executionSpeedCounter = 0;

    m_executionSpeed = std::accumulate(m_executionSpeedBuffer.begin(), m_executionSpeedBuffer.end(), 0) / static_cast<double>(bufferSize);

    emit executionSpeedChanged(m_executionSpeed);
}
