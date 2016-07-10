//Outputs the terrain file format
//
#include "terrainExport.h"
#include "miscExport.h"

//Maya Includes
#include <maya/MArgDatabase.h>
#include <maya/MItDag.h>
#include <maya/MDagPath.h>
#include <maya/MGlobal.h>
#include <maya/MItSelectionList.h>
#include <maya/MitMeshPolygon.h>
#include <maya/MIntArray.h>
#include <maya/MVector.h>
#include <maya/MPointArray.h>
#include <maya/MVectorArray.h>
#include <maya/MQuaternion.h>
#include <maya/MTime.h>
#include <maya/MObjectArray.h>
#include <maya/MPlug.h>
#include <maya/MPlugArray.h>
#include <maya/MObject.h>
#include <maya/MFnBlinnShader.h>
#include <maya/MFnPhongShader.h>
#include <maya/MItMeshVertex.h>

//The flags used for argument parsing

const char* kTDirFlagLong = "-directory";
const char* kTDirFlag = "-d";



exportTerrain::~exportTerrain() {}

void* exportTerrain::creator()
{
        return new exportTerrain;
}

MStatus exportTerrain::parseArguments(const MArgList &args)
{
        MStatus status;
        
        MArgDatabase  argData(syntax(), args,&status);
        if(!status){
                cout << status << "Some syntax error!";
                return status;
        }    
        
        if(argData.isFlagSet(kTDirFlag,&status)){
                argData.getFlagArgument(kTDirFlag,0,outputDir);
                //Check if the user already has appended a slash at the end
                if(outputDir.substring(outputDir.length() - 1,outputDir.length() - 1) != "\\")   
                        outputDir+= "\\";
        }
        else
                outputDir = "";                     //Defaults to current directory, whatever that may be
                
        return MStatus::kSuccess;        
}

MSyntax exportTerrain::newSyntax()
{
    MSyntax syntax;
    //Adding the flags
    syntax.addFlag(kTDirFlag,kTDirFlagLong,MSyntax::kString);
   
    return syntax;
}

MStatus	exportTerrain::doIt( const MArgList& args )
{
        MStatus status;

	//Parse the arguments
	if(!(status = parseArguments(args)))
		return status;

        //Traverse the whole dag 
        MItDag::TraversalType  traversalType   = MItDag::kDepthFirst;
        MFn::Type              filter          = MFn::kMesh;
        
        MItDag dagIterator( traversalType, filter, &status);
        checkError(status,"MItDag constructor");
        
        
        for ( ; !dagIterator.isDone(); dagIterator.next() ) {
                MDagPath dagPath;
                
                status = dagIterator.getPath(dagPath);
                if ( !status ) {
                        status.perror("MItDag::getPath");
                        continue;
                }
                
                MFnDagNode dagNode(dagPath, &status);
                if ( !status ) {
                        status.perror("MFnDagNode constructor");
                        continue;
                }
                
                MFnMesh theMesh(dagPath, &status);					
                if( !status ){
                        status.perror("MFnMesh constructor");
                        continue;
                }		
                
                status = printData(theMesh);
                if(!status)
                        status.perror("PrintData failed!");
        }	
	
        return MS::kSuccess;
}



MStatus exportTerrain::printData(const MFnMesh &theMesh)
{
	MStatus status;
	MString meshName;

	//Transformation Data
	unsigned	numParents = theMesh.parentCount(&status);
	MObject		transformNode;
	//will only use the first parent
	
	if(numParents != 0){
		transformNode = theMesh.parent(0,&status);
		
	}
	// This node has no transform - i.e., it's the world node
	else{
		status.perror("Got the world node for parent for a mesh");
		return MS::kFailure;
	}
		
	if (!status && status.statusCode () == MStatus::kInvalidParameter)
		return MS::kFailure;
	
	MFnTransform transform(transformNode, &status);
	checkError(status, "MFnTransform constructor");
	
	//Open the output file
	meshName = transform.fullPathName(&status);
	MStringArray names;
	meshName.split('|',names);
	
        MString tempMeshName;
        unsigned i;
        for(i = 0;i < names.length() - 1 ; ++i){
		tempMeshName+=names[i];
                tempMeshName+="_";
        }                      
        
        tempMeshName+=names[i];
        meshName = tempMeshName;

	MString filename(outputDir + meshName);

	ofstream fout(filename.asChar());
	if(!fout){
		status.perror("Could not open " + filename);
		return MS::kFailure;
	}

        //Output all the goodies...
        printPolygonData(theMesh,fout);
	
	return MS::kSuccess;	
}

