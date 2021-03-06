//
//  mocked-functions.cpp
//  mock the Canon EDSDK functions for testing
//
//  Created by Rob McKay on 09/01/2021.
//

#if !defined __MACOS__
#if defined __APPLE__ && defined __MACH__
#define __MACOS__ 1
#endif
#endif
#define __STDC_WANT_LIB_EXT1__ 1
#include <string.h>

#include "EDSDK.h"
#include "EDSDKErrors.h"
#include "gtest/gtest.h"
#include <cstddef>
#include <memory>
#include <span>
#include <string>
#include <vector>

#include "gsl/span"
#include "gsl/span_ext"

namespace
{
struct object_properties
{
    EdsDataType data_type;
    EdsUInt32 size;
    std::string char_data;
    std::vector<std::byte> binary_data;
};

std::vector<std::byte> vectorise(gsl::span<const std::byte> sp)
{
    return std::vector<std::byte>(sp.begin(), sp.end());
}
}

struct __EdsObject
{
    int count { 0 };
    void retain()
    {
        ASSERT_GE(count, 0);
        count++;
    }

    void release()
    {
        ASSERT_GE(count, 1);
        count--;
    }

    virtual std::map<EdsPropertyID, object_properties>& get_object_properties() = 0;
    virtual EdsUInt32 get_property_size(EdsPropertyID propertyID)
    {
        auto& props = get_object_properties();
        auto prop = props.find(propertyID);
        if (prop != props.end())
            return prop->second.size;
        return 0;
    }

    virtual EdsDataType get_property_type(EdsPropertyID propertyID)
    {
        auto& props = get_object_properties();
        auto prop = props.find(propertyID);
        if (prop != props.end())
            return prop->second.data_type;
        return kEdsDataType_Unknown;
    }

    virtual EdsError get_property_data(EdsPropertyID propertyID, EdsInt32 /*inParam*/,
        EdsUInt32 inPropertySize, EdsVoid* outPropertyData)
    {
        EXPECT_GE(count, 1);

        auto& props = get_object_properties();
        auto prop = props.find(propertyID);
        if (prop != props.end())
        {
            if (prop->second.data_type == kEdsDataType_String)
            {
                strncpy(reinterpret_cast<char*>(outPropertyData), prop->second.char_data.c_str(),
                    inPropertySize);
            }
            else
            {
                memcpy(outPropertyData, prop->second.binary_data.data(),
                    std::min(prop->second.binary_data.size(), static_cast<size_t>(inPropertySize)));
            }
            return EDS_ERR_OK;
        }
        return EDS_ERR_UNIMPLEMENTED;
    }

    virtual EdsError get_child_at_index(int, EdsBaseRef*) { return EDS_ERR_SELECTION_UNAVAILABLE; }
    virtual EdsError get_child_count(EdsUInt32*) { return EDS_ERR_SELECTION_UNAVAILABLE; }
    virtual ~__EdsObject() {};
};

struct entries
{
    EdsPropertyID key;
    object_properties value;
};

struct camera_info_data
{
    std::string product_name;
    std::string body_ID_ex;
    std::string owner_name;
    std::string maker_name;
    struct tm date_time;
    std::string firmware_version;
    int32_t battery_level;
    uint32_t save_to;
    std::string current_storage;
    std::string current_folder;
    uint32_t lens_status;
    std::string lens_name;
    std::string artist;
    std::string copyright;
    size_t available_shots;
};

class EdsCamera : public __EdsObject
{
    EdsDeviceInfo info;
    std::map<EdsPropertyID, object_properties> properties;

public:
    EdsCamera(std::string port, std::string name, const camera_info_data& data)
    {
        strncpy(info.szPortName, port.c_str(), sizeof(info.szPortName));
        strncpy(info.szDeviceDescription, name.c_str(), sizeof(info.szDeviceDescription));
        info.deviceSubType = 1;
        info.reserved = 0;

        EdsTime camera_time { static_cast<EdsUInt32>(data.date_time.tm_year + 1900), static_cast<EdsUInt32>(data.date_time.tm_mon + 1),
            static_cast<EdsUInt32>(data.date_time.tm_mday), static_cast<EdsUInt32>(data.date_time.tm_hour), static_cast<EdsUInt32>(data.date_time.tm_min),
            static_cast<EdsUInt32>(data.date_time.tm_sec), 0 };

        for (auto p :
            std::initializer_list<entries> {
                { kEdsPropID_ProductName,
                    { kEdsDataType_String, EDS_MAX_NAME, data.product_name, {} } },
                { kEdsPropID_BodyIDEx, { kEdsDataType_String, EDS_MAX_NAME, data.body_ID_ex, {} } },
                { kEdsPropID_OwnerName,
                    { kEdsDataType_String, EDS_MAX_NAME, data.owner_name, {} } },
                //{ kEdsPropID_MakerName, { kEdsDataType_String, EDS_MAX_NAME, "Canon", {} } },
                { kEdsPropID_DateTime,
                    { kEdsDataType_Time, sizeof(EdsTime), "",
                        vectorise(gsl::as_bytes(gsl::make_span(&camera_time, 1))) } },
                { kEdsPropID_FirmwareVersion,
                    { kEdsDataType_String, EDS_MAX_NAME, "11.22.33", {} } },
                { kEdsPropID_BatteryLevel,
                    { kEdsDataType_Int32, sizeof(EdsInt32), "",
                        vectorise(gsl::as_bytes(gsl::make_span(&data.battery_level, 1))) } },
                { kEdsPropID_SaveTo,
                    { kEdsDataType_UInt32, sizeof(EdsUInt32), "",
                        vectorise(gsl::as_bytes(gsl::make_span(&data.save_to, 1))) } },
                { kEdsPropID_CurrentStorage, { kEdsDataType_String, EDS_MAX_NAME, "CF", {} } },
                { kEdsPropID_CurrentFolder, { kEdsDataType_String, EDS_MAX_NAME, "100CANON", {} } },
                { kEdsPropID_AvailableShots,
                    { kEdsDataType_UInt32, sizeof(EdsUInt32), "",
                        vectorise(gsl::as_bytes(gsl::make_span(&data.available_shots, 1))) } },
                { kEdsPropID_LensStatus,
                    { kEdsDataType_UInt32, sizeof(EdsUInt32), "",
                        vectorise(gsl::as_bytes(gsl::make_span(&data.lens_status, 1))) } },
                { kEdsPropID_LensName,
                    { kEdsDataType_String, EDS_MAX_NAME, "EF-S10-18mm f/4.5-5.6 IS STM", {} } },
                { kEdsPropID_Artist,
                    { kEdsDataType_String, EDS_MAX_NAME, "Photographer:Test User", {} } },
                { kEdsPropID_Copyright,
                    { kEdsDataType_String, EDS_MAX_NAME, "Copyright:Rob McKay", {} } } })
        {
            properties.emplace(p.key, p.value);
        }
    }

