//
//
//

#include <iostream>

#include <lpmd/coulomb.h>

using namespace lpmd;

Coulomb::Coulomb(double Q1, double Q2)
{
 q1 = Q1;
 q2 = Q2;
}

Coulomb::Coulomb()
{
 q1 = 0;
 q2 = 0;
}

Coulomb::~Coulomb() { }

void Coulomb::SetValue1(double Q1)
{
 q1 = Q1;
}

void Coulomb::SetValue2(double Q2)
{
 q2 = Q2;
}

double Coulomb::pairEnergy(double r) const
{
 std::cerr << "Calculando energia Coulomb para r=" << r << std::endl; 
 double  E = q1*q2/r;
 return E; 
}

Vector Coulomb::pairForce(Vector r) const
{
 std::cerr << "Calculando fuerza Coulomb para vector r=" << r << std::endl; 
 double rr = r.Mod();
 double ff = q1*q2/(rr*rr*rr);
 Vector fv = r;
 fv.Scale(ff);
 return fv;
}

