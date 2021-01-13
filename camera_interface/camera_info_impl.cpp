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

#include <cstdio>
#include <iostream>
#include <cstdint>

#include "camera_interface.hpp"
#include "camera_interface_impl.hpp"

#include "EDSDK.h"

#include "Poco/Logger.h"
#include "Poco/LocalDateTime.h"
#include "Poco/DateTimeFormat.h"
#include "Poco/DateTimeFormatter.h"

using namespace std::string_literals;

namespace implementation
{
    static const std::string UNKNOWN = "<Unknown>"s;

    static bool is_property_available(camera_ref_lock<EdsCameraRef> &ref, EdsPropertyID id)
    {
        EdsDataType data_type = kEdsDataType_Unknown;
        EdsUInt32 data_size = 0;

        if (auto err = EdsGetPropertySize(ref.get_ref(), id, 0, &data_type, &data_size); err != EDS_ERR_OK)
        {
            switch (err)
            {
            case EDS_ERR_PROPERTIES_UNAVAILABLE:
            case EDS_ERR_PROTECTION_VIOLATION:
                return false;
            default:
                throw eds_exception("Failed to read camera property metadata "s + std::to_string(id), err, __FUNCTION__);
            }
        }

        return true;
    }

    static void ensure_data_type_is(EdsDataType wanted_data_type, EdsPropertyID id, camera_ref_lock<EdsCameraRef> &ref)
    {
        EdsDataType data_type = kEdsDataType_Unknown;
        EdsUInt32 data_size = 0;

        if (auto err = EdsGetPropertySize(ref.get_ref(), id, 0, &data_type, &data_size); err != EDS_ERR_OK)
        {
            throw eds_exception("Failed to read camera property metadata "s + std::to_string(id), err, __FUNCTION__);
        }

        if (data_type != wanted_data_type)
            throw eds_exception("Invalid data type ("s + std::to_string(data_type) + ") while reading camera property "s + std::to_string(id), EDS_ERR_PROPERTIES_MISMATCH);
    }

    static std::string get_camera_property_string(camera_ref_lock<EdsCameraRef> &ref, EdsPropertyID id)
    {
        char buffer[2048];
        ensure_data_type_is(kEdsDataType_String, id, ref);

        if (auto err = EdsGetPropertyData(ref.get_ref(), id, 0, sizeof(buffer), buffer); err != EDS_ERR_OK)
        {
            throw eds_exception("Failed to read camera property "s + std::to_string(id), err, __FUNCTION__);
        }

        return buffer;
    }

    static int32_t get_camera_property_int32(camera_ref_lock<EdsCameraRef> &ref, EdsPropertyID id)
    {
        ensure_data_type_is(kEdsDataType_Int32, id, ref);

        int32_t buffer;
        if (auto err = EdsGetPropertyData(ref.get_ref(), id, 0, sizeof(buffer), &buffer); err != EDS_ERR_OK)
        {
            throw eds_exception("Failed to read camera property "s + std::to_string(id), err, __FUNCTION__);
        }

        return buffer;
    }

    static uint32_t get_camera_property_uint32(camera_ref_lock<EdsCameraRef> &ref, EdsPropertyID id)
    {
        ensure_data_type_is(kEdsDataType_UInt32, id, ref);

        uint32_t buffer;
        if (auto err = EdsGetPropertyData(ref.get_ref(), id, 0, sizeof(buffer), &buffer); err != EDS_ERR_OK)
        {
            throw eds_exception("Failed to read camera property "s + std::to_string(id), err, __FUNCTION__);
        }

        return buffer;
    }

    static Poco::LocalDateTime get_camera_property_datetime(camera_ref_lock<EdsCameraRef> &ref, EdsPropertyID id)
    {
        ensure_data_type_is(kEdsDataType_Time, id, ref);
        EdsTime dt;
        if (auto err = EdsGetPropertyData(ref.get_ref(), id, 0, sizeof(dt), &dt); err != EDS_ERR_OK)
        {
            throw eds_exception("Failed to read camera property "s + std::to_string(id), err, __FUNCTION__);
        }

        return Poco::LocalDateTime(dt.year, dt.month, dt.day, dt.hour, dt.minute, dt.second, dt.milliseconds);
    }

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
        impl_camera_session session(ref);

        product_name = get_camera_property_string(ref, kEdsPropID_ProductName);
        body_ID_ex = get_camera_property_string(ref, kEdsPropID_BodyIDEx);
        owner_name = get_camera_property_string(ref, kEdsPropID_OwnerName);
        maker_name = is_property_available(ref, kEdsPropID_MakerName) ? get_camera_property_string(ref, kEdsPropID_MakerName) : UNKNOWN;

        if (is_property_available(ref, kEdsPropID_UTCTime))
        {
            Poco::Logger::get("camera_info").notice("UTCTime is available but not used");
        }
        date_time = is_property_available(ref, kEdsPropID_DateTime) ? Poco::DateTimeFormatter::format(get_camera_property_datetime(ref, kEdsPropID_DateTime), "%d-%b-%Y %H:%M:%S"s) : UNKNOWN;
        firmware_version = get_camera_property_string(ref, kEdsPropID_FirmwareVersion);
        battery_level = format_battery_level(get_camera_property_int32(ref, kEdsPropID_BatteryLevel));
        battery_quality = is_property_available(ref, kEdsPropID_BatteryQuality) ? std::to_string(get_camera_property_uint32(ref, kEdsPropID_BatteryQuality)) : UNKNOWN;
        save_to = format_save_to(get_camera_property_uint32(ref, kEdsPropID_SaveTo));
        current_storage = get_camera_property_string(ref, kEdsPropID_CurrentStorage);
        current_folder = get_camera_property_string(ref, kEdsPropID_CurrentFolder);
        lens_status = get_camera_property_uint32(ref, kEdsPropID_LensStatus) != 0;
        lens_name = get_camera_property_string(ref, kEdsPropID_LensName);
        artist = get_camera_property_string(ref, kEdsPropID_Artist);
        copyright = get_camera_property_string(ref, kEdsPropID_Copyright);

        available_shots = get_camera_property_uint32(ref, kEdsPropID_AvailableShots);
    }

    impl_camera_info::~impl_camera_info() noexcept
    {
        //TODO;
    }

} // namespace implementation
