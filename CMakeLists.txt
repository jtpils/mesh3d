PROJECT(MESH3D)

# Nonsense for cmake 2.6 compatibility
CMAKE_MINIMUM_REQUIRED(VERSION 2.8.12)
if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

# Get ITK
FIND_PACKAGE(VTK REQUIRED)
INCLUDE(${VTK_USE_FILE})

# Sources
INCLUDE_DIRECTORIES(src)
INCLUDE_DIRECTORIES(adapters)

# Adapter sources
SET(ADAPTER_SRC
  src/CommandAdapter.cxx
  adapters/DiffuseArray.cxx
  adapters/DumpArray.cxx
  adapters/PrintInfo.cxx
  adapters/ReadMesh.cxx
  adapters/WriteMesh.cxx
  adapters/AddArray.cxx
  )

# Main executable
ADD_EXECUTABLE(mesh3d ${ADAPTER_SRC} src/Mesh3D.cxx)

TARGET_LINK_LIBRARIES(mesh3d ${VTK_LIBRARIES})

