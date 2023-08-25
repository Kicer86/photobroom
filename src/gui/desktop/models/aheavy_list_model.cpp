
#include "aheavy_list_model.hpp"


bool AHeavyListModelBase::isEmpty() const
{
    return rowCount({}) == 0;
}


AHeavyListModelBase::State AHeavyListModelBase::state() const
{
    return m_state;
}


void AHeavyListModelBase::setState(AHeavyListModelBase::State state)
{
    m_state = state;

    emit stateChanged();
}
