#!/bin/bash -e
set -x

#
# Builds KomodoDrop and packages it for the host platform, dropping the result in
# bin/. Signing and notarization are driven entirely by environment variables and
# are skipped (with a message, not an error) when they aren't set, so this runs
# unchanged on a dev machine.
#
#   macOS    APPLICATION, INSTALLER  base64 .p12 for Developer ID App/Installer
#            APPLE_USER, APPLE_PASS  Apple ID + app-specific password (notarize)
#   Windows  AZURE_TENANT_ID, AZURE_CLIENT_ID, AZURE_CLIENT_SECRET
#                                    Microsoft Trusted Signing credentials
#

cd "$(dirname "$0")"
cd ..
PROJECT_ROOT=$(pwd)
APP_NAME="KomodoDrop"
BUNDLE_BASE="com.rabien.KomodoDrop"

VERSION=$(cat "$PROJECT_ROOT/VERSION")

rm -Rf "$PROJECT_ROOT/bin"
mkdir -p "$PROJECT_ROOT/bin"

if [ "$(uname)" = "Darwin" ]; then
  PLATFORM="macOS"
elif [ "$(expr substr "$(uname -s)" 1 5)" = "Linux" ]; then
  PLATFORM="linux"
else
  PLATFORM="win"
fi

############################################################
# macOS — pkgbuild + productbuild, signed and notarized
############################################################
if [ "$PLATFORM" = "macOS" ]; then
  TEAM_ID="${TEAM_ID:-3FS7DJDG38}"
  DEV_APP_ID="${DEV_APP_ID:-Developer ID Application: Roland Rabien (${TEAM_ID})}"
  DEV_INST_ID="${DEV_INST_ID:-Developer ID Installer: Roland Rabien (${TEAM_ID})}"

  #
  # In CI the signing identities arrive as base64-encoded .p12 secrets, so load
  # them into a throwaway keychain first.
  #
  if [ -n "${APPLICATION:-}" ] && [ -n "${INSTALLER:-}" ]; then
    KC_PASS="${KEYCHAIN_PASSWORD:-nr4aGPyz}"
    P12_PASS="${P12_PASSWORD:-aym9PKWB}"

    security create-keychain -p "$KC_PASS" Keys.keychain || true

    echo "$APPLICATION" | base64 -D -o /tmp/Application.p12
    echo "$INSTALLER"   | base64 -D -o /tmp/Installer.p12

    security import /tmp/Application.p12 -t agg -k Keys.keychain -P "$P12_PASS" -A -T /usr/bin/codesign
    security import /tmp/Installer.p12   -t agg -k Keys.keychain -P "$P12_PASS" -A -T /usr/bin/productsign

    security list-keychains -s Keys.keychain
    security default-keychain -s Keys.keychain
    security unlock-keychain -p "$KC_PASS" Keys.keychain
    security set-keychain-settings -l -u -t 13600 Keys.keychain
    security set-key-partition-list -S apple-tool:,apple: -s -k "$KC_PASS" Keys.keychain
  fi

  cd "$PROJECT_ROOT"
  cmake --preset xcode
  cmake --build --preset xcode --config Release

  ART_DIR="$PROJECT_ROOT/Builds/xcode/${APP_NAME}_artefacts/Release"

  STAGE="$PROJECT_ROOT/Installer/macOS/bin/stage"
  PKG_DIR="$PROJECT_ROOT/Installer/macOS/bin/pkgs"
  rm -Rf "$STAGE" "$PKG_DIR"
  mkdir -p "$STAGE/app/Applications" "$PKG_DIR"

  cp -RL "$ART_DIR/$APP_NAME.app" "$STAGE/app/Applications/"
  find "$STAGE" -name ".DS_Store" -delete

  if security find-identity -v -p codesigning | grep -q "$DEV_APP_ID"; then
    codesign --force --options runtime --timestamp \
             --sign "$DEV_APP_ID" "$STAGE/app/Applications/$APP_NAME.app"
  else
    echo "Skipping codesign — Developer ID Application not in keychain"
  fi

  # pkgbuild marks .app bundles relocatable by default, which lets the installer
  # redirect us onto any other copy Spotlight happens to find. Pin it down.
  pkgbuild --analyze --root "$STAGE/app" "$PKG_DIR/app-component.plist"
  plutil -replace 0.BundleIsRelocatable -bool false "$PKG_DIR/app-component.plist"

  pkgbuild --root "$STAGE/app" \
           --component-plist "$PKG_DIR/app-component.plist" \
           --identifier "${BUNDLE_BASE}.app.pkg" \
           --version "$VERSION" \
           "$PKG_DIR/app.pkg"

  PKG_OUT="$PROJECT_ROOT/Installer/macOS/bin/${APP_NAME}.pkg"

  productbuild --distribution "$PROJECT_ROOT/Installer/macOS/distribution.xml" \
               --package-path "$PKG_DIR" \
               --version "$VERSION" \
               "$PKG_OUT.unsigned"

  if security find-identity -v | grep -q "$DEV_INST_ID"; then
    productsign --sign "$DEV_INST_ID" "$PKG_OUT.unsigned" "$PKG_OUT"
    rm "$PKG_OUT.unsigned"
  else
    echo "Skipping productsign — Developer ID Installer not in keychain"
    mv "$PKG_OUT.unsigned" "$PKG_OUT"
  fi

  if [ -n "${APPLE_USER:-}" ] && [ -n "${APPLE_PASS:-}" ]; then
    SUBMISSION_OUTPUT=$(xcrun notarytool submit --verbose \
      --apple-id "$APPLE_USER" --password "$APPLE_PASS" --team-id "$TEAM_ID" \
      --wait --timeout 30m "$PKG_OUT" 2>&1) || NOTARY_FAILED=1
    echo "$SUBMISSION_OUTPUT"

    SUBMISSION_ID=$(echo "$SUBMISSION_OUTPUT" | awk "/^  id:/ { print \$2; exit }")

    if [ "${NOTARY_FAILED:-0}" = "1" ] && [ -n "$SUBMISSION_ID" ]; then
      echo "Notarization failed — fetching log for $SUBMISSION_ID"
      xcrun notarytool log "$SUBMISSION_ID" --apple-id "$APPLE_USER" --password "$APPLE_PASS" \
        --team-id "$TEAM_ID" "$PROJECT_ROOT/bin/notarization_log.json" || true
      exit 1
    fi

    xcrun stapler staple "$PKG_OUT"
  else
    echo "Skipping notarization — APPLE_USER / APPLE_PASS not set"
  fi

  cp "$PKG_OUT" "$PROJECT_ROOT/bin/"

  # dSYM for symbolicating crash reports
  if [ -d "$ART_DIR/$APP_NAME.app.dSYM" ]; then
    ( cd "$ART_DIR" && zip -qr "$PROJECT_ROOT/bin/Symbols_Mac.zip" "$APP_NAME.app.dSYM" )
  fi

