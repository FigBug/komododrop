KomodoDrop
==========

Komodo Drop is small, light weight application for Windows, Mac OS X and Linux that makes it easy and fast to upload images to SmugMug. It also allows basic management of categories, sub categories and albums. Komodo Drop is free and open source. If you find it useful please let me know.

Features
Komodo Drop is still beta software; however, it is stable and full featured. It is ready to use today.
 
* Some key features include:
* Small and light weight
* Multi platform: Windows, Mac OS X & Linux
* Fast multi-threaded uploads
* Resumeable uploads
* Duplicate image detection
* Create categories and sub categories
* Delete albums, categories and sub categories

Downloads
=========

Version 0.0.0.5:  
Windows: http://downloads.rabien.com/KomodoDrop.exe  
Mac OS X: http://downloads.rabien.com/KomodoDrop.zip

Building
========

KomodoDrop builds with CMake. JUCE and Gin are git submodules, so clone recursively:

```
git clone --recurse-submodules https://github.com/FigBug/KomodoDrop.git
cd KomodoDrop
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

If you already have the repo, pull the submodules with
`git submodule update --init --recursive`.

The app is written to `build/KomodoDrop_artefacts/Release/`.

Requirements: a C++20 compiler and CMake 3.24+.
On Linux, install the JUCE dependencies listed in
`modules/JUCE/docs/Linux Dependencies.md`.

IDE builds use the presets shared with the other Rabien Software projects (they
come from Gin, so they match everywhere):

```
cmake --preset xcode        # macOS, writes Builds/xcode
cmake --preset vs           # Windows, writes Builds/vs
cmake --preset ninja-gcc    # Linux, writes Builds/ninja-gcc
cmake --build --preset xcode --config Release
```

Installers
==========

`./Installer/build.sh` configures, builds and packages for whichever platform it
is run on, leaving the result in `bin/`:

* macOS — a signed, notarized `.pkg` that installs to `/Applications`
* Windows — a signed Inno Setup `.exe`
* Linux — a `.tar.gz` of the binary

Signing and notarization are driven by environment variables and are skipped with
a message (not an error) when they aren't set, so the script also works unsigned
on a dev machine. The CI secrets it looks for are `APPLICATION`, `INSTALLER`,
`APPLE_USER` and `APPLE_PASS` on macOS, and `AZURE_TENANT_ID`, `AZURE_CLIENT_ID`
and `AZURE_CLIENT_SECRET` for Windows Trusted Signing.

Releasing
=========

`VERSION` is the single source of truth for the version number — CMake and the
installer scripts both read it. To cut a release, bump `VERSION`, add a matching
`## <version>` section to `Changelist.txt`, commit, then run `./tag.sh`. Pushing
the tag runs `.github/workflows/release.yaml`, which builds all three platforms
and creates a GitHub release using that changelog section. Every branch push
builds via `.github/workflows/build.yaml`.
