#ifndef DEMANGLER_H
#define DEMANGLER_H

#include "macho_global.h"

class Demangler
{
public:
    Demangler();
    virtual ~Demangler();

    string demangleName(const string& name);

private:
    char *(*swift_demangle)(const char *, size_t, char *, size_t, uint32_t);
    void *swiftRuntimeDSOHandle;
};

#endif // DEMANGLER_H
