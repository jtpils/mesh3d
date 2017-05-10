/*=========================================================================

  Program:   Mesh3D: Command-line tool for 3D mesh manipulation
  Module:    Mesh3D.h
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
#ifndef __Mesh3D_h_
#define __Mesh3D_h_

#include <vtkSmartPointer.h>
#include <vector>
#include <string>

using std::string;

class vtkPointSet;
class vtkPolyData;
class CommandAdapter;

/**
 * A simple exception class with string formatting
 */
class MeshException : public std::exception
{
public:

  MeshException(const char *format, ...)
    {
    buffer = new char[4096];
    va_list args;
    va_start (args, format);
    vsprintf (buffer,format, args);
    va_end (args);
    }

  virtual const char* what() const throw() { return buffer; }

  virtual ~MeshException() throw() { delete buffer; }

private:

  char *buffer;

};


/**
 * Main class that manages the mesh stack
 */
class Mesh3D
{
public:
  // Common VTK types
  typedef vtkPolyData PolyDataType;
  typedef vtkSmartPointer<PolyDataType> PolyDataPointer;

  typedef vtkPointSet PointSetType;
  typedef vtkSmartPointer<PointSetType> PointSetPointer;

  // Constructor
  Mesh3D();

  // Main method
  void ProcessCommandLine(int argc, char *argv[]);

  // Stack-related methods
  PolyDataPointer PopPolyData();
  PolyDataPointer TopPolyData();

  void Push(PointSetType *mesh);

  // Output to standard out and debug stream
  void Debug(const char *text);
  void Info(const char *text);

protected:

  // A stack of VTK objects
  typedef std::vector<PointSetPointer> MeshStack;
  MeshStack m_Stack;

  // A list of command adapters
  std::vector<CommandAdapter *> m_Adapters;

  // Verbose mode?
  bool m_Verbose;
};


#endif
