#ifndef RENDERCONTEXT_H
#define RENDERCONTEXT_H

class RenderContext
{
public:
	static GLboolean CheckExtension(const char* extName, const GLubyte* extString)
	{
		/*
		** Search for extName in the extensions string.  Use of strstr()
		** is not sufficient because extension names can be prefixes of
		** other extension names.  Could use strtok() but the constant
		** string returned by glGetString can be in read-only memory.
		*/
		char* p = (char*)extString;
		char* end;
		int extNameLen;

		extNameLen = strlen(extName);
		end = p + strlen(p);

		while (p < end) {
			int n = strcspn(p, " ");
			if ((extNameLen == n) && (strncmp(extName, p, n) == 0)) {
				return GL_TRUE;
			}
			p += (n + 1);
		}
		return GL_FALSE;
	}
};
#endif