//
//  camera_interface.hpp
//  camera_interface
//
//  Created by Rob McKay on 08/01/2021.
//

#ifndef camera_interface_
#define camera_interface_

#include <memory>
#include <regex>
#include <string>

/* The classes below are exported */
#pragma GCC visibility push(default)

#include "eds_exception.hpp"

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
    virtual bool get_lens_status() const = 0;
    virtual std::string get_lens_name() const = 0;
    virtual std::string get_artist() const = 0;
    virtual std::string get_copyright() const = 0;
    virtual size_t get_available_shots() const = 0;
};

class directory_ref
{
public:
    typedef std::size_t size_type;
    typedef uint32_t format_t;

    virtual size_type get_file_size() const = 0;
    virtual format_t get_format() const = 0;
    virtual std::string get_name() const = 0;
    virtual std::string get_date_time() const = 0;
    virtual std::time_t get_timestamp() const = 0;
    virtual uint32_t get_group_ID() const = 0;
    virtual void download_to(std::string destination) const = 0;

    virtual bool is_a_folder() const = 0;
    virtual size_type get_directory_count() const = 0;
    virtual std::shared_ptr<directory_ref> get_directory_entry(
        size_type directory_entry_number) const = 0;

    virtual std::shared_ptr<directory_ref> find_directory(std::string image_folder) const = 0;
};

class volume_ref
{
public:
    typedef std::size_t size_type;
    enum storage_type_t
    {
        none,
        compact_flash,
        sd_card,
        HD,
        CFast
    };

    enum access_type_t
    {
        read,
        write,
        read_write,
        unknown
    };

    /// Get the capacity of the volume (in KB)
    virtual uint64_t get_max_capacity() const = 0;
    /// Get the free space on the volume (in KB)
    virtual uint64_t get_free_space() const = 0;
    virtual std::string get_label() const = 0;
    virtual storage_type_t get_storage_type() const = 0;
    virtual access_type_t get_access() const = 0;

    virtual size_type get_directory_count() const = 0;
    virtual std::shared_ptr<directory_ref> select_directory(size_type directory_number) = 0;

    virtual std::vector<std::shared_ptr<directory_ref>> find_matching_files(
        std::string image_folder, std::regex filename_expression)
        = 0;
};

class camera_ref
{
public:
    typedef uint32_t size_type;

    virtual std::shared_ptr<const connection_info> get_connection_info() const = 0;
    virtual std::shared_ptr<camera_info> get_camera_info() = 0;
    virtual size_type get_volume_count() const = 0;
    virtual std::shared_ptr<volume_ref> select_volume(size_type volume_number) = 0;

    virtual void set_ui_status(bool enabled) = 0;
};

class camera_connection
{
public:
    typedef int32_t size_type;

    virtual size_type number_of_cameras() const = 0;
    virtual std::shared_ptr<camera_ref> select_camera(size_type camera_number) = 0;
    virtual ~camera_connection() {};
};

std::unique_ptr<camera_connection> get_camera_connection();

#pragma GCC visibility pop
#endif