############################################################
# Linux — plain tarball, no signing
############################################################
elif [ "$PLATFORM" = "linux" ]; then
  cd "$PROJECT_ROOT"
  cmake --preset ninja-gcc
  cmake --build --preset ninja-gcc --config Release

  ART_DIR="$PROJECT_ROOT/Builds/ninja-gcc/${APP_NAME}_artefacts/Release"

  STAGE="$PROJECT_ROOT/Installer/linux/bin/$APP_NAME-$VERSION"
  rm -Rf "$PROJECT_ROOT/Installer/linux/bin"
  mkdir -p "$STAGE"

  cp "$ART_DIR/$APP_NAME" "$STAGE/"
  cp "$PROJECT_ROOT/LICENSE" "$STAGE/"
  cp "$PROJECT_ROOT/README.md" "$STAGE/"

  # Keep an unstripped copy for symbolication, ship the stripped one.
  objcopy --only-keep-debug "$STAGE/$APP_NAME" "$STAGE/$APP_NAME.debug"
  strip --strip-debug --strip-unneeded "$STAGE/$APP_NAME"
  objcopy --add-gnu-debuglink="$STAGE/$APP_NAME.debug" "$STAGE/$APP_NAME"

  ( cd "$PROJECT_ROOT/Installer/linux/bin" && zip -qr "$PROJECT_ROOT/bin/Symbols_Linux.zip" "$APP_NAME-$VERSION/$APP_NAME.debug" )
  rm "$STAGE/$APP_NAME.debug"

  ( cd "$PROJECT_ROOT/Installer/linux/bin" && tar czf "$PROJECT_ROOT/bin/$APP_NAME-$VERSION-linux-x86_64.tar.gz" "$APP_NAME-$VERSION" )

