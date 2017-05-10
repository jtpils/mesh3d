/*=========================================================================

  Program:   Mesh3D: Command-line tool for 3D mesh manipulation
  Module:    CommandAdapter.h
  Language:  C++
  Website:   itksnap.org/mesh3d
  Copyright (c) 2017 Paul A. Yushkevich
  
  This file is part of Mesh3D, a command-line tool for 3D mesh manipulation

  Mesh3D is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
 
  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.

=========================================================================*/
#ifndef __CommandAdapter_h_
#define __CommandAdapter_h_

#include "Mesh3D.h"

class CommandLineHelper;

// Common typedefs for all child classes
#define MESH3D_STANDARD_TYPEDEFS \
  typedef Mesh3D Converter; \
  typedef Converter::PolyDataType PolyDataType; \
  typedef Converter::PolyDataPointer PolyDataPointer;

/**
 * Base class for all command adapters - provides easy access 
 * to the mesh stack.
 */
class CommandAdapter
{
public:
  MESH3D_STANDARD_TYPEDEFS 

  CommandAdapter(Converter *conv) : c(conv) {}

  virtual bool Parse(CommandLineHelper &cl) = 0;
  
protected:

  // Methods available to every adapter
  void Debug(const char *format,...);
  void Info(const char *format,...);
  void ThrowException(const char *format,...);

  // Direct access to the push/pull methods from the converter
  PolyDataPointer TopPolyData() { return c->TopPolyData(); }
  PolyDataPointer PopPolyData() { return c->PopPolyData(); }
  void Push(PolyDataType *pd);

  Converter *c;
};

#endif
