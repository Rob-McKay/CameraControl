//
//  camera_interface.hpp
//  camera_interface
//
//  Created by Rob McKay on 08/01/2021.
//

#ifndef camera_interface_
#define camera_interface_

#include <string>
#include <memory>

/* The classes below are exported */
#pragma GCC visibility push(default)

class connection_info
{
public:
    virtual std::string get_port() const = 0;
    virtual std::string get_desc() const = 0;
};

class camera_info
{
public:
    virtual std::string get_product_name() const = 0;
    virtual std::string get_body_ID_ex() const = 0;
    virtual std::string get_owner_name() const = 0;
    virtual std::string get_maker_name() const = 0;
    virtual std::string get_date_time() const = 0;
    virtual std::string get_firmware_version() const = 0;
    virtual std::string get_battery_level() const = 0;
    virtual std::string get_battery_quality() const = 0;
    virtual std::string get_save_to() const = 0;
    virtual std::string get_current_storage() const = 0;
    virtual std::string get_current_folder() const = 0;
    virtual std::string get_lens_status() const = 0;
    virtual std::string get_artist() const = 0;
    virtual std::string get_copyright() const = 0;
};

class camera_ref
{
public:
    virtual std::shared_ptr<const connection_info> get_connection_info() const = 0;
    virtual std::shared_ptr<camera_info> get_camera_info() = 0;
};

class camera_connection
{
public:
    typedef std::size_t size_type;

    virtual int number_of_cameras() const = 0;
    virtual std::shared_ptr<camera_ref> select_camera(size_type camera_number) = 0;
    virtual ~camera_connection() {};
};

std::unique_ptr<camera_connection> get_camera_connection();

#pragma GCC visibility pop
#endif