    virtual ~EdsCamera() { }
    EdsDeviceInfo& get_device_info()
    {
        EXPECT_GE(count, 1);
        return info;
    }

    std::map<EdsPropertyID, object_properties>& get_object_properties() override
    {
        return properties;
    }
};

class EdsCameraList : public __EdsObject
{
    std::vector<EdsCamera> cameras;
    std::map<EdsPropertyID, object_properties> properties;

public:
    EdsCameraList() { }

    void add_camera(std::string port, std::string name, const camera_info_data& data) // TODO: Add other camera settings
    {
        cameras.emplace_back(port, name, data);
    }

    int size()
    {
        EXPECT_GE(count, 1);
        return cameras.size();
    }

    EdsError get_child_at_index(int offset, EdsBaseRef* out) override
    {
        EXPECT_GE(count, 0);
        if (offset >= static_cast<int>(cameras.size()))
            return EDS_ERR_SELECTION_UNAVAILABLE; // TODO: Check proper return code

        *out = &cameras.at(offset);
        (*out)->retain();
        return EDS_ERR_OK;
    }

    EdsError get_child_count(EdsUInt32* outCount) override
    {
        *outCount = cameras.size();
        return EDS_ERR_OK;
    }

    virtual ~EdsCameraList() { }

    std::map<EdsPropertyID, object_properties>& get_object_properties() override
    {
        return properties;
    }

    EdsUInt32 get_property_size(EdsPropertyID) override
    {
        throw std::runtime_error("Invalid object for get_property");
    }

    EdsDataType get_property_type(EdsPropertyID) override
    {
        throw std::runtime_error("Invalid object for get_property");
    }
};

std::shared_ptr<EdsCameraList> camera_list;
EdsCamera* current_camera = nullptr;
int initialised_count = 0;
int finalised_count = 0;
int max_num_cameras = 1;

void reset_environment()
{
    current_camera = nullptr;
    camera_list = nullptr;
    initialised_count = 0;
    finalised_count = 0;
}

void add_camera(std::string port, std::string camera_name, const camera_info_data& data)
{
    if (!camera_list)
        camera_list = std::make_shared<EdsCameraList>();

    camera_list->add_camera(port, camera_name, data);
}

#pragma clang diagnostic ignored "-Wunused-parameter"

EdsError EDSAPI EdsInitializeSDK()
{
    initialised_count++;
    return EDS_ERR_OK;
}

EdsError EDSAPI EdsTerminateSDK()
{
    finalised_count++;
    return EDS_ERR_OK;
}

EdsUInt32 EDSAPI EdsRetain(EdsBaseRef inRef)
{
    inRef->retain();
    return inRef->count;
}

EdsUInt32 EDSAPI EdsRelease(EdsBaseRef inRef)
{
    inRef->release();
    return inRef->count;
}

EdsError EDSAPI EdsGetChildCount(EdsBaseRef inRef, EdsUInt32* outCount)
{
    return inRef->get_child_count(outCount);
}

EdsError EDSAPI EdsGetChildAtIndex(EdsBaseRef inRef, EdsInt32 inIndex, EdsBaseRef* outRef)
{
    return inRef->get_child_at_index(inIndex, outRef);
}

EdsError EDSAPI EdsGetParent(EdsBaseRef inRef, EdsBaseRef* outParentRef)
{
    return EDS_ERR_UNIMPLEMENTED;
}

EdsError EDSAPI EdsGetPropertySize(EdsBaseRef inRef, EdsPropertyID inPropertyID, EdsInt32 inParam,
    EdsDataType* outDataType, EdsUInt32* outSize)
{
    *outDataType = inRef->get_property_type(inPropertyID);
    if (*outDataType == kEdsDataType_Unknown)
        return EDS_ERR_PROPERTIES_UNAVAILABLE;

    *outSize = inRef->get_property_size(inPropertyID);
    return EDS_ERR_OK;
}

EdsError EDSAPI EdsGetPropertyData(EdsBaseRef inRef, EdsPropertyID inPropertyID, EdsInt32 inParam,
    EdsUInt32 inPropertySize, EdsVoid* outPropertyData)
{
    return inRef->get_property_data(inPropertyID, inParam, inPropertySize, outPropertyData);
}

