cmake_minimum_required(VERSION 4.2)
project(Client C)

set(CMAKE_C_STANDARD 11)

# 1. Look for the Ncurses library on your system
find_package(Curses REQUIRED)

# 2. Look for the Threads (pthread) library
find_package(Threads REQUIRED)

# 3. Create the executable (assuming your file is main.c)
add_executable(Client main.c)

# 4. Link the libraries to your executable
# This tells the compiler to "attach" the library code to your program
target_link_libraries(Client PRIVATE
        ${CURSES_LIBRARIES}
        Threads::Threads
)

# 5. Tell the compiler where to find the ncurses header files
target_include_directories(Client PRIVATE ${CURSES_INCLUDE_DIRS})