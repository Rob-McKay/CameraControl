#include "camera_interface.hpp"
#include "mocked-functions.hpp"
#include "gtest/gtest.h"

TEST(get_camera_connection, init)
{
    reset_environment();
    EXPECT_EQ(0, initialised_count);
    EXPECT_EQ(0, finalised_count);

    auto cameras = get_camera_connection();

    EXPECT_NE(nullptr, cameras.get());

    EXPECT_EQ(1, initialised_count);
    EXPECT_EQ(0, finalised_count);

    cameras = nullptr;
    EXPECT_EQ(1, finalised_count);
}

TEST(get_camera_connection, count)
{
    reset_environment();

    auto cameras = get_camera_connection();

    ASSERT_NE(nullptr, cameras.get());

    EXPECT_EQ(1, cameras->number_of_cameras());
}

TEST(get_camera_connection, only_camera)
{
    reset_environment();

    auto cameras = get_camera_connection();

    ASSERT_NE(nullptr, cameras.get());

    auto camera = cameras->select_camera(0);
    auto info = camera->get_camera_info();
    ASSERT_NE(nullptr, info);
}
