
#ifndef ROLES_EXPANSION_HPP_INCLUDED
#define ROLES_EXPANSION_HPP_INCLUDED

#include <QAbstractItemModel>


template<typename T> requires std::is_base_of<QAbstractItemModel, T>::value
class RolesExpansion: public T
{
    public:
        QHash<int, QByteArray> roleNames() const override
        {
            QHash<int, QByteArray> result = T::roleNames();

            result.insert(m_customRoles);

            return result;
        }

    protected:
        void registerRole(int role, const QByteArray& name)
        {
             m_customRoles[role] = name;
        }

    private:
        QHash<int, QByteArray> m_customRoles;
};

#endif
