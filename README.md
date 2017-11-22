# Ohmcoin

Ohmcoin is a PoS-based cryptocurrency.

Ohmcoin uses libsecp256k1,
			  libgmp,
			  Boost1.55,
			  OR Boost1.57,  
			  Openssl1.01m,
			  Berkeley DB 4.8,
			  QT5 to compile


Block Spacing: 90 Seconds
Stake Minimum Age: 6 Hours

Port: 33678
RPC Port: 33578


BUILD LINUX
-----------
`git clone https://github.com/theohmproject/OhmCoin`

#### Set up dependencies
```
sudo apt-get install -y software-properties-common
sudo add-apt-repository ppa:bitcoin/bitcoin
sudo apt-get update
sudo apt-get install libdb4.8-dev libdb4.8++-dev build-essential -y
sudo apt-get install libtool autotools-dev automake pkg-config libssl-dev -y
sudo apt-get install libevent-dev bsdmainutils libboost-all-dev libminiupnpc-dev -y
```

If you are setting this up on a VPS with less than 2GB of ram. Use this guide https://www.digitalocean.com/community/tutorials/how-to-add-swap-on-ubuntu-14-04 to set up a swap partition

You need to add a folder to the obj folder so from the src folder `cd obj` and then `mkdir crypto`

`cd ..` so that you are in the `src` folder again.

`cd leveldb` and then `chmod 755 *`
This last command gives execute and read permissions to this whole folder which is required to use it during the building of the wallet executable.

`cd ..` so that you are back in `src`

`sudo make -f makefile.unix` # Headless Ohmcoin

### Optional steps

`strip Ohmcoind`

this will let you run `ohmcoind` from any location

`sudo cp ohmcoind /usr/local/bin`




BUILD WINDOWS
-------------

1) Download Qt.zip from https://github.com/theohmproject/OhmCoin and unpack to C:/

2) Download Ohmcoin source from https://github.com/theohmproject/OhmCoin 

2.1) Unpack to C:/Ohmcoin

3) Install Perl for windows from the homepage http://www.activestate.com/activeperl/downloads

4) Download Python 2.7 https://www.python.org/downloads/windows/

4.1) While installing python make sure to add python.exe to the path.

5) Run msys.bat located in C:\MinGW49-32\msys\1.0

6) cd /C/Ohmcoin/src/leveldb

7) Type "TARGET_OS=NATIVE_WINDOWS make libleveldb.a libmemenv.a" and hit enter to build leveldb

8) Exit msys shell

9) Open windows command prompt

10) cd C:/dev

11) Type "49-32-qt5.bat" and hit enter to run

12) cd ../Ohmcoin

13) Type "qmake USE_UPNP=0" and hit enter to run

14) Type "mingw32-make" and hit enter to start building. When it's finished you can find your .exe in the release folder.
