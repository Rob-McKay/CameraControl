//
//  camera_interfacePriv.hpp
//  camera_interface
//
//  Created by Rob McKay on 08/01/2021.
//

#include "camera_interface.hpp"

#include <memory>

#if !defined __MACOS__
#if defined __APPLE__ && defined __MACH__
#define __MACOS__ 1
#else
#error "Only for MacOS"
#endif
#endif

#include "EDSDK.h"


/* The classes below are not exported */
#pragma GCC visibility push(hidden)

namespace implementation
{
class impl_connection_info: public connection_info
{
    std::string port;
    std::string desc;
public:
    impl_connection_info(std::string port, std::string desc);
    impl_connection_info(const impl_connection_info& other);
    impl_connection_info(impl_connection_info&& other);
    impl_connection_info& operator=(const impl_connection_info& other);
    impl_connection_info& operator=(impl_connection_info&& other);
    virtual ~impl_connection_info();
    
    std::string get_port() const override;
    std::string get_desc() const override;
};

class impl_camera_info : public camera_info
{
    std::string product_name;
    std::string body_ID_ex;
    std::string owner_name;
    std::string maker_name;
    std::string date_time;
    std::string firmware_version;
    std::string battery_level;
    std::string battery_quality;
    std::string save_to;
    std::string current_storage;
    std::string current_folder;
    std::string lens_status;
    std::string artist;
    std::string copyright;

public:
    std::string get_product_name() const override {return product_name; };
    std::string get_body_ID_ex() const override {return body_ID_ex; };
    std::string get_owner_name() const override {return owner_name; };
    std::string get_maker_name() const override {return maker_name; };
    std::string get_date_time() const override {return date_time; };
    std::string get_firmware_version() const override {return firmware_version; };
    std::string get_battery_level() const override {return battery_level; };
    std::string get_battery_quality() const override {return battery_quality; };
    std::string get_save_to() const override {return save_to; };
    std::string get_current_storage() const override {return current_storage; };
    std::string get_current_folder() const override {return current_folder; };
    std::string get_lens_status() const override {return lens_status; };
    std::string get_artist() const override {return artist; };
    std::string get_copyright() const override {return copyright; };
    
    impl_camera_info();
    virtual ~impl_camera_info() noexcept;
};

class impl_camera_ref : public camera_ref
{
    EdsCameraRef ref;
    std::shared_ptr<connection_info> conn_info;
    
public:
    impl_camera_ref(EdsCameraRef camera);
    virtual ~impl_camera_ref();
    
    std::shared_ptr<const connection_info> get_connection_info() const override;
    std::shared_ptr<camera_info> get_camera_info() override;
};

class impl_camera_list
{
    std::shared_ptr<impl_camera_ref> current_camera;
public:
    typedef std::size_t size_type;
    
    impl_camera_list();
    ~impl_camera_list();
    size_type size() const noexcept;
    std::shared_ptr<camera_ref> at(size_type offset);

protected:
    EdsCameraListRef list;
    size_type count;
};


class impl_camera_connection : public camera_connection
{
protected:
    std::unique_ptr<impl_camera_list> cameras;
    
public:
    int number_of_cameras() const override;
    std::shared_ptr<camera_ref> select_camera(size_type camera_number) override;
    
    impl_camera_connection();
    virtual ~impl_camera_connection();
};


}

#pragma GCC visibility pop
