/*=========================================================================

  Program:   Mesh3D: Command-line tool for 3D mesh manipulation
  Module:    DiffuseArray.cxx
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
#include "DiffuseArray.h"
#include "CommandLineHelper.h"
#include "vtkPolyData.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkCell.h"
#include "vtkDoubleArray.h"
#include <algorithm>
#include <set>

namespace diffuse_array {

class Edge : public std::pair<vtkIdType, vtkIdType>
{
public:
  Edge(vtkIdType a, vtkIdType b) : std::pair<vtkIdType,vtkIdType>(std::min(a,b), std::max(a,b)) {}
};

} // namespace

using namespace diffuse_array;

DiffuseArray::DiffuseArray(Converter *c) : CommandAdapter(c)
{
  m_DeltaT = 0.01;
}

bool
DiffuseArray::Parse(CommandLineHelper &cl)
{
  if(!cl.try_command("-diffuse") && !cl.try_command("-diffuse-array"))
    return false;

  // Run command
  this->Run(cl.read_string(), cl.read_double());

  return true;
}

void
DiffuseArray::Run(const string &array, double time)
{
  if(this->c->GetCellMode())
    this->RunCellArray(array, time);
  else
    this->RunPointArray(array, time);
}

void
DiffuseArray::RunPointArray(const string &array, double time)
{
  // Diffusion simulates heat equation, dF/dt = -Laplacian(F), for t = time
  // We use the most basic approximation of the laplacian L(F) = [Sum_{j\in N(i)} F(j) - F(i)] / |N(i)|
  PolyDataPointer mesh = this->TopPolyData();

  // Create a set of all edges in the mesh
  typedef std::set<Edge> EdgeSet;
  EdgeSet edges;

  // Report
  Info("Performing diffusion on point data (t = %f, delta_t = %f)\n", time, m_DeltaT);

  // Get all edges into the edge set
  for(int i = 0; i < mesh->GetNumberOfCells(); i++)
    {
    vtkCell *cell = mesh->GetCell(i);
    vtkIdType *p = cell->GetPointIds()->GetPointer(0);
    if(cell->GetCellType() == VTK_TRIANGLE)
      {
      edges.insert(Edge(p[0], p[1]));
      edges.insert(Edge(p[1], p[2]));
      edges.insert(Edge(p[0], p[2]));
      }
    else if(cell->GetCellType() == VTK_TETRA)
      {
      edges.insert(Edge(p[0], p[1]));
      edges.insert(Edge(p[0], p[2]));
      edges.insert(Edge(p[0], p[3]));
      edges.insert(Edge(p[1], p[2]));
      edges.insert(Edge(p[1], p[3]));
      edges.insert(Edge(p[2], p[3]));
      }
    else this->ThrowException("Wrong cell type for diffusion, must be triangle or tetra");
    }

  // Count the number of neighbors of each vertex
  std::vector<int> nbr(mesh->GetNumberOfPoints(), 0);
  for(EdgeSet::iterator it = edges.begin(); it!=edges.end(); ++it)
    {
    nbr[it->first]++; 
    nbr[it->second]++;
    }

  // Create an array for the updates
  vtkDataArray *f = mesh->GetPointData()->GetArray(array.c_str());
  vtkDoubleArray *f_upd = vtkDoubleArray::New();
  f_upd->SetNumberOfComponents(f->GetNumberOfComponents());
  f_upd->SetNumberOfTuples(f->GetNumberOfTuples());

  // Copy f to f_upd
  for(int i = 0; i < f->GetNumberOfTuples(); i++)
    for(int j = 0; j < f->GetNumberOfComponents(); j++)
      f_upd->SetComponent(i, j, f->GetComponent(i, j));

  // Iterate
  unsigned int jt = 0;
  for(double t = 0; t < time - m_DeltaT/2; t+=m_DeltaT)
    {
    // Update f_upd
    for(EdgeSet::iterator it = edges.begin(); it!=edges.end(); ++it)
      {
      double wa = m_DeltaT / nbr[it->first];
      double wb = m_DeltaT / nbr[it->second];
      int ia = it->first;
      int ib = it->second;

      for(int j = 0; j < f->GetNumberOfComponents(); j++)
        {
        f_upd->SetComponent(ia, j, 
          f_upd->GetComponent(ia, j) + wa * (f->GetComponent(ib,j) - f->GetComponent(ia, j)));
        f_upd->SetComponent(ib, j, 
          f_upd->GetComponent(ib, j) + wb * (f->GetComponent(ia,j) - f->GetComponent(ib, j)));
        }
      }

    // Copy f_upd to f
    for(int i = 0; i < f->GetNumberOfTuples(); i++)
      for(int j = 0; j < f->GetNumberOfComponents(); j++)
        f->SetComponent(i, j, f_upd->GetComponent(i, j));
    }
}


void
DiffuseArray::RunCellArray(const string &array, double time)
{
  // Get the mesh
  PolyDataPointer mesh = this->TopPolyData();

  // Diffusion, but between cells. This is really pretty ad hoc now
  mesh->BuildLinks();

  // Create a set of all edges in the mesh. These are pairs of adjacent cells that
  // share an edge
  typedef std::set<Edge> EdgeSet;
  EdgeSet edges;

  // Report
  this->Debug("Performing diffusion on cell data (t = %f, delta_t = %f)\n", time, m_DeltaT);

  // Get all edges into the edge set
  for(int i = 0; i < mesh->GetNumberOfCells(); i++)
    {
    vtkCell *cell = mesh->GetCell(i);
    if(cell->GetCellType() == VTK_TETRA)
      {
      for(int j = 0; j < cell->GetNumberOfFaces(); j++)
        {
        vtkSmartPointer<vtkIdList> nbr = vtkIdList::New();
        vtkCell *face = cell->GetFace(j);
        mesh->GetCellNeighbors(i, face->GetPointIds(), nbr);
        for(int k = 0; k < nbr->GetNumberOfIds(); k++)
          edges.insert(Edge(i, nbr->GetId(k)));
        }
      }
    else if(cell->GetCellType() == VTK_TRIANGLE)
      {
      for(int j = 0; j < cell->GetNumberOfEdges(); j++)
        {
        vtkSmartPointer<vtkIdList> nbr = vtkIdList::New();
        vtkCell *edge = cell->GetEdge(j);
        mesh->GetCellNeighbors(i, edge->GetPointIds(), nbr);
        for(int k = 0; k < nbr->GetNumberOfIds(); k++)
          edges.insert(Edge(i, nbr->GetId(k)));
        }
      }
    else throw MeshException("Wrong cell type in CellDataDiffusion");
    }

  this->Debug("There are %d pairs of adjacent cells\n", (int) edges.size());

  // Count the number of neighbors of each cell
  std::vector<int> nbr(mesh->GetNumberOfCells(), 0);
  for(EdgeSet::iterator it = edges.begin(); it!=edges.end(); ++it)
    {
    nbr[it->first]++; 
    nbr[it->second]++;
    }

  // Create an array for the updates
  vtkDataArray *f = mesh->GetCellData()->GetArray(array.c_str());
  vtkDoubleArray *f_upd = vtkDoubleArray::New();
  f_upd->SetNumberOfComponents(f->GetNumberOfComponents());
  f_upd->SetNumberOfTuples(f->GetNumberOfTuples());

  // Copy f to f_upd
  for(int i = 0; i < f->GetNumberOfTuples(); i++)
    for(int j = 0; j < f->GetNumberOfComponents(); j++)
      f_upd->SetComponent(i, j, f->GetComponent(i, j));

  // Iterate
  unsigned int jt = 0;
  for(double t = 0; t < time - m_DeltaT/2; t+=m_DeltaT)
    {
    // Update f_upd
    for(EdgeSet::iterator it = edges.begin(); it!=edges.end(); ++it)
      {
      double wa = m_DeltaT / nbr[it->first];
      double wb = m_DeltaT / nbr[it->second];
      int ia = it->first;
      int ib = it->second;

      for(int j = 0; j < f->GetNumberOfComponents(); j++)
        {
        f_upd->SetComponent(ia, j, 
          f_upd->GetComponent(ia, j) + wa * (f->GetComponent(ib,j) - f->GetComponent(ia, j)));
        f_upd->SetComponent(ib, j, 
          f_upd->GetComponent(ib, j) + wb * (f->GetComponent(ia,j) - f->GetComponent(ib, j)));
        }
      }

    // Copy f_upd to f
    for(int i = 0; i < f->GetNumberOfTuples(); i++)
      for(int j = 0; j < f->GetNumberOfComponents(); j++)
        f->SetComponent(i, j, f_upd->GetComponent(i, j));
    }
}
