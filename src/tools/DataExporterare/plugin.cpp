//The master file for this plugin
#include "reaperExport.h"
#include "terrainExport.h"
#include "miscExport.h"


#include <maya/MStatus.h>
#include <maya/MObject.h>
#include <maya/MFnPlugin.h>
#include <maya/MGlobal.h>



MStatus initializePlugin( MObject obj )
{ 
	MStatus status;
	
	MFnPlugin plugin ( obj, "Reaper Output", "Developer: 0.1", "Any" );

/*	status = plugin.registerCommand( "exportTerrain", 
		exportTerrain::creator,
		exportTerrain::newSyntax); 
	if (!status) {
		status.perror("exportTerrain registerCommand");
		return status;
	}
*/
	status = plugin.registerCommand( "exportReaper", 
		exportReaper::creator,
		exportReaper::newSyntax);
	if (!status) {
		status.perror("exportReaper registerCommand");
		return status;
	}



	return status;
}

MStatus uninitializePlugin( MObject obj )
{
	MStatus status;
	
	MFnPlugin plugin( obj );
/*	status = plugin.deregisterCommand( "exportTerrain" );
	if (!status) {
		status.perror("deregisterCommand");
		return status;
	}
*/	
	status = plugin.deregisterCommand( "exportReaper" );
	if (!status) {
		status.perror("deregisterCommand exportReaper");
		return status;
	}
	
	return status;
}
