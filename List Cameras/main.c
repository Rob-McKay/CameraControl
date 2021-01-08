//
//  main.c
//  List Cameras
//
//  Created by Rob McKay on 08/01/2021.
//

#if !defined __MACOS__
#if defined __APPLE__ && defined __MACH__
#define __MACOS__ 1
#else
#error "Only for MacOS"
#endif
#endif

#include <stdio.h>
#include "EDSDK.h"
#include <stdbool.h>

// Private functions

static EdsError EDSCALLBACK handleObjectEvent( EdsObjectEvent event, EdsBaseRef object, EdsVoid * context)
{
    // do something
    /*
     switch(event)
     {
     case kEdsObjectEvent_DirItemRequestTransfer:
     downloadImage(object);
     break;
     default:
     break;
     }
     */
    
    // Object must be released
    if (object)
    {
        EdsRelease(object);
    }
    
    return EDS_ERR_OK;
}


static EdsError EDSCALLBACK handlePropertyEvent (EdsPropertyEvent event, EdsPropertyID property, EdsUInt32 param, EdsVoid * context)
{
    // do something
    
    return EDS_ERR_OK;
}


static EdsError EDSCALLBACK handleStateEvent (EdsStateEvent event, EdsUInt32 parameter, EdsVoid * context)
{
    // do something
    
    return EDS_ERR_OK;
}

static EdsError getCameraCount()
{
    EdsError err = EDS_ERR_OK;
    EdsCameraListRef cameraList = NULL;
    EdsUInt32 count = 0;
       
    // Get camera list
    err = EdsGetCameraList(&cameraList);
    
    // Get number of cameras
    if (err == EDS_ERR_OK)
    {
        err = EdsGetChildCount(cameraList, &count);
    }
    
    // Release camera list
    if (cameraList != NULL)
    {
        EdsRelease(cameraList);
        cameraList = NULL;
    }

    return count;
}

static EdsError getCamera(const int cameraNumber, EdsCameraRef *camera)
{
    EdsError err = EDS_ERR_OK;
    EdsCameraListRef cameraList = NULL;
    EdsUInt32 count = 0;
    *camera = NULL;
    
    // Get camera list
    err = EdsGetCameraList(&cameraList);
    
    // Get number of cameras
    if (err == EDS_ERR_OK)
    {
        err = EdsGetChildCount(cameraList, &count);
        if (count <= cameraNumber)
        {
            printf("Camera %d found\n", cameraNumber);
            err = EDS_ERR_DEVICE_NOT_FOUND;
        }
    }
    
    // Get the camera retrieved
    if (err == EDS_ERR_OK)
    {
        err = EdsGetChildAtIndex(cameraList, cameraNumber , camera);
    }
    
    // Release camera list
    if (cameraList != NULL)
    {
        EdsRelease(cameraList);
        cameraList = NULL;
    }
    
    return err;
}

EdsError getCameraInfo(EdsCameraRef camera, char* name)
{
    EdsError err = EDS_ERR_OK;
    EdsDataType dataType;
    EdsUInt32 dataSize;
    
    *name = 0;
    
    err = EdsGetPropertySize(camera, kEdsPropID_ProductName, 0 , &dataType, &dataSize);
    if(err == EDS_ERR_OK)
    {
        err = EdsGetPropertyData(camera, kEdsPropID_ProductName, 0 , dataSize, name);
    }
    return err;
}

int main(int argc, const char * argv[])
{
    EdsError err = EDS_ERR_OK;
    EdsCameraRef camera = NULL;
    bool isSDKLoaded = false;
    
    // Initialize SDK
    err = EdsInitializeSDK();
    if(err == EDS_ERR_OK)
    {
        isSDKLoaded = true;
    }
    
    if(err == EDS_ERR_OK)
    {
        int count =  getCameraCount();
        for (int cameraNumber = 0; cameraNumber < count; cameraNumber++)
        {
            EdsDeviceInfo info;
            EdsCameraRef camera;
            
            EdsError e = getCamera(cameraNumber, &camera);
            if (e == EDS_ERR_OK)
            {
                e = EdsGetDeviceInfo(camera, &info);
                if (e == EDS_ERR_OK)
                {
                    printf("Camera at port %s, device %s\n", info.szPortName, info.szDeviceDescription);
                }
                EdsRelease(camera);
            }
        }
    }
    
//    // Get first camera
//    if(err == EDS_ERR_OK)
//    {
//        printf("Getting first camera\n");
//        err = getCamera(0, &camera);
//    }
//
//    if(err == EDS_ERR_OK)
//    {
//        
//    }
//    
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
//    // Close session with camera
//    if(err == EDS_ERR_OK)
//    {
//        printf("Closing session\n");
//        err = EdsCloseSession(camera);
//    }
//    
//    // Release camera
//    if(camera != NULL)
//    {
//        printf("Releasing camera\n");
//        EdsRelease(camera);
//    }
    
    // Terminate SDK
    if(isSDKLoaded)
    {
        EdsTerminateSDK();
    }

    return err;
}
