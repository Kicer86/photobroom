
#include "observable_executor_model.hpp"


namespace
{
    constexpr quintptr kTopLevelId = 0;
    constexpr quintptr kTaskChildId = 1;
    constexpr int kRowAwaiting = static_cast<int>(ObservableExecutorModel::Row::AwaitingTasks);
    constexpr int kRowExecuted = static_cast<int>(ObservableExecutorModel::Row::TasksExecuted);
    constexpr int kRowSpeed = static_cast<int>(ObservableExecutorModel::Row::ExecutionSpeed);
    constexpr int kRowTasks = static_cast<int>(ObservableExecutorModel::Row::Tasks);
    constexpr int kTopLevelRowCount = static_cast<int>(ObservableExecutorModel::Row::Count);
    constexpr int kColumnCount = 1;

    bool isTasksRootIndex(const QModelIndex& index)
    {
        return index.isValid()
            && index.internalId() == kTopLevelId
            && index.row() == kRowTasks
            && index.column() == 0;
    }
}


ObservableExecutorModel::ObservableExecutorModel(QObject* parent)
    : QAbstractItemModel(parent)
{
}


ObservableExecutor* ObservableExecutorModel::executor() const
{
    return m_executor.data();
}


void ObservableExecutorModel::setExecutor(ObservableExecutor* executor)
{
    if (m_executor == executor)
        return;

    beginResetModel();
    disconnectExecutor();
    m_executor = executor;
    connectExecutor();
    endResetModel();

    emit executorChanged();
    emit titleChanged();
}


QString ObservableExecutorModel::title() const
{
    return m_executor? m_executor->name() : QString{};
}


QModelIndex ObservableExecutorModel::index(int row, int column, const QModelIndex& parent) const
{
    if (row < 0 || column < 0 || column >= kColumnCount)
        return {};

    if (!parent.isValid())
    {
        if (row >= kTopLevelRowCount)
            return {};

        return createIndex(row, column, kTopLevelId);
    }

    if (parent.column() != 0)
        return {};

    if (isTasksRootIndex(parent))
    {
        if (!m_executor || row >= m_taskEntries.size())
            return {};

        return createIndex(row, column, kTaskChildId);
    }

    return {};
}


QModelIndex ObservableExecutorModel::parent(const QModelIndex& child) const
{
    if (!child.isValid())
        return {};

    if (child.internalId() == kTaskChildId)
        return createIndex(kRowTasks, 0, kTopLevelId);

    return {};
}


int ObservableExecutorModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid() && parent.column() != 0)
        return 0;

    if (!m_executor)
        return 0;

    if (!parent.isValid())
        return kTopLevelRowCount;

    if (isTasksRootIndex(parent))
        return m_taskEntries.size();

    return 0;
}


int ObservableExecutorModel::columnCount(const QModelIndex&) const
{
    return kColumnCount;
}


QVariant ObservableExecutorModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.column() != 0 || !m_executor)
        return {};

    if (index.internalId() == kTopLevelId)
    {
        if (role == static_cast<int>(Role::LabelRole) || role == Qt::DisplayRole)
        {
            switch (index.row())
            {
                case kRowAwaiting:
                    return tr("Tasks in queue");
                case kRowExecuted:
                    return tr("Tasks executed");
                case kRowSpeed:
                    return tr("Execution speed");
                case kRowTasks:
                    return tr("Tasks");
                default:
                    return {};
            }
        }

        if (role == static_cast<int>(Role::ValueRole))
        {
            switch (index.row())
            {
                case kRowAwaiting:
                    return m_executor->awaitingTasks();
                case kRowExecuted:
                    return m_executor->tasksExecuted();
                case kRowSpeed:
                    return tr("%1 %2")
                        .arg(m_executor->executionSpeed())
                        .arg(tr("tps", "tasks per second"));
                case kRowTasks:
                    return m_taskEntries.size();
                default:
                    return {};
            }
        }

        return {};
    }

    if (index.internalId() == kTaskChildId)
    {
        if (role == static_cast<int>(Role::LabelRole) || role == Qt::DisplayRole)
            return m_taskEntries[index.row()].name;

        if (role == static_cast<int>(Role::ValueRole))
            return m_taskEntries[index.row()].count;

        return {};
    }

    return {};
}


