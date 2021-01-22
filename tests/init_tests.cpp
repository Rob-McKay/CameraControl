#include "camera_interface.hpp"
#include "mocked-functions.hpp"
#include "gtest/gtest.h"

TEST(get_camera_connection, init)
{
    reset_environment();
    add_camera("0", "Test");

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
    reset_environment();
    add_camera("0", "Test");

    auto cameras = get_camera_connection();

    ASSERT_NE(nullptr, cameras.get());

    EXPECT_EQ(1, cameras->number_of_cameras());
}

TEST(get_camera_connection, count_0)
{
    reset_environment();

    auto cameras = get_camera_connection();

    ASSERT_NE(nullptr, cameras.get());

    EXPECT_EQ(0, cameras->number_of_cameras());
}

TEST(get_camera_connection, only_camera_connection)
{
    reset_environment();
    add_camera("Port 0", "Test");

    auto cameras = get_camera_connection();

    ASSERT_NE(nullptr, cameras.get());

    auto camera = cameras->select_camera(0);
    auto conn = camera->get_connection_info();
    ASSERT_NE(nullptr, conn);

    EXPECT_EQ("Port 0", conn->get_port());
    EXPECT_EQ("Test", conn->get_desc());
}

TEST(get_camera_connection, second_camera_connection)
{
    reset_environment();
    add_camera("0", "Test");
    add_camera("Port 1", "Test Camera 1");

    auto cameras = get_camera_connection();

    ASSERT_NE(nullptr, cameras.get());

    auto camera = cameras->select_camera(1);
    auto conn = camera->get_connection_info();
    ASSERT_NE(nullptr, conn);

    EXPECT_EQ("Port 1", conn->get_port());
    EXPECT_EQ("Test Camera 1", conn->get_desc());
}

TEST(get_camera_connection, only_camera_info)
{
    reset_environment();
    add_camera("0", "Test");

    auto cameras = get_camera_connection();

    ASSERT_NE(nullptr, cameras.get());

    auto camera = cameras->select_camera(0);
    auto info = camera->get_camera_info();
    ASSERT_NE(nullptr, info);

    EXPECT_EQ("Canon EOS 50000D", info->get_product_name());
}

TEST(get_camera_connection, second_then_first_camera_connection)
{
    reset_environment();
    add_camera("Port 0", "Test");
    add_camera("Port 1", "Test Camera 1");

    auto cameras = get_camera_connection();

    ASSERT_NE(nullptr, cameras.get());

    auto camera = cameras->select_camera(1);
    auto conn = camera->get_connection_info();
    ASSERT_NE(nullptr, conn);

    EXPECT_EQ("Port 1", conn->get_port());
    EXPECT_EQ("Test Camera 1", conn->get_desc());

    cameras->deselect_camera(camera);

    camera = cameras->select_camera(0);
    conn = camera->get_connection_info();
    ASSERT_NE(nullptr, conn);

    EXPECT_EQ("Port 0", conn->get_port());
    EXPECT_EQ("Test", conn->get_desc());
}
