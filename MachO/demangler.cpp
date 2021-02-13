#include "demangler.h"
#include "machodemangleexception.h"
#include <cxxabi.h>
#include <dlfcn.h>

// Objective C name demangling
char *objc_demangle (const char *mangled);

/**
 class to demangle names.
 */
Demangler::Demangler()
{
	void *swiftRuntimeDSOHandle = dlopen("/usr/lib/swift/libswiftCore.dylib", RTLD_LOCAL | RTLD_LAZY);
	if (!swiftRuntimeDSOHandle) {
		swift_demangle = NULL;
		throw MachODemangleException(string("Unable to load library: ") + dlerror());
	}
	swift_demangle = (char *(*)(const char *, size_t, char *, size_t, uint32_t))dlsym(swiftRuntimeDSOHandle, "swift_demangle");
	if (!swift_demangle) {
		dlclose(swiftRuntimeDSOHandle);
		swiftRuntimeDSOHandle = NULL;
		throw MachODemangleException(string("Unable to get symbol: ") + dlerror());
	}
}

Demangler::~Demangler()
{
	if (swiftRuntimeDSOHandle != NULL)
		dlclose(swiftRuntimeDSOHandle);
}

string Demangler::demangleName(const string& name) {
	std::string result = name;
	int status = 0;
	// +1 to skip the leading underscore
	char *realName = abi::__cxa_demangle(name.c_str() + 1, NULL, NULL, &status);
	if (realName != NULL)
		result = realName;
	free(realName);

	// name is not a valid name under the C++ ABI mangling rules
	if (status == -2) {
		char *realName = objc_demangle(name.c_str());
		if (realName != NULL) {
			result = realName;
			status = 0;
		} else {
			status = -2;
		}
		free(realName);
	}

	if (status == -2 && swift_demangle != NULL) {
		char *realName = swift_demangle(name.c_str(), name.length(), NULL, NULL, 0);
		if (realName != NULL) {
			result = realName;
			status = 0;
		} else {
			status = -2;
		}
		free(realName);
	}
	return result;
}


// MARK: - Objective C name demangling
// https://opensource.apple.com/source/gdb/gdb-228/src/gdb/objc-lang.c.auto.html

char *
objc_demangle (const char *mangled)
{
	char *demangled, *cp;

	if (mangled[0] == '_' && (mangled[1] == 'i' || mangled[1] == 'c') && mangled[2] == '_')
	{
		cp = demangled = (char *)malloc(strlen(mangled) + 2);

		if (mangled[1] == 'i')
			*cp++ = '-';		/* for instance method */
		else
			*cp++ = '+';		/* for class    method */

		*cp++ = '[';		/* opening left brace  */
		strcpy(cp, mangled+3);	/* tack on the rest of the mangled name */

		while (*cp && *cp == '_')
			cp++;			/* skip any initial underbars in class name */

		if (!(cp = strchr(cp, '_')))	/* find first non-initial underbar */
		{
			free(demangled);	/* not mangled name */
			return NULL;
		}
		if (cp[1] == '_') {	/* easy case: no category name     */
			*cp++ = ' ';		/* replace two '_' with one ' '    */
			strcpy(cp, mangled + (cp - demangled) + 2);
		}
		else {
			*cp++ = '(';		/* less easy case: category name */
			if (!(cp = strchr(cp, '_')))
			{
				free(demangled);	/* not mangled name */
				return NULL;
			}
			*cp++ = ')';
			*cp++ = ' ';		/* overwriting 1st char of method name... */
			strcpy(cp, mangled + (cp - demangled));	/* get it back */
		}

		while (*cp && *cp == '_')
			cp++;			/* skip any initial underbars in method name */

		for (; *cp; cp++)
			if (*cp == '_')
				*cp = ':';		/* replace remaining '_' with ':' */

		*cp++ = ']';		/* closing right brace */
		*cp++ = 0;		/* string terminator */
		return demangled;
	}
	else
		return NULL;	/* not an objc mangled name */
}
