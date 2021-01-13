//
//  camera_interfacePriv.hpp
//  camera_interface
//
//  Created by Rob McKay on 08/01/2021.
//

#include "camera_interface.hpp"

#include <memory>

// Ensure that __MACOS__ is defined when compiling for macOS. (required for EDSDK.h)
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
    template <class ref_class>
    class camera_ref_lock
    {
        ref_class ref;

    public:
        camera_ref_lock(ref_class ref) : ref(ref)
        {
            EdsRetain(ref);
        }

        camera_ref_lock(const camera_ref_lock<ref_class> &other) : ref(other.ref)
        {
            EdsRetain(ref);
        }

        camera_ref_lock(camera_ref_lock<ref_class> &&other)
        {
            std::swap(ref, other.ref);
        }

        camera_ref_lock &operator=(const camera_ref_lock<ref_class> &other) = delete;
        camera_ref_lock &operator=(camera_ref_lock<ref_class> &&other)
        {
            std::swap(ref, other.ref);
            return *this;
        }

        ~camera_ref_lock()
        {
            if (ref != nullptr)
                EdsRelease(ref);
            ref = nullptr;
        }

        ref_class get_ref() const { return ref; }
    };

    class impl_connection_info : public connection_info
    {
        std::string port;
        std::string desc;

    public:
        impl_connection_info(std::string port, std::string desc);
        impl_connection_info(const impl_connection_info &other);
        impl_connection_info(impl_connection_info &&other);
        impl_connection_info &operator=(const impl_connection_info &other);
        impl_connection_info &operator=(impl_connection_info &&other);
        virtual ~impl_connection_info();

        std::string get_port() const override;
        std::string get_desc() const override;
    };

    class impl_directory_ref : public directory_ref
    {
        camera_ref_lock<EdsDirectoryItemRef> ref;
        size_type file_size;
        format_t format;
        std::string name;
        bool is_folder;
        uint32_t group_id;

    public:
        impl_directory_ref(EdsDirectoryItemRef r);
        virtual ~impl_directory_ref();

        size_type get_file_size() const override { return file_size; }
        format_t get_format() const override { return format; }
        std::string get_name() const override { return name; }
        bool is_a_folder() const override { return is_folder; };
        uint32_t get_group_ID() const override { return group_id; }
    };

    class impl_volume_ref : public volume_ref
    {
        camera_ref_lock<EdsVolumeRef> ref;
        size_type count;
        uint64_t max_capacity;
        uint64_t free_space;
        std::string label;
        storage_type_t storage_type;
        access_type_t access;

    public:
        impl_volume_ref(EdsVolumeRef r);
        virtual ~impl_volume_ref();
        uint64_t get_max_capacity() const override { return max_capacity; }
        uint64_t get_free_space_bytes() const override { return free_space; }
        std::string get_label() const override { return label; }
        storage_type_t get_storage_type() const override { return storage_type; }
        access_type_t get_access() const override { return access; }

        size_type get_directory_count() const override { return count; }
        std::shared_ptr<directory_ref> select_directory(size_type directory_number) override;
    };

    class impl_camera_session
    {
        camera_ref_lock<EdsCameraRef> ref;

    public:
        impl_camera_session(camera_ref_lock<EdsCameraRef> ref) : ref(ref)
        {
            if (ref.get_ref() != nullptr)
            {
                EdsOpenSession(ref.get_ref());
            }
        }
        ~impl_camera_session()
        {
            if (ref.get_ref() != nullptr)
            {
                EdsCloseSession(ref.get_ref());
            }
        }
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
        bool lens_status;
        std::string lens_name;
        std::string artist;
        std::string copyright;
        size_t available_shots;

    public:
        std::string get_product_name() const override { return product_name; }
        std::string get_body_ID_ex() const override { return body_ID_ex; }
        std::string get_owner_name() const override { return owner_name; }
        std::string get_maker_name() const override { return maker_name; }
        std::string get_date_time() const override { return date_time; }
        std::string get_firmware_version() const override { return firmware_version; }
        std::string get_battery_level() const override { return battery_level; }
        std::string get_battery_quality() const override { return battery_quality; }
        std::string get_save_to() const override { return save_to; }
        std::string get_current_storage() const override { return current_storage; }
        std::string get_current_folder() const override { return current_folder; }
        bool get_lens_status() const override { return lens_status; }
        std::string get_lens_name() const override { return lens_name; }
        std::string get_artist() const override { return artist; }
        std::string get_copyright() const override { return copyright; }
        size_t get_available_shots() const override { return available_shots; }

        impl_camera_info(camera_ref_lock<EdsCameraRef> ref);
        virtual ~impl_camera_info() noexcept;
    };

    class impl_camera_ref : public camera_ref
    {
        camera_ref_lock<EdsCameraRef> ref;
        std::shared_ptr<connection_info> conn_info;

    public:
        impl_camera_ref(EdsCameraRef camera);
        virtual ~impl_camera_ref();

        std::shared_ptr<const connection_info> get_connection_info() const override;
        std::shared_ptr<camera_info> get_camera_info() override;
        size_type get_volume_count() const override;
        std::shared_ptr<volume_ref> select_volume(size_type volume_number) override;
    };

    class impl_camera_list
    {
        std::shared_ptr<impl_camera_ref> current_camera;

    public:
        typedef camera_connection::size_type size_type;

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

} // namespace implementation

#pragma GCC visibility pop
