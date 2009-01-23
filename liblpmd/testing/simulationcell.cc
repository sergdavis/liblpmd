//
//
//

#include <lpmd/paramlist.h>
#include <lpmd/simulationcell.h>
#include <lpmd/cellmanager.h>
#include <lpmd/physunits.h>
#include <lpmd/util.h>
#include <lpmd/error.h>

#include "distcache.h"

#include <cmath>

using namespace lpmd;

//
//
CellManagerMissing::CellManagerMissing(): Error("No CellManager plugin enabled, cannot calculate neighbor lists!") { }

//
//
//
class lpmd::SimCellImpl
{
 public:
   unsigned int na, nb, nc;
   double virial;
   double s[3][3];
   CellManager * cm;
   DistanceCache * dc;

   ParamList mdata;

   SimCellImpl(): na(1), nb(1), nc(1), cm(NULL), dc(NULL) { }
   SimCellImpl(int nra, int nrb, int nrc): na(nra), nb(nrb), nc(nrc), cm(NULL), dc(NULL) { }
   ~SimCellImpl() { if (dc != NULL) delete dc; }
};

SimulationCell::SimulationCell() { impl = new SimCellImpl();}

SimulationCell::SimulationCell(int nra, int nrb, int nrc, bool pba, bool pbb, bool pbc) 
{
 impl = new SimCellImpl(nra, nrb, nrc); 
 SetPeriodicity(0, pba);
 SetPeriodicity(1, pbb);
 SetPeriodicity(2, pbc);
 for (int i=0;i<3;i++) 
 {
  for (int j=0;j<3;j++) impl->s[i][j]=0.0e0;
 }
}

SimulationCell::SimulationCell(const SimulationCell & sc): Particles(sc), Cell(sc)
{
 impl = new SimCellImpl(sc.impl->na, sc.impl->nb, sc.impl->nc);
 for (int i=0;i<3;++i) SetPeriodicity(i, sc.Periodic(i));
 if (sc.impl->dc != NULL) impl->dc = new DistanceCache(sc);
 //if (sc.impl->cm != NULL) SetCell(sc);
 SetCell(sc);
}

SimulationCell::~SimulationCell() { delete impl; }

SimulationCell & SimulationCell::operator=(const SimulationCell & sc)
{
 Particles::operator=(sc);
 delete impl;
 impl = new SimCellImpl(sc.impl->na, sc.impl->nb, sc.impl->nc);
 for (int i=0;i<3;++i) SetPeriodicity(i, sc.Periodic(i));
 if (sc.impl->dc != NULL) impl->dc = new DistanceCache(sc);
 //if (sc.impl->cm != NULL) SetCell(sc);
 SetCell(sc);
 return (*this);
}

void SimulationCell::RealPos()
{
 const long n = Size();
 for (int i=0;i<n;i++)
 {
  Atom tmp = GetAtom(i);
  Vector a = tmp.Position();
  tmp.SetPos(ScaleByCell(a));
  SetAtom(tmp, i);
 }
}

void SimulationCell::FracPos() 
{
 const long n = Size();
 for (int i=0;i<n;i++)
 {
  Vector a = GetAtom(i).Position(), n;
  ConvertToInternal(a);
  for (int j=0;j<3;++j) n.Set(j, a.Get(j)/GetVector(j).Mod());
  Atom tmp = GetAtom(i);
  tmp.SetPos(n);
  SetAtom(tmp, i);
 }
}

void SimulationCell::Center()
{
 const long n = Size();
 Vector displacement = ScaleByCell(Vector(0.5e0, 0.5e0, 0.5e0));
 for (long i=0;i<n;i++)
 {
  Atom tmp = GetAtom(i);
  tmp.SetPos(tmp.Position() - displacement);
  SetAtom(tmp, i);
 }
}

void SimulationCell::UnCenter()
{
 const long n = Size();
 Vector displacement = ScaleByCell(Vector(0.5e0, 0.5e0, 0.5e0));
 for (long i=0;i<n;i++)
 {
  Atom tmp = GetAtom(i);
  tmp.SetPos(tmp.Position() + displacement);
  SetAtom(tmp, i);
 }
}

