#ifndef DYNAMICLOADER_H
#define DYNAMICLOADER_H

#include "macho_global.h"

#include <list>

typedef list<string> StringList;

class MachOArchitecture;
class DynamicLoader
{
public:
    DynamicLoader();
    virtual ~DynamicLoader() {}

    string replacePlaceholder(const string& name, const MachOArchitecture* architecture) const;
    string getPathname(const string& name, const MachOArchitecture* architecture, const string& workingDirectory) const;

private:
    class EnvironmentPathVariable
    {

    public:
            EnvironmentPathVariable();
            EnvironmentPathVariable(const string& name, const StringList& defaultValues = StringList());

            bool isEmpty() const;
            const StringList& getPaths() const { return paths; }

    private:
            void setPaths(const StringList& paths);
            void addPath(const string& path);
            bool replaceHomeDirectory(string& path);
            StringList paths;
            static const char* PATHS_SEPARATOR;
            static const char* HOME_PATH;
    };

    enum {
        LdLibraryPath,
        DyldFrameworkPath,
        DyldLibraryPath,
        DyldFallbackFrameworkPath,
        DyldFallbackLibraryPath,
        DyldImageSuffix,
        NumEnvironmentVariables
    };

    enum Placeholder {
        ExecutablePath,
        LoaderPath,
        RPath,
        NumPlaceholders
    };

    static const char* PLACEHOLDERS[NumPlaceholders];
    static const char* ENVIRONMENT_VARIABLE_NAMES[NumEnvironmentVariables];
    static const char* PATH_SEPARATOR;
    static const StringList ENVIRONMENT_VARIABLE_DEFAULT_VALUES[NumEnvironmentVariables];

    static const char* DEFAULT_FRAMEWORK_PATH[];
    static const char* DEFAULT_LIBRARY_PATH[];

    EnvironmentPathVariable environmentVariables[NumEnvironmentVariables];

    string getFrameworkName(const string& name, const bool strippedSuffix = false) const;
    string getExistingPathname(const string& name, const EnvironmentPathVariable& environmentPathVariable) const;
    string getExistingPathname(const string& name, const string& directory) const;
    string getExistingPathname(const string& name) const;
	
	static bool endsWith(const string& str, const string& substr);
};

#endif // DYNAMICLOADER_H
