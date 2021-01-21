#include "camera_interface.hpp"
#include "mocked-functions.hpp"
#include "gtest/gtest.h"

TEST(get_camera_connection, init)
{
    reset_environment(1);
    EXPECT_EQ(0, initialised_count);
    EXPECT_EQ(0, finalised_count);

    auto cameras = get_camera_connection();

    EXPECT_NE(nullptr, cameras.get());

    EXPECT_EQ(1, initialised_count);
    EXPECT_EQ(0, finalised_count);

    cameras = nullptr;
    EXPECT_EQ(1, finalised_count);
}

TEST(get_camera_connection, count_1)
{
    reset_environment(1);

    auto cameras = get_camera_connection();

    ASSERT_NE(nullptr, cameras.get());

    EXPECT_EQ(1, cameras->number_of_cameras());
}

TEST(get_camera_connection, count_0)
{
    reset_environment(0);

    auto cameras = get_camera_connection();

    ASSERT_NE(nullptr, cameras.get());

    EXPECT_EQ(0, cameras->number_of_cameras());
}

TEST(get_camera_connection, only_camera_connection)
{
    reset_environment(1);

    auto cameras = get_camera_connection();

    ASSERT_NE(nullptr, cameras.get());

    auto camera = cameras->select_camera(0);
    auto conn = camera->get_connection_info();
    ASSERT_NE(nullptr, conn);

    EXPECT_EQ("Port 0", conn->get_port());
}

TEST(get_camera_connection, second_camera_connection)
{
    reset_environment(2);

    auto cameras = get_camera_connection();

    ASSERT_NE(nullptr, cameras.get());

    auto camera = cameras->select_camera(1);
    auto conn = camera->get_connection_info();
    ASSERT_NE(nullptr, conn);

    EXPECT_EQ("Port 1", conn->get_port());
}

TEST(get_camera_connection, only_camera_info)
{
    reset_environment(1);

    auto cameras = get_camera_connection();

    ASSERT_NE(nullptr, cameras.get());

    auto camera = cameras->select_camera(0);
    auto info = camera->get_camera_info();
    ASSERT_NE(nullptr, info);

    EXPECT_EQ("Canon EOS 50000D", info->get_product_name());
}

TEST(get_camera_connection, second_then_first_camera_connection)
{
    reset_environment(2);

    auto cameras = get_camera_connection();

    ASSERT_NE(nullptr, cameras.get());

    auto camera = cameras->select_camera(1);
    auto conn = camera->get_connection_info();
    ASSERT_NE(nullptr, conn);

    EXPECT_EQ("Port 1", conn->get_port());
    cameras->deselect_camera(camera);
    

    camera = cameras->select_camera(0);
    conn = camera->get_connection_info();
    ASSERT_NE(nullptr, conn);

    EXPECT_EQ("Port 0", conn->get_port());
}
