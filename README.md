IfcExtract
==========
Simple tool to convert an IFC file into an XML file containing 2D geometry and the properties of objects in the IFC file. The output can be processed further in other applications. This program is based on ifcextract which is one of the tools that comes with IfcOpenShell.

Building IfcExtract
-------------------
First download IfcOpenShell and its dependencies.

You may want to update the following options in CMakeLists.txt in $root/cmake as they are not needed by IfcExtract.

      OPTION(UNICODE_SUPPORT "Build IfcOpenShell with Unicode support (requires ICU)." OFF)
      OPTION(COLLADA_SUPPORT "Build IfcConvert with COLLADA support (requires OpenCOLLADA)." OFF)
      OPTION(BUILD_IFCPYTHON "Build IfcPython." OFF)
      OPTION(BUILD_EXAMPLES "Build example applications." OFF)

Add the IfCExtract build target at the end to CMakeLists.txt in /path/to/IfcOpenShell/cmake:

      # IfcExtract
      file(GLOB ELK_CPP_FILES ../src/ifcextract/*.cpp)
      file(GLOB ELK_H_FILES ../src/ifcextract/*.h)
      set(ELK_FILES ${ELK_CPP_FILES} ${ELK_H_FILES})
      ADD_EXECUTABLE(IfcExtract ${ELK_FILES})
      if (IFCCONVERT_DOUBLE_PRECISION)
          set_target_properties(IfcExtract PROPERTIES COMPILE_FLAGS -DIFCCONVERT_DOUBLE_PRECISION)
      endif()
      TARGET_LINK_LIBRARIES(IfcExtract ${IFCOPENSHELL_LIBRARIES} ${ICU_LIBRARIES} ${Boost_LIBRARIES})

Copy the files in the src directory to /path/to/IfcOpenShell/src/ifcextract

Then follow the IfcOpenShell build instructions. This will give you the IfCExtract executable.

Usage
-----
Enter 'IfcExtract -h' to get additional information.
