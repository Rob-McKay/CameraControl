//
//  camera_ref_impl.cpp
//  camera_interface
//
//  Created by Rob McKay on 09/01/2021.
//

#include "properties.hpp"
#include "eds_exception.hpp"
#include <string>

using namespace std::string_literals;

static const std::string UNKNOWN = "<Unknown>"s;

bool is_property_available(EdsBaseRef ref, EdsPropertyID id)
{
    EdsDataType data_type = kEdsDataType_Unknown;
    EdsUInt32 data_size = 0;

    if (auto err = EdsGetPropertySize(ref, id, 0, &data_type, &data_size); err != EDS_ERR_OK)
    {
        switch (err)
        {
        case EDS_ERR_PROPERTIES_UNAVAILABLE:
        case EDS_ERR_PROTECTION_VIOLATION:
            return false;
        default:
            throw eds_exception("Failed to read camera property metadata "s + std::to_string(id),
                err, __FUNCTION__);
        }
    }

    return true;
}

void ensure_data_type_is(EdsDataType wanted_data_type, EdsPropertyID id, EdsBaseRef ref)
{
    EdsDataType data_type = kEdsDataType_Unknown;
    EdsUInt32 data_size = 0;

    if (auto err = EdsGetPropertySize(ref, id, 0, &data_type, &data_size); err != EDS_ERR_OK)
    {
        throw eds_exception(
            "Failed to read camera property metadata "s + std::to_string(id), err, __FUNCTION__);
    }

    if (data_type != wanted_data_type)
        throw eds_exception("Invalid data type ("s + std::to_string(data_type)
                + ") while reading camera property "s + std::to_string(id),
            EDS_ERR_PROPERTIES_MISMATCH);
}

std::string get_camera_property_string(EdsBaseRef ref, EdsPropertyID id)
{
    char buffer[2048];
    ensure_data_type_is(kEdsDataType_String, id, ref);

    if (auto err = EdsGetPropertyData(ref, id, 0, sizeof(buffer), buffer); err != EDS_ERR_OK)
    {
        throw eds_exception(
            "Failed to read camera property "s + std::to_string(id), err, __FUNCTION__);
    }

    return buffer;
}

int32_t get_camera_property_int32(EdsBaseRef ref, EdsPropertyID id)
{
    ensure_data_type_is(kEdsDataType_Int32, id, ref);

    int32_t buffer;
    if (auto err = EdsGetPropertyData(ref, id, 0, sizeof(buffer), &buffer); err != EDS_ERR_OK)
    {
        throw eds_exception(
            "Failed to read camera property "s + std::to_string(id), err, __FUNCTION__);
    }

    return buffer;
}

uint32_t get_camera_property_uint32(EdsBaseRef ref, EdsPropertyID id)
{
    ensure_data_type_is(kEdsDataType_UInt32, id, ref);

    uint32_t buffer;
    if (auto err = EdsGetPropertyData(ref, id, 0, sizeof(buffer), &buffer); err != EDS_ERR_OK)
    {
        throw eds_exception(
            "Failed to read camera property "s + std::to_string(id), err, __FUNCTION__);
    }

    return buffer;
}

Poco::LocalDateTime get_camera_property_datetime(EdsBaseRef ref, EdsPropertyID id)
{
    ensure_data_type_is(kEdsDataType_Time, id, ref);
    EdsTime dt;
    if (auto err = EdsGetPropertyData(ref, id, 0, sizeof(dt), &dt); err != EDS_ERR_OK)
    {
        throw eds_exception(
            "Failed to read camera property "s + std::to_string(id), err, __FUNCTION__);
    }

    return Poco::LocalDateTime(
        dt.year, dt.month, dt.day, dt.hour, dt.minute, dt.second, dt.milliseconds);
}
