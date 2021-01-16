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

#include "EDSDK.h"
#include "camera_interface_impl.hpp"
#include "int_to_hex.hpp"
#include "properties.hpp"

#include "Poco/DateTimeFormat.h"
#include "Poco/DateTimeFormatter.h"
#include "Poco/LocalDateTime.h"
#include "Poco/Logger.h"

namespace implementation
{
camera_ref_lock<EdsStreamRef> create_memory_stream()
{
    EdsStreamRef stream(nullptr);
    THROW_ERRORS(EdsCreateMemoryStream(0, &stream), "create_memory_stream",
        "Failed to create memory stream");
    return stream;
}

camera_ref_lock<EdsImageRef> create_image_ref(EdsStreamRef stream)
{
    EdsImageRef img_ref(nullptr);
    THROW_ERRORS(
        EdsCreateImageRef(stream, &img_ref), "create_image_ref", "Failed to create image ref");
    return img_ref;
}

thumbnail::thumbnail(EdsDirectoryItemRef dir_item)
    : date_time(0)
{
    auto stream = create_memory_stream();

    THROW_ERRORS(EdsDownloadThumbnail(dir_item, stream.get_ref()), "thumbnail",
        "Failed to download thumbnail");

    auto img = create_image_ref(stream.get_ref());

    if (is_property_available(img.get_ref(), kEdsPropID_DateTime))
    {
        date_time = get_camera_property_datetime(img.get_ref(), kEdsPropID_DateTime);
    }
}
}
