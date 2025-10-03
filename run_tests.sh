#!/bin/bash

# SyntheseImage3D Test Runner
# Comprehensive script to compile and execute all test files

# Note: Removed 'set -e' to allow continuing when tests fail

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Configuration
TEST_DIR="test"
EXECUTABLE_DIR="$TEST_DIR/Executables"
CXX_COMPILER="g++"
CXX_FLAGS="-std=c++17 -Wall -Wextra -g"

# Test files and their corresponding executables
declare -A TESTS=(
    ["vector_test"]="$TEST_DIR/vector_test.cpp"
    ["quaternion_test"]="$TEST_DIR/quaternion_test.cpp"
    ["rgba_color_test"]="$TEST_DIR/rgba_color_test.cpp"
    ["matrix_test"]="$TEST_DIR/matrix_test.cpp"
)

# Function to print colored output
print_status() {
    local color=$1
    local message=$2
    echo -e "${color}${message}${NC}"
}

# Function to print section headers
print_header() {
    echo
    echo "=========================================="
    print_status $BLUE "$1"
    echo "=========================================="
}

# Function to compile a test
compile_test() {
    local test_name=$1
    local source_file=$2
    local executable="$EXECUTABLE_DIR/$test_name"
    
    print_status $YELLOW "Compiling $test_name..."
    
    # Library source files that need to be linked
    local lib_files="Lib/Geometry/Quaternion.cpp Lib/Geometry/Vector3D.cpp Lib/Rendering/RGBA_Color.cpp"
    
    if $CXX_COMPILER $CXX_FLAGS -o "$executable" "$source_file" $lib_files; then
        print_status $GREEN "✓ $test_name compiled successfully"
        return 0
    else
        print_status $RED "✗ Failed to compile $test_name"
        return 1
    fi
}

# Function to run a test
run_test() {
    local test_name=$1
    local executable="$EXECUTABLE_DIR/$test_name"
    
    print_status $YELLOW "Running $test_name..."
    
    if [ ! -f "$executable" ]; then
        print_status $RED "✗ Executable $executable not found"
        return 1
    fi
    
    if ./"$executable"; then
        print_status $GREEN "✓ $test_name passed"
        return 0
    else
        print_status $RED "✗ $test_name failed"
        return 1
    fi
}

# Function to clean executables
clean_tests() {
    print_header "Cleaning Test Executables"
    
    if [ -d "$EXECUTABLE_DIR" ]; then
        rm -f "$EXECUTABLE_DIR"/*
        print_status $GREEN "✓ Cleaned test executables"
    else
        print_status $YELLOW "! Executable directory doesn't exist"
    fi
}

# Function to compile all tests
compile_all_tests() {
    print_header "Compiling All Tests"
    
    # Create executable directory if it doesn't exist
    mkdir -p "$EXECUTABLE_DIR"
    
    local compiled=0
    local failed=0
    
    for test_name in "${!TESTS[@]}"; do
        if compile_test "$test_name" "${TESTS[$test_name]}"; then
            ((compiled++))
        else
            ((failed++))
        fi
    done
    
    echo
    print_status $BLUE "Compilation Summary:"
    print_status $GREEN "✓ Compiled: $compiled"
    if [ $failed -gt 0 ]; then
        print_status $RED "✗ Failed: $failed"
        return 1
    fi
    
    return 0
}

# Function to run all tests
run_all_tests() {
    print_header "Running All Tests"
    
    local passed=0
    local failed=0
    local failed_tests=()
    
    for test_name in "${!TESTS[@]}"; do
        if run_test "$test_name"; then
            ((passed++))
        else
            ((failed++))
            failed_tests+=("$test_name")
        fi
        echo  # Add spacing between tests
    done
    
    echo
    print_status $BLUE "Test Results Summary:"
    print_status $GREEN "✓ Passed: $passed"
    
    if [ $failed -gt 0 ]; then
        print_status $RED "✗ Failed: $failed"
        print_status $RED "Failed tests: ${failed_tests[*]}"
        return 1
    else
        print_status $GREEN "🎉 All tests passed!"
    fi
    
    return 0
}

# Function to show usage
show_usage() {
    echo "Usage: $0 [OPTION]"
    echo
    echo "Options:"
    echo "  compile, -c     Compile all tests only"
    echo "  run, -r         Run all tests (assumes already compiled)"
    echo "  clean           Clean all test executables"
    echo "  help, -h        Show this help message"
    echo "  (no option)     Compile and run all tests (default)"
    echo
    echo "Individual test commands:"
    for test_name in "${!TESTS[@]}"; do
        echo "  $test_name      Compile and run only $test_name"
    done
}

# Function to run individual test
run_individual_test() {
    local test_name=$1
    
    if [[ ! "${!TESTS[@]}" =~ "$test_name" ]]; then
        print_status $RED "✗ Unknown test: $test_name"
        echo "Available tests: ${!TESTS[@]}"
        exit 1
    fi
    
    print_header "Running Individual Test: $test_name"
    
    # Compile the test
    mkdir -p "$EXECUTABLE_DIR"
    if ! compile_test "$test_name" "${TESTS[$test_name]}"; then
        exit 1
    fi
    
    echo
    
    # Run the test
    if ! run_test "$test_name"; then
        exit 1
    fi
    
    print_status $GREEN "🎉 $test_name completed successfully!"
}

# Main execution
main() {
    print_header "SyntheseImage3D Test Suite"
    
    case "${1:-}" in
        "compile"|"-c")
            compile_all_tests
            ;;
        "run"|"-r")
            run_all_tests
            ;;
        "clean")
            clean_tests
            ;;
        "help"|"-h")
            show_usage
            ;;
        "")
            # Default: compile and run all tests
            if compile_all_tests; then
                echo
                run_all_tests
            else
                print_status $RED "✗ Compilation failed. Cannot run tests."
                exit 1
            fi
            ;;
        *)
            # Check if it's an individual test name
            if [[ "${!TESTS[@]}" =~ "$1" ]]; then
                run_individual_test "$1"
            else
                print_status $RED "✗ Unknown option: $1"
                echo
                show_usage
                exit 1
            fi
            ;;
    esac
}

# Run main function with all arguments
main "$@"