//Outputs the reaper file format
//
#include "reaperExport.h"
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
#include <maya/MItDependencyNodes.h>

//The flags used for argument parsing
const char* kStartFlagLong = "-startFrame";
const char* kStartFlag = "-s";
const char* kEndFlagLong = "-endFrame";
const char* kEndFlag = "-e";
const char* kByFlagLong = "-byFrame";
const char* kByFlag = "-b";
const char* kTextureFlag = "-tex";
const char* kTextureFlagLong = "-texture";
const char* kDirFlagLong = "-directory";
const char* kDirFlag = "-d";
const char* kSelectedFlagLong = "-selected";
const char* kSelectedFlag = "-sel";


exportReaper::~exportReaper() {}

void* exportReaper::creator()
{
        return new exportReaper;
}

MStatus exportReaper::parseArguments(const MArgList &args)
{
        MStatus status;
        
        MArgDatabase  argData(syntax(), args,&status);
        if(!status){
                cout << status << "Some error!";
                return status;
        }
        
        
        //Start flag
        if ( argData.isFlagSet(kStartFlag) ){
                status = argData.getFlagArgument(kStartFlag,0,start);
                if(!status){
                        cout << "Start flag error!\n";
                        return status;
                }
        }
        else
                start=0;	    //If start is not set, default to start frame 0
        
        
        //End flag
        if ( argData.isFlagSet(kEndFlag) ){
                status = argData.getFlagArgument(kEndFlag,0,end);
                if(!status){
                        cout << "End error!\n";
                        return status;
                }
        }
        else
                end=0;	    //If End is not set, default to End frame 0
        
        
        //By flag
        if ( argData.isFlagSet(kByFlag) ){
                status = argData.getFlagArgument(kByFlag,0,by);
                if(!status){
                        cout << "By error!\n";
                        return status;
                }
        }
        else
                by=1;	    //If by frame is not set, default to by frame 10
        
        
        if(argData.isFlagSet(kTextureFlag) ){
                status = argData.getFlagArgument(kTextureFlag,0,texture);
                if(!status){
                        cout << "Tex error!\n";
                        return status;
                }
        }
        
        if(argData.isFlagSet(kDirFlag,&status)){
                argData.getFlagArgument(kDirFlag,0,outputDir);
                //Check if the user already has appended a slash at the end
                if(outputDir.substring(outputDir.length() - 1,outputDir.length() - 1) != "\\")   
                        outputDir+= "\\";
        }
        else
                outputDir = "";                     //Defaults to current directory, whatever that may be
        
        onlySelectedObjects = argData.isFlagSet(kSelectedFlag,&status);
        
        return MStatus::kSuccess;        
}

MSyntax exportReaper::newSyntax()
{
    MSyntax syntax;
    //Adding the flags
    syntax.addFlag(kStartFlag,kStartFlagLong,MSyntax::kLong);
    syntax.addFlag(kEndFlag,kEndFlagLong,MSyntax::kLong);
    syntax.addFlag(kByFlag,kByFlagLong,MSyntax::kLong);
    syntax.addFlag(kTextureFlag,kTextureFlagLong,MSyntax::kString);
    syntax.addFlag(kSelectedFlag,kSelectedFlagLong);
    syntax.addFlag(kDirFlag,kDirFlagLong,MSyntax::kString);
   
    return syntax;
}

