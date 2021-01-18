# CameraControl

Canon Camera Controls command line tool

## Introduction

This tool was created so that I didn't have to keep booting an old version of macOS (10.11) to download images from my Canon EOS 50D camera. I had to use this version as this is the last version which would run the Canon supplied tools which are able to retrieve images etc from my EOS 50D.

Interestingly the latest version of the Canon EOS SDK (version 13.13) on the latest version of macOS (11.1) will talk to my EOS 50D without any special setup (apart from using Rosetta 2 - see below).

This code uses the Canon EOS Digital SDK (EDSDK) version 13.13.0 as a git submodule. This git submodule is the EOS SDK from Canon (Europe) put into git.

## MacOS builds

This project currently only builds for macOS Catalina (10.15) and macOS Big Sur (11.0) for Intel 64bit processors - forced when compiled on Apple Silicon. It does support Apple silicon but has to use Rosetta 2 because the Canon EDSDK currently only supports Intel 64bit applications.

### Notes

The EDSDK produces an error message (see below) when an application using it starts up. It does not (seem to) stop the applications from working correctly.

```lang-none
[Release build] CHHLLite_InitializeCHHLLiteEx 
2021-01-18 15:27:40.876 cpimage[50710:6880017] get NSURLIsDirectoryKey Error : Error Domain=NSCocoaErrorDomain Code=260 "The file “ic_hevcdec.framework” couldn’t be opened because there is no such file." UserInfo={NSURL=Frameworks/ic_hevcdec.framework -- file:///Users/rob/development/camera/CameraControl/build-rel/, NSFilePath=/Users/rob/development/camera/CameraControl/build-rel/Frameworks/ic_hevcdec.framework, NSUnderlyingError=0x7fa6bec21e50 {Error Domain=NSPOSIXErrorDomain Code=2 "No such file or directory"}}
```

## Windows builds

This project is has not (yet) been setup to build for Windows.

## Usage

This code produces a command line tool `lscamera`.

`lscamera` without any parameters produces a list of all the (Canon) cameras connected to the computer and their information.

`lscamera --help` displays version and parameter information

`lscamera -c0 -f` displays files for camera 0.

## TODO

- [ ] Provide a means of converting raw images to other format(s).
- [ ] Port to windows.
- [ ] Provide downloadable executables (if anyone want one let me know)
