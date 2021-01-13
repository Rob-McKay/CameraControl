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

#include "Poco/Logger.h"

namespace implementation
{
    impl_volume_ref::impl_volume_ref(EdsVolumeRef r) : ref(r), count(0), max_capacity(0), free_space(0), storage_type(storage_type_t::none), access(access_type_t::unknown)
    {
        EdsUInt32 volumeCount = 0;
        if (auto err = EdsGetChildCount(ref.get_ref(), &volumeCount); err != EDS_ERR_OK)
        {
            Poco::Logger::get("volume").error("Failed to get volume directory count (%lu)", err);
            throw eds_exception("Failed to get volume directory count", err, __FUNCTION__);
        }

        count = volumeCount;
        EdsVolumeInfo volume;

        if (auto err = EdsGetVolumeInfo(ref.get_ref(), &volume); err != EDS_ERR_OK)
        {
            Poco::Logger::get("volume").error("Failed to get volume info (%lu)", err);
            throw eds_exception("Failed to get volume info", err, __FUNCTION__);
        }

        max_capacity = volume.maxCapacity;
        free_space = volume.freeSpaceInBytes;
        label = volume.szVolumeLabel;

        switch (volume.storageType)
        {
        default:
        case EdsStorageType::kEdsStorageType_Non:
            storage_type = storage_type_t::none;
            break;
        case EdsStorageType::kEdsStorageType_CF:
            storage_type = storage_type_t::compact_flash;
            break;
        case EdsStorageType::kEdsStorageType_SD:
            storage_type = storage_type_t::sd_card;
            break;
        case EdsStorageType::kEdsStorageType_HD:
            storage_type = storage_type_t::HD;
            break;
        case EdsStorageType::kEdsStorageType_CFast:
            storage_type = storage_type_t::CFast;
            break;
        }

        switch (volume.access)
        {
        case EdsAccess::kEdsAccess_Read:
            access = access_type_t::read;
            break;
        case EdsAccess::kEdsAccess_Write:
            access = access_type_t::write;
            break;
        case EdsAccess::kEdsAccess_ReadWrite:
            access = access_type_t::read_write;
            break;
        case EdsAccess::kEdsAccess_Error:
        default:
            access = access_type_t::unknown;
        }
    }

    impl_volume_ref::~impl_volume_ref()
    {
    }

    std::shared_ptr<directory_ref> impl_volume_ref::select_directory(size_type directory_number)
    {
        if (directory_number >= count)
        {
            Poco::Logger::get("volume_ref").error("Directory entry out of range (%lu)", directory_number);
            throw std::out_of_range("Directory entry out of range");
        }

        EdsDirectoryItemRef dir(nullptr);
        if (auto err = EdsGetChildAtIndex(ref.get_ref(), static_cast<EdsInt32>(directory_number), &dir); err != EDS_ERR_OK)
        {
            Poco::Logger::get("camera_list").error("Failed to select camera (%lu)", err);
            throw eds_exception("Failed to select camera", err, __FUNCTION__);
        }

        std::shared_ptr<directory_ref> dir_impl = std::make_shared<impl_directory_ref>(dir);

        return dir_impl;
    }
} // namespace implementation
