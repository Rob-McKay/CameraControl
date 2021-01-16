//
//  eds_exception.cpp
//  camera_interface
//
//  Created by Rob McKay on 09/01/2021.
//

#if !defined __MACOS__
#if defined __APPLE__ && defined __MACH__
#define __MACOS__ 1
#else
#error "Only for MacOS"
#endif
#endif

#include "camera_interface.hpp"

#include "EDSDK.h"

static std::string format_error(std::string message, int err, std::string method)
{
    char buffer[42];
    sprintf(buffer, " (Error %d [0x%X]", err, err);
    return message + buffer + ((!method.empty()) ? (" in method " + method):"") + ")";
}

eds_exception::eds_exception(std::string message, int err, std::string method) : std::runtime_error(format_error(message, err, method))
{
}
