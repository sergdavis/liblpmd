/*
 *
 *
 *
 */

#ifndef __LPMD_REGION_H__
#define __LPMD_REGION_H__

#include <lpmd/vector.h>

namespace lpmd
{

 class Region
 {
  public:
    virtual ~Region() { }

    virtual bool IsInside(const Vector & v) const = 0;

    virtual double Volume() const = 0;
 };

 //
 //
 //
 class Box: public Region
 {
  public:
    Box(double xlength, double ylength, double zlength)
    {
     for (int q=0;q<3;++q) vmin[q] = 0.0;
     vmax[0] = xlength;
     vmax[1] = xlength;
     vmax[2] = xlength;
    }
    
    Box(double xmin, double xmax, double ymin, double ymax, double zmin, double zmax)
    {
     vmin[0] = xmin;
     vmin[1] = ymin;
     vmin[2] = zmin;
     vmax[0] = xmax;
     vmax[1] = ymax;
     vmax[2] = zmax;
    }

    inline bool IsInside(const Vector & v) const 
    {
     if ((v[0] < vmin[0]) || (v[0] > vmax[0])) return false;
     if ((v[1] < vmin[1]) || (v[1] > vmax[1])) return false;
     if ((v[2] < vmin[2]) || (v[2] > vmax[2])) return false;
     return true;
    }

    inline double Volume() const { return (vmax[0]-vmin[0])*(vmax[1]-vmin[1])*(vmax[2]-vmin[2]); }

  private:
   double vmin[3], vmax[3];
 };

 class Sphere: public Region
 {
  public:
    Sphere(const Vector & center, double radius): _center(center), _radius(radius) { }
    
    inline bool IsInside(const Vector & v) const { return ((v-_center).Module() < _radius); }
    
    inline double Volume() const { return (4.0/3.0)*M_PI*pow(_radius, 3.0); }

  private:
    Vector _center;
    double _radius;
 };

 class Cylinder: public Region
 {
  public:
   Cylinder(const Vector & S, const Vector & origin, double rmax, double rmin): _S(S), _origin(origin), _rmax(rmax), _rmin(rmin) { }

   inline bool IsInside(const Vector & v) const 
   { 
    double temp = (Cross(v-_origin,_S).Module())/(_S.Module());
    if ( temp < _rmax && temp > _rmin)
    {
      Vector norm = _S ;
      norm.Normalize();
      double dp = Dot(norm, v-_origin);
      if (dp < 0 || dp > _S.Module()) return false;
      else return true;
    }
    else return false;
   }
   inline double Volume() const { return ((M_PI*(_rmax*_rmax-_rmin*_rmin))*_S.Module()); }
  private:
   Vector _S,_origin;
   double _rmax;
   double _rmin;
 };

}  // lpmd

#endif