EdsError EDSAPI EdsSetPropertyData(EdsBaseRef inRef, EdsPropertyID inPropertyID, EdsInt32 inParam,
    EdsUInt32 inPropertySize, const EdsVoid* inPropertyData)
{
    return EDS_ERR_UNIMPLEMENTED;
}

EdsError EDSAPI EdsGetPropertyDesc(
    EdsBaseRef inRef, EdsPropertyID inPropertyID, EdsPropertyDesc* outPropertyDesc)
{
    return EDS_ERR_UNIMPLEMENTED;
}

EdsError EDSAPI EdsGetCameraList(EdsCameraListRef* outCameraListRef)
{
    if (!camera_list)
        camera_list = std::make_shared<EdsCameraList>();

    camera_list->retain();
    *outCameraListRef = camera_list.get();
    return EDS_ERR_OK;
}

EdsError EDSAPI EdsGetDeviceInfo(EdsCameraRef inCameraRef, EdsDeviceInfo* outDeviceInfo)
{
    EXPECT_NE(inCameraRef, nullptr);
    EXPECT_GE(inCameraRef->count, 1);

    *outDeviceInfo = reinterpret_cast<EdsCamera*>(inCameraRef)->get_device_info();
    return EDS_ERR_OK;
}

EdsError EDSAPI EdsOpenSession(EdsCameraRef inCameraRef)
{
    EXPECT_NE(inCameraRef, nullptr);
    EXPECT_GE(inCameraRef->count, 1);

    EXPECT_EQ(nullptr, current_camera);
    current_camera = reinterpret_cast<EdsCamera*>(inCameraRef);
    current_camera->retain();
    return EDS_ERR_OK;
}

EdsError EDSAPI EdsCloseSession(EdsCameraRef inCameraRef)
{
    EXPECT_NE(inCameraRef, nullptr);
    EXPECT_GE(inCameraRef->count, 1);

    EXPECT_EQ(current_camera, inCameraRef);
    current_camera->release();
    current_camera = nullptr;
    return EDS_ERR_OK;
}

/*-----------------------------------------------------------------------------
//
//  Function:   EdsSendCommand
//
//  Description:
//       Sends a command such as "Shoot" to a remote camera.
//
//  Parameters:
//       In:    inCameraRef - The reference of the camera which will receive the
//                      command.
//              inCommand - Specifies the command to be sent.
//              inParam -     Specifies additional command-specific information.
//      Out:    None
//
//  Returns:    Any of the sdk errors.
-----------------------------------------------------------------------------*/
EdsError EDSAPI EdsSendCommand(
    EdsCameraRef inCameraRef, EdsCameraCommand inCommand, EdsInt32 inParam)
{
    return EDS_ERR_UNIMPLEMENTED;
}

/*-----------------------------------------------------------------------------
//
//  Function:   EdsSendStatusCommand
//
//  Description:
//       Sets the remote camera state or mode.
//
//  Parameters:
//       In:    inCameraRef - The reference of the camera which will receive the
//                      command.
//              inStatusCommand - Specifies the command to be sent.
//              inParam -     Specifies additional command-specific information.
//      Out:    None
//
//  Returns:    Any of the sdk errors.
-----------------------------------------------------------------------------*/
EdsError EDSAPI EdsSendStatusCommand(
    EdsCameraRef inCameraRef, EdsCameraStatusCommand inStatusCommand, EdsInt32 inParam)
{
    return EDS_ERR_UNIMPLEMENTED;
}

/*-----------------------------------------------------------------------------
//
//  Function:   EdsSetCapacity
//
//  Description:
//      Sets the remaining HDD capacity on the host computer
//          (excluding the portion from image transfer),
//          as calculated by subtracting the portion from the previous time.
//      Set a reset flag initially and designate the cluster length
//          and number of free clusters.
//      Some type 2 protocol standard cameras can display the number of shots
//          left on the camera based on the available disk capacity
//          of the host computer.
//      For these cameras, after the storage destination is set to the computer,
//          use this API to notify the camera of the available disk capacity
//          of the host computer.
//
//  Parameters:
//       In:    inCameraRef - The reference of the camera which will receive the
//                      command.
//              inCapacity -  The remaining capacity of a transmission place.
//      Out:    None
//
//  Returns:    Any of the sdk errors.
-----------------------------------------------------------------------------*/
EdsError EDSAPI EdsSetCapacity(EdsCameraRef inCameraRef, EdsCapacity inCapacity)
{
    return EDS_ERR_UNIMPLEMENTED;
}

/******************************************************************************
*******************************************************************************
//
//  Volume operating functions
//
*******************************************************************************
******************************************************************************/

/*-----------------------------------------------------------------------------
//
//  Function:   EdsGetVolumeInfo
//
//  Description:
//      Gets volume information for a memory card in the camera.
//
//  Parameters:
//       In:    inVolumeRef - The reference of the volume.
//      Out:    outVolumeInfo - information of  the volume.
//
//  Returns:    Any of the sdk errors.
-----------------------------------------------------------------------------*/
EdsError EDSAPI EdsGetVolumeInfo(EdsVolumeRef inVolumeRef, EdsVolumeInfo* outVolumeInfo)
{
    return EDS_ERR_UNIMPLEMENTED;
}

/*-----------------------------------------------------------------------------
//
//  Function:   EdsFormatVolume
//
//  Description:
//       Formats volumes of memory cards in a camera.
//
//  Parameters:
//       In:    inVolumeRef - The reference of volume .
//
//  Returns:    Any of the sdk errors.
-----------------------------------------------------------------------------*/
EdsError EDSAPI EdsFormatVolume(EdsVolumeRef inVolumeRef) { return EDS_ERR_UNIMPLEMENTED; }

