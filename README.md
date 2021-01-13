# CameraControl

Canon Camera Controls command line tool

## Introduction

This tool was created so that I didn't have to keep booting an old version of macOS (10.11) to download images from my Canon EOS 50D camera. I had to use this version as this is the last version which would run the Canon supplied tools which are able to retrieve images etc from my EOS 50D.

Interestingly the latest version of the Canon EOS SDK (version 13.13) on the latest version of macOS (11.1) will talk to my EOS 50D without any special setup (apart from using Rosetta 2 - see below).

This code uses the Canon EOS Digital SDK (EDSDK) version 13.13.0 as a git submodule. This git submodule is the EOS SDK from Canon (Europe) put into git.

## MacOS builds

This project currently only builds for macOS Catalina (10.15) and macOS Big Sur (11.0) for Intel 64bit processors - forced when compiled on Apple Silicon. It does support Apple silicon but has to use Rosetta 2 because the Canon EDSDK currently only supports Intel 64bit applications.

## Windows builds

This project is has not (yet) been setup to build for Windows.


## Usage

This code produces a command line tool `lscamera`.

`lscamera` without any parameters produces a list of all the (Canon) cameras connected to the computer and their information.

`lscamera --help` displays version and parameter information

`lscamera -c0 -f` displays files for camera 0.
