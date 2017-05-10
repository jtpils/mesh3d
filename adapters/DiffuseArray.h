/*=========================================================================

  Program:   Mesh3D: Command-line tool for 3D mesh manipulation
  Module:    %fn%.h
  Module:    WriteMesh.h
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
#ifndef __DiffuseArray_h_
#define __DiffuseArray_h_

#include "CommandAdapter.h"

class DiffuseArray : public CommandAdapter
{
public:

  // Common typedefs
  MESH3D_STANDARD_TYPEDEFS

  // Basic constructor
  DiffuseArray(Converter *c);

  /** The command-line parsing functionality */
  bool Parse(CommandLineHelper &cl);

  /** Point array diffusion */
  void RunPointArray(const string &array, double t);

  /** Cell array diffusion */
  void RunCellArray(const string &array, double t);

  /** The main entrypoint for the API */
  void Run(const string &array, double t);

protected:

  double m_DeltaT;
};

#endif

