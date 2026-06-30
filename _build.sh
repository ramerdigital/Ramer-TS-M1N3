#!/bin/bash

# Exit immediately if any command exits with a non-zero status
set -e

# Ensure relative paths resolve relative to the script directory
cd "$(dirname "$0")"

BUILD_CONFIG="Release"

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

echo "=========================================================="
echo "Build completed successfully!"
echo "=========================================================="


