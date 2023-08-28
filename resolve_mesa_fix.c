#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include <string.h>

/* GL types & defs */
#define GL_EXTENSIONS     0x1F03
#define GL_NUM_EXTENSIONS 0x821D

typedef const unsigned char *(*PFNGLGETSTRINGIPROC)  (unsigned int name,  unsigned int index);
typedef void                 (*PFNGLGETINTEGERVPROC) (unsigned int pname, int *data);

/* Pointers to real functions */
static void *(*real_getprocaddress)(const char *) = NULL;

static PFNGLGETSTRINGIPROC  real_glGetStringi = NULL;
static PFNGLGETINTEGERVPROC real_glGetIntegerv = NULL;
static int num_exts = -1;

/* Injected GL functions */
static const unsigned char *
fix_glGetStringi(unsigned int name,  unsigned int index)
{
	if ((name == GL_EXTENSIONS) && (index == num_exts))
		return (unsigned char *)"GL_ARB_texture_border_clamp";

	return real_glGetStringi(name, index);
}

static void
fix_glGetIntegerv(unsigned int pname, int *data)
{
	if (num_exts == -1)
		real_glGetIntegerv(GL_NUM_EXTENSIONS, &num_exts);

	if (pname == GL_NUM_EXTENSIONS)
		*data = num_exts + 1;
	else
		real_glGetIntegerv(pname, data);
}


/* Injected GLX functions */
void *
glXGetProcAddress(const char *fname)
{
	/* Init */
	if (!real_getprocaddress) {
		real_getprocaddress = dlsym(RTLD_NEXT, "glXGetProcAddress");
		real_glGetStringi   = real_getprocaddress("glGetStringi");
		real_glGetIntegerv  = real_getprocaddress("glGetIntegerv");
	}

	if (!strcmp(fname, "glGetStringi"))
		return fix_glGetStringi;

	if (!strcmp(fname, "glGetIntegerv"))
		return fix_glGetIntegerv;

	return real_getprocaddress(fname);
}
