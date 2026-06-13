#!/bin/bash

# Exit immediately if any command exits with a non-zero status
set -e

# Ensure relative paths resolve relative to the script directory
cd "$(dirname "$0")"

# Parse arguments
BUILD_CONFIG="Release"

for arg in "$@"; do
    case $arg in
        --debug|--build-debug)
            BUILD_CONFIG="Debug"
            ;;
        *)
            # Ignore unknown options or handle differently
            ;;
    esac
done

# Clean up old build outputs to guarantee a clean build
echo "Cleaning old build artifacts..."
xcodebuild -shutdown >/dev/null 2>&1 || true

# Robustly delete old build directories by renaming first (bypasses locked handles)
if [ -d .build ]; then
    OLD_BUILD=".build_old_$(date +%s)"
    mv .build "$OLD_BUILD"
    rm -Rf "$OLD_BUILD" || true
fi
rm -Rf .build_old_* || true
rm -Rf .bin || true
rm -Rf /tmp/TS-M1N3-build || true

# Configure standard Ad-hoc signing for macOS targets
echo "Configuring project with Ad-hoc signing..."
declare -a SIGN_FLAGS=(
    -DCMAKE_XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY="-"
    -DCMAKE_XCODE_ATTRIBUTE_CODE_SIGN_STYLE="Automatic"
)


# Generate the Xcode project
echo "Generating Xcode project using CMake (Config: $BUILD_CONFIG)..."
cmake -B.build -GXcode \
    -D"CMAKE_OSX_ARCHITECTURES=arm64" \
    -DCMAKE_XCODE_ATTRIBUTE_OBJROOT=/tmp/TS-M1N3-build/obj \
    "${SIGN_FLAGS[@]}"

echo "=========================================================="
echo "Xcode project generation completed successfully!"
echo "Xcode project: .build/ramer-ts-m1n3.xcodeproj"
echo "=========================================================="

echo "Building the product configuration: $BUILD_CONFIG..."
cmake --build .build --config "$BUILD_CONFIG"

# Create Products folder inside build directory
PRODUCTS_DIR=".build/Products"
echo "Creating Products folder at $PRODUCTS_DIR..."
mkdir -p "$PRODUCTS_DIR"

# Copy build artifacts to Products folder
# JUCE places build artifacts in .build/ramer-ts-m1n3_artefacts/<Config>
ARTFACTS_DIR=".build/ramer-ts-m1n3_artefacts/$BUILD_CONFIG"

echo "Copying VST3 and AU plugins to $PRODUCTS_DIR..."

found_vst=false
found_au=false

if [ -d "$ARTFACTS_DIR/VST3/Ramer-TS-M1N3.vst3" ]; then
    cp -R "$ARTFACTS_DIR/VST3/Ramer-TS-M1N3.vst3" "$PRODUCTS_DIR/"
    echo "Successfully copied VST3: Ramer-TS-M1N3.vst3"
    found_vst=true
fi

if [ -d "$ARTFACTS_DIR/AU/Ramer-TS-M1N3.component" ]; then
    cp -R "$ARTFACTS_DIR/AU/Ramer-TS-M1N3.component" "$PRODUCTS_DIR/"
    echo "Successfully copied AU: Ramer-TS-M1N3.component"
    found_au=true
fi

if [ "$found_vst" = false ]; then
    echo "Searching recursively for VST3 plugin..."
    find .build/ramer-ts-m1n3_artefacts -type d -name "Ramer-TS-M1N3.vst3" -exec cp -R {} "$PRODUCTS_DIR/" \;
fi

if [ "$found_au" = false ]; then
    echo "Searching recursively for AU plugin..."
    find .build/ramer-ts-m1n3_artefacts -type d -name "Ramer-TS-M1N3.component" -exec cp -R {} "$PRODUCTS_DIR/" \;
fi

USER_COMPONENTS_DIR="$HOME/Library/Audio/Plug-Ins/Components"
if [ -d "$PRODUCTS_DIR/Ramer-TS-M1N3.component" ]; then
    echo "Automatically installing AudioUnit component to $USER_COMPONENTS_DIR..."
    mkdir -p "$USER_COMPONENTS_DIR"
    rm -Rf "$USER_COMPONENTS_DIR/Ramer-TS-M1N3.component"
    cp -R "$PRODUCTS_DIR/Ramer-TS-M1N3.component" "$USER_COMPONENTS_DIR/"
    echo "Successfully installed component at: $USER_COMPONENTS_DIR/Ramer-TS-M1N3.component"
    echo "Restarting AudioComponentRegistrar to force macOS to rescan the new plugin..."
    killall -9 AudioComponentRegistrar || true
fi

echo "=========================================================="
echo "Build and copying to Products completed successfully!"
echo "Products path: $PRODUCTS_DIR"
ls -la "$PRODUCTS_DIR"
echo "=========================================================="

