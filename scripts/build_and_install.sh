#!/bin/bash
DEVLIBS="build-essential libtool automake autotools-dev autoconf pkg-config libssl-dev libgmp3-dev libevent-dev bsdmainutils"
BUILDLIBS="libboost-system-dev libboost-filesystem-dev libboost-chrono-dev libboost-program-options-dev libboost-test-dev libboost-thread-dev libboost-chrono-dev libminiupnpc-dev"

DB4LIB="libdb4."
DB5LIB="libdb5."

ISSUDO=`sudo -v`

function checkPackage {

if [ $(dpkg-query -W -f='${Status}' $1  2>/dev/null | grep -c "ok installed") -eq 0 ];
then
  echo "Installing package: $1"
  sudo apt-get -y install $1
fi

}

echo "Check development dependencies, and install if need"
for pac in $(echo $DEVLIBS | tr " " "\n")
do
  checkPackage $pac
done

echo "Check building dependencies, and install if need"
for pac in $(echo $BUILDLIBS | tr " " "\n")
do
  checkPackage $pac
done

echo "Check Database dependencies, and install if need"
DB4LIBS=`sudo aptitude search $DB4LIB | awk '/^[pi].*[0-9](\+\+-dev |-dev )/{print $2}'`
DB5LIBS=`sudo aptitude search $DB5LIB | awk '/^[pi].*[0-9](\+\+-dev |-dev )/{print $2}'`

if [ $(sudo aptitude search $DB4LIB | awk '/^[pi].*[0-9](\+\+-dev |-dev )/{print $2}' | wc -l) -gt 0  ];
then
for pac in $(echo $DB4LIBS | tr " " "\n")
do
  checkPackage $pac
done
elif [ $(sudo aptitude search $DB5LIB | awk '/^[pi].*[0-9](\+\+-dev |-dev )/{print $2}' | wc -l ) -gt 0 ];
then

for pac in $(echo $DB5LIBS | tr " " "\n")
do
  checkPackage $pac
done

else
  echo "Error: not found compatible Berkeley DB packages"
  exit
fi

cd $PWD/../src
echo "Build PayDayd binary. Wait.."
doit=`make -f makefile.unix`

