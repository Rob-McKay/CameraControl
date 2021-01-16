//
//  main.c
//  List Cameras
//
//  Created by Rob McKay on 08/01/2021.
//

#include "Poco/Util/Application.h"
#include "Poco/Util/HelpFormatter.h"
#include "Poco/Util/IntValidator.h"
#include "Poco/Util/Option.h"

#include "LSCameraConfig.h"

#include <iomanip>
#include <iostream>

using namespace Poco::Util;

#include "camera_interface.hpp"

constexpr int LABEL_WIDTH = 20;
constexpr int NAME_WIDTH = 12;
constexpr int SIZE_WIDTH = 8;
constexpr int DATE_WIDTH = 16;
constexpr int FORMAT_WIDTH = 10;
constexpr int ID_WIDTH = 16;

constexpr int DEFAULT_CAMERA_NUMBER = 0;

class my_app : public Poco::Util::Application
{
    bool help_requested = false;
    std::unique_ptr<camera_connection> cameras;

public:
    my_app() { cameras = get_camera_connection(); }

    void defineOptions(OptionSet& options) override
    {
        Application::defineOptions(options);
        options.addOption(Option("help", "h", "Display help information")
                              .required(false)
                              .repeatable(false)
                              .callback(OptionCallback<my_app>(this, &my_app::handle_help)));

        const int count = cameras->number_of_cameras();

        options.addOption(Option("camera-number", "c", "Choose camera (0..n-1). Defaults to camera 0")
                              .required(false)
                              .argument("camera")
                              .validator(new IntValidator(0, count))
                              .binding("camera_number"));

        options.addOption(Option("files", "f",
            "Display file information for the selected camera.")
                              .required(false)
                              .binding("show_files"));
    }

    void initialize(Application& self) override
    {
        loadConfiguration(); // load default configuration files
        Application::initialize(self);
    }

#define STR2(x) #x
#define STR(x) STR2(x)

    void display_help()
    {
        HelpFormatter helpFormatter(options());
        helpFormatter.setCommand(commandName());
        helpFormatter.setUsage("OPTIONS");
        helpFormatter.setHeader("\n" + commandName()
            + " Version " STR(LSCAMERA_VERSION_MAJOR) "." STR(
                LSCAMERA_VERSION_MINOR) "\n\nList Canon cameras connect to the computer (via USB)");
        helpFormatter.format(std::cout);
    }

    void handle_help(const std::string&, const std::string&)
    {
        help_requested = true;
        display_help();
        stopOptionsProcessing();
    }

    void camera_details(int camera_number)
    {
        auto camera_ref = cameras->select_camera(camera_number);
        auto conn_info = camera_ref->get_connection_info();
        logger().debug("Found camera %d  on port %s: %s", camera_number, conn_info->get_port(),
            conn_info->get_desc());

        auto camera_info = camera_ref->get_camera_info();
        std::cout << std::left; // << std::setfill('_');
        std::cout << std::setw(LABEL_WIDTH) << "Product" << std::setw(0)
                  << camera_info->get_product_name() << std::endl;
        std::cout << std::setw(LABEL_WIDTH) << "Body" << camera_info->get_body_ID_ex() << std::endl;
        std::cout << std::setw(LABEL_WIDTH) << "Owner Name" << camera_info->get_owner_name()
                  << std::endl;
        std::cout << std::setw(LABEL_WIDTH) << "Maker" << camera_info->get_maker_name()
                  << std::endl;
        std::cout << std::setw(LABEL_WIDTH) << "Date/Time" << camera_info->get_date_time()
                  << std::endl;
        std::cout << std::setw(LABEL_WIDTH) << "Firmware" << camera_info->get_firmware_version()
                  << std::endl;
        std::cout << std::setw(LABEL_WIDTH) << "Battery Level" << camera_info->get_battery_level()
                  << std::endl;
        std::cout << std::setw(LABEL_WIDTH) << "Battery Quality"
                  << camera_info->get_battery_quality() << std::endl;
        std::cout << std::setw(LABEL_WIDTH) << "Save to" << camera_info->get_save_to() << std::endl;
        std::cout << std::setw(LABEL_WIDTH) << "Current Storage"
                  << camera_info->get_current_storage() << std::endl;
        std::cout << std::setw(LABEL_WIDTH) << "Current Folder" << camera_info->get_current_folder()
                  << std::endl;
        std::cout << std::setw(LABEL_WIDTH) << "Lens Status"
                  << (camera_info->get_lens_status() ? "Lens Attached" : "No Lens") << std::endl;
        std::cout << std::setw(LABEL_WIDTH) << "Lens Name" << camera_info->get_lens_name()
                  << std::endl;
        std::cout << std::setw(LABEL_WIDTH) << "Artist" << camera_info->get_artist() << std::endl;
        std::cout << std::setw(LABEL_WIDTH) << "Copyright" << camera_info->get_copyright()
                  << std::endl;
        std::cout << std::setw(LABEL_WIDTH) << "Available Shots"
                  << camera_info->get_available_shots() << std::endl;
    }

