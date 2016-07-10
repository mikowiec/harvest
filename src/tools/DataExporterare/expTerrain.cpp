//////////////////////////////////////////////////////////////////////////////
//Program to scan the dag hierarchy and output interesting data to
//a file.
//////////////////////////////////////////////////////////////////////////////

#include <maya/MFnDagNode.h>
#include <maya/MItDag.h>
#include <maya/MObject.h>
#include <maya/MDagPath.h>
#include <maya/MPxCommand.h>
#include <maya/MSyntax.h>
#include <maya/MArgDatabase.h>
#include <maya/MStatus.h>
#include <maya/MString.h>
#include <maya/MFnPlugin.h>
#include <maya/MArgList.h>
#include <maya/MFnCamera.h>
#include <maya/MPoint.h>
#include <maya/MVector.h>
#include <maya/MMatrix.h>
#include <maya/MTransformationMatrix.h>
#include <maya/MFnLight.h>
#include <maya/MColor.h>
#include <maya/MFnNurbsSurface.h>
#include <maya/MFnMesh.h>
#include <maya/MPointArray.h>
#include <maya/MIntArray.h>
#include <maya/MFloatVectorArray.h>
#include <maya/MVector.h>
#include <maya/MItMeshPolygon.h>
#include <maya/MFnTransform.h>
#include <maya/MQuaternion.h>
#include <maya/MTime.h>
#include <maya/MGlobal.h>
#include <maya/MPlug.h>
#include <maya/MPlugArray.h>
#include <maya/MFnLambertShader.h>
#include <maya/MFnBlinnShader.h>
#include <maya/MFnPhongShader.h>
#include <maya/MItMeshVertex.h>


#include <vector>

#include <fstream.h>

namespace{
	ofstream fout("d:/exported_terrain");//,ios::binary);
}

//The flags used for argument parsing

const char* kPhysicsFlagLong = "-physics";
const char* kPhysicsFlag = "-phy";



class exportTerrain: public MPxCommand
{
public:
    exportTerrain() {};
    virtual			~exportTerrain();
    static void*	creator();
    virtual MStatus	doIt( const MArgList& args);
    static MSyntax	newSyntax();
    
private:
    
    MStatus			doScan( const MArgList& args,
					const MItDag::TraversalType traversalType,
					MFn::Type filter);

    void			printPolygonData(const MFnMesh& theMesh,const bool phy);
	void			printTransformData(const MFnLight& theLight);
    void			printShadingData(const MFnMesh& theMesh, MString texture);
	void			printLightData(const MFnLight& theLight);
    MStatus			parseArguments(const MArgList &args, 
					    bool& phy) ;
    
};

exportTerrain::~exportTerrain() {}

void* exportTerrain::creator()
{
    return new exportTerrain;
}

MStatus exportTerrain::parseArguments(const MArgList &args, 
				bool& phy)
{
    MStatus status;
    


    MArgDatabase  argData(syntax(), args,&status);
    if(!status){
	fout << status << "Some error!";
	return status;
    }
   
    phy=argData.isFlagSet(kPhysicsFlag); 
    

    return MStatus::kSuccess;

}

MSyntax exportTerrain::newSyntax()
{
    MSyntax syntax;
    //Adding the flags
    syntax.addFlag(kPhysicsFlag,kPhysicsFlagLong);
   
   
    return syntax;
}



MStatus	exportTerrain::doIt( const MArgList& args )
{
    MItDag::TraversalType  traversalType   = MItDag::kDepthFirst;
    MFn::Type              filter          = MFn::kMesh;
    MStatus                status;
	
	if(!fout.good())
		cout << "Error opening file: " << endl;

    status = doScan( args,traversalType, filter );

    return status;

	fout.close();
};

void exportTerrain::printLightData(const MFnLight& theLight){
	fout << "Color: " << theLight.intensity()*theLight.color() << endl;

	MVector dir = theLight.lightDirection( 0, MSpace::kWorld);
	dir.normalize();
	fout << "Direction: " << dir << endl;

}

