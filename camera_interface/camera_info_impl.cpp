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
#include "camera_interface.hpp"
#include "camera_interface_impl.hpp"

#include "EDSDK.h"

namespace implementation
{
impl_camera_info::impl_camera_info() {
    //TODO;
}

impl_camera_info::~impl_camera_info() noexcept {
    //TODO;
}

}