    void dump_volume_info(const volume_ref* vol)
    {
        std::cout << std::left;
        std::cout << "Selected volume number " << 0 << std::endl;
        std::cout << std::setw(LABEL_WIDTH) << "Volume" << std::setw(0) << vol->get_label()
                  << std::endl;
        std::cout << std::setw(LABEL_WIDTH) << "Storage Type" << std::setw(0)
                  << vol->get_storage_type() << std::endl;
        std::cout << std::setw(LABEL_WIDTH) << "Access" << std::setw(0) << vol->get_access()
                  << std::endl;
        std::cout << std::setw(LABEL_WIDTH) << "Max Capacity" << std::setw(0)
                  << vol->get_max_capacity() / (1024.0 * 1024.0) << " GB" << std::endl;
        std::cout << std::setw(LABEL_WIDTH) << "Free Space" << std::setw(0)
                  << vol->get_free_space() / (1024.0 * 1024.0) << " GB" << std::endl;
    }

    void dump_directory_item(const directory_ref* dir_item, std::string indent = "")
    {
        std::ios::fmtflags f(std::cout.flags());
        std::cout << std::left << std::showpoint;

        if (dir_item->is_a_folder())
        {
            std::cout << indent << dir_item->get_name() << " (folder)" << std::endl;

            indent += "  ";
            const auto count = dir_item->get_directory_count();
            if (count < 1)
            {
                std::cout << indent << "--  Empty  --" << std::endl;
            }
            else
            {
                for (directory_ref::size_type c = 0; c < count; c++)
                {
                    auto folder_ref = dir_item->get_directory_entry(c);
                    dump_directory_item(folder_ref.get(), indent);
                }
            }
        }
        else
        {
            std::cout << indent << std::setw(NAME_WIDTH) << dir_item->get_name()
                      << std::setw(SIZE_WIDTH) << std::right << std::setprecision(2) << std::fixed
                      << dir_item->get_file_size() / (1024.0 * 1024.0) << std::setw(3) << "MB "
                      << std::setw(DATE_WIDTH) << dir_item->get_date_time()
                      << std::showbase << std::setw(FORMAT_WIDTH) << std::hex 
                      << dir_item->get_format() << std::noshowbase << std::setw(ID_WIDTH) << std::dec
                      << std::setfill(' ') << dir_item->get_group_ID() << std::endl;
        }
        std::cout.flags(f);
    }

    int main(const std::vector<std::string>&) override
    {
        if (help_requested)
            return EXIT_USAGE;

        const int count = cameras->number_of_cameras();

        if (count < 1)
        {
            std::cerr << "No cameras found\n";
            return EXIT_FAILURE;
        }

        if (!config().hasOption("show_files"))
        {
            if (!config().hasOption("camera_number"))
                for (int camera_number = 0; camera_number < count; camera_number++)
                {
                    camera_details(camera_number);
                }
            else
            {
                camera_details(config().getInt("camera_number"));
            }
        }
        else
        {
            int camera_number = config().getInt("camera_number", DEFAULT_CAMERA_NUMBER);

            auto camera_ref = cameras->select_camera(camera_number);

            auto vol = camera_ref->select_volume(0);
            auto dir_item_count = vol->get_directory_count();

            dump_volume_info(vol.get());
            std::cout << std::setw(LABEL_WIDTH) << "Root Dir Entry" << std::setw(0)
                      << dir_item_count << std::endl;
            std::cout << std::endl;

            if (dir_item_count < 1)
            {
                std::cout << "--  Empty  --" << std::endl;
            }
            else
            {
                for (volume_ref::size_type dir_item_no = 0; dir_item_no < dir_item_count;
                     dir_item_no++)
                {
                    auto dir_item = vol->select_directory(dir_item_no);
                    dump_directory_item(dir_item.get());
                }
            }
        }

        return EXIT_OK;
    }
};

POCO_APP_MAIN(my_app);