/******************************************************************************
*******************************************************************************
//
//  Directory-item operating functions
//
*******************************************************************************
******************************************************************************/

/*-----------------------------------------------------------------------------
//
//  Function:   EdsGetDirectoryItemInfo
//
//  Description:
//      Gets information about the directory or file objects
//          on the memory card (volume) in a remote camera.
//
//  Parameters:
//       In:    inDirItemRef - The reference of the directory item.
//      Out:    outDirItemInfo - information of the directory item.
//
//  Returns:    Any of the sdk errors.
-----------------------------------------------------------------------------*/
EdsError EDSAPI EdsGetDirectoryItemInfo(
    EdsDirectoryItemRef inDirItemRef, EdsDirectoryItemInfo* outDirItemInfo)
{
    return EDS_ERR_UNIMPLEMENTED;
}

/*-----------------------------------------------------------------------------
//
//  Function:   EdsDeleteDirectoryItem
//
//  Description:
//      Deletes a camera folder or file.
//      If folders with subdirectories are designated, all files are deleted
//          except protected files.
//      EdsDirectoryItem objects deleted by means of this API are implicitly
//          released by the EDSDK. Thus, there is no need to release them
//          by means of EdsRelease.
//
//  Parameters:
//       In:    inDirItemRef - The reference of the directory item.
//
//  Returns:    Any of the sdk errors.
-----------------------------------------------------------------------------*/
EdsError EDSAPI EdsDeleteDirectoryItem(EdsDirectoryItemRef inDirItemRef)
{
    return EDS_ERR_UNIMPLEMENTED;
}

/*-----------------------------------------------------------------------------
//
//  Function:   EdsDownload
//
//  Description:
//       Downloads a file on a remote camera
//          (in the camera memory or on a memory card) to the host computer.
//      The downloaded file is sent directly to a file stream created in advance.
//      When dividing the file being retrieved, call this API repeatedly.
//      Also in this case, make the data block size a multiple of 512 (bytes),
//          excluding the final block.
//
//  Parameters:
//       In:    inDirItemRef - The reference of the directory item.
//              inReadSize   -
//
//      Out:    outStream    - The reference of the stream.
//
//  Returns:    Any of the sdk errors.
-----------------------------------------------------------------------------*/
EdsError EDSAPI EdsDownload(
    EdsDirectoryItemRef inDirItemRef, EdsUInt64 inReadSize, EdsStreamRef outStream)
{
    return EDS_ERR_UNIMPLEMENTED;
}

/*-----------------------------------------------------------------------------
//
//  Function:   EdsDownloadCancel
//
//  Description:
//       Must be executed when downloading of a directory item is canceled.
//      Calling this API makes the camera cancel file transmission.
//      It also releases resources.
//      This operation need not be executed when using EdsDownloadThumbnail.
//
//  Parameters:
//       In:    inDirItemRef - The reference of the directory item.
//
//  Returns:    Any of the sdk errors.
-----------------------------------------------------------------------------*/
EdsError EDSAPI EdsDownloadCancel(EdsDirectoryItemRef inDirItemRef)
{
    return EDS_ERR_UNIMPLEMENTED;
}

/*-----------------------------------------------------------------------------
//
//  Function:   EdsDownloadComplete
//
//  Description:
//       Must be called when downloading of directory items is complete.
//          Executing this API makes the camera
//              recognize that file transmission is complete.
//          This operation need not be executed when using EdsDownloadThumbnail.
//
//  Parameters:
//       In:    inDirItemRef - The reference of the directory item.
//
//      Out:    outStream    - None.
//
//  Returns:    Any of the sdk errors.
-----------------------------------------------------------------------------*/
EdsError EDSAPI EdsDownloadComplete(EdsDirectoryItemRef inDirItemRef)
{
    return EDS_ERR_UNIMPLEMENTED;
}

/*-----------------------------------------------------------------------------
//
//  Function:   EdsDownloadThumbnail
//
//  Description:
//      Extracts and downloads thumbnail information from image files in a camera.
//      Thumbnail information in the camera's image files is downloaded
//          to the host computer.
//      Downloaded thumbnails are sent directly to a file stream created in advance.
//
//  Parameters:
//       In:    inDirItemRef - The reference of the directory item.
//
//      Out:    outStream - The reference of the stream.
//
//  Returns:    Any of the sdk errors.
-----------------------------------------------------------------------------*/
EdsError EDSAPI EdsDownloadThumbnail(EdsDirectoryItemRef inDirItemRef, EdsStreamRef outStream)
{
    return EDS_ERR_UNIMPLEMENTED;
}

/*-----------------------------------------------------------------------------
//
//  Function:   EdsGetAttribute
//
//  Description:
//      Gets attributes of files on a camera.
//
//  Parameters:
//       In:    inDirItemRef - The reference of the directory item.
//      Out:    outFileAttribute  - Indicates the file attributes.
//                  As for the file attributes, OR values of the value defined
//                  by enum EdsFileAttributes can be retrieved. Thus, when
//                  determining the file attributes, you must check
//                  if an attribute flag is set for target attributes.
//
//  Returns:    Any of the sdk errors.
-----------------------------------------------------------------------------*/
EdsError EDSAPI EdsGetAttribute(
    EdsDirectoryItemRef inDirItemRef, EdsFileAttributes* outFileAttribute)
{
    return EDS_ERR_UNIMPLEMENTED;
}

