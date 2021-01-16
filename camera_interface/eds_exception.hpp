//
//  camera_interfacePriv.hpp
//  camera_interface
//
//  Created by Rob McKay on 08/01/2021.
//

#include <stdexcept>

class eds_exception : public std::runtime_error
{
public:
    eds_exception(std::string message, int err, std::string method = "");
};
