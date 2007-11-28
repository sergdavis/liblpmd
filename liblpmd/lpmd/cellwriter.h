//
//
//

#ifndef __LPMD_CELLWRITER_H__
#define __LPMD_CELLWRITER_H__

#include "simulationcell.h"
#include <string>
#include <iostream>

namespace lpmd
{

class CellWriter
{
 public:
  //
  virtual ~CellWriter();

  virtual void WriteHeader(std::ostream & os) const = 0;
  virtual void WriteCell(std::ostream & os, SimulationCell & sc) const = 0;
  virtual long int GetInterval() const = 0;

  void Write(const std::string & filename, SimulationCell & sc) const;
  std::string GetFile();

 protected:
  std::string writefile;
};

} // lpmd

#endif

