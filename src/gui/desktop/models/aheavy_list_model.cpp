
#include "aheavy_list_model.hpp"

AHeavyListModelBase::State AHeavyListModelBase::state() const
{
    return m_state;
}


void AHeavyListModelBase::setState(AHeavyListModelBase::State state)
{
    m_state = state;

    emit stateChanged();
}
