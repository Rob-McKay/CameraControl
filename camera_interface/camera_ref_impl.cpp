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

#include "camera_interface.hpp"
#include "camera_interface_impl.hpp"
#include <cstdio>
#include <iostream>

#include "EDSDK.h"

#include "Poco/Logger.h"
#include "Poco/Format.h"

namespace implementation
{
impl_camera_ref::impl_camera_ref(EdsCameraRef camera)
    : ref(camera)
{
    EdsDeviceInfo device_info;

    THROW_ERRORS(
        EdsGetDeviceInfo(ref.get_ref(), &device_info), "camera_ref", "Failed to get device info");

    conn_info = std::make_shared<impl_connection_info>(
        device_info.szPortName, device_info.szDeviceDescription);
    session = std::make_unique<impl_camera_session>(ref);
}

impl_camera_ref::~impl_camera_ref() { }

std::shared_ptr<const connection_info> impl_camera_ref::get_connection_info() const
{
    return conn_info;
}

std::shared_ptr<camera_info> impl_camera_ref::get_camera_info()
{
    auto cam = std::make_shared<impl_camera_info>(ref);

    return cam;
}

impl_camera_ref::size_type impl_camera_ref::get_volume_count() const
{
    EdsUInt32 count = 0;
    THROW_ERRORS(EdsGetChildCount(ref.get_ref(), &count),"camera_ref.volume","Failed to get child count");

    return count;
}

std::shared_ptr<volume_ref> impl_camera_ref::select_volume(size_type volume_number)
{
    if (volume_number >= get_volume_count())
        throw std::out_of_range("Volume number too big");

    std::shared_ptr<volume_ref> selected_volume;

    EdsVolumeRef vol_ref = nullptr;

    THROW_ERRORS(EdsGetChildAtIndex(ref.get_ref(), volume_number, &vol_ref),"camera_ref.volume",Poco::format("Failed to get child at index %lu", volume_number));

    selected_volume = std::make_shared<impl_volume_ref>(vol_ref);

    return selected_volume;
}

void impl_camera_ref::set_ui_status(bool enabled)
{
    if (auto err = EdsSendStatusCommand(ref.get_ref(),
            enabled ? kEdsCameraStatusCommand_UILock : kEdsCameraStatusCommand_UIUnLock, 0);
        err != EDS_ERR_OK)
    {
        Poco::Logger::get("camera_ref").error("Failed to set ui status (%x)", err);
        throw eds_exception("Failed to set ui status", err, __FUNCTION__);
    }
}

} // namespace implementation
