/*=========================================================================

  Program:   Mesh3D: Command-line tool for 3D mesh manipulation
  Module:    WriteMesh.cxx
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
#include "WriteMesh.h"
#include "CommandLineHelper.h"

#include <vtkPolyData.h>
#include <vtkBYUWriter.h>
#include <vtkSTLWriter.h>
#include <vtkPolyDataWriter.h>
#include <vtkOBJExporter.h>
#include <vtkOOGLExporter.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkActor.h>
#include <vtkPolyDataMapper.h>

bool
WriteMesh::Parse(CommandLineHelper &cl)
{
  if(cl.try_command("-o"))
    {
    this->Run(cl.read_output_filename());
    return true;
    }
  return false;
}

void
WriteMesh::Run(const string &fn)
{
  // Get mesh from stack
  PolyDataPointer data = this->TopPolyData();

  if(fn.rfind(".byu") == fn.length() - 4)
    {
    vtkBYUWriter *writer = vtkBYUWriter::New();
    writer->SetGeometryFileName(fn.c_str());
    writer->SetInputData(data);
    writer->Update();
    }
  else if(fn.rfind(".stl") == fn.length() - 4)
    {
    vtkSTLWriter *writer = vtkSTLWriter::New();
    writer->SetFileName(fn.c_str());
    writer->SetInputData(data);
    writer->Update();
    }
  else if(fn.rfind(".vtk") == fn.length() - 4)
    {
    vtkPolyDataWriter *writer = vtkPolyDataWriter::New();
    writer->SetFileName(fn.c_str());
    writer->SetInputData(data);
    writer->Update();
    }
  else if(fn.rfind(".obj") == fn.length() - 4)
    {
    vtkRenderer *renderer = vtkRenderer::New();
    vtkPolyDataMapper *myDataMapper = vtkPolyDataMapper::New();
    myDataMapper->SetInputData(data);
    vtkActor *myActor = vtkActor::New();
    myActor->SetMapper(myDataMapper);
    renderer->AddActor(myActor);
    vtkRenderWindow *renWin = vtkRenderWindow::New();
    renWin->AddRenderer(renderer);

    vtkOBJExporter *writer = vtkOBJExporter::New();
    string prefix = fn.substr(0, fn.length() - 4);
    writer->SetFilePrefix(prefix.c_str());
    writer->SetRenderWindow(renWin);
    writer->Update();
    }
  else if(fn.rfind(".off") == fn.length() - 4)
    {
    vtkRenderer *renderer = vtkRenderer::New();
    vtkPolyDataMapper *myDataMapper = vtkPolyDataMapper::New();
    myDataMapper->SetInputData(data);
    vtkActor *myActor = vtkActor::New();
    myActor->SetMapper(myDataMapper);
    renderer->AddActor(myActor);
    vtkRenderWindow *renWin = vtkRenderWindow::New();
    renWin->AddRenderer(renderer);

    vtkOOGLExporter *writer = vtkOOGLExporter::New();
    writer->SetFileName(fn.c_str());
    writer->SetRenderWindow(renWin);
    writer->Update();
    }
  else
    {
    cout << "Could not find a writer for " << fn << endl;
    cout << fn.rfind(".byu") << endl;
    cout << fn.rfind(".vtk") << endl;
    return;
    }
}
