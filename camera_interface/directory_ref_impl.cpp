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

namespace implementation
{
impl_directory_ref::impl_directory_ref(EdsDirectoryItemRef r)
    : ref(r)
    , file_size(0)
    , format(0)
    , is_folder(false)
    , group_id(0)
    , count(0)
{
    EdsDirectoryItemInfo item;

    if (auto err = EdsGetDirectoryItemInfo(ref.get_ref(), &item); err != EDS_ERR_OK)
    {
        Poco::Logger::get("directory_item").error("Failed to get directory item info (%lu)", err);
        throw eds_exception("Failed to get directory item info", err, __FUNCTION__);
    }

    file_size = item.size;
    format = item.format;
    name = item.szFileName;
    is_folder = item.isFolder;
    group_id = item.groupID;

    if (is_folder)
    {
        EdsUInt32 listCount = 0;
        if (auto err = EdsGetChildCount(ref.get_ref(), &listCount); err != EDS_ERR_OK)
        {
            Poco::Logger::get("directory_item")
                .error("Failed to get directory folder item count (0x%lX)", err);
            throw eds_exception("Failed to get directory folder item count", err, __FUNCTION__);
        }

        count = listCount;
    }
}

impl_directory_ref::~impl_directory_ref() { }

volume_ref::size_type impl_directory_ref::get_directory_count() const
{
    if (!is_folder)
        throw std::logic_error("Not a directory");

    return count;
}

std::shared_ptr<directory_ref> impl_directory_ref::get_directory_entry(
    volume_ref::size_type directory_entry_number) const
{
    if (!is_folder)
        throw std::logic_error("Not a directory");

    if (directory_entry_number >= count)
    {
        Poco::Logger::get("directory_ref")
            .error("Directory entry out of range (0x%lX)", directory_entry_number);
        throw std::out_of_range("Directory entry out of range");
    }

    EdsDirectoryItemRef dir(nullptr);
    if (auto err
        = EdsGetChildAtIndex(ref.get_ref(), static_cast<EdsInt32>(directory_entry_number), &dir);
        err != EDS_ERR_OK)
    {
        Poco::Logger::get("directory_ref").error("Failed to get directory entry (0x%lX)", err);
        throw eds_exception("Failed to get directory entry", err, __FUNCTION__);
    }

    std::shared_ptr<directory_ref> dir_impl = std::make_shared<impl_directory_ref>(dir);

    return dir_impl;
}

std::shared_ptr<directory_ref> impl_directory_ref::find_directory(std::string image_folder) const
{
    if (!is_folder)
        throw std::logic_error("Not a directory");

    for (volume_ref::size_type directory_entry_number = 0; directory_entry_number < count;
         directory_entry_number++)
    {
        EdsDirectoryItemRef dir(nullptr);
        if (auto err = EdsGetChildAtIndex(
                ref.get_ref(), static_cast<EdsInt32>(directory_entry_number), &dir);
            err != EDS_ERR_OK)
        {
            Poco::Logger::get("directory_ref").error("Failed to get directory entry (0x%lX)", err);
            throw eds_exception("Failed to get directory entry", err, __FUNCTION__);
        }

        std::shared_ptr<directory_ref> dir_impl = std::make_shared<impl_directory_ref>(dir);

        if ((dir_impl->is_a_folder()) && (dir_impl->get_name().compare(image_folder) == 0))
            return dir_impl;
    }

    return nullptr;
}

} // namespace implementation
