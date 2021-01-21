//
//  camera_interface.cpp
//  camera_interface
//
//  Created by Rob McKay on 08/01/2021.
//

#if !defined __MACOS__
#if defined __APPLE__ && defined __MACH__
#define __MACOS__ 1
#else
#error "Only for MacOS"
#endif
#endif

#include "camera_interface.hpp"
#include "camera_interface_impl.hpp"
#include <cstdio>
#include <iostream>
#include <memory>

#include "EDSDK.h"

#include "Poco/Logger.h"

std::unique_ptr<camera_connection> get_camera_connection()
{
    return std::make_unique<implementation::impl_camera_connection>();
}

namespace implementation
{
impl_camera_connection::impl_camera_connection()
{
    // Initialize SDK
    THROW_ERRORS(EdsInitializeSDK(), "camera_connection", "Failed to initialise the EDS SDK");

    cameras = std::make_unique<impl_camera_list>();
}

impl_camera_connection::~impl_camera_connection()
{
    // Tidyup SDK
    EdsTerminateSDK();
}

std::shared_ptr<camera_ref> impl_camera_connection::select_camera(size_type camera_number)
{
    return cameras->at(camera_number);
}

camera_connection::size_type impl_camera_connection::number_of_cameras() const
{
    return cameras->size();
}

void impl_camera_connection::deselect_camera(std::shared_ptr<camera_ref>& camera)
{
    cameras->deselect_camera(camera);
    camera.reset();
}


} // namespace implementation