void SimulationCell::SuperCell() { throw Error("SimulationCell::SuperCell() is obsolete! Please use the \"replicate\" plugin instead."); }

CellManager & SimulationCell::GetCellManager() const { return *(impl->cm); }

void SimulationCell::SetCellManager(CellManager & cellman) 
{ 
 cellman.Reset();
 impl->cm = &cellman;
 (impl->cm)->UpdateCell(*this);
}

void SimulationCell::UseDistanceCache(bool activ)
{
 if (activ)
 {
  if (impl->dc == NULL) impl->dc = new DistanceCache(*this);
 }
 else if (impl->dc != NULL) 
 {
  delete impl->dc;
  impl->dc = NULL;
 }
}

void SimulationCell::SetCell(const Cell & c)
{
 for (int i=0;i<3;++i) SetVector(i, c.GetVector(i));
}

void SimulationCell::SetPart(const Particles & p)
{
 for (int i=0;i<p.Size();i++) AppendAtom(p.GetAtom(i));
 NumEspec();
 if (impl->cm != NULL) (impl->cm)->UpdateCell(*this);
}

Vector SimulationCell::FracPosition(long i) const
{
 Vector a = GetAtom(i).Position(), n;
 ConvertToInternal(a);
 for (int j=0;j<3;++j) n.Set(j, a.Get(j)/GetVector(j).Mod());
 return n;
}

void SimulationCell::SetPosition(long i, const Vector & p)
{
 Vector vtmp = p;
 ConvertToInternal(vtmp);
 for (int j=0;j<3;++j)
 {
  double q = vtmp.Get(j);
  const double ll = GetVector(j).Mod();
  if (Periodic(j))
  {
   if (q < 0) q += ll*(1+floor(-q/ll)); 
   else if (q > ll) q -= ll*floor(q/ll);
   vtmp.Set(j, q);
  }
 }
 ConvertToExternal(vtmp);
 Atom a = GetAtom(i);
 a.SetIndex(i);
 a.SetPos(vtmp);
 SetAtom(a, i);
}

void SimulationCell::SetFracPosition(long i, const Vector & fp)
{
 SetPosition(i, ScaleByCell(fp));
}

void SimulationCell::SetVelocity(long i, const Vector & v)
{
 Atom a = GetAtom(i);
 a.SetVel(v);
 SetAtom(a, i);
}

void SimulationCell::SetAcceleration(long i, const Vector & ac)
{
 Atom a = GetAtom(i);
 a.SetAccel(ac);
 SetAtom(a, i);
}

void SimulationCell::ClearForces()
{
 const Vector zero;
 for (long i=0;i<Size();++i) SetAcceleration(i, zero);
 if (impl->dc != NULL) (impl->dc)->Clear();
 if (impl->cm != NULL) (impl->cm)->UpdateCell(*this);
 impl->virial = 0.0;
}

void SimulationCell::BuildNeighborList(long i, std::list<Neighbor> & nlist, bool full, double rcut)
{
 if (impl->cm == NULL) throw CellManagerMissing();
 else (impl->cm)->BuildNeighborList(*this, i, nlist, full, rcut);
}

void SimulationCell::BuildList(bool full, double rcut)
{
 if (impl->cm == NULL) throw CellManagerMissing();
 else (impl->cm)->BuildList(*this,full,rcut);
}

Vector SimulationCell::VectorDistance(long i, long j)
{
 if (impl->dc != NULL) return (impl->dc)->VectorDistance(i, j);
 return Displacement(Particles::GetAtom(i).Position(), Particles::GetAtom(j).Position());
}

void SimulationCell::SortBySpecies(void)
{
 //Generamos la lista de especies.
 std::list<std::string> lista=(*this).SpeciesList();
 std::vector<lpmd::Atom> *tmp = new std::vector<lpmd::Atom>[lista.size()]; 
 for(int i=0 ; i < (*this).Size() ; ++i)
 { 
  int j=0;
  for(std::list<std::string>::const_iterator it=lista.begin();it!=lista.end();++it)
  {
   if(ElemNum(*it)==(*this)[i].Species()) 
   {
    tmp[j].push_back((*this)[i]);
   }
   j++;
  }
 }
 (*this).Clear();
 int j=0;
 for(std::list<std::string>::const_iterator it=lista.begin();it!=lista.end();++it)
 {
  for(unsigned long int i=0 ; i < tmp[j].size() ; ++i)
  {
   (*this).AppendAtom((tmp[j])[i]);
  }
  j++;
 }
}