void exportTerrain::printTransformData(const MFnLight& theLight)
{
    MStatus		status;
    unsigned		numParents = theLight.parentCount(&status);
    MObject		transformNode;
    //will only use the first parent
    
    if(numParents != 0){
	transformNode = theLight.parent(0,&status);
	// This node has no transform - i.e., it's the world node
    }
    else
	return;
    
    if (!status && status.statusCode () == MStatus::kInvalidParameter)
	return;
    MFnTransform   transform(transformNode, &status);
    if (!status) {
	status.perror("MFnTransform constructor");
	return;
    }
    fout << "Name: " << transform.name(&status) << endl;
    fout << "Transform: " << transform.translation( MSpace::kTransform,&status) << endl;

    MQuaternion rot;
    status = transform.getRotation(rot);
    double rotD[4];
    rot.get(rotD);
    MColor tempRot(rotD[0],rotD[1],rotD[2],rotD[3]);
 //   fout << "Rotation: " << tempRot << endl;


    printLightData(theLight);
}

MStatus exportTerrain::doScan(const MArgList& args, 
			     const MItDag::TraversalType traversalType,
			     MFn::Type filter)
{   
    MStatus status;

    bool phy;
    status = parseArguments(args,phy);
    if(status!=MStatus::kSuccess){
	fout << status << "Syntax error!" << endl;
	return status;
    }
    
    MItDag dagIterator( traversalType, filter, &status);
    
    if ( !status) {
	status.perror("MItDag constructor");
	return status;
    }
          
    int objectCount = 0;
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
		
		objectCount += 1;
		
		MFnMesh theMesh(dagPath, &status);
		if( !status ){
			status.perror("MFnMesh constructor");
		}
		printPolygonData(theMesh,phy);
		//if(!phy)
		//Obsolete, data specified in 
			//	printShadingData(theMesh,"");
		
    }
    
    setResult(objectCount);

    /*   Obsolete: Light data goes to a separate file nowadays

	//Output all the lights in the scene, that have been used for prelighting
	MFn::Type filter2 = MFn::kLight;
	MItDag dag2(traversalType,filter2, &status);
	
	int numLights = 0;

	fout << "Light data: \n";

	for( ; !dag2.isDone(); dag2.next() ){
		fout << "Light_number " << numLights++ << ": \n"; 
		
		MFnLight theLight(dag2.item() );

		printTransformData(theLight);

	}
     */


    return MS::kSuccess;
}

