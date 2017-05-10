/*=========================================================================

  Program:   Mesh3D: Command-line tool for 3D mesh manipulation
  Module:    Mesh3D.cxx
  Language:  C++
  Website:   none yet
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
#include "Mesh3D.h"
#include <CommandLineHelper.h>

#include "CommandAdapter.h"

#include "AddArray.h"
#include "DiffuseArray.h"
#include "DumpArray.h"
#include "PrintInfo.h"
#include "ReadMesh.h"
#include "WriteMesh.h"

#include <vtkPolyData.h>

Mesh3D::Mesh3D()
{
  // Register all the adapters
  m_Adapters.push_back(new AddArray(this));
  m_Adapters.push_back(new DiffuseArray(this));
  m_Adapters.push_back(new DumpArray(this));
  m_Adapters.push_back(new PrintInfo(this));
  m_Adapters.push_back(new ReadMesh(this));
  m_Adapters.push_back(new WriteMesh(this));

  // Global flags
  m_Verbose = false;
  m_CellMode = false;
}

void Mesh3D::ProcessCommandLine(const int argc, char *argv[])
{
  // Command line helper
  CommandLineHelper cl(argc, argv);

  // Parse the command line
  while(!cl.is_at_end())
    {
    // Try 'built-in' commands
    if(cl.try_command("-verbose"))
      {
      m_Verbose = true;
      }
    else if(cl.try_command("-cell-mode") || cl.try_command("-cm"))
      {
      m_CellMode = true;
      }
    else if(cl.try_command("-point-mode") || cl.try_command("-pm"))
      {
      m_CellMode = false;
      }
    else
      {
      // Try all adapters until one accepts the command
      bool command_accepted = false;
      for(int i = 0; i < m_Adapters.size() && !command_accepted; i++)
        command_accepted = m_Adapters[i]->Parse(cl);

      if(!command_accepted)
        throw MeshException("Unknown command or argument %s", cl.peek_arg());
      }
    }
}

void Mesh3D::Info(const char *text)
{
  std::cout << text;
}

void Mesh3D::Debug(const char *text)
{
  if(m_Verbose)
    std::cout << text;
}

Mesh3D::PolyDataPointer Mesh3D::TopPolyData()
{
  if(m_Stack.size() == 0)
    throw MeshException("Attempt to pop mesh from an empty stack");

  PointSetPointer p = m_Stack.back();
  PolyDataType *ppd = dynamic_cast<PolyDataType *>(p.GetPointer());
  PolyDataPointer pd = ppd;
  return pd;
}

Mesh3D::PolyDataPointer Mesh3D::PopPolyData()
{
  PolyDataPointer p = this->TopPolyData();
  m_Stack.pop_back();
  return p;
}

void Mesh3D::Push(PointSetType *data)
{
  m_Stack.push_back(data);
}


int main(int argc, char *argv[])
{
  Mesh3D mesh3D;
  try
    {
    mesh3D.ProcessCommandLine(argc, argv);
    return 0;
    }
  catch(std::exception &exc)
    {
    std::cerr << "Processing failed due to exception" << std::endl;
    std::cerr <<  exc.what() << std::endl;
    return -1;
    }
}
