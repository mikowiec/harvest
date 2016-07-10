#ifndef REAPEREXPORT_H
#define REAPEREXPORT_H

#include <maya/MStatus.h>
#include <maya/MSyntax.h>
#include <maya/MPxCommand.h>
#include <maya/MFnTransform.h>
#include <maya/MFnMesh.h>
#include <maya/MArgList.h>
#include <maya/MFnDagNode.h>
#include <maya/MSelectionList.h>
#include <maya/MFnLambertShader.h>

#include <vector>
#include <fstream.h>

class exportReaper: public MPxCommand
{
public:
        exportReaper():
                onlySelectedObjects(false),
                start(0),
                end(0),
                by(1)
                {};
        virtual	        ~exportReaper();
        static void*    creator();
        virtual MStatus	doIt( const MArgList& args);
        static MSyntax	newSyntax();
        
private:
        
        void     printTransformData(const MFnTransform& transform,ofstream& fout);
        MStatus  printPolygonData(const MFnMesh& theMesh, ofstream & fout);
        void	 printAnimationData(const MFnTransform& transform, ofstream& fout);
        void     printSubObjects(const MFnDagNode& transform, ofstream& fout);
	MStatus  printShader(const MFnLambertShader& shader);
        MStatus  printShadingData(const MFnMesh& theMesh, ofstream& fout);
        MStatus  printMesh(const MFnMesh &theMesh);
	MStatus  printTransform(const MFnTransform& trans);
        MStatus	 parseArguments(const MArgList &args);
        MStatus  selectHierarchy( MSelectionList &list, MFnDagNode &node);
              
 //Data
        MString texture;
        MString outputDir;
	MStringArray shadersToOutput;
        bool onlySelectedObjects;
        int start;
        int end;
        int by;

	
                
};

#endif
