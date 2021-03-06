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

#include "EDSDK.h"

#include "Poco/Logger.h"

namespace implementation
{
impl_camera_list::impl_camera_list()
    : list(nullptr)
    , count(0)
{
    EdsUInt32 listCount = 0;

    // Get camera list
    THROW_ERRORS(EdsGetCameraList(&list), "camera_list", "Failed to get camera list");
    THROW_ERRORS(
        EdsGetChildCount(list, &listCount), "camera_list", "Failed to get camera list count");

    count = listCount;
}

impl_camera_list::~impl_camera_list()
{
    if (list != nullptr)
    {
        EdsRelease(list);
        list = nullptr;
    }
}

std::shared_ptr<camera_ref> impl_camera_list::at(size_type camera_number)
{
    if ((camera_number >= size()) || (camera_number > std::numeric_limits<EdsInt32>::max()))
    {
        Poco::Logger::get("camera_list").error("Failed to select camera (%d)", camera_number);
        throw std::out_of_range("Camera number too big");
        // throw eds_exception("Failed to select camera - camera not found",
        // EDS_ERR_DEVICE_NOT_FOUND, __FUNCTION__);
    }

    EdsCameraRef camera(nullptr);
    if (auto err = EdsGetChildAtIndex(list, static_cast<EdsInt32>(camera_number), &camera);
        err != EDS_ERR_OK)
    {
        Poco::Logger::get("camera_list").error("Failed to select camera (%lu)", err);
        throw eds_exception("Failed to select camera", err, __FUNCTION__);
    }

    current_camera = std::make_shared<impl_camera_ref>(camera);
    return current_camera;
}

impl_camera_list::size_type impl_camera_list::size() const noexcept { return count; }

void impl_camera_list::deselect_camera(std::shared_ptr<camera_ref>& camera)
{
    if (current_camera != camera)
    {
        throw eds_exception("Failed to deselect camera", 42, __FUNCTION__);
    }

    current_camera.reset();
}

} // namespace implementation
