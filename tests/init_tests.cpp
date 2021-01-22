#include "camera_interface.hpp"
#include "mocked-functions.hpp"
#include "gtest/gtest.h"

// struct camera_info_data
// {
//     std::string product_name;
//     std::string body_ID_ex;
//     std::string owner_name;
//     std::string maker_name;
//     struct tm date_time;
//     std::string firmware_version;
//     int32_t battery_level;
//     uint32_t save_to;
//     std::string current_storage;
//     std::string current_folder;
//     uint32_t lens_status;
//     std::string lens_name;
//     std::string artist;
//     std::string copyright;
//     size_t available_shots;
// };
// struct tm {
// 	int	tm_sec;		/* seconds after the minute [0-60] */
// 	int	tm_min;		/* minutes after the hour [0-59] */
// 	int	tm_hour;	/* hours since midnight [0-23] */
// 	int	tm_mday;	/* day of the month [1-31] */
// 	int	tm_mon;		/* months since January [0-11] */
// 	int	tm_year;	/* years since 1900 */
// 	int	tm_wday;	/* days since Sunday [0-6] */
// 	int	tm_yday;	/* days since January 1 [0-365] */
// 	int	tm_isdst;	/* Daylight Savings Time flag */
// 	long	tm_gmtoff;	/* offset from UTC in seconds */
// 	char	*tm_zone;	/* timezone abbreviation */
// };

camera_info_data camera1 { "Canon EOS 50000D", "1234567890", "Owner", "Maker",
    { 59, 59, 23, 31, 0, 121, 0, 0, 0, 0, nullptr }, "11.22.33", 42, 1, "CF", "100CANON", 1,
    "EF-S10-18mm f/4.5-5.6 IS STM", "Photographer:Rob McKay", "Copyright:Rob McKay", 1234 };

camera_info_data camera2 { "Canon EOS 50D", "1234567890", "Owner", "Maker",
    { 59, 59, 23, 31, 0, 121, 0, 0, 0, 0, nullptr }, "11.22.33", 42, 1, "CF", "100CANON", 1,
    "EF-S10-18mm f/4.5-5.6 IS STM", "Photographer:Rob McKay", "Copyright:Rob McKay", 1234 };

TEST(get_camera_connection, init)
{
    reset_environment();
    add_camera("0", "Test", camera1);

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
    add_camera("0", "Test", camera1);

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
    add_camera("Port 0", "Test", camera1);

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
    add_camera("0", "Test", camera1);
    add_camera("Port 1", "Test Camera 1", camera2);

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
    add_camera("0", "Test", camera1);

    auto cameras = get_camera_connection();

    ASSERT_NE(nullptr, cameras.get());

    auto camera = cameras->select_camera(0);
    auto info = camera->get_camera_info();
    ASSERT_NE(nullptr, info);

    EXPECT_EQ(camera1.product_name, info->get_product_name());
}

TEST(get_camera_connection, second_then_first_camera_connection)
{
    reset_environment();
    add_camera("Port 0", "Test", camera1);
    add_camera("Port 1", "Test Camera 1", camera2);

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
