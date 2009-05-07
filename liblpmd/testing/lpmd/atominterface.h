/*
 *
 *
 *
 */

#ifndef __LPMD_ATOMINTERFACE_H__
#define __LPMD_ATOMINTERFACE_H__

#include <lpmd/basicvector.h>
#include "elements.h"

namespace lpmd
{

class AtomInterface
{
 public:
  AtomInterface(int z0): z(z0) { }
  virtual ~AtomInterface() { }

  inline int Z() const { return z; }
  inline std::string Symbol() const { return ElemSym[Z()]; }
  inline BasicVector & Position() { return iv[0]; }
  inline BasicVector & Velocity() { return iv[1]; }
  inline BasicVector & Acceleration() { return iv[2]; }
  inline double Mass() const { return ElemMass[Z()]; }

 protected:
  int z;
  BasicVector * iv;
};

}

#endif
