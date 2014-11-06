
#ifndef PROJECT_PICKER_HPP
#define PROJECT_PICKER_HPP

#include <QDialog>

class QStringListModel;

namespace Ui
{
    class ProjectPicker;
}
struct IPluginLoader;
struct IProjectManager;

class ProjectPicker: public QDialog
{
        Q_OBJECT

    public:
        explicit ProjectPicker(QWidget *parent = 0);
        ProjectPicker(const ProjectPicker &) = delete;
        ~ProjectPicker();

        ProjectPicker& operator=(const ProjectPicker &) = delete;
        
        void set(IProjectManager *);
        void set(IPluginLoader *);

        QString choosenProjectName() const;

    private slots:
        void on_openButton_clicked();
        void on_newButton_clicked();
        void on_deleteButton_clicked();

private:
        Ui::ProjectPicker *ui;
        QString m_choosenProjectName;
        QStringListModel* m_model;
        IProjectManager* m_prjManager;
        IPluginLoader* m_pluginLoader;
};

#endif // PROJECT_PICKER_HPP
 
