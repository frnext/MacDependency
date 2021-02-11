#ifndef MACHOARCHITECTURE_H
#define MACHOARCHITECTURE_H

#include "macho_global.h"
#include <list>
#include <vector>

class MachOFile;
class MachOHeader;
class LoadCommand;
class DylibCommand;

class EXPORT MachOArchitecture
{
private:
	typedef std::list<LoadCommand*> LoadCommands;
	typedef LoadCommands::iterator LoadCommandsIterator;
public:
	typedef LoadCommands::const_iterator LoadCommandsConstIterator;
	
    MachOArchitecture(MachOFile& file, uint32_t magic, unsigned int size);
    ~MachOArchitecture();

    const MachOHeader* getHeader() { return header; }
    LoadCommandsConstIterator getLoadCommandsBegin() const { if(!hasReadLoadCommands) { readLoadCommands(); } return loadCommands.begin(); }
    LoadCommandsConstIterator getLoadCommandsEnd() const { if(!hasReadLoadCommands) { readLoadCommands(); } return loadCommands.end(); }
    DylibCommand* getDynamicLibIdCommand() const { if(!hasReadLoadCommands) { readLoadCommands(); } return dynamicLibIdCommand; }
    unsigned int getSize() const;
    void initParentArchitecture(const MachOArchitecture* parent);
    const MachOFile* getFile() const { return &file; }

    std::vector<string*> getRPaths() const;
    string getResolvedName(const string& name, const string& workingPath) const;

private:
    MachOHeader* header;
    MachOFile& file;
    const unsigned int size;
    mutable bool hasReadLoadCommands;
    void readLoadCommands() const;
    const MachOArchitecture* parent;	// architecture from which this architecture was loaded

    mutable LoadCommands loadCommands;
    mutable DylibCommand* dynamicLibIdCommand;
    mutable std::vector<string*> rPaths;
};

#endif // MACHOARCHITECTURE_H