############################################################
# Windows — Inno Setup + Azure Trusted Signing
############################################################
else
  cd "$PROJECT_ROOT"
  cmake --preset vs
  cmake --build --preset vs --config Release

  ART_DIR="$PROJECT_ROOT/Builds/vs/${APP_NAME}_artefacts/Release"
  STAGE="$PROJECT_ROOT/Installer/win/bin"

  rm -Rf "$STAGE"
  mkdir -p "$STAGE/app"
  # Staged under app/ because Inno writes its own KomodoDrop.exe into $STAGE.
  cp "$ART_DIR/$APP_NAME.exe" "$STAGE/app/"

  #
  # Sign via Microsoft Trusted Signing. All three Azure vars must be set, or none
  # — a half-configured set is a mistake worth failing on rather than silently
  # shipping unsigned binaries.
  #
  uuid_re='^[0-9a-fA-F]{8}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{12}$'
  WIN_SIGN=0
  if [ -n "${AZURE_TENANT_ID:-}" ] || [ -n "${AZURE_CLIENT_ID:-}" ] || [ -n "${AZURE_CLIENT_SECRET:-}" ]; then
    if   [[ ! "${AZURE_TENANT_ID:-}" =~ $uuid_re ]]; then
      echo "ERROR: AZURE_TENANT_ID is not a valid GUID — set the secret on the repo or unset all three to skip signing."; exit 1
    elif [[ ! "${AZURE_CLIENT_ID:-}" =~ $uuid_re ]]; then
      echo "ERROR: AZURE_CLIENT_ID is not a valid GUID — set the secret on the repo or unset all three to skip signing."; exit 1
    elif [ -z "${AZURE_CLIENT_SECRET:-}" ]; then
      echo "ERROR: AZURE_CLIENT_SECRET is empty — set the secret on the repo or unset all three to skip signing."; exit 1
    else
      WIN_SIGN=1
    fi
  fi

  if [ "$WIN_SIGN" = "1" ]; then
    # The version directory under Windows Kits varies by runner image.
    SIGNTOOL=$(ls -1 "/c/Program Files (x86)/Windows Kits/10/bin/"*/x64/signtool.exe 2>/dev/null | sort | tail -1)
    if [ -z "$SIGNTOOL" ]; then
      echo "signtool.exe not found in Windows Kits"; exit 1
    fi

    TOOLS_DIR="$STAGE/_signingtools"
    mkdir -p "$TOOLS_DIR"
    nuget install Microsoft.Trusted.Signing.Client -Version 1.0.86 \
                  -OutputDirectory "$TOOLS_DIR" -ExcludeVersion -NonInteractive
    DLIB="$TOOLS_DIR/Microsoft.Trusted.Signing.Client/bin/x64/Azure.CodeSigning.Dlib.dll"
    METADATA="$PROJECT_ROOT/Installer/win/metadata.json"

    sign_file () {
      "$SIGNTOOL" sign -v -fd SHA256 \
        -tr "http://timestamp.acs.microsoft.com" -td SHA256 \
        -dlib "$DLIB" -dmdf "$METADATA" "$1"
    }

    sign_file "$STAGE/app/$APP_NAME.exe"
  else
    echo "Skipping Windows binary signing — Azure secrets not set"
  fi

  ISCC="/c/Program Files (x86)/Inno Setup 6/ISCC.exe"
  if [ ! -f "$ISCC" ]; then
    ISCC="/c/Program Files/Inno Setup 6/ISCC.exe"
  fi

  # ISCC is a native Windows binary and Git Bash rewrites arguments that look like
  # unix paths, which mangles both "/DName=Value" switches and absolute script
  # paths. Hand the version over in a generated include and run ISCC from the
  # script's own directory with a bare relative name.
  printf '#define MyAppVersion "%s"\n' "$VERSION" > "$PROJECT_ROOT/Installer/win/version.iss"

  ( cd "$PROJECT_ROOT/Installer/win" && "$ISCC" "KomodoDrop.iss" )

  EXE_OUT="$STAGE/${APP_NAME}.exe"

  # The installer lands on top of the staged app binary, so sign it after Inno runs.
  if [ "$WIN_SIGN" = "1" ]; then
    sign_file "$EXE_OUT"
  fi

  cp "$EXE_OUT" "$PROJECT_ROOT/bin/"

  if [ -f "$ART_DIR/$APP_NAME.pdb" ]; then
    SYM_DIR="$STAGE/symbols"
    mkdir -p "$SYM_DIR"
    cp "$ART_DIR/$APP_NAME.pdb" "$SYM_DIR/"
    ( cd "$SYM_DIR" && 7z a "$PROJECT_ROOT/bin/Symbols_Win.zip" "$APP_NAME.pdb" )
  fi
fi
