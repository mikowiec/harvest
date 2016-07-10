#ifndef TERRAINEXPORT_H
#define TERRAINEXPORT_H

#include <maya/MStatus.h>
#include <maya/MSyntax.h>
#include <maya/MPxCommand.h>
#include <maya/MFnTransform.h>
#include <maya/MFnMesh.h>
#include <maya/MArgList.h>
#include <maya/MFnDagNode.h>
#include <maya/MSelectionList.h>

#include <vector>
#include <fstream.h>

class exportTerrain: public MPxCommand
{
public:
        exportTerrain() 
                {};
        virtual	        ~exportTerrain();
        static void*    creator();
        virtual MStatus	doIt( const MArgList& args);
        static  MSyntax	newSyntax();
        
private:
        
        MStatus  printPolygonData(const MFnMesh& theMesh, ofstream & fout);
        MStatus  printData(const MFnMesh &theMesh);
        MStatus	 parseArguments(const MArgList &args);
              
 //Data
        MString outputDir;
              
};

#endif
