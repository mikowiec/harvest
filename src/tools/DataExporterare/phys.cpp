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

#include <iostream.h>



//The flags used for argument parsing

class exportPhysics: public MPxCommand
{
public:
    exportPhysics() {};
    virtual			~exportPhysics();
    static void*	creator();
    virtual MStatus	doIt( const MArgList& args);
    static MSyntax	newSyntax();
    
private:
    
    MStatus			doScan( const MArgList& args,
					const MItDag::TraversalType traversalType,
					MFn::Type filter);
    void			printTransformData(const MFnMesh& theMesh);
    
    void			printSubObjects(const MFnTransform transform);
    void			printShadingData(const MFnMesh& theMesh, MString texture);
    
};

exportPhysics::~exportPhysics() {}

void* exportPhysics::creator()
{
    return new exportPhysics;
}




MStatus	exportPhysics::doIt( const MArgList& args )
{
    MItDag::TraversalType  traversalType   = MItDag::kBreadthFirst;
    MFn::Type              filter          = MFn::kMesh;
    MStatus                status;
    
    status = doScan( args,traversalType, filter );

    return status;
};



MStatus exportPhysics::doScan(const MArgList& args, 
			     const MItDag::TraversalType traversalType,
			     MFn::Type filter)
{   
    MStatus status;
    
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
	
	printTransformData(MFnMesh(dagPath,&status));

    }
    
   
    cout.flush();
    
    setResult(objectCount);
    return MS::kSuccess;
}




void exportPhysics::printTransformData(const MFnMesh& theMesh)
{
    MStatus		status;
    unsigned		numParents = theMesh.parentCount(&status);
    MObject		transformNode;
    //will only use the first parent
    
    if(numParents != 0){
	transformNode = theMesh.parent(0,&status);
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
    cout << "Name: " << transform.name(&status) << endl;
    cout << "Transform: " << transform.translation( MSpace::kTransform,&status) << endl;

    MQuaternion rot;
    status = transform.getRotation(rot);
    double rotD[4];
    rot.get(rotD);
    MColor tempRot(rotD[0],rotD[1],rotD[2],rotD[3]);
    cout << "Rotation: " << tempRot << endl;

    double scale[3];
    status = transform.getScale(scale);
    cout << "Scale: [" << scale[0] << ", " 
		       << scale[1] << ", "
		       << scale[2] << "]" << endl;
    


    printSubObjects(transform);

}



void exportPhysics::printSubObjects(MFnTransform transform)
{
    int i;
    MObject tObject;
    cout << "SubObjects: [";
    
    for(i=0;i<transform.childCount();++i){
	tObject=transform.child(i);
	if(tObject.hasFn(MFn::kTransform) ){
	    MFnTransform tempTrans(tObject);
	    cout << tempTrans.name() << " ";
	}
    }
    cout << " ]" << endl;
    
}

void exportPhysics::printShadingData(const MFnMesh& theMesh, MString texture)
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

    cout << "Shading Data:" << endl;

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
    cout << "Color:" << shader.color(&status) << endl;
    cout << "Diffuse_Color: " << shader.diffuseCoeff(&status)*(MColor(1.0,1.0,1.0) - shader.color(&status)) * shader.transparency(&status) << endl;
    cout << "Ambient: " << shader.ambientColor(&status) << endl;
    cout << "Emmision_Color: " << shader.incandescence(&status) << endl;	

	
    if(shaderObject.hasFn(MFn::kBlinn) ){
	MFnBlinnShader blinn(shaderObject);
	cout << "Specular Color: " << blinn.specularColor() << endl;
	cout << "Shininess: " << blinn.eccentricity() << endl;
    }
    else if(shaderObject.hasFn(MFn::kPhong) ){
	MFnPhongShader phong(shaderObject);
	cout << "Specular Color: " << phong.specularColor() << endl;
	cout << "Shininess: " << phong.cosPower() << endl;
    }
    else{
	cout << "Specular Color: " << MColor() << endl;
	cout << "Shininess: " << double(0) << endl;
    }
    	
    cout << "Texture:" << texture << endl;

}

MStatus initializePlugin( MObject obj )
{ 
    MStatus status;
    
    MFnPlugin plugin ( obj, "Peter Pipkorn Reaper Output", "0.1", "Any" );
    status = plugin.registerCommand( "exportPhysics", 
	exportPhysics::creator); 
    
    return status;
}

MStatus uninitializePlugin( MObject obj )
{
    MStatus status;
    
    MFnPlugin plugin( obj );
    status = plugin.deregisterCommand( "exportPhysics" );
    
    return status;
}