MStatus	exportReaper::doIt( const MArgList& args )
{
        MStatus status;

	//Parse the arguments
	if(!(status = parseArguments(args)))
		return status;

	//Traverse the whole dag if flag is not present
	//else traverse only the selectionlist
	if(!onlySelectedObjects){
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
						
			MFnMesh theMesh(dagPath, &status);					
			if( !status ){
				status.perror("MFnMesh constructor");
				continue;
			}		
			
			printMesh(theMesh);
			if(!status)
				status.perror("PrintData failed!");
		}

		filter          = MFn::kTransform;
		MItDag dagIterator2( traversalType, filter, &status);
		checkError(status,"MItDag constructor");

		for ( ; !dagIterator2.isDone(); dagIterator2.next() ) {
			MDagPath dagPath;
			
			status = dagIterator2.getPath(dagPath);
			if ( !status ) {
				status.perror("MItDag::getPath");
				continue;
			}
			
			MFnTransform theNode(dagPath, &status);					
			if( !status ){
				status.perror("MFnTransform constructor");
				continue;
			}		
			
			status = printTransform(theNode);
			if(!status)
				status.perror("PrintTransform failed!");
		}

	}
	//Only selected objects
	else{
		MSelectionList      list;
		MDagPath            node;
		MObject             component;
		MFnDagNode          nodeFn;
		
		MGlobal::getActiveSelectionList(list);
	//	cout << "List length before: " << list.length() << endl;
		for(MItSelectionList listIter( list);!listIter.isDone();listIter.next()){
			listIter.getDagPath(node,component);
			nodeFn.setObject( node );
			selectHierarchy(list,nodeFn);
		}	
	
		//Remove all duplicates	
		MSelectionList newList;
		for( MItSelectionList listIter( list,MFn::kMesh ); !listIter.isDone(); listIter.next() ) {
			listIter.getDagPath( node, component );
			newList.merge(node);			
			nodeFn.setObject( node );		
		}

		for( MItSelectionList listIter( list,MFn::kTransform); !listIter.isDone(); listIter.next() ) {
			listIter.getDagPath( node, component );
			newList.merge(node);			
			nodeFn.setObject( node );
		}
		
		//Iterate and print out all the meshdata
		//meshes
		for( MItSelectionList listIter( newList,MFn::kMesh ); !listIter.isDone(); listIter.next() ) {
			listIter.getDagPath( node, component );			
			nodeFn.setObject( node );		
			
			MFnMesh theMesh(node, &status);					
			if( !status ){
				status.perror("MFnMesh constructor");
				continue;
			}		
			
			status = printMesh(theMesh);
			if(!status)
				status.perror("PrintData failed!");			
		}
		//transforms
		for( MItSelectionList listIter( newList,MFn::kTransform ); !listIter.isDone(); listIter.next() ) {
			listIter.getDagPath( node, component );			
			nodeFn.setObject( node );		
			
			MFnTransform theTrans(node, &status);					
			if( !status ){
				status.perror("MFnTransform constructor");
				continue;
			}		
			
			status = printTransform(theTrans);
			if(!status)
				status.perror("PrintData failed!");			
		}		
	}

	//Print the shaders
	MFn::Type	filter          = MFn::kLambert;
	MItDependencyNodes depIt(filter,&status);
	checkError(status,"MItDependency constructor");
	
	for ( ; !depIt.isDone(); depIt.next() ) {
		
		
		MFnLambertShader theShader(depIt.item(), &status);					
		if( !status ){
			status.perror("MFnLambert constructor");
			continue;
		}		
		for(int i = 0; i < shadersToOutput.length(); ++i)
		{
			if(theShader.name() == shadersToOutput[i] ) {
				printShader(theShader);
				if(!status)
					status.perror("PrintShader failed!");
			}

			//Else shader is not present among the objects which are to be output
		}
	}	
	
        return MS::kSuccess;
}

MStatus  exportReaper::printTransform(const MFnTransform& trans)
{

	MStatus status;
	MString nodeName;

	//Transformation Data
	unsigned	numParents = trans.parentCount(&status);
	MObject		transformNode;
		
	//Open the output file
	nodeName = MFnDagNode(trans.object()).fullPathName(&status);
	MStringArray names;
	nodeName.split('|',names);
	
        MString tempMeshName;
        unsigned i;
        for(i = 0;i < names.length() - 1 ; ++i){
		tempMeshName+=names[i];
                tempMeshName+="_";
        }                      
        
        tempMeshName+=names[i];
        nodeName = tempMeshName;

	MString filename(outputDir + nodeName);

	ofstream fout(filename.asChar());
	if(!fout){
		status.perror("Could not open " + filename);
		return MS::kFailure;
	}

        printTransformData(trans,fout);
	printSubObjects(trans,fout);
        printAnimationData(trans,fout);

	return MS::kSuccess;
}


