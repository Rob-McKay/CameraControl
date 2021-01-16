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
#include "int_to_hex.hpp"
#include "properties.hpp"
#include <cstdio>
#include <iostream>
#include <string>

#include "EDSDK.h"

#include "Poco/DateTimeFormat.h"
#include "Poco/DateTimeFormatter.h"
#include "Poco/LocalDateTime.h"
#include "Poco/Logger.h"

namespace implementation
{
using namespace std::string_literals;

impl_directory_ref::impl_directory_ref(EdsDirectoryItemRef r)
    : ref(r)
    , file_size(0)
    , format(0)
    , is_folder(false)
    , group_id(0)
    , count(0)
{
    EdsDirectoryItemInfo item;

    THROW_ERRORS(EdsGetDirectoryItemInfo(ref.get_ref(), &item), "directory_item",
        "Failed to get directory item info");

    file_size = item.size;
    format = item.format;
    name = item.szFileName;
    is_folder = item.isFolder;
    group_id = item.groupID;

    if (is_folder)
    {
        EdsUInt32 listCount = 0;
        THROW_ERRORS(EdsGetChildCount(ref.get_ref(), &listCount), "directory_item",
            "Failed to get directory folder item count");

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
            .error("Directory entry out of range (%s)", std::to_string(directory_entry_number));
        throw std::out_of_range("Directory entry out of range");
    }

    EdsDirectoryItemRef dir(nullptr);
    THROW_ERRORS(
        EdsGetChildAtIndex(ref.get_ref(), static_cast<EdsInt32>(directory_entry_number), &dir),
        "directory_ref", "Failed to get directory entry");

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
        THROW_ERRORS(
            EdsGetChildAtIndex(ref.get_ref(), static_cast<EdsInt32>(directory_entry_number), &dir),
            "directory_ref", "Failed to get directory entry");

        std::shared_ptr<directory_ref> dir_impl = std::make_shared<impl_directory_ref>(dir);

        if ((dir_impl->is_a_folder()) && (dir_impl->get_name().compare(image_folder) == 0))
            return dir_impl;
    }

    return nullptr;
}

std::string impl_directory_ref::get_date_time() const
{
    Poco::LocalDateTime date_time(0);

    if (!is_folder)
    {
        thumbnail t(ref.get_ref());
        date_time = t.get_date_stamp();
    }

    return (date_time != 0) ? Poco::DateTimeFormatter::format(date_time, "%d-%b-%Y %H:%M:%S"s)
                            : ""s;
}

void impl_directory_ref::download_to(std::string destination) const
{
    // TODO: implement
}

} // namespace implementation
