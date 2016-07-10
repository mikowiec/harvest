
#include "glh_extensions.h"

/////////////////////////////////////////////////////////////////////////////////////////////////
// from glh_extensions.h  -- nvparse needs this

#define CHECK_MEMORY(ptr) \
    if (NULL == ptr) { \
		fprintf(stderr, "Error allocating memory in file %s, line %d\n", __FILE__, __LINE__); \
		exit(-1); \
	}

static char *unsupportedExts = NULL;
static char *sysExts = NULL;

static int ExtensionExists(const char* extName, const char* sysExts)
{
    char *padExtName = (char*)malloc(strlen(extName) + 2);
    strcat(strcpy(padExtName, extName), " ");

	if (0 == strcmp(extName, "GL_VERSION_1_2")) {
		const char *version = (const char*)glGetString(GL_VERSION);
		if (strstr(version, "1.0") == version || strstr(version, "1.1") == version) {
			return GL_FALSE;
		} else {
			return GL_TRUE;
		}
	}
	if (0 == strcmp(extName, "GL_VERSION_1_3")) {
		const char *version = (const char*)glGetString(GL_VERSION);
		if (strstr(version, "1.0") == version ||
            strstr(version, "1.1") == version ||
            strstr(version, "1.2") == version) {
			return GL_FALSE;
		} else {
			return GL_TRUE;
		}
	}
    if (strstr(sysExts, padExtName)) {
		free(padExtName);
        return GL_TRUE;
    } else {
		free(padExtName);
        return GL_FALSE;
    }
}

static const char* EatWhiteSpace(const char *str)
{
	for (; *str && (' ' == *str || '\t' == *str || '\n' == *str); str++);
	return str;
}

static const char* EatNonWhiteSpace(const char *str)
{
	for (; *str && (' ' != *str && '\t' != *str && '\n' != *str); str++);
	return str;
}

int glh_init_extensions(const char *origReqExts)
{
	// Length of requested extensions string
	unsigned reqExtsLen;
	char *reqExts;
	// Ptr for individual extensions within reqExts
	char *reqExt;
	int success = GL_TRUE;
	// build space-padded extension string
	if (NULL == sysExts) {
		const char *extensions = (const char*)glGetString(GL_EXTENSIONS);
		int sysExtsLen = strlen(extensions);
		const char *winsys_extensions = "";		
		int winsysExtsLen = 0;
#ifdef _WIN32
		{
			PFNWGLGETEXTENSIONSSTRINGARBPROC wglGetExtensionsStringARB = 0;
			wglGetExtensionsStringARB = (PFNWGLGETEXTENSIONSSTRINGARBPROC)wglGetProcAddress("wglGetExtensionsStringARB");
		    if(wglGetExtensionsStringARB)
			{
				winsys_extensions = wglGetExtensionsStringARB(wglGetCurrentDC());
				winsysExtsLen = strlen(winsys_extensions);
			}
		}
#else
		{

		  winsys_extensions = glXQueryExtensionsString (glXGetCurrentDisplay(),DefaultScreen(glXGetCurrentDisplay())) ;
		  winsysExtsLen = strlen (winsys_extensions);
		}
#endif
		// Add 2 bytes, one for padding space, one for terminating NULL
		sysExts = (char*)malloc(sysExtsLen + winsysExtsLen + 3);
		CHECK_MEMORY(sysExts);
		strcpy(sysExts, extensions);
		sysExts[sysExtsLen] = ' ';
		sysExts[sysExtsLen + 1] = 0;
		strcat(sysExts, winsys_extensions);
		sysExts[sysExtsLen + 1 + winsysExtsLen] = ' ';
		sysExts[sysExtsLen + 1 + winsysExtsLen + 1] = 0;
	}

	if (NULL == origReqExts)
		return GL_TRUE;
	reqExts = strdup(origReqExts);
	reqExtsLen = strlen(reqExts);
	if (NULL == unsupportedExts) {
		unsupportedExts = (char*)malloc(reqExtsLen + 10);
	} else if (reqExtsLen > strlen(unsupportedExts)) {
		unsupportedExts = (char*)realloc(unsupportedExts, reqExtsLen + 10);
	}
	CHECK_MEMORY(unsupportedExts);
	*unsupportedExts = 0;

	// Parse requested extension list
	for (reqExt = reqExts;
		(reqExt = (char*)EatWhiteSpace(reqExt)) && *reqExt;
		reqExt = (char*)EatNonWhiteSpace(reqExt))
	{
		char *extEnd = (char*)EatNonWhiteSpace(reqExt);
		char saveChar = *extEnd;
		*extEnd = (char)0;
		if (!ExtensionExists(reqExt, sysExts)) {
			// add reqExt to end of unsupportedExts
			strcat(unsupportedExts, reqExt);
			strcat(unsupportedExts, " ");
			success = GL_FALSE;
		}
		*extEnd = saveChar;
	}
	//free(reqExts);
	return success;
}

const char* glh_get_unsupported_extensions()
{
	return (const char*)unsupportedExts;
}

void glh_shutdown_extensions()
{
	if (unsupportedExts)
    {
        free(unsupportedExts);
        unsupportedExts = NULL;
    }
	if (sysExts)
    {
        free(sysExts);
        sysExts = NULL;
    }
}

int APIENTRY glh_extension_supported(const char *extension)
{
    static const GLubyte *extensions = NULL;
    const GLubyte *start;
    GLubyte *where, *terminator;
    
    // Extension names should not have spaces.
    where = (GLubyte *) strchr(extension, ' ');
    if (where || *extension == '\0')
      return 0;
    
    if (!extensions)
      extensions = glGetString(GL_EXTENSIONS);

    // It takes a bit of care to be fool-proof about parsing the
    // OpenGL extensions string.  Don't be fooled by sub-strings,
    // etc.
    start = extensions;
    for (;;) 
    {
        where = (GLubyte *) strstr((const char *) start, extension);
        if (!where)
            break;
        terminator = where + strlen(extension);
        if (where == start || *(where - 1) == ' ') 
        {
            if (*terminator == ' ' || *terminator == '\0') 
            {
                return 1;
            }
        }
        start = terminator;
    }
    return 0;
}