void exportTerrain::printPolygonData(const MFnMesh &theMesh,const bool phy)
{
    MStatus status;   
    MPointArray   vertices;
    MIntArray     perPolyVertices, numVerticesPerPoly;
    MVector	  tNorm;
    MVectorArray  perVertexNormals, normals;
    MItMeshPolygon itPoly( theMesh.object() );
    
    long numPolygons = theMesh.numPolygons();
    long numVertices = theMesh.numVertices();
   
    theMesh.getPoints(vertices);

    fout << "Vertices: " << endl;
    fout << "Number of Vertices: " << numVertices << endl;
    fout << vertices << endl;


    fout << "Number of polygons: " << numPolygons << endl;
    fout << "Polygon Connection List:" << endl;
    fout << "["; 
    for(int i = 0; i < numPolygons - 1; ++i){
	fout << i << ": ";
	status = theMesh.getPolygonVertices(i, perPolyVertices);
	fout << perPolyVertices;
	fout << ", " << endl;
    }
    //last is a special case
    fout << numPolygons -1 << ": ";
    status = theMesh.getPolygonVertices(numPolygons -1, perPolyVertices);
    fout << perPolyVertices;
    fout << "]" << endl;

    //Per Vertex Normals
    for(i=0; i < numVertices; ++i){
	theMesh.getVertexNormal( i, tNorm);
	perVertexNormals.append(tNorm);
    }
    fout << "Per Vertex Normals: \n";
    fout << perVertexNormals << endl;


/*    //per vertex per polygon Normals
      //Not supported by the Reaper graphic engine
    fout << "Normals" << endl;
    i = 0;
    fout << "[ ";
    while (!itPoly.isDone() ){
	itPoly.getNormals(normals);
	fout << i << ": " << normals << endl;
	++i;
	itPoly.next();
    }
    fout << " ]" << endl;
*/

    if( !phy ){
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



    for(i = 0;i<numPolygons;++i){
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

    fout << "Texture Coordinates: \n";
    fout << "[" ;
    for(index = 0;index < (u_s.length() - 1); ++index){
	fout << index << ": [" << u_s[index] << ", " <<
	    v_s[index] << "]," << endl;
    }
	
	//last is a special case
	fout << index << ": [" << u_s[index] << ", " <<
	    v_s[index] << "]";
    fout << "]" << endl;

	
	//Color data, for coloring. Make sure
	
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
}
	
	}

	

void exportTerrain::printShadingData(const MFnMesh& theMesh, MString texture)
{
    MObjectArray    shaders;
    MIntArray	    indices;
    MPlug	    tPlug;
    MPlugArray      connections,inConnections;
    MObject	    node,shaderObject;
    MFnDependencyNode dpNode;
    MStatus	    status;
    
    int i,j;

    theMesh.getConnectedShaders(0 , shaders, indices);

    fout << "Shading Data:" << endl;

    //Will assume that only one shader is used, and therefore only prints
    //data for the first index;
    
    //Assuming only one shader
    dpNode.setObject( shaders[0] );
    dpNode.getConnections(connections);
    for(i=0;i < connections.length();++i){
	connections[i].connectedTo(inConnections,true,true);
	for(j=0;j<inConnections.length();++j){
	    node = inConnections[j].node();	
	    dpNode.setObject(node);
	    if(node.hasFn(MFn::kLambert) ){
		shaderObject = node;
	    }
	}
    }
    

    MFnLambertShader shader(shaderObject, &status);
    if(!status){
	status.perror("Unable to create MFnLambertShader!");
	return;
    }
    //Collect all the data
    fout << "Diffuse_Color: " << (shader.diffuseCoeff(&status)*(MColor(1.0,1.0,1.0) )*shader.color(&status)) * 
						(MColor(1.0,1.0,1.0) - shader.transparency(&status) )<< endl;
    fout << "Ambient: " << shader.ambientColor(&status) << endl;
    fout << "Emmision_Color: " << shader.incandescence(&status) << endl;	

	
    if(shaderObject.hasFn(MFn::kBlinn) ){
	MFnBlinnShader blinn(shaderObject);
	fout << "Specular_Color: " << blinn.specularColor() << endl;
	fout << "Shininess: " << blinn.eccentricity() << endl;
    }
    else if(shaderObject.hasFn(MFn::kPhong) ){
	MFnPhongShader phong(shaderObject);
	fout << "Specular_Color: " << phong.specularColor() << endl;
	fout << "Shininess: " << phong.cosPower() << endl;
    }
    else{
	fout << "Specular_Color: " << MColor() << endl;
	fout << "Shininess: " << double(0) << endl;
    }
    	
    fout << "Texture: " << texture << endl;

}

MStatus initializePlugin( MObject obj )
{ 
    MStatus status;
    
    MFnPlugin plugin ( obj, "Peter Pipkorn Reaper Output", "0.1", "Any" );
    status = plugin.registerCommand( "exportTerrain", 
	exportTerrain::creator,
	exportTerrain::newSyntax); 
    
    return status;
}

MStatus uninitializePlugin( MObject obj )
{
    MStatus status;
    
    MFnPlugin plugin( obj );
    status = plugin.deregisterCommand( "exportTerrain" );
    
    return status;
}


