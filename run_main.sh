#!/usr/bin/env bash

# run_main.sh
# Build and run the project's main executable.

set -euo pipefail

CXX=${CXX:-g++}
CXXFLAGS="-std=c++17 -Wall -Wextra -g -fdiagnostics-color=always"
PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
MAIN_SRC="$PROJECT_ROOT/src/main.cpp"
EXECUTABLE="$PROJECT_ROOT/main"

usage() {
    cat <<EOF
Usage: $0 [options]

Options:
    --build           Build the project using g++ (main + Lib/*.cpp)
    --run             Run the main executable (builds first if missing)
    --build-only      Build and exit
    --run-only        Run without building
    --clean           Remove the main executable
    -h, --help        Show this help

Examples:
  $0 --build         Build the project and keep the binary at ./main
  $0 --run           Build (if needed) and run ./main
  $0 --build-only    Only build, do not run
EOF
}

BUILD=false
RUN=false
BUILD_ONLY=false
RUN_ONLY=false
CLEAN=false
DEBUG=false

while [[ $# -gt 0 ]]; do
    case "$1" in
        --build) BUILD=true; shift ;; 
        --run) RUN=true; shift ;; 
        --build-only) BUILD_ONLY=true; shift ;; 
        --run-only) RUN_ONLY=true; shift ;; 
        --clean) CLEAN=true; shift ;; 
    --debug) # kept for compatibility but ignored
          shift ;; 
        -h|--help) usage; exit 0 ;; 
        *) echo "Unknown option: $1"; usage; exit 1 ;;
    esac
done

if [ "$CLEAN" = true ]; then
    echo "Cleaning main executable..."
    rm -f "$EXECUTABLE"
    exit 0
fi

# Default behavior: build and run
if [ "$BUILD" = false ] && [ "$RUN" = false ] && [ "$BUILD_ONLY" = false ] && [ "$RUN_ONLY" = false ]; then
    BUILD=true
    RUN=true
fi

# If run-only requested, skip building
if [ "$RUN_ONLY" = true ]; then
    RUN=true
    BUILD=false
fi

# Build step
if [ "$BUILD" = true ] || [ "$BUILD_ONLY" = true ]; then
    echo "Building project with g++ (main + Lib/*.cpp)"
    LIB_SRCS=$(find "$PROJECT_ROOT/Lib" -name "*.cpp" | sort)
    echo "$CXX $CXXFLAGS -o $EXECUTABLE $MAIN_SRC $LIB_SRCS"
    $CXX $CXXFLAGS -o "$EXECUTABLE" "$MAIN_SRC" $LIB_SRCS
    echo "Build complete."

    if [ "$BUILD_ONLY" = true ]; then
        echo "Build-only requested; exiting after build."
        exit 0
    fi
fi

# Run step
if [ "$RUN" = true ]; then
    if [ ! -f "$EXECUTABLE" ]; then
        echo "Executable $EXECUTABLE not found. Please build first (use --build)."
        exit 1
    fi

    echo "Running $EXECUTABLE"
    exec "$EXECUTABLE"
fi

exit 0
