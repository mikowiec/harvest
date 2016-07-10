#ifndef COMPAT_H
#define COMPAT_H

//Usla visual
#define for if(0);else for

// Macro for error checking
//First parameter is a MStatus
//Second is a string
#define checkError( stat,msg )		\
    if ( MS::kSuccess != (stat) ) {	\
        stat.perror( (msg) );		\
		return (stat);		\
	}

#define checkErrorDisplay( stat,msg )	\
    if ( MS::kSuccess != (stat) ) {	\
    MGlobal::displayError( (msg) );	\
		return (stat);		\
	}

// Warns when debug symbols gets truncated to 255 chars
// Very disturbing when compiling with nested templates
#pragma warning(disable: 4786)

#endif  //ifndef