/*-----------------------------------------------------------------------------
//
//  Function:   EdsSetAttribute
//
//  Description:
//      Changes attributes of files on a camera.
//
//  Parameters:
//       In:    inDirItemRef - The reference of the directory item.
//              inFileAttribute  - Indicates the file attributes.
//                      As for the file attributes, OR values of the value
//                      defined by enum EdsFileAttributes can be retrieved.
//      Out:    None
//
//  Returns:    Any of the sdk errors.
-----------------------------------------------------------------------------*/
EdsError EDSAPI EdsSetAttribute(EdsDirectoryItemRef inDirItemRef, EdsFileAttributes inFileAttribute)
{
    return EDS_ERR_UNIMPLEMENTED;
}

/******************************************************************************
*******************************************************************************
//
//  Stream operating functions
//
*******************************************************************************
******************************************************************************/

/*-----------------------------------------------------------------------------
//
//  Function:   EdsCreateFileStream
//
//  Description:
//      Creates a new file on a host computer (or opens an existing file)
//          and creates a file stream for access to the file.
//      If a new file is designated before executing this API,
//          the file is actually created following the timing of writing
//          by means of EdsWrite or the like with respect to an open stream.
//
//  Parameters:
//       In:    inFileName - Pointer to a null-terminated string that specifies
//                           the file name.
//              inCreateDisposition - Action to take on files that exist,
//                                and which action to take when files do not exist.
//              inDesiredAccess - Access to the stream (reading, writing, or both).
//      Out:    outStream - The reference of the stream.
//
//  Returns:    Any of the sdk errors.
-----------------------------------------------------------------------------*/
EdsError EDSAPI EdsCreateFileStream(const EdsChar* inFileName,
    EdsFileCreateDisposition inCreateDisposition, EdsAccess inDesiredAccess,
    EdsStreamRef* outStream)
{
    return EDS_ERR_UNIMPLEMENTED;
}

/*-----------------------------------------------------------------------------
//
//  Function:   EdsCreateMemoryStream
//
//  Description:
//      Creates a stream in the memory of a host computer.
//      In the case of writing in excess of the allocated buffer size,
//          the memory is automatically extended.
//
//  Parameters:
//       In:    inBufferSize - The number of bytes of the memory to allocate.
//      Out:    outStream - The reference of the stream.
//
//  Returns:    Any of the sdk errors.
-----------------------------------------------------------------------------*/
EdsError EDSAPI EdsCreateMemoryStream(EdsUInt64 inBufferSize, EdsStreamRef* outStream)
{
    return EDS_ERR_UNIMPLEMENTED;
}

/*-----------------------------------------------------------------------------
//
//  Function:   EdsCreateStreamEx
//
//  Description:
//      An extended version of EdsCreateStreamFromFile.
//      Use this function when working with Unicode file names.
//
//  Parameters:
//       In:    inURL (for Macintosh) - Designate CFURLRef.
//              inFileName (for Windows) - Designate the file name.
//              inCreateDisposition - Action to take on files that exist,
//                                and which action to take when files do not exist.
//              inDesiredAccess - Access to the stream (reading, writing, or both).
//
//      Out:    outStream - The reference of the stream.
//
//  Returns:    Any of the sdk errors.
-----------------------------------------------------------------------------*/
EdsError EDSAPI EdsCreateFileStreamEx(
#if defined __MACOS__ || TARGET_OS_IPHONE
    const CFURLRef inURL,
#elif defined TARGET_MOBILE
    const char* inFileName,
#else
    const WCHAR* inFileName,
#endif
    EdsFileCreateDisposition inCreateDisposition, EdsAccess inDesiredAccess,
    EdsStreamRef* outStream)
{
    return EDS_ERR_UNIMPLEMENTED;
}

/*-----------------------------------------------------------------------------
//
//  Function:   EdsCreateMemoryStreamFromPointer
//
//  Description:
//      Creates a stream from the memory buffer you prepare.
//      Unlike the buffer size of streams created by means of EdsCreateMemoryStream,
//      the buffer size you prepare for streams created this way does not expand.
//
//  Parameters:
//       In:    inUserBuffer - Pointer to the buffer you have prepared.
//                    Streams created by means of this API lead to this buffer.
//              inBufferSize - The number of bytes of the memory to allocate.
//      Out:    outStream - The reference of the stream.
//
//  Returns:    Any of the sdk errors.
-----------------------------------------------------------------------------*/
EdsError EDSAPI EdsCreateMemoryStreamFromPointer(
    EdsVoid* inUserBuffer, EdsUInt64 inBufferSize, EdsStreamRef* outStream)
{
    return EDS_ERR_UNIMPLEMENTED;
}

/*-----------------------------------------------------------------------------
//
//  Function:   EdsGetPointer
//
//  Description:
//      Gets the pointer to the start address of memory managed by the memory stream.
//      As the EDSDK automatically resizes the buffer, the memory stream provides
//          you with the same access methods as for the file stream.
//      If access is attempted that is excessive with regard to the buffer size
//          for the stream, data before the required buffer size is allocated
//          is copied internally, and new writing occurs.
//      Thus, the buffer pointer might be switched on an unknown timing.
//      Caution in use is therefore advised.
//
//  Parameters:
//       In:    inStream - Designate the memory stream for the pointer to retrieve.
//      Out:    outPointer - If successful, returns the pointer to the buffer
//                  written in the memory stream.
//
//  Returns:    Any of the sdk errors.
-----------------------------------------------------------------------------*/
EdsError EDSAPI EdsGetPointer(EdsStreamRef inStream, EdsVoid** outPointer)
{
    return EDS_ERR_UNIMPLEMENTED;
}

