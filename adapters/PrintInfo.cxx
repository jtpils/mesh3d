/*=========================================================================

  Program:   Mesh3D: Command-line tool for 3D mesh manipulation
  Module:    PrintInfo.cxx
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
#include "PrintInfo.h"
#include "CommandLineHelper.h"
#include "vtkPolyData.h"

bool
PrintInfo::Parse(CommandLineHelper &cl)
{
  if(!cl.try_command("-info"))
    return false;

  this->Run();

  return true;
}

void
PrintInfo::Run()
{
  // Get the top mesh from the stack
  PolyDataPointer mesh = this->TopPolyData();

  // Print some basic statistics about the mesh
  this->Info("Mesh Information\n");
  this->Info("  Number of Points: %d\n", mesh->GetNumberOfPoints());
  this->Info("  Number of Cells: %d\n", mesh->GetNumberOfCells());

  // Print information on point data, cell data, etc.
}