Qt::ItemFlags ObservableExecutorModel::flags(const QModelIndex& index) const
{
    if (index.isValid())
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    else
        return Qt::NoItemFlags;
}


QHash<int, QByteArray> ObservableExecutorModel::roleNames() const
{
    auto roles = QAbstractItemModel::roleNames();
    roles.insert(Qt::DisplayRole, "display");
    roles.insert(static_cast<int>(Role::LabelRole), "label");
    roles.insert(static_cast<int>(Role::ValueRole), "value");

    return roles;
}


QModelIndex ObservableExecutorModel::tasksRootIndex() const
{
    return index(kRowTasks, 0);
}


void ObservableExecutorModel::notifyRowChanged(int row)
{
    const QModelIndex idx = index(row, 0);
    if (idx.isValid())
        emit dataChanged(idx, idx, {Qt::DisplayRole,
                                    static_cast<int>(Role::LabelRole),
                                    static_cast<int>(Role::ValueRole)});
}

void ObservableExecutorModel::syncTasksFromExecutor()
{
    m_taskEntries.clear();
    m_taskRowForText.clear();

    if (!m_executor)
        return;

    const auto& tasks = m_executor->tasks();
    m_taskEntries.reserve(static_cast<int>(tasks.size()));
    for (int row = 0; row < static_cast<int>(tasks.size()); ++row)
    {
        m_taskEntries.push_back(tasks[static_cast<std::size_t>(row)]);
        m_taskRowForText.insert(m_taskEntries.back().name, row);
    }
}


void ObservableExecutorModel::connectExecutor()
{
    if (!m_executor)
        return;

    syncTasksFromExecutor();

    connect(m_executor, &ObservableExecutor::awaitingTasksChanged, this,
            [this](int) { notifyRowChanged(kRowAwaiting); });
    connect(m_executor, &ObservableExecutor::tasksExecutedChanged, this,
            [this](int) { notifyRowChanged(kRowExecuted); });
    connect(m_executor, &ObservableExecutor::executionSpeedChanged, this,
            [this](double) { notifyRowChanged(kRowSpeed); });
    connect(m_executor, &ObservableExecutor::taskEntryChanged, this,
            [this](const QString& name, int count)
            {
                const auto it = m_taskRowForText.constFind(name);
                if (it == m_taskRowForText.constEnd())
                {
                    const QModelIndex parentIndex = tasksRootIndex();
                    if (!parentIndex.isValid())
                        return;

                    const int row = m_taskEntries.size();
                    beginInsertRows(parentIndex, row, row);
                    m_taskEntries.push_back(ObservableExecutor::TaskEntry{.name = name, .count = count});
                    m_taskRowForText.insert(name, row);
                    endInsertRows();

                    notifyRowChanged(kRowTasks);
                    return;
                }

                const int row = *it;
                if (row < 0 || row >= m_taskEntries.size())
                    return;

                ObservableExecutor::TaskEntry& entry = m_taskEntries[row];
                entry.count = count;

                const QModelIndex parentIndex = tasksRootIndex();
                const QModelIndex idx = index(row, 0, parentIndex);
                if (idx.isValid())
                {
                    emit dataChanged(idx, idx, {Qt::DisplayRole,
                                                static_cast<int>(Role::LabelRole),
                                                static_cast<int>(Role::ValueRole)});
                }
            });
    connect(m_executor, &QObject::destroyed, this, [this]()
            {
                setExecutor(nullptr);
            });
}


void ObservableExecutorModel::disconnectExecutor()
{
    if (!m_executor)
        return;

    disconnect(m_executor, nullptr, this, nullptr);
    m_taskEntries.clear();
    m_taskRowForText.clear();
}
