//
//  camera_info_impl.cpp
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

#include "EDSDK.h"

#include "Poco/Logger.h"
#include "Poco/LocalDateTime.h"
#include "Poco/DateTimeFormat.h"
#include "Poco/DateTimeFormatter.h"

#include "properties.hpp"

#include <string>
using namespace std::string_literals;

namespace implementation
{
    static const std::string UNKNOWN = "<Unknown>"s;

    static std::string format_battery_level(EdsUInt32 level)
    {
        if (level != 0xffffffff)
            return std::to_string(level) + "%";
        return "AC power";
    }

    static std::string format_save_to(EdsUInt32 save_to_code)
    {
        std::string result;

        if (save_to_code & 1)
            result += "Memory Card"s;

        if (save_to_code & 2)
        {
            if (!result.empty())
                result += ", "s;

            result += "Host Computer"s;
        }

        return result;
    }

    impl_camera_info::impl_camera_info(camera_ref_lock<EdsCameraRef> ref)
    {
        product_name = get_camera_property_string(ref.get_ref(), kEdsPropID_ProductName);
        body_ID_ex = get_camera_property_string(ref.get_ref(), kEdsPropID_BodyIDEx);
        owner_name = get_camera_property_string(ref.get_ref(), kEdsPropID_OwnerName);
        maker_name = is_property_available(ref.get_ref(), kEdsPropID_MakerName) ? get_camera_property_string(ref.get_ref(), kEdsPropID_MakerName) : UNKNOWN;

        if (is_property_available(ref.get_ref(), kEdsPropID_UTCTime))
        {
            Poco::Logger::get("camera_info").notice("UTCTime is available but not used");
        }
        date_time = is_property_available(ref.get_ref(), kEdsPropID_DateTime) ? Poco::DateTimeFormatter::format(get_camera_property_datetime(ref.get_ref(), kEdsPropID_DateTime), "%d-%b-%Y %H:%M:%S"s) : UNKNOWN;
        firmware_version = get_camera_property_string(ref.get_ref(), kEdsPropID_FirmwareVersion);
        battery_level = format_battery_level(get_camera_property_int32(ref.get_ref(), kEdsPropID_BatteryLevel));
        battery_quality = is_property_available(ref.get_ref(), kEdsPropID_BatteryQuality) ? std::to_string(get_camera_property_uint32(ref.get_ref(), kEdsPropID_BatteryQuality)) : UNKNOWN;
        save_to = format_save_to(get_camera_property_uint32(ref.get_ref(), kEdsPropID_SaveTo));
        current_storage = get_camera_property_string(ref.get_ref(), kEdsPropID_CurrentStorage);
        current_folder = get_camera_property_string(ref.get_ref(), kEdsPropID_CurrentFolder);
        lens_status = get_camera_property_uint32(ref.get_ref(), kEdsPropID_LensStatus) != 0;
        lens_name = get_camera_property_string(ref.get_ref(), kEdsPropID_LensName);
        artist = get_camera_property_string(ref.get_ref(), kEdsPropID_Artist);
        copyright = get_camera_property_string(ref.get_ref(), kEdsPropID_Copyright);

        available_shots = get_camera_property_uint32(ref.get_ref(), kEdsPropID_AvailableShots);
    }

    impl_camera_info::~impl_camera_info() noexcept
    {
    }

} // namespace implementation
