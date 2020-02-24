require 'mkmf'

CWD = File.expand_path(File.dirname(__FILE__))
LINK_DIR = File.join(CWD, 'link')

HEADER_DIRS = [
  "..",
  File.join(LINK_DIR, "include"),
  File.join(LINK_DIR, "modules/asio-standalone/asio/include"),
]

LIB_DIRS = []

dir_config("libs", HEADER_DIRS, LIB_DIRS)

$defs << ' -DDEBUG=1 '

case RUBY_PLATFORM
when /.*arm.*-linux.*/
  $defs << ' -DLINK_PLATFORM_LINUX=1 '
  $CXXFLAGS += " -std=c++11 "
when /.*linux.*/
  $defs << ' -DLINK_PLATFORM_LINUX=1 '
  $CXXFLAGS += " -std=c++11 "
when /.*darwin.*/
  $defs << ' -DLINK_PLATFORM_MACOSX=1 '
  $CXXFLAGS += " -std=c++11 -stdlib=libc++ "
when /.*mingw.*/, /bccwin32/
  $defs << ' -DLINK_PLATFORM_WINDOWS=1 '
  # This was copied from node-abletonlink but appears
  # to be failing with MinGW on appveyor
  # $defs << ' /wd4503 '
else
  RUBY_PLATFORM
end

create_makefile('ableton_link') {|conf| puts conf if ENV['CI']; conf.gsub("/wd4503", "") }
