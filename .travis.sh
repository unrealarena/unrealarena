#!/bin/bash

# Copyright (C) 2015-2016  Unreal Arena
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

# Travis CI support script


################################################################################
# Setup
################################################################################

# Arguments parsing
if [ $# -ne 1 ]; then
	echo "Usage: ${0} <STEP>"
	exit 1
fi


################################################################################
# Routines (linux)
################################################################################

# before_install
linux-before_install() {
	sudo add-apt-repository -y ppa:nschloe/cmake-backports
	sudo add-apt-repository -y ppa:ubuntu-toolchain-r/test
	sudo apt-get -qq update
}

# install
linux-install() {
	sudo apt-get -qq install cmake\
	                         cmake-data\
	                         gcc-4.7\
	                         g++-4.7\
	                         libgl1-mesa-dev\
	                         libx11-dev\
	                         libxext-dev\
	                         zip
	sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-4.7 100\
	                         --slave   /usr/bin/g++ g++ /usr/bin/g++-4.7
}

# before_script
linux-before_script() {
	true
}

# script
linux-script() {
	cmake -H. -Bbuild -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release\
	                                      -DBUILD_GAME_NATIVE_EXE=0\
	                                      -DBUILD_GAME_NATIVE_DLL=0
	cmake --build build -- -j8 || cmake --build build -- VERBOSE=1
}

# before_deploy
linux-before_deploy() {
	curl -Ls "http://geolite.maxmind.com/download/geoip/database/GeoLiteCountry/GeoIP.dat.gz" |
		gunzip > build/GeoIP.dat
	curl -Ls "http://geolite.maxmind.com/download/geoip/database/GeoIPv6.dat.gz" |
		gunzip > build/GeoIPv6.dat
	cd build && zip -r9 --symlinks "../unrealarena-${TRAVIS_OS_NAME}.pre.zip" daemon\
	                                                                          daemonded\
	                                                                          daemon-tty\
	                                                                          irt_core-x86_64.nexe\
	                                                                          libGLEW.so.1.12\
	                                                                          libopenal.so.1\
	                                                                          libSDL2-2.0.so.0\
	                                                                          nacl_helper_bootstrap\
	                                                                          nacl_loader\
	                                                                          cgame-x86-stripped.nexe\
	                                                                          sgame-x86-stripped.nexe\
	                                                                          cgame-x86_64-stripped.nexe\
	                                                                          sgame-x86_64-stripped.nexe\
	                                                                          GeoIP.dat\
	                                                                          GeoIPv6.dat
}


################################################################################
# Routines (osx)
################################################################################

# before_install
osx-before_install() {
	true
}

# install
osx-install() {
	true
}

# before_script
osx-before_script() {
	true
}

# script
osx-script() {
	cmake -H. -Bbuild -G "Unix Makefiles" -DCMAKE_OSX_ARCHITECTURES=x86_64\
	                                      -DCMAKE_OSX_DEPLOYMENT_TARGET=10.9\
	                                      -DCMAKE_BUILD_TYPE=Release\
	                                      -DBUILD_SERVER=0\
	                                      -DBUILD_GAME_NATIVE_EXE=0\
	                                      -DBUILD_GAME_NATIVE_DLL=0\
	                                      -DBUILD_GAME_NACL=0\
	                                      -DBUILD_TTY_CLIENT=0
	cmake --build build -- -j8 || cmake --build build -- VERBOSE=1
}

# before_deploy
osx-before_deploy() {
	cd build && zip -r9 --symlinks "../unrealarena-${TRAVIS_OS_NAME}.pre.zip" daemon\
	                                                                          irt_core-x86_64.nexe\
	                                                                          nacl_loader\
	                                                                          libGLEW.1.12.0.dylib\
	                                                                          libopenal.1.16.0.dylib\
	                                                                          SDL2.framework
}


################################################################################
# Main
################################################################################

# Arguments check
if ! `declare -f "${TRAVIS_OS_NAME}-${1}" > /dev/null`; then
	echo "Error: unknown step \"${TRAVIS_OS_NAME}-${1}\""
	exit 1
fi

# Enable exit on error & display of executing commands
set -ex

# Run <STEP>
${TRAVIS_OS_NAME}-${1}
