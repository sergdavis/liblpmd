//
//
//

#include <lpmd/onestepintegrator.h>
#include <lpmd/potential.h>
#include <lpmd/simulationcell.h>

using namespace lpmd;

void OneStepIntegrator::Advance(SimulationCell & sc, Potential & p)
{
 p.UpdateForces(sc);
 // Setea a cero las aceleraciones de los atomos con fixedvel
 const Vector aczero(0.0, 0.0, 0.0);
 for (long i=0;i<sc.Size();++i) 
 {
  const Atom & at = sc[i];
  if (at.IsTypeSet() && at.Type().GetBool("fixedvel")) sc.SetAcceleration(i, aczero);
 }
 for (long i=0;i<sc.Size();++i)
 {
  const Atom & at = sc[i];
  if (at.IsTypeSet() && at.Type().GetBool("fixedpos")) continue;
  else Advance(sc, i);
 }
}
