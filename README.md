KomodoDrop
==========

Komodo Drop is a small, light weight application for Windows, macOS and Linux
that makes it easy and fast to upload images to SmugMug. It also allows basic
management of categories, sub categories and albums. Komodo Drop is free and
open source. If you find it useful please let me know.

Drop files or folders onto the window — or use the Upload menu — and Komodo Drop
asks which album to put them in, creating it first if you want.

Features
========

* Small and light weight
* Multi platform: Windows, macOS & Linux
* Fast multi-threaded uploads
* Resumeable uploads
* Duplicate image detection
* Create categories and sub categories
* Delete albums, categories and sub categories

Categories and sub categories are the top two levels of your SmugMug folder
tree — SmugMug dropped the separate category concept when it moved to API v2,
but the names stuck.

First run
=========

Komodo Drop talks to SmugMug over OAuth, so the first launch opens your browser
to authorize the app against your SmugMug account. SmugMug then shows a short
code; paste it into the dialog Komodo Drop is waiting on. The access token is
saved, so this only happens once.

Downloads
=========

Installers for all three platforms are attached to each release:

https://github.com/FigBug/komododrop/releases/latest

* Windows — signed installer (.exe)
* macOS — signed and notarized installer (.pkg)
* Linux — tarball (.tar.gz)

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