/*
Vector SimulationCell::VectorDistance(const Atom & i, const Atom & j)
{
 return Displacement(i.Position(), j.Position());
}
*/

Vector SimulationCell::VectorRealDistance(long i, long j) { return GetAtom(i).Position() - GetAtom(j).Position(); }

Vector SimulationCell::VectorDistanceToReplica(long i, long j, long nx, long ny, long nz)
{
 if (impl->dc != NULL) return (impl->dc)->VectorDistanceToReplica(i, j, nx, ny, nz);
 Vector vi = Particles::GetAtom(i).Position();
 Vector vj = Particles::GetAtom(j).Position(); 
 if (nx==0 && (ny==0 && nz==0)) return vj-vi;  
 else return (ScaleByCell(Vector(nx, ny, nz)) + vj - vi);
}

double SimulationCell::Distance(long i, long j)
{
 if (impl->dc != NULL) return (impl->dc)->Distance(i, j);
 return VectorDistance(i, j).Mod();
}

double SimulationCell::RealDistance(long i, long j) { return VectorRealDistance(i,j).Mod(); }

double SimulationCell::DistanceToReplica(long i, long j, long nx, long ny, long nz) { return VectorDistanceToReplica(i,j,nx,ny,nz).Mod(); }

double SimulationCell::Distance2(long i,long j)
{
 if (impl->dc != NULL) return (impl->dc)->Distance2(i, j);
 return VectorDistance(i, j).Mod2();
}

double SimulationCell::Angle(long i, long j, long k) 
{
 Vector a = VectorDistance(i,j);
 Vector b = VectorDistance(k,j);
 return acos(Dot(a, b) / (a.Mod()*b.Mod())); 
}

void SimulationCell::Rescale(double f)
{
 FracPos();
 for (int i=0;i<3;++i) Cell::Scale(i, f);
 RealPos();
}

void SimulationCell::Rescale(double f, int i)
{
 FracPos();
 Cell::Scale(i, f);
 RealPos();
}

void SimulationCell::RescalePercent(double p)
{
 FracPos();
 for (int i=0;i<3;++i) Cell::ScalePercent(i, p);
 RealPos();
}

void SimulationCell::RescalePercent(double p, int i)
{
 FracPos();
 Cell::ScalePercent(i,p);
 RealPos();
}

void SimulationCell::RescaleVector(Vector sx, Vector sy, Vector sz)
{
 FracPos();
 Cell::ScaleVector(0,sx);
 Cell::ScaleVector(1,sy);
 Cell::ScaleVector(2,sz);
 RealPos();
}

void SimulationCell::RescaleVector(Vector s, int i)
{
 FracPos();
 Cell::ScaleVector(i,s);
 RealPos();
}

//
// SECCION :
// Calculo y/o asignacion de Propiedades Fisicas para un
// SimulationCell
//

double SimulationCell::Virial() const { return impl->virial; }
void SimulationCell::AddToVirial(double vir) { impl->virial += vir; }

double SimulationCell::Density() const
{
 double m = 0.0e0, v = Volume();
 for(long i=0;i<Size();++i)
 {
  Atom a = GetAtom(i);
  m += a.Mass();
 }
 return m/v;
}

double SimulationCell::ParticleDensity() const
{
 double N = (double)Size();
 double V = Volume();
 return N/V;
}

double & SimulationCell::StressTensor(int alpha, int beta) //Combina Get y Set
{
 if(alpha>2 || alpha<0) 
 {
  throw Error("bad alpha index for stress set/get, please report");
 }
 else if(beta>2 || beta<0)
 {
  throw Error("bad beta index for stress set/get, please report");
 }
 else return impl->s[alpha][beta];
}


double SimulationCell::CMCutoff() const
{
 if (impl->cm == NULL) throw CellManagerMissing();
 CellManager & cm = GetCellManager();
 return cm.Cutoff();
}

ParamList & SimulationCell::MetaData() const { return impl->mdata; }


