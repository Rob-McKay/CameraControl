# CameraControl
Canon Camera Controls (command line)

This code uses the Canon EOS Digital SDK (EDSDK) version 13.13.0 as a git submodule. This git submodule is the EOS SDK from Canon (Europe) put into git.

# MacOS builds
This project is has not (yet) been setup to build for Windows. It currently only builds for macOS Catalina (10.15) and macOS Big Sur (11.0) for Intel 64bit processors - forced when compiled on Apple Silicon. It does support Apple silicon but has to use Rosetta 2 because the Canon EDSDK currently only supports Intel 64bit applications.