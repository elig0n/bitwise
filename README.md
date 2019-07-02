# Bitwise
## Terminal based bitwise calculator in curses
[![Build Status](https://travis-ci.org/mellowcandle/bitwise.svg?branch=master)](https://travis-ci.org/mellowcandle/bitwise)[![codecov](https://codecov.io/gh/mellowcandle/bitwise/branch/master/graph/badge.svg)](https://codecov.io/gh/mellowcandle/bitwise)
[![Snap Status](https://build.snapcraft.io/badge/mellowcandle/bitwise.svg)](https://build.snapcraft.io/user/mellowcandle/bitwise)<a href="https://scan.coverity.com/projects/mellowcandle-bitwise">
  <img alt="Coverity Scan Build Status"
       src="https://img.shields.io/coverity/scan/18170.svg"/>
</a>

_Bitwise_ is multi base interactive calculator supporting dynamic base convertion and bit manipulation.
It's a handy tool for low level hackers, kernel developers and device drivers developers.

Some of the features include:
* Interactive ncurses interface
* Command line calculator supporting all bitwise operations.
* Individual bit manipulator.
* Bitwise operations such as NOT, OR, AND, XOR, and shifts.

## IRC support
#bitwise @ irc.oftc.net

##

![Demo](https://github.com/mellowcandle/bitwise/raw/master/resources/bitwise.gif "Bitwise demo")

## Usage
_bitwise_ can be used both Interactively and in command line mode.

### Command line calculator mode
In command line mode, bitwise will calculate the given expression and will output the result in all bases including binary representation.

_bitwise_ detects the base by the preface of the input (_0x/0X_ for hexadecimal, leading _0_ for octal, _b_ for binary, and the rest is decimal).

### Examples:


#### Simple base conversion

```
$ bitwise 0x12312311
Decimal: 305210129
Hexdecimal: 0x12312311
Octal: 02214221421
Size: 291.07MB
Binary:
0 0 0 1 0 0 1 0 | 0 0 1 1 0 0 0 1 | 0 0 1 0 0 0 1 1 | 0 0 0 1 0 0 0 1 
    31 - 24           23 - 16           15 -  8            7 -  0     
```

#### Calculator with predence

```
$ bitwise "0x30 * 0x20 + 30 / 2"
Decimal: 1551
Hexdecimal: 0x60F
Octal: 03017
Size: 1.51KB
Binary:
0 0 0 0 0 1 1 0 | 0 0 0 0 1 1 1 1 
    15 -  8            7 -  0 
```

### Interactive mode
_bitwise_ starts in interactive mode if no command line parameters are passed or if the _-i | --interactive_ flag is passed.
In this mode, you can input a number and manipulate it and see the other bases change dynamically.
It also allows changing individual bits in the binary.

#### Navigation in interactive mode
To move around use the arrow keys, or use _vi_ key bindings : <kbd> h </kbd> <kbd> j </kbd> <kbd> k </kbd> <kbd> l </kbd>.
Leave the program by pressing <kbd> q </kbd>.

##### Binary specific movement
You can toggle a bit bit using the <kbd> space </kbd> key.
You can jump a byte forward using <kbd> w </kbd> and backwards one byte using <kbd> b </kbd>. 

#### Bitwise operation in interactive mode

##### Setting the bit width:

Reducing or extending the bit width interactively is also very easy, just use:
<kbd> ! </kbd> for 8bit, <kbd> @ </kbd>  for 16Bit, <kbd> $ </kbd> for 32Bit and <kbd> * </kbd> for 64Bit.
When changing the bit width, the number is *masked* with the new width, so you might lost precision, use with care.

##### NOT:

Press <kbd> ~ </kbd> to perform the NOT operator.

## Installation

### Linux
#### Ubuntu
```
sudo add-apt-repository ppa:ramon-fried/bitwise
sudo apt-get update
sudo apt-get install bitwise
```
#### Snap
If your distribution supports Snap just type:
`
sudo snap install bitwise
`
#### Arch
Your can use the AUR repository: https://aur.archlinux.org/packages/bitwise/

### Windows
No, wait. really ???

### Building from source
*Make sure you have ncurses package installed*

- Download [the latest release](https://github.com/mellowcandle/bitwise/releases/latest)

```sh
tar xfz RELEASE-FILE.TAR.GZ
cd RELEASE-DIR

bash bootstrap.sh
./configure
make
sudo make install
```
