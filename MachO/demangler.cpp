#include "demangler.h"
#include "machodemangleexception.h"
#include <cxxabi.h>
#include <dlfcn.h>

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
	if (status == -2 && swift_demangle != NULL) {
		char *realName = swift_demangle(name.c_str(), name.length(), NULL, NULL, 0);
		if (realName != NULL)
			result = realName;
		free(realName);
	}
	return result;
}
