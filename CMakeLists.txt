cmake_minimum_required(VERSION 3.8)

# we're using the copy of the import files from the 32Blit SDK here
# this example checks if PICO_SDK_PATH is set to allow compiling for other targets when it isn't set
if(PICO_SDK_PATH)
  include(${32BLIT_DIR}/32blit-pico/pico_sdk_import.cmake)
  include(${32BLIT_DIR}/32blit-pico/pico_extras_import.cmake)
endif()

project (blit-racers)
find_package (32BLIT CONFIG REQUIRED PATHS ../32blit-sdk)
blit_executable (blit-racers blit-racers.cpp blit-racers.hpp animation.cpp animation.hpp track.hpp track.cpp position.hpp actor.hpp actor.cpp util.hpp util.cpp game.hpp game.cpp tiledata.cpp tiledata.hpp)
blit_assets_yaml (blit-racers assets.yml)
blit_metadata (blit-racers metadata.yml)

target_compile_definitions(blit-racers PRIVATE ALLOW_HIRES=0)
