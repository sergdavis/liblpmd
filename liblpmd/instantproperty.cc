//
//
//

#include <lpmd/instantproperty.h>

using namespace lpmd;

InstantProperty::InstantProperty() { }

InstantProperty::~InstantProperty() { }

void InstantProperty::Evaluate(SimulationCell & simcell, Potential & pot)
{
 std::cerr << "[Warning] Ejecutando InstantProperty::Evaluate, que es una implementacion vacia!" << '\n';
}


