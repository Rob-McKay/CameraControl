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

#include <cstdio>
#include <iostream>
#include "camera_interface.hpp"
#include "camera_interface_impl.hpp"

#include "EDSDK.h"


namespace implementation
{
impl_camera_list::impl_camera_list() : list(nullptr), count(0)
{
    EdsUInt32 listCount = 0;
       
    // Get camera list
    if (auto err = EdsGetCameraList(&list); err != EDS_ERR_OK)
    {
        std::cerr << "Failed to get camera list (" << err << ")\n";
        throw std::runtime_error("Failed to get camera list");
    }
    
    if (auto err = EdsGetChildCount(list, &listCount); err != EDS_ERR_OK)
    {
        std::cerr << "Failed to get camera list count (" << err << ")\n";
        throw std::runtime_error("Failed to get camera list count");
    }

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
        std::cerr << "Failed to select camera " << camera_number << " (" << EDS_ERR_DEVICE_NOT_FOUND << ")\n";
        throw std::runtime_error("Failed to select camera - camera not found");
    }
    
    EdsCameraRef camera(nullptr);
    if (auto err = EdsGetChildAtIndex(list, static_cast<EdsInt32>(camera_number), &camera); err != EDS_ERR_OK)
    {
        std::cerr << "Failed to select camera " << camera_number << " (" << err << ")\n";
        throw std::runtime_error("Failed to select camera");
    }
    
    current_camera = std::make_shared<impl_camera_ref>(camera);
    return current_camera;
}

impl_camera_list::size_type impl_camera_list::size() const noexcept
{
    return count;
}

}
