//
//  main.c
//  List Cameras
//
//  Created by Rob McKay on 08/01/2021.
//

#define __STDC_WANT_LIB_EXT1__ 1
#include <chrono>
#include <ctime>
#include <thread>

#include "Poco/Util/Application.h"
#include "Poco/Util/HelpFormatter.h"
#include "Poco/Util/IntValidator.h"
#include "Poco/Util/Option.h"

#include "LSCameraConfig.h"

#include <iomanip>
#include <iostream>

#include <filesystem>

using namespace Poco::Util;

#include "camera_interface.hpp"
#include "wildcards.hpp"

constexpr int DEFAULT_CAMERA_NUMBER = 0;
constexpr int DEFAULT_VOLUME_NUMBER = 0;

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

        options.addOption(Option("camera", "c", "Choose camera (0..n-1) defaults to 0")
                              .required(false)
                              .argument("camera")
                              .validator(new IntValidator(0, count))
                              .binding("camera_number"));

        options.addOption(Option("volume", "v", "Choose volume (0..n-1) defaults to 0")
                              .required(false)
                              .argument("volume")
                              .validator(new IntValidator(0, count))
                              .binding("volume_number"));

        options.addOption(Option(
            "folder", "f", "Folder to search. Defaults to current folder from the camera info")
                              .required(false)
                              .argument("folder")
                              .validator(new IntValidator(0, count))
                              .binding("folder_name"));

        options.addOption(
            Option("no-date-folders", "nd", "Do not put downloaded file into date folders")
                .required(false)
                .binding("no_date_folders"));
    }

    void initialize(Application& self) override
    {
        loadConfiguration(); // load default configuration files
        Application::initialize(self);
    }

    std::string format_name(time_t dt, std::string name)
    {
        std::tm t;
        memmove(&t, gmtime(&dt), sizeof(t));

        char buf[100];
        std::strftime(buf, sizeof(buf), "%Y_%m_%d", &t);
        std::filesystem::path dir(buf);

        if (!std::filesystem::exists(dir))
        {
            std::filesystem::create_directories(dir);
        }

        return dir /= name;
    }

#define STR2(x) #x
#define STR(x) STR2(x)

    void display_help()
    {
        HelpFormatter help_formatter(options());
        help_formatter.setCommand(commandName());
        help_formatter.setUsage("OPTIONS <file> [<file> ...]");
        help_formatter.setHeader("\n" + commandName()
            + " Version " STR(LSCAMERA_VERSION_MAJOR) "." STR(
                LSCAMERA_VERSION_MINOR) "\n\nDownload files from a Canon camera connect to the "
                                        "computer (via USB)");
        help_formatter.setFooter(
            "\n<file> can be a standard file wildcard expression.\ne.g. 'IMG_732*' will match "
            "every file "
            "starting with 'IMG_732' but 'IMG_732*.CR2' Will match every file starting with "
            "'IMG_732' and ending with '.CR2'");
        help_formatter.format(std::cout);
    }

    void handle_help(const std::string&, const std::string&)
    {
        help_requested = true;
        display_help();
        stopOptionsProcessing();
    }

    int main(const std::vector<std::string>& args) override
    {
        if (help_requested)
            return EXIT_USAGE;

        const int count = cameras->number_of_cameras();

        if (count < 1)
        {
            std::cerr << "No cameras found\n";
            return EXIT_FAILURE;
        }

        const bool no_date_folders = config().hasProperty("no_date_folders");

        int camera_number = config().getInt("camera_number", DEFAULT_CAMERA_NUMBER);

        auto camera_ref = cameras->select_camera(camera_number);

        int volume_number = config().getInt("volume_number", DEFAULT_VOLUME_NUMBER);
        auto camera_info = camera_ref->get_camera_info(); // link to camera is held open as long as
                                                          // the camera_info object is alive

        std::string folder_name
            = config().getString("folder_name", camera_info->get_current_folder());

        auto vol = camera_ref->select_volume(volume_number);

        int c = 0;

        try
        {
            for (const auto& file_pattern : args)
            {
                auto matching_files = vol->find_matching_files(
                    folder_name, convert_file_wildcards_to_regex(file_pattern));
                for (const auto& file : matching_files)
                {
                    const auto timestamp = file->get_timestamp();
                    auto name = (no_date_folders) ? file->get_name()
                                                  : format_name(timestamp, file->get_name());

                    std::cout << "Copying file " << file->get_name() << " to " << name << std::endl;
                    c++;
                    try
                    {
                        file->download_to(name);
                    }
                    catch (const eds_exception& ex)
                    {
                        std::cerr << "Failed to copy file " << file->get_name() << " to " << name
                                  << ". Error " << ex.what() << std::endl;
                        std::filesystem::remove(name);
                        throw;
                    }
                    auto ft = std::filesystem::file_time_type::clock::from_time_t(timestamp);
                    std::filesystem::last_write_time(name, ft);
                    using namespace std::chrono_literals;
                    std::this_thread::sleep_for(100ms);
                }
            }

            std::cout << c << " file(s) copied\n";

            return EXIT_OK;
        }
        catch (const eds_exception& ex)
        {
            return EXIT_FAILURE;
        }
    }
};

POCO_APP_MAIN(my_app);
