
#include "backend.hpp"

#include "entry.hpp"

namespace Database
{
    
    PrimitiveBackend::PrimitiveBackend()
    {
    }
    
    
    PrimitiveBackend::~PrimitiveBackend()
    {
    }
    
    
    bool PrimitiveBackend::store(const Entry &entry)
    {
		return true;        
    }
    
}
