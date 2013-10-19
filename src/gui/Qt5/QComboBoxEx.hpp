 

#ifndef Q_COMBO_BOX_EX_HPP
#define Q_COMBO_BOX_EX_HPP

#include <memory>

#include <QComboBox>

class QComboBoxEx;

struct ModelDeleter
{
    ModelDeleter(QComboBoxEx *);
    void operator()(QAbstractItemModel *);
    
    QComboBoxEx *m_combo;
};

class QComboBoxEx: public QComboBox
{
        Q_OBJECT
        
        friend class ModelDeleter;
    
    public:
        explicit QComboBoxEx(QWidget* parent = 0);
        virtual ~QComboBoxEx();
        
        void setDropValues(QAbstractItemModel *); //behaves like setModel, but doesn't change current value
        
    private:
        std::unique_ptr<QAbstractItemModel, ModelDeleter> m_model;
        
    private slots:
        void dataChanged();
};

#endif