MStatus exportReaper::printMesh(const MFnMesh &theMesh)
{
	MStatus status;
	MString meshName;

	//Transformation Data
	unsigned	numParents = theMesh.parentCount(&status);
	MObject		transformNode;
	//will only use the first parent
	
	if(numParents != 0){
		transformNode = theMesh.object(&status);
		checkError(status, "object transformnode");
		
	}
	// This node has no transform - i.e., it's the world node
	else{
		status.perror("Got the world node for parent for a mesh");
		return MS::kFailure;
	}
		
	if (!status && status.statusCode () == MStatus::kInvalidParameter)
		return MS::kFailure;
	
//	MFnTransform transform(transformNode, &status);
//	checkError(status, "MFnTransform constructor");

	
	//Open the output file
	meshName = MFnDagNode(theMesh.object()).fullPathName(&status);
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

//        printTransformData(transform,fout);
//        printAnimationData(transform,fout);

	MDagPath path;
	status = MFnDagNode(theMesh.object()).getPath(path);
	checkError(status, "GetPath");
        
	MFnDagNode dNode( path );

	printSubObjects( dNode,fout);
	printShadingData(theMesh,fout);
        printPolygonData(theMesh,fout);
	

	return MS::kSuccess;	
}

MStatus exportReaper::printPolygonData(const MFnMesh &theMesh, ofstream & fout)
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

        theMesh.getPoints(vertices);
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
                
        fout << "Number of Vertices: " << numVertices << endl;
        fout << vertices << endl;
                 
        fout << "Number of polygons: " << numPolygons << endl;
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
        fout << "Per Vertex Normals\n";
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
        
        fout << "Texture Coordinates\n";
        fout << "[" ;
        for(index = 0;index < (u_s.length() - 1); ++index){
                fout << index << ": [" << u_s[index] << ", " <<
                        v_s[index] << "]," << endl;
        }
        
        //last is a special case
        fout << index << ": [" << u_s[index] << ", " <<
                v_s[index] << "]";
        fout << "]" << endl;
       
        return MS::kSuccess;
}


void exportReaper::printTransformData(const MFnTransform& transform, ofstream& fout)
{
        MStatus		status;
        
        fout << "Name: " << transform.name(&status) << endl;
        fout << "Translate: " << transform.translation( MSpace::kTransform,&status) << endl;
        
        MQuaternion rot;
        status = transform.getRotation(rot);
        double rotD[4];
        rot.get(rotD);
        MColor tempRot(rotD[0],rotD[1],rotD[2],rotD[3]);
        fout << "Rotation: " << tempRot << endl;
        
        double scale[3];
        status = transform.getScale(scale);
        fout << "Scale: [" << scale[0] << ", " 
                << scale[1] << ", "
                << scale[2] << "]" << endl;        
}

void exportReaper::printAnimationData(const MFnTransform& transform, ofstream &fout)
{
        MTime currentTime;
        MTime workingTime;
        MTime endTime;
        MTime byTime;
        
        bool noScale= true;
        bool noTrans= true;
        bool noRot  = true;
        
        byTime.setValue(by);
        endTime.setValue(end);
        
        MVector trans;
        MQuaternion rot;
        MVector scale;
        double tScale[3];
        
        int index=0;
        
        MVectorArray scaleArray;
        MVectorArray transArray;
        
        std::vector<MQuaternion> rotArray;
        
        fout << "Animation data" << endl;
        if( (end <= start) || by == 0 ){
		fout << "By frame: " << by << endl ;				//If no animation is to be written
                fout << "Trans: []" << endl;
                fout << "Rot: []" << endl;
                fout << "Scale: []" << endl;
                return;
        }
        
        fout << "By frame: " << by << endl ;
        for( workingTime.setValue(start); workingTime < endTime; workingTime += byTime ){
                MGlobal::viewFrame(workingTime);	
                transform.getScale(tScale);
                scale=MVector(tScale);
                transform.getRotation(rot);
                trans = transform.translation(MSpace::kTransform);
                
                scaleArray.append(scale);
                transArray.append(trans);
                rotArray.push_back(rot);	
        }
        
        //Now sort out those vectors that contains only static channels
        
        trans=transArray[0];
        rot=rotArray.front();
        scale=scaleArray[0];
        
        //Assuming all arrays are equally long
        for(index=0;index < rotArray.size();++index){
                noTrans = noTrans && (trans == transArray[index] );
                noRot   = noRot && (rot == rotArray[index] );
                noScale = noScale && (scale == scaleArray[index]);
        }
        
        //Translation
        fout << "Trans: ";
        if(noTrans)
                fout << "[]" << endl;
        else
                fout << transArray << endl;
        //Rotation
        fout << "Rot: ";
        if(noRot)
                fout << "[]" << endl;
        else{
                fout << "[ ";
                MQuaternion rot(rotArray[index]);
                double rotD[4];
		
		for(index = 0;index < rotArray.size() -1; ++index){
			rot = rotArray[index];
                        rot.get(rotD);
                        fout  << " " << index << ": [" << rot[0] << ", " 
						<< rot[1] << ", " 
						<< rot[2] << ", " 
						<< rot[3] << "]" 
				<< "," << endl;
                        
                }
		
		rot = rotArray[index];
                rot.get(rotD);
		fout  << " " << index << ": [" << rot[0] << ", " 
						<< rot[1] << ", " 
						<< rot[2] << ", " 
						<< rot[3] << "]" ;
                fout << "]" << endl;
        }
        
        //Scale
        fout << "Scale: ";
        if(noScale)
                fout << "[]" << endl;
        else
                fout << scaleArray << endl;	    
        
        //Reset the frame        
        
        MGlobal::viewFrame(currentTime);
}

