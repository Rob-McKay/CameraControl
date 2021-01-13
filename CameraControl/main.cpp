//
//  main.c
//  List Cameras
//
//  Created by Rob McKay on 08/01/2021.
//

#include "Poco/Util/Application.h"
#include "Poco/Util/Option.h"
#include "Poco/Util/HelpFormatter.h"
#include "Poco/Util/IntValidator.h"

#include "LSCameraConfig.h"

#include <iostream>
#include <iomanip>

using namespace Poco::Util;

#include "camera_interface.hpp"

constexpr int LABEL_WIDTH = 16;
constexpr int DEFAULT_CAMERA_NUMBER = 0;

class my_app : public Poco::Util::Application
{
    bool help_requested = false;
    std::unique_ptr<camera_connection> cameras;

public:
    my_app()
    {
        cameras = get_camera_connection();
    }

    void defineOptions(OptionSet &options) override
    {
        Application::defineOptions(options);
        options.addOption(
            Option("help", "h", "Display help information")
                .required(false)
                .repeatable(false)
                .callback(OptionCallback<my_app>(this, &my_app::handle_help)));

        const int count = cameras->number_of_cameras();

        options.addOption(
            Option("camera-number", "c", "Choose camera (0..n-1)")
                .required(false)
                .argument("camera")
                .validator(new IntValidator(0, count))
                .binding("camera_number"));

        options.addOption(
            Option("files", "f", "Display file information for the selected camera, or camera 0 if no other camera is selected")
                .required(false)
                .binding("show_files"));
    }

    void initialize(Application &self) override
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
        helpFormatter.setHeader("\n" + commandName() + " Version " STR(LSCAMERA_VERSION_MAJOR) "." STR(LSCAMERA_VERSION_MINOR) "\n\nList Canon cameras connect to the computer (via USB)");
        helpFormatter.format(std::cout);
    }

    void handle_help(const std::string &, const std::string &)
    {
        help_requested = true;
        display_help();
        stopOptionsProcessing();
    }

    void camera_details(int camera_number)
    {
        auto camera_ref = cameras->select_camera(camera_number);
        auto conn_info = camera_ref->get_connection_info();
        logger().debug("Found camera %d  on port %s: %s", camera_number, conn_info->get_port(), conn_info->get_desc());

        auto camera_info = camera_ref->get_camera_info();
        std::cout << std::left; // << std::setfill('_');
        std::cout << std::setw(LABEL_WIDTH) << "Product" << std::setw(0) << camera_info->get_product_name() << std::endl;
        std::cout << std::setw(LABEL_WIDTH) << "Body" << camera_info->get_body_ID_ex() << std::endl;
        std::cout << std::setw(LABEL_WIDTH) << "Owner Name" << camera_info->get_owner_name() << std::endl;
        std::cout << std::setw(LABEL_WIDTH) << "Maker" << camera_info->get_maker_name() << std::endl;
        std::cout << std::setw(LABEL_WIDTH) << "Date/Time" << camera_info->get_date_time() << std::endl;
        std::cout << std::setw(LABEL_WIDTH) << "Firmware" << camera_info->get_firmware_version() << std::endl;
        std::cout << std::setw(LABEL_WIDTH) << "Battery Level" << camera_info->get_battery_level() << std::endl;
        std::cout << std::setw(LABEL_WIDTH) << "Battery Quality" << camera_info->get_battery_quality() << std::endl;
        std::cout << std::setw(LABEL_WIDTH) << "Save to" << camera_info->get_save_to() << std::endl;
        std::cout << std::setw(LABEL_WIDTH) << "Current Storage" << camera_info->get_current_storage() << std::endl;
        std::cout << std::setw(LABEL_WIDTH) << "Current Folder" << camera_info->get_current_folder() << std::endl;
        std::cout << std::setw(LABEL_WIDTH) << "Lens Status" << (camera_info->get_lens_status() ? "Lens Attached" : "No Lens") << std::endl;
        std::cout << std::setw(LABEL_WIDTH) << "Lens Name" << camera_info->get_lens_name() << std::endl;
        std::cout << std::setw(LABEL_WIDTH) << "Artist" << camera_info->get_artist() << std::endl;
        std::cout << std::setw(LABEL_WIDTH) << "Copyright" << camera_info->get_copyright() << std::endl;
        std::cout << std::setw(LABEL_WIDTH) << "Available Shots" << camera_info->get_available_shots() << std::endl;
    }

    int main(const std::vector<std::string> &) override
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
            std::cerr << "TODO: Show file details for camera " << camera_number << std::endl;
        }
        

        //    // Set Object event handler
        //    if(err == EDS_ERR_OK)
        //    {
        //        printf("Registering object event handler\n");
        //        err = EdsSetObjectEventHandler(camera, kEdsObjectEvent_All, handleObjectEvent, NULL);
        //    }
        //
        //    // Set Property event handler
        //    if(err == EDS_ERR_OK)
        //    {
        //        printf("Registering property event handler\n");
        //        err = EdsSetPropertyEventHandler(camera, kEdsPropertyEvent_All, handlePropertyEvent, NULL);
        //    }
        //
        //    // Set State event handler
        //    if(err == EDS_ERR_OK)
        //    {
        //        printf("Registering camera state event handler\n");
        //        err = EdsSetCameraStateEventHandler(camera, kEdsStateEvent_All, handleStateEvent, NULL);
        //    }
        //
        //    // Open session with camera
        //    if(err == EDS_ERR_OK)
        //    {
        //        printf("Opening session\n");
        //        err = EdsOpenSession(camera);
        //    }
        //
        //    //
        //    // do something
        //    //
        //    if(err == EDS_ERR_OK)
        //    {
        //        char name[1024];
        //        EdsError e = getCameraInfo(camera, name);
        //        if (e == EDS_ERR_OK)
        //        {
        //            printf("Found camera: %s\n", name);
        //        }
        //        else
        //        {
        //            printf("Error %d while getting camera name\n", e);
        //        }
        //    }
        //

        return EXIT_OK;
    }
};

POCO_APP_MAIN(my_app);