/*-----------------------------------------------------------------------------
//
//  Function:   EdsRead
//
//  Description:
//      Reads data the size of inReadSize into the outBuffer buffer,
//          starting at the current read or write position of the stream.
//      The size of data actually read can be designated in outReadSize.
//
//  Parameters:
//       In:    inStreamRef - The reference of the stream or image.
//              inReadSize -  The number of bytes to read.
//      Out:    outBuffer - Pointer to the user-supplied buffer that is to receive
//                          the data read from the stream.
//              outReadSize - The actually read number of bytes.
//
//  Returns:    Any of the sdk errors.
-----------------------------------------------------------------------------*/
EdsError EDSAPI EdsRead(
    EdsStreamRef inStreamRef, EdsUInt64 inReadSize, EdsVoid* outBuffer, EdsUInt64* outReadSize)
{
    return EDS_ERR_UNIMPLEMENTED;
}

/*-----------------------------------------------------------------------------
//
//  Function:   EdsWrite
//
//  Description:
//      Writes data of a designated buffer
//          to the current read or write position of the stream.
//
//  Parameters:
//       In:    inStreamRef  - The reference of the stream or image.
//              inWriteSize - The number of bytes to write.
//              inBuffer - A pointer to the user-supplied buffer that contains
//                         the data to be written to the stream.
//      Out:    outWrittenSize - The actually written-in number of bytes.
//
//  Returns:    Any of the sdk errors.
-----------------------------------------------------------------------------*/
EdsError EDSAPI EdsWrite(EdsStreamRef inStreamRef, EdsUInt64 inWriteSize, const EdsVoid* inBuffer,
    EdsUInt64* outWrittenSize)
{
    return EDS_ERR_UNIMPLEMENTED;
}

/*-----------------------------------------------------------------------------
//
//  Function:   EdsSeek
//
//  Description:
//      Moves the read or write position of the stream
            (that is, the file position indicator).
//
//  Parameters:
//       In:    inStreamRef  - The reference of the stream or image.
//              inSeekOffset - Number of bytes to move the pointer.
//              inSeekOrigin - Pointer movement mode. Must be one of the following
//                             values.
//                  kEdsSeek_Cur     Move the stream pointer inSeekOffset bytes
//                                   from the current position in the stream.
//                  kEdsSeek_Begin   Move the stream pointer inSeekOffset bytes
//                                   forward from the beginning of the stream.
//                  kEdsSeek_End     Move the stream pointer inSeekOffset bytes
//                                   from the end of the stream.
//
//  Returns:    Any of the sdk errors.
-----------------------------------------------------------------------------*/
EdsError EDSAPI EdsSeek(EdsStreamRef inStreamRef, EdsInt64 inSeekOffset, EdsSeekOrigin inSeekOrigin)
{
    return EDS_ERR_UNIMPLEMENTED;
}

/*-----------------------------------------------------------------------------
//
//  Function:   EdsGetPosition
//
//  Description:
//       Gets the current read or write position of the stream
//          (that is, the file position indicator).
//
//  Parameters:
//       In:    inStreamRef - The reference of the stream or image.
//      Out:    outPosition - The current stream pointer.
//
//  Returns:    Any of the sdk errors.
-----------------------------------------------------------------------------*/
EdsError EDSAPI EdsGetPosition(EdsStreamRef inStreamRef, EdsUInt64* outPosition)
{
    return EDS_ERR_UNIMPLEMENTED;
}

/*-----------------------------------------------------------------------------
//
//  Function:   EdsGetLength
//
//  Description:
//      Gets the stream size.
//
//  Parameters:
//       In:    inStreamRef - The reference of the stream or image.
//      Out:    outLength - The length of the stream.
//
//  Returns:    Any of the sdk errors.
-----------------------------------------------------------------------------*/
EdsError EDSAPI EdsGetLength(EdsStreamRef inStreamRef, EdsUInt64* outLength)
{
    return EDS_ERR_UNIMPLEMENTED;
}

/*-----------------------------------------------------------------------------
//
//  Function:   EdsCopyData
//
//  Description:
//      Copies data from the copy source stream to the copy destination stream.
//      The read or write position of the data to copy is determined from
//          the current file read or write position of the respective stream.
//      After this API is executed, the read or write positions of the copy source
//          and copy destination streams are moved an amount corresponding to
//          inWriteSize in the positive direction.
//
//  Parameters:
//       In:    inStreamRef - The reference of the stream or image.
//              inWriteSize - The number of bytes to copy.
//      Out:    outStreamRef - The reference of the stream or image.
//
//  Returns:    Any of the sdk errors.
-----------------------------------------------------------------------------*/
EdsError EDSAPI EdsCopyData(
    EdsStreamRef inStreamRef, EdsUInt64 inWriteSize, EdsStreamRef outStreamRef)
{
    return EDS_ERR_UNIMPLEMENTED;
}

