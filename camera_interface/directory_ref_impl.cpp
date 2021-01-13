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
    impl_directory_ref::impl_directory_ref(EdsDirectoryItemRef r) : ref(r), file_size(0), format(0), is_folder(false), group_id(0)
    {
        EdsDirectoryItemInfo item;

        if (auto err = EdsGetDirectoryItemInfo(ref.get_ref(), &item); err != EDS_ERR_OK)
        {
            Poco::Logger::get("directory item").error("Failed to get directory item info (%lu)", err);
            throw eds_exception("Failed to get directory item info", err, __FUNCTION__);
        }

        file_size = item.size;
        format = item.format;
        name = item.szFileName;
        is_folder = item.isFolder;
        group_id = item.groupID;
    }

    impl_directory_ref::~impl_directory_ref()
    {
    }
} // namespace implementation
