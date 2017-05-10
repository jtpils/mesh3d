/*=========================================================================

  Program:   Mesh3D: Command-line tool for 3D mesh manipulation
  Module:    DumpArray.cxx
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
#include "DumpArray.h"
#include "CommandLineHelper.h"
#include "vtkPolyData.h"
#include "vtkPointData.h"
#include "vtkCellData.h"

bool
DumpArray::Parse(CommandLineHelper &cl)
{
  if(!cl.try_command("-da", "-dump-array"))
    return false;

  this->Run(cl.read_string(), cl.read_output_filename());
  return true;
}

void
DumpArray::Run(const string &array, const string &fout)
{
  // Get mesh from stack
  PolyDataPointer p = this->TopPolyData();

  // Get the array - this will crash if the array is missing
  DataArrayPointer arr = this->GetDataArray(p, array);

  // Create a stream
  std::ofstream fs(fout.c_str());

  // Write the array values
  for(int iPoint = 0; iPoint < arr->GetNumberOfTuples(); iPoint++)
    {
    for(int iComp = 0; iComp < arr->GetNumberOfComponents(); iComp++)
      {
      fs << arr->GetComponent(iPoint, iComp);
      if(iComp + 1 < arr->GetNumberOfComponents())
        fs << " ";
      else
        fs << std::endl;
      }
    }
}
