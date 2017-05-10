/*=========================================================================

  Program:   Mesh3D: Command-line tool for 3D mesh manipulation
  Module:    ReadMesh.cxx
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
#include "ReadMesh.h"
#include "CommandLineHelper.h"

#include <vtkBYUReader.h>
#include <vtkSTLReader.h>
#include <vtkPolyDataReader.h>
#include <vtkOBJReader.h>

bool
ReadMesh::Parse(CommandLineHelper &cl)
{
  // If the commandline has something that does not start with a '-' we take it
  if(cl.peek_arg()[0] != '-' || cl.try_command("-i"))
    {
    this->Run(cl.read_existing_filename());

    return true;
    }

  return false;
}

void
ReadMesh::Run(const string &fn)
{
   vtkPolyData *p1 = NULL;

  // Choose the reader based on extension
  if(fn.rfind(".byu") == fn.length() - 4)
    {
    vtkBYUReader *reader = vtkBYUReader::New();
    reader->SetFileName(fn.c_str());
    reader->Update();
    p1 = reader->GetOutput();
    }
  else if(fn.rfind(".stl") == fn.length() - 4)
    {
    vtkSTLReader *reader = vtkSTLReader::New();
    reader->SetFileName(fn.c_str());
    reader->Update();
    p1 = reader->GetOutput();
    }
  else if(fn.rfind(".vtk") == fn.length() - 4)
    {
    vtkPolyDataReader *reader = vtkPolyDataReader::New();
    reader->SetFileName(fn.c_str());
    reader->Update();
    p1 = reader->GetOutput();
    }
  else if(fn.rfind(".obj") == fn.length() - 4)
    {
    vtkOBJReader *reader = vtkOBJReader::New();
    reader->SetFileName(fn.c_str());
    reader->Update();
    p1 = reader->GetOutput();
    }
  else
    {
    this->ThrowException("No mesh reader configured for filename %s", fn.c_str());
    }

  this->Push(p1);
}
