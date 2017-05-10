/*=========================================================================

  Program:   Mesh3D: Command-line tool for 3D mesh manipulation
  Module:    AddArray.cxx
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
#include "AddArray.h"
#include "CommandLineHelper.h"
#include "vtkPolyData.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkDoubleArray.h"

bool
AddArray::Parse(CommandLineHelper &cl)
{
  if(!cl.try_command("-aa", "-add-array"))
    return false;

  this->Run(cl.read_string(), cl.read_existing_filename());
  return true;
}

void
AddArray::Run(const string &array, const string &fin)
{
  // Get mesh from stack
  PolyDataPointer p = this->TopPolyData();

  // Get the number of points or cells to read
  int n = this->GetDataArraySize(p);
  int ncol = 0;

  // Open the file
  std::ifstream is(fin.c_str());

  // Figure out the number of rows
  string line;
  if(!getline(is, line))
    this->ThrowException("Unable to read one line from %s\n", fin.c_str());

  double val = 0.0;
  std::vector<double> row;
  std::istringstream iss(line);
  while(iss >> val)
    row.push_back(val);

  // Figure out number of columsn
  ncol = row.size();


  if(ncol == 0)
    this->ThrowException("No columns in file %s\n", fin.c_str());

  // Allocate the array
  vtkSmartPointer<vtkDoubleArray> da = vtkDoubleArray::New();
  da->SetNumberOfComponents(ncol);
  da->SetNumberOfTuples(n);

  // Fill the first entry
  for(int i = 0; i < ncol; i++)
    da->SetComponent(0, i, row[i]);

  // Read the rest of the file
  for(int j = 1; j < n; j++)
    {
    if(getline(is, line))
      {
      for(int i = 0; i < ncol; i++)
        {
        std::istringstream iss(line);
        double val;
        if(iss >> val)
          {
          da->SetComponent(j, i, val);
          }
        else
          this->ThrowException("Too few components in row %d of file %s. Expected %d, read %d",
            j, fin.c_str(), ncol, i);
        }
      }
    else this->ThrowException("File %s ended on line %d, but %d array tuples were expected",
      fin.c_str(), j, n);
    }

  // Add the array
  da->SetName(array.c_str());
  this->AddDataArray(p, da);
}
