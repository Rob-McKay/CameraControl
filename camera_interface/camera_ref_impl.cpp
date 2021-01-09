//
//  camera_ref_impl.cpp
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
impl_camera_ref::impl_camera_ref(EdsCameraRef camera) : ref(camera)
{
    EdsDeviceInfo device_info;
    
    if (auto err = EdsGetDeviceInfo(camera, &device_info); err != EDS_ERR_OK)
    {
        throw std::runtime_error("oops");
    }
    
    conn_info = std::make_shared<impl_connection_info>(device_info.szPortName, device_info.szDeviceDescription);
}

impl_camera_ref::~impl_camera_ref()
{
    //TODO:;
}

std::shared_ptr<const connection_info> impl_camera_ref::get_connection_info() const
{
    return conn_info;
}

std::shared_ptr<camera_info> impl_camera_ref::get_camera_info()
{
    auto cam = std::make_shared<impl_camera_info>();
    
    return cam;
}

}
