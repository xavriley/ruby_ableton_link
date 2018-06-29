require 'mkmf-rice'

HEADER_DIRS = [
  "..",
  "/Users/xriley/Projects/link/include",
  "/Users/xriley/Projects/link/modules/asio-standalone/asio/include",
]

LIB_DIRS = []

dir_config("libs", HEADER_DIRS, LIB_DIRS)

# TODO: match on RUBY_PLATFORM to set these properly
# see here for example https://github.com/2bbb/node-abletonlink/blob/master/binding.gyp
# also this for windows https://github.com/2bbb/node-abletonlink/blob/master/src/node-abletonlink.hpp#L3
#
# if(UNIX)
#   set_property(TARGET Ableton::Link APPEND PROPERTY
#     INTERFACE_COMPILE_DEFINITIONS
#     LINK_PLATFORM_UNIX=1
#   )
# endif()
#
# if(APPLE)
#   set_property(TARGET Ableton::Link APPEND PROPERTY
#     INTERFACE_COMPILE_DEFINITIONS
#     LINK_PLATFORM_MACOSX=1
      $defs << ' -DLINK_PLATFORM_MACOSX=1 '
#   )
# elseif(WIN32)
#   set_property(TARGET Ableton::Link APPEND PROPERTY
#     INTERFACE_COMPILE_DEFINITIONS
#     LINK_PLATFORM_WINDOWS=1
#   )
#   set_property(TARGET Ableton::Link APPEND PROPERTY
#     INTERFACE_COMPILE_OPTIONS
#     "/wd4503" # 'Identifier': decorated name length exceeded, name was truncated
#   )
# elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
#   set_property(TARGET Ableton::Link APPEND PROPERTY
#     INTERFACE_COMPILE_DEFINITIONS
#     LINK_PLATFORM_LINUX=1
#   )
# endif()

$CXXFLAGS += " -std=c++11 -stdlib=libc++ "

create_makefile('ruby_link')
