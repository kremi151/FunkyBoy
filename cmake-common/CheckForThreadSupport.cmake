include(CMakePushCheckState)
include(CheckIncludeFileCXX)
include(CheckCXXSourceCompiles)

cmake_push_check_state(RESET)
set(CMAKE_CXX_STANDARD 17)

check_include_file_cxx("thread" HAVE_STD_THREAD)

if(HAVE_STD_THREAD)
    add_library(CXX::Thread INTERFACE IMPORTED)
    target_compile_definitions(CXX::Thread INTERFACE HAS_STD_THREAD=$<BOOL:${HAVE_STD_THREAD}>)

    string(CONFIGURE [[
        #include <thread>
        #include <chrono>

        int main() {
            std::this_thread::sleep_for(std::chrono::milliseconds(0));
            return 0;
        }
    ]] code @ONLY)

    check_cxx_source_compiles("${code}" CAN_COMPILE_THREAD_THIS_THREAD)

    target_compile_definitions(CXX::Thread INTERFACE HAS_STD_THIS_THREAD=$<BOOL:${CAN_COMPILE_THREAD_THIS_THREAD}>)
    if(NOT CAN_COMPILE_THREAD_THIS_THREAD)
        message(STATUS "No C++ support for std::this_thread")
    endif()
else()
    message(STATUS "No C++ support for std::thread")
endif()

cmake_pop_check_state()
