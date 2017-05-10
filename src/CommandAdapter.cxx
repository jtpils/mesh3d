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
#include "CommandAdapter.h"
#include <vtkPolyData.h>
#include <vtkCellData.h>
#include <vtkPointData.h>

void
CommandAdapter::Push(PolyDataType *p)
{
  c->Push(p);
}

void CommandAdapter::Info(const char *format,...)
{
  char buffer[4096];
  va_list args;
  va_start (args, format);
  vsprintf (buffer,format, args);
  va_end (args);

  c->Info(buffer);
}

void CommandAdapter::Debug(const char *format,...)
{
  char buffer[4096];
  va_list args;
  va_start (args, format);
  vsprintf (buffer,format, args);
  va_end (args);

  c->Debug(buffer);
}

void CommandAdapter::ThrowException(const char *format,...)
{
  char buffer[4096];
  va_list args;
  va_start (args, format);
  vsprintf (buffer,format, args);
  va_end (args);

  throw MeshException(buffer);
}

int CommandAdapter::GetDataArraySize(PolyDataType *mesh)
{
  return c->GetCellMode() ? mesh->GetNumberOfCells() : mesh->GetNumberOfPoints();
}

void CommandAdapter::AddDataArray(PolyDataType *mesh, DataArrayType *array)
{
  if(c->GetCellMode())
    {
    mesh->GetCellData()->AddArray(array);
    }
  else
    {
    mesh->GetPointData()->AddArray(array);
    }
}

CommandAdapter::DataArrayPointer 
CommandAdapter::GetDataArray(PolyDataType *mesh, const string &array, bool throw_if_missing)
{
  DataArrayPointer p;
  if(c->GetCellMode())
    {
    p = mesh->GetCellData()->GetArray(array.c_str());
    }
  else
    {
    p = mesh->GetPointData()->GetArray(array.c_str());
    }

  if(throw_if_missing && p == 0)
    this->ThrowException("Missing array %s in mesh", array.c_str());

  return p;
}