/*-----------------------------------------------------------------------------
//
//  Function:   EdsSetProgressCallback
//
//  Description:
//      Register a progress callback function.
//      An event is received as notification of progress during processing that
//          takes a relatively long time, such as downloading files from a
//          remote camera.
//      If you register the callback function, the EDSDK calls the callback
//          function during execution or on completion of the following APIs.
//      This timing can be used in updating on-screen progress bars, for example.
//
//  Parameters:
//       In:    inRef - The reference of the stream or image.
//              inProgressCallback - Pointer to a progress callback function.
//              inProgressOption - The option about progress is specified.
//                              Must be one of the following values.
//                         kEdsProgressOption_Done
//                             When processing is completed,a callback function
//                             is called only at once.
//                         kEdsProgressOption_Periodically
//                             A callback function is performed periodically.
//              inContext - Application information, passed in the argument
//                      when the callback function is called. Any information
//                      required for your program may be added.
//      Out:    None
//
//  Returns:    Any of the sdk errors.
-----------------------------------------------------------------------------*/
EdsError EDSAPI EdsSetProgressCallback(EdsBaseRef inRef, EdsProgressCallback inProgressCallback,
    EdsProgressOption inProgressOption, EdsVoid* inContext)
{
    return EDS_ERR_UNIMPLEMENTED;
}

/******************************************************************************
*******************************************************************************
//
//  Image operating functions
//
*******************************************************************************
******************************************************************************/

/*-----------------------------------------------------------------------------
//
//  Function:   EdsCreateImageRef
//
//  Description:
//      Creates an image object from an image file.
//      Without modification, stream objects cannot be worked with as images.
//      Thus, when extracting images from image files,
//          you must use this API to create image objects.
//      The image object created this way can be used to get image information
//          (such as the height and width, number of color components, and
//           resolution), thumbnail image data, and the image data itself.
//
//  Parameters:
//       In:    inStreamRef - The reference of the stream.
//
//       Out:    outImageRef - The reference of the image.
//
//  Returns:    Any of the sdk errors.
-----------------------------------------------------------------------------*/
EdsError EDSAPI EdsCreateImageRef(EdsStreamRef inStreamRef, EdsImageRef* outImageRef)
{
    return EDS_ERR_UNIMPLEMENTED;
}

/*-----------------------------------------------------------------------------
//
//  Function:   EdsGetImageInfo
//
//  Description:
//      Gets image information from a designated image object.
//      Here, image information means the image width and height,
//          number of color components, resolution, and effective image area.
//
//  Parameters:
//       In:    inStreamRef - Designate the object for which to get image information.
//              inImageSource - Of the various image data items in the image file,
//                  designate the type of image data representing the
//                  information you want to get. Designate the image as
//                  defined in Enum EdsImageSource.
//
//                      kEdsImageSrc_FullView
//                                  The image itself (a full-sized image)
//                      kEdsImageSrc_Thumbnail
//                                  A thumbnail image
//                      kEdsImageSrc_Preview
//                                  A preview image
//       Out:    outImageInfo - Stores the image data information designated
//                      in inImageSource.
//
//  Returns:    Any of the sdk errors.
-----------------------------------------------------------------------------*/
EdsError EDSAPI EdsGetImageInfo(
    EdsImageRef inImageRef, EdsImageSource inImageSource, EdsImageInfo* outImageInfo)
{
    return EDS_ERR_UNIMPLEMENTED;
}

/*-----------------------------------------------------------------------------
//
//  Function:   EdsGetImage
//
//  Description:
//      Gets designated image data from an image file, in the form of a
//          designated rectangle.
//      Returns uncompressed results for JPEGs and processed results
//          in the designated pixel order (RGB, Top-down BGR, and so on) for
//           RAW images.
//      Additionally, by designating the input/output rectangle,
//          it is possible to get reduced, enlarged, or partial images.
//      However, because images corresponding to the designated output rectangle
//          are always returned by the SDK, the SDK does not take the aspect
//          ratio into account.
//      To maintain the aspect ratio, you must keep the aspect ratio in mind
//          when designating the rectangle.
//
//  Parameters:
//      In:
//              inImageRef - Designate the image object for which to get
//                      the image data.
//              inImageSource - Designate the type of image data to get from
//                      the image file (thumbnail, preview, and so on).
//                      Designate values as defined in Enum EdsImageSource.
//              inImageType - Designate the output image type. Because
//                      the output format of EdGetImage may only be RGB, only
//                      kEdsTargetImageType_RGB or kEdsTargetImageType_RGB16
//                      can be designated.
//                      However, image types exceeding the resolution of
//                      inImageSource cannot be designated.
//              inSrcRect - Designate the coordinates and size of the rectangle
//                      to be retrieved (processed) from the source image.
//              inDstSize - Designate the rectangle size for output.
//
//      Out:
//              outStreamRef - Designate the memory or file stream for output of
//                      the image.
//  Returns:    Any of the sdk errors.
-----------------------------------------------------------------------------*/
EdsError EDSAPI EdsGetImage(EdsImageRef inImageRef, EdsImageSource inImageSource,
    EdsTargetImageType inImageType, EdsRect inSrcRect, EdsSize inDstSize, EdsStreamRef outStreamRef)
{
    return EDS_ERR_UNIMPLEMENTED;
}

/*-----------------------------------------------------------------------------
//
//  Function:   EdsCreateEvfImageRef
//  Description:
//       Creates an object used to get the live view image data set.
//
//  Parameters:
//      In:     inStreamRef - The stream reference which opened to get EVF JPEG image.
//      Out:    outEvfImageRef - The EVFData reference.
//
//  Returns:    Any of the sdk errors.
-----------------------------------------------------------------------------*/
EdsError EDSAPI EdsCreateEvfImageRef(EdsStreamRef inStreamRef, EdsEvfImageRef* outEvfImageRef)
{
    return EDS_ERR_UNIMPLEMENTED;
}

