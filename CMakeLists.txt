cmake_minimum_required (VERSION 3.3)
project (BlockHash)

add_executable(blockhash blockhash.c)

find_package(ImageMagick REQUIRED COMPONENTS MagickWand)

if(ImageMagick_FOUND)
	include_directories(${ImageMagick_INCLUDE_DIRS})
	target_link_libraries(blockhash ${ImageMagick_LIBRARIES})
endif(ImageMagick_FOUND)

target_link_libraries(blockhash m)