MStatus exportTerrain::printPolygonData(const MFnMesh &theMesh, ofstream & fout)
{
        MStatus status;   
        MPointArray   vertices;
        MIntArray     vertexIndices;  
        MIntArray     perPolyVertices, numVerticesPerPoly;
        MVector	  tNorm, tPoint;
        MVectorArray  perVertexNormals, normals;
        MItMeshPolygon itPoly( theMesh.object() );
        
        if(! itPoly.hasValidTriangulation() ){
                status.perror("Object has no valid triangulation: " + theMesh.name() );
                return MS::kFailure;
        }

        long numPolygons(0);
        long numVertices(0);

        theMesh.getPoints(vertices,MSpace::kWorld);
        numVertices = theMesh.numVertices();
        
        for( itPoly.reset(); ! itPoly.isDone(); itPoly.next()) {
                int numTris;
                MPointArray localVertices;
                MIntArray localIndices;
                status = itPoly.numTriangles(numTris);
                checkError(status,"Could not get numTriangles");
                
                numPolygons+=numTris;
                
                for(int i = 0;i<numTris;i++){
                        status = itPoly.getTriangle(i,localVertices,localIndices, MSpace::kTransform);
                        checkError(status,"Could not get triangles");

                        vertexIndices.append(localIndices[0]);
                        vertexIndices.append(localIndices[1]);
                        vertexIndices.append(localIndices[2]);
                }
        }
        
        fout << "Vertices: \n"; 
        fout << "Number of Vertices: " << numVertices << endl;
        fout << vertices << endl;
                 
        fout << "Number of polygons: " << numPolygons << endl;
        fout << "Polygon Connection List" << endl;
        fout << "[";
        int polyIndex;
        for(polyIndex = 0; polyIndex < (numPolygons - 1) ; ++polyIndex){
                fout << " " << polyIndex << ": ";
                fout << "[" << vertexIndices[polyIndex*3] <<
                         ", " << vertexIndices[polyIndex*3 + 1] <<
                         ", " << vertexIndices[polyIndex*3 + 2] <<
                         "]";
                fout << ", " << endl;
        }
        //last is a special case
        fout << numPolygons -1 << ": ";
        fout << "[" << vertexIndices[polyIndex*3] <<
                         ", " << vertexIndices[polyIndex*3 + 1] <<
                         ", " << vertexIndices[polyIndex*3 + 2] <<
                         "]";
        fout << "]" << endl;
        
        //Per Vertex Normals
        for(int i=0; i < numVertices; ++i){
                theMesh.getVertexNormal( i, tNorm);
                perVertexNormals.append(tNorm);
        }
        fout << "Per Vertex Normals:\n";
        fout << perVertexNormals << endl;
        

        //Texture coordinate information       
        MIntArray ids;
        int index;
        
        MFloatArray Us, Vs;
        theMesh.getUVs(Us,Vs);
        
        MFloatArray u_s(numVertices),v_s(numVertices);
        MIntArray numuvs(numVertices,0);    
        //Now the tactic is:
        //Iterate over all polygons, and for each vertex read out the UV out of the list
        //Insert that in two float arrays, and keep track of how many uvs 
        //that have been inserted. At last divd the us and vs by the number you 
        //already go there
        
        //Shared uvs will look strange, but it is the average uvs that is shown
        //Make sure to model using no shared uvs
                
        for(int i = 0;i<numPolygons;++i){
                MIntArray polyVertices;
                theMesh.getPolygonVertices(i,polyVertices);
                for(int j = 0;j<polyVertices.length();++j){
                        int uv_id;
                        theMesh.getPolygonUVid(i,j,uv_id);
                        u_s[polyVertices[j]] += Us[uv_id];
                        v_s[polyVertices[j]] += Vs[uv_id];
                        numuvs[polyVertices[j]]+=1;
                }
        }
             
        for(int k = 0;k<numVertices;++k){
                u_s[k] /= numuvs[k];
                v_s[k] /= numuvs[k];
        }
        
        fout << "Texture Coordinates:\n";
        fout << "[" ;
        for(index = 0;index < (u_s.length() - 1); ++index){
                fout << index << ": [" << u_s[index] << ", " <<
                        v_s[index] << "]," << endl;
        }
        
        //last is a special case
        fout << index << ": [" << u_s[index] << ", " <<
                v_s[index] << "]";
        fout << "]" << endl;
        


        //Color data, for coloring. 
        
        MItMeshVertex vertexIt(theMesh.object());
        MColor color;
        std::vector<MColor> colorArray;
        while (!vertexIt.isDone() ){
                vertexIt.getColor(color);
                colorArray.push_back(color);
                vertexIt.next();
        }
        
        fout << "PerVertexColors: \n";
        fout << "[" ;
        for(index = 0;index < colorArray.size()-1; ++index){
                fout << index << ": " << colorArray[index] << "," << endl;
        }
        //last is a special case
        fout << index << ": " << colorArray[index];
        fout << "]" << endl;

        return MS::kSuccess;

}

