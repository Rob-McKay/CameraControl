#pragma once

// Ensure that __MACOS__ is defined when compiling for macOS. (required for EDSDK.h)
#if !defined __MACOS__
#if defined __APPLE__ && defined __MACH__
#define __MACOS__ 1
#endif
#endif

#include "EDSDK.h"
#include "Poco/LocalDateTime.h"
#include <cstdint>
#include <string>

bool is_property_available(EdsBaseRef ref, EdsPropertyID id);

void ensure_data_type_is(EdsDataType wanted_data_type, EdsPropertyID id, EdsBaseRef ref);

std::string get_camera_property_string(EdsBaseRef ref, EdsPropertyID id);

int32_t get_camera_property_int32(EdsBaseRef ref, EdsPropertyID id);

uint32_t get_camera_property_uint32(EdsBaseRef ref, EdsPropertyID id);

Poco::LocalDateTime get_camera_property_datetime(EdsBaseRef ref, EdsPropertyID id);
