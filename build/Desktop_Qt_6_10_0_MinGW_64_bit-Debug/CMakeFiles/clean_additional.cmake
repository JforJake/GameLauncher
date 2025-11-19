# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\GameLauncher_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\GameLauncher_autogen.dir\\ParseCache.txt"
  "GameLauncher_autogen"
  )
endif()
