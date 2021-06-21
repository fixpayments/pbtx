# Ubuntu 18.04

```
#Update and install packages
apt-get update && apt-get -y install cmake g++ git python3-protobuf protobuf-compiler

#Install eosio.cdt v1.70.0-1
wget https://github.com/EOSIO/eosio.cdt/releases/download/v1.7.0/eosio.cdt_1.7.0-1-ubuntu-18.04_amd64.deb
apt install ./eosio.cdt_1.7.0-1-ubuntu-18.04_amd64.deb

#Build and install eosio v2.0.3
git clone --recursive -j8 -b v2.0.3 --depth 1 https://github.com/EOSIO/eos.git

#Change broken boost link in ./eos/scripts/helpers/eosio.sh to this:
curl -LO https://boostorg.jfrog.io/artifactory/main/release/$BOOST_VERSION_MAJOR.$BOOST_VERSION_MINOR.$BOOST_VERSION_PATCH/source/boost_$BOOST_VERSION.tar.bz2

cd eosio
./scripts/eosio_build.sh
./scripts/eosio_install.sh

#Build and install boost 1.70.0
wget http://downloads.sourceforge.net/project/boost/boost/1.70.0/boost_1_70_0.tar.gz
tar xfz boost_1_70_0.tar.gz
rm boost_1_70_0.tar.gz
cd boost_1_70_0
./bootstrap.sh --prefix=/usr/local --with-libraries=date_time,filesystem,system,chrono,iostreams,regex,test
./b2 install link=static

```