void exportReaper::printSubObjects(const MFnDagNode& transform, ofstream &fout)
{
        int i;
        MObject tObject;
        fout << "SubObjects: [";
        
        for(i=0;i<transform.childCount();++i){
                tObject=transform.child(i);
                if(tObject.hasFn(MFn::kDagNode) ){
                        MFnDagNode tempTrans(tObject);
                        fout << tempTrans.name() << " ";
                }
		
        }
        fout << " ]" << endl;
        
}

MStatus exportReaper::printShader(const MFnLambertShader& shader)
{

	MObjectArray    shaders;
	MObject	    shaderObject;
	MStatus	    status;
	

	shaderObject = shader.object();
	
	//Open shader file
	//Later fina a way to organize materials in subdirs
	MString shaderName = shader.name();
	MString filename(outputDir + shaderName);
	ofstream fout(filename.asChar());
	if(!fout){
		status.perror("Could not open " + filename);
		return MS::kFailure;
	}


	//Collect all the data
	float diff = shader.diffuseCoeff(&status);
	checkError(status, "diff1");
	MColor col = shader.color(&status);
	checkError(status,"diff2");
	MColor transp = shader.transparency(&status);
	checkError(status, "diff3");

	fout << "Diffuse_Color: " << (diff*(MColor(1.0,1.0,1.0) )*col ) * 
		(MColor(1.0,1.0,1.0) - transp )  << endl;
	checkError(status, "Diffuse");
	fout << "Ambient_Color: " << shader.ambientColor(&status) << endl;
	checkError(status, "ambient");
	fout << "Emmision_Color: " << shader.incandescence(&status) << endl;
	checkError(status, "emmision");
	
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
		fout << "Shininess: " << double(0.0) << endl;
	}

	return MS::kSuccess;
}

MStatus exportReaper::printShadingData(const MFnMesh& theMesh, ofstream &fout)
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
				break;
			}
		}
	}
		
		
	MFnLambertShader shader;

	if(shaderObject.hasFn(MFn::kLambert)) {
		status = shader.setObject(shaderObject);
		if(!status){
			status.perror("Unable to create MFnLambertShader!");
			return status;
		}
	}
	else{
		cout << "hmm" << endl;
		return MS::kFailure;
	}
	
	fout << "Shader: " << shader.name() << endl;

	shadersToOutput.append(shader.name());

	return MS::kSuccess;
}

MStatus exportReaper::selectHierarchy( MSelectionList &list, MFnDagNode &node)
{
	MStatus status;

	//cout << "Node " << node.name() << " has " << node.childCount() << " children" << endl;


	for(unsigned i=0; i < node.childCount() ;++i)
	{
		MObject tObj(node.child(i));		
		MFnDagNode nodeFn(tObj);
	//	MString(node.fullPathName() + "|" + nodeFn.name()))
	//	cout << "Child " << nodeFn.fullPathName() << endl;
		status = selectHierarchy(list,nodeFn);
		if(!status)
			return status;
		
	}
	
	if(node.object().hasFn(MFn::kMesh) || node.object().hasFn(MFn::kTransform) ){
		MDagPath path;
		status = MDagPath::getAPathTo(node.object(),path);
		checkError(status, "could not add " + node.fullPathName() + " no such path");
			
		status = list.merge(path,MObject::kNullObj,MSelectionList::kMergeNormal);
		checkError(status, "could not add " + node.name() );
		
		//cout << "Added " << node.name() << endl;
	}

	//Everything went well
	return MS::kSuccess;
}
