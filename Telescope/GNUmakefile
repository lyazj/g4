# This script delegates all targets to cmake.
#
# Written by <lyazj@github.com>.

all:

%:
	@[ -d build ] || (mkdir build && cd build && cmake ..)
	@cd build && $(MAKE) -j $(shell cat /proc/cpuinfo | grep MHz | wc -l) "$@"
