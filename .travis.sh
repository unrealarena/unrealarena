#!/bin/bash

# Travis CI support script


# Arguments parsing
if [ $# -ne 1 ]; then
	echo "Usage: ${0} <STEP>"
	exit 1
fi


# Routines ---------------------------------------------------------------------

# linux64

# before_install
linux64-before_install() {
	sudo add-apt-repository -y ppa:smspillaz/cmake-2.8.12
	sudo add-apt-repository -y ppa:ubuntu-toolchain-r/test
	sudo apt-get -qq update
}

# install
linux64-install() {
	sudo apt-get -qq install cmake cmake-data gcc-4.7 g++-4.7 libgl1-mesa-dev libx11-dev libxext-dev zip
	sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-4.7 100 --slave /usr/bin/g++ g++ /usr/bin/g++-4.7
}

# before_script
linux64-before_script() {
	mkdir "daemon/deps"
	wget -qP "daemon/deps" "https://github.com/unrealarena/unrealarena-deps/releases/download/v0.1/linux64-3.tar.xz"
	tar xf "daemon/deps/linux64-3.tar.xz" -C "daemon/deps"
}

# script
linux64-script() {
	mkdir build
	cd build
	cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release\
	                          -DBUILD_GAME_NATIVE_EXE=0\
	                          -DBUILD_GAME_NATIVE_DLL=0\
	                          ..
	cmake --build . -- -j8 || cmake --build . -- VERBOSE=1
}

# before_deploy
linux64-before_deploy() {
	cd build
	wget -q "http://geolite.maxmind.com/download/geoip/database/GeoLiteCountry/GeoIP.dat.gz" && gunzip "GeoIP.dat.gz"
	wget -q "http://geolite.maxmind.com/download/geoip/database/GeoIPv6.dat.gz" && gunzip "GeoIPv6.dat.gz"
	zip -9 "../unrealarena-linux64.pre.zip" daemon\
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
if ! `declare -f "${OS}-${1}" > /dev/null`; then
	echo "Error: unknown step \"${OS}-${1}\""
	exit 1
fi

# Enable exit on error & display of executing commands
set -ex

# Run <STEP>
${OS}-${1}
