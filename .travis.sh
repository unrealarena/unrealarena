#!/bin/bash

# Travis CI support script


# Arguments parsing
if [ $# -ne 1 ]; then
	echo "Usage: ${0} <STEP>"
	exit 1
fi


# Routines ---------------------------------------------------------------------

# linux

# before_install
linux-before_install() {
	sudo add-apt-repository -y ppa:smspillaz/cmake-2.8.12
	sudo add-apt-repository -y ppa:ubuntu-toolchain-r/test
	sudo apt-get -qq update
}

# install
linux-install() {
	sudo apt-get -qq install cmake cmake-data gcc-4.7 g++-4.7 libgl1-mesa-dev libx11-dev libxext-dev zip
	sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-4.7 100 --slave /usr/bin/g++ g++ /usr/bin/g++-4.7
}

# before_script
linux-before_script() {
	true
}

# script
linux-script() {
	mkdir build
	cd build
	cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release\
	                          -DBUILD_GAME_NATIVE_EXE=0\
	                          -DBUILD_GAME_NATIVE_DLL=0\
	                          ..
	cmake --build . -- -j8 || cmake --build . -- VERBOSE=1
}

# before_deploy
linux-before_deploy() {
	cd build
	wget -q "http://geolite.maxmind.com/download/geoip/database/GeoLiteCountry/GeoIP.dat.gz" && gunzip "GeoIP.dat.gz"
	wget -q "http://geolite.maxmind.com/download/geoip/database/GeoIPv6.dat.gz" && gunzip "GeoIPv6.dat.gz"
	zip -9 "../unrealarena-linux.pre.zip" daemon\
	                                      daemon-tty\
	                                      daemonded\
	                                      irt_core-x86_64.nexe\
	                                      nacl_helper_bootstrap\
	                                      nacl_loader\
	                                      cgame-x86-stripped.nexe\
	                                      sgame-x86-stripped.nexe\
	                                      cgame-x86_64-stripped.nexe\
	                                      sgame-x86_64-stripped.nexe\
	                                      GeoIP.dat\
	                                      GeoIPv6.dat
}


# Main -------------------------------------------------------------------------

# Arguments check
if ! `declare -f "${TRAVIS_OS_NAME}-${1}" > /dev/null`; then
	echo "Error: unknown step \"${TRAVIS_OS_NAME}-${1}\""
	exit 1
fi

# Enable exit on error & display of executing commands
set -ex

# Run <STEP>
${TRAVIS_OS_NAME}-${1}