/*-----------------------------------------------------------------------------
//
//  Function:   EdsDownloadEvfImage
//  Description:
//		Downloads the live view image data set for a camera currently in live view mode.
//		Live view can be started by using the property ID:kEdsPropertyID_Evf_OutputDevice
and
//		data:EdsOutputDevice_PC to call EdsSetPropertyData.
//		In addition to image data, information such as zoom, focus position, and histogram
data
//		is included in the image data set. Image data is saved in a stream maintained by
EdsEvfImageRef.
//		EdsGetPropertyData can be used to get information such as the zoom, focus position,
etc.
//		Although the information of the zoom and focus position can be obtained from
EdsEvfImageRef,
//		settings are applied to EdsCameraRef.
//
//  Parameters:
//      In:     inCameraRef - The Camera reference.
//      In:     inEvfImageRef - The EVFData reference.
//
//  Returns:    Any of the sdk errors.
-----------------------------------------------------------------------------*/
EdsError EDSAPI EdsDownloadEvfImage(EdsCameraRef inCameraRef, EdsEvfImageRef inEvfImageRef)
{
    return EDS_ERR_UNIMPLEMENTED;
}

/******************************************************************************
*******************************************************************************
//
//  Event handler registering functions
//
*******************************************************************************
******************************************************************************/

/*-----------------------------------------------------------------------------
//
//  Function:   EdsSetCameraAddedHandler
//
//  Description:
//      Registers a callback function for when a camera is detected.
//
//  Parameters:
//       In:    inCameraAddedHandler - Pointer to a callback function
//                          called when a camera is connected physically
//              inContext - Specifies an application-defined value to be sent to
//                          the callback function pointed to by CallBack parameter.
//      Out:    None
//
//  Returns:    Any of the sdk errors.
-----------------------------------------------------------------------------*/
EdsError EDSAPI EdsSetCameraAddedHandler(
    EdsCameraAddedHandler inCameraAddedHandler, EdsVoid* inContext)
{
    return EDS_ERR_UNIMPLEMENTED;
}

/*-----------------------------------------------------------------------------
//
//  Function:   EdsSetPropertyEventHandler
//
//  Description:
//       Registers a callback function for receiving status
//          change notification events for property states on a camera.
//
//  Parameters:
//       In:    inCameraRef - Designate the camera object.
//              inEvent - Designate one or all events to be supplemented.
//              inPropertyEventHandler - Designate the pointer to the callback
//                      function for receiving property-related camera events.
//              inContext - Designate application information to be passed by
//                      means of the callback function. Any data needed for
//                      your application can be passed.
//      Out:    None
//
//  Returns:    Any of the sdk errors.
-----------------------------------------------------------------------------*/
EdsError EDSAPI EdsSetPropertyEventHandler(EdsCameraRef inCameraRef, EdsPropertyEvent inEvnet,
    EdsPropertyEventHandler inPropertyEventHandler, EdsVoid* inContext)
{
    return EDS_ERR_UNIMPLEMENTED;
}

/*-----------------------------------------------------------------------------
//
//  Function:   EdsSetObjectEventHandler
//
//  Description:
//       Registers a callback function for receiving status
//          change notification events for objects on a remote camera.
//      Here, object means volumes representing memory cards, files and directories,
//          and shot images stored in memory, in particular.
//
//  Parameters:
//       In:    inCameraRef - Designate the camera object.
//              inEvent - Designate one or all events to be supplemented.
//                  To designate all events, use kEdsObjectEvent_All.
//              inObjectEventHandler - Designate the pointer to the callback function
//                  for receiving object-related camera events.
//              inContext - Passes inContext without modification,
//                  as designated as an EdsSetObjectEventHandler argument.
//      Out:    None
//
//  Returns:    Any of the sdk errors.
-----------------------------------------------------------------------------*/
EdsError EDSAPI EdsSetObjectEventHandler(EdsCameraRef inCameraRef, EdsObjectEvent inEvnet,
    EdsObjectEventHandler inObjectEventHandler, EdsVoid* inContext)
{
    return EDS_ERR_UNIMPLEMENTED;
}

/*-----------------------------------------------------------------------------
//
//  Function:  EdsSetCameraStateEventHandler
//
//  Description:
//      Registers a callback function for receiving status
//          change notification events for property states on a camera.
//
//  Parameters:
//       In:    inCameraRef - Designate the camera object.
//              inEvent - Designate one or all events to be supplemented.
//                  To designate all events, use kEdsStateEvent_All.
//              inStateEventHandler - Designate the pointer to the callback function
//                  for receiving events related to camera object states.
//              inContext - Designate application information to be passed
//                  by means of the callback function. Any data needed for
//                  your application can be passed.
//      Out:    None
//
//  Returns:    Any of the sdk errors.
-----------------------------------------------------------------------------*/
EdsError EDSAPI EdsSetCameraStateEventHandler(EdsCameraRef inCameraRef, EdsStateEvent inEvnet,
    EdsStateEventHandler inStateEventHandler, EdsVoid* inContext)
{
    return EDS_ERR_UNIMPLEMENTED;
}

/*----------------------------------------------------------------------------*/
EdsError EDSAPI EdsCreateStream(EdsIStream* inStream, EdsStreamRef* outStreamRef);

/*-----------------------------------------------------------------------------
//
//  Function:   EdsGetEvent
//
//  Description:
//      This function acquires an event.
//      In console application, please call this function regularly to acquire
//      the event from a camera.
//
//  Parameters:
//       In:    None
//      Out:    None
//
//  Returns:    Any of the sdk errors.
-----------------------------------------------------------------------------*/
EdsError EDSAPI EdsGetEvent() { return EDS_ERR_UNIMPLEMENTED; }
