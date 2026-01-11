
#ifndef OBSERVABLE_EXECUTOR_MODEL_HPP_INCLUDED
#define OBSERVABLE_EXECUTOR_MODEL_HPP_INCLUDED

#include <QAbstractItemModel>
#include <QPointer>

#include <core/observable_executor.hpp>


class ObservableExecutorModel: public QAbstractItemModel
{
        Q_OBJECT
        Q_PROPERTY(ObservableExecutor* executor READ executor WRITE setExecutor NOTIFY executorChanged)
        Q_PROPERTY(QString title READ title NOTIFY titleChanged)

    public:
        enum class Role
        {
            LabelRole = Qt::UserRole + 1,
            ValueRole,
        };
        Q_ENUM(Role)

        enum class Row
        {
            AwaitingTasks = 0,
            TasksExecuted,
            ExecutionSpeed,
            Tasks,
            Count,
        };

        explicit ObservableExecutorModel(QObject* parent = nullptr);

        ObservableExecutor* executor() const;
        void setExecutor(ObservableExecutor* executor);
        QString title() const;

        QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
        QModelIndex parent(const QModelIndex& child) const override;
        int rowCount(const QModelIndex& parent = QModelIndex()) const override;
        int columnCount(const QModelIndex& parent = QModelIndex()) const override;
        QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
        Qt::ItemFlags flags(const QModelIndex& index) const override;
        QHash<int, QByteArray> roleNames() const override;

    signals:
        void executorChanged();
        void titleChanged();

    private:
        QPointer<ObservableExecutor> m_executor;
        bool m_pendingTaskInsert = false;

        QModelIndex tasksRootIndex() const;
        void notifyRowChanged(int row);
        void connectExecutor();
        void disconnectExecutor();
};

#endif
