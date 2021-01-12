//
//  connection_info_impl.cpp
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

#include <cstdio>
#include <iostream>
#include "camera_interface.hpp"
#include "camera_interface_impl.hpp"

#include "EDSDK.h"

namespace implementation
{
impl_connection_info::impl_connection_info(std::string port, std::string desc) : port(port), desc(desc)
{}

impl_connection_info::~impl_connection_info()
{}

impl_connection_info::impl_connection_info(const implementation::impl_connection_info &other) : port(other.port), desc(other.desc)
{}

impl_connection_info::impl_connection_info(implementation::impl_connection_info &&other) : port(std::move(other.port)), desc(std::move(other.desc))
{}

impl_connection_info &impl_connection_info::operator=(const impl_connection_info &other)
{
    port = other.port;
    desc = other.desc;
    
    return *this;
}

impl_connection_info &impl_connection_info::operator=(impl_connection_info &&other)
{
    std::swap(port, other.port);
    std::swap(desc, other.desc);
    
    return *this;
}

std::string impl_connection_info::get_port() const
{
    return port;
}

std::string impl_connection_info::get_desc() const
{
    return desc;
}

}
