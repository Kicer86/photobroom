#ifndef CONFIGURATION_DIALOG_HPP
#define CONFIGURATION_DIALOG_HPP

#include <QDialog>

namespace Ui {
class ConfigurationDialog;
}

class ConfigurationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConfigurationDialog(QWidget *parent = 0);
    ConfigurationDialog(const ConfigurationDialog &) = delete;

    virtual ~ConfigurationDialog();

    ConfigurationDialog& operator=(const ConfigurationDialog &) = delete;

private:
    Ui::ConfigurationDialog *ui;
};

#endif // CONFIGURATION_DIALOG_HPP
