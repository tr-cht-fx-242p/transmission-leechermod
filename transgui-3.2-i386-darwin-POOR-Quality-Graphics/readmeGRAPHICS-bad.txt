10.5.8 and higher
Transmission Remote GUI

Posted by Instab

here is the one for 10.5.8 and higher. based on the source tr-cht-fx-242p linked to
in his reply to my last build. it does however still have similar gui issues
but see for yourself 

re:
here's the remote gui for 10.6.8 and later. it seems to have gfx problems but
have a look for yourself ...


Posted by tr-cht-fx-242p

Sorry about the graphics problems on Mac. For a long while I have used a port
of the official Transmission Remote GUI based on this (note comments #4, #35
and others):
Issue 463 - transmisson-remote-gui - transmission 2.4x support: Status of all
torrents is incorrect/unknown, no status icons - Cross platform remote GUI for
the Transmission Bit-Torrent client - Google Project Hosting

I thought ivanal's work was good and I was introduced to the GUI code this
way. I maintained the GUI code to match the changesets of the daemon as I went
and additionally to match changes to the official remote GUI. The
lazarus-0.9.30-fpc-2.4.2 worked good on my old machines and so that is what I
used. Changes were committed to the source for working with 0.9.30. Testing
with Window XP showed graphics in an OK manner. The changes by ivanal patches
from the official source all functioned well. i.e."forced launch button and
slightly tuned graphics (although they are still in alpha state)". Only
recently do I have a Windows 7 capable computer and graphics are good there as
well.

I have a large array of PCs that I got all for free or nearly free from either
the dump or real cheap from people who don't know how to fix, update
(hardware/software) or use them. But in all these computers only one Mac, a
Power Mac G4 (guess that people just love them old Macs?) and now here there
is a mandatory government recycling so the good old days of the free machines
seems gone for me.

I haven't set up the Power Mac G4, but it's my understanding it will take Mac
OS X 10.4.11 "Tiger" or unofficially Mac OS X 10.5.8 "Leopard" but I don't
know if that would suite development of the GUI properly.

It's great to have a compiled version for Mac of the modded GUI to support my
little Transmission Leecher Mod project.






Transmission Remote GUI.
Copyright (c) 2008-2011 by Yury Sidorov.

Transmission Remote GUI is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

Transmission Remote GUI is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
*********************************************************************************

Transmission Remote GUI is feature rich cross platform front-end to remotely control Transmission daemon via its RPC protocol. It is faster and has more functionality than build-in Transmission web interface.

Transmission Remote GUI is developed using Lazarus RAD and Free Pascal compiler.

Features:
 * Native application for Windows, Linux and MacOS X
 * uTorrent-like interface
 * Select files to download
 * Choose files priority
 * View details about connected peers
 * Full information about each torrent
 * Per torrent options

Project home:
http://code.google.com/p/transmisson-remote-gui/

INSTALLATION

LINUX:

Easy way (recommended).

There are precompiled program's binaries for i386 and x86_64 Linux architectures.
- Download a .zip archive for your architecture.
- Unzip it to your home dir.
- Create a desktop or menu shortcut to the transgui executable.
  * (If needed, change the transgui file permissions to executable).
- Run the program using the created shortcut.

Harder way.

Build the program by yourself.
- Make sure you have working Lazarus and Free Pascal compiler installed.
  * Free Pascal Compiler 2.4 and Lazarus 0.9.28.2 is used to develop Transmission Remote GUI. You may use different versions of FPC and Lazarus at your own risk.
- Download the sources archive and extract it to some folder or perform svn checkout. 
- Open terminal/command line prompt and cd to the sources folder;
- Execute "make" command to build the application;
- Execute "make zipdist" command to create a release .zip archive in the "Release" sub-folder. 

More information about building here:
http://code.google.com/p/transmisson-remote-gui/wiki/Building

COMMAND LINE PARAMETERS

You can specify path to a .torrent file or a magnet link as a command line parameter. The program will add the specified torrent.

-hidden : Start the program hidden. Only the program's tray icon will be visible.
--home=<home_dir> : Specifies a home directory for the program. All program's settings are stored in the home directory. You can run multiple instances of the program by specifying different home directories.

PORTABLE MODE

If the program finds the transgui.ini file in the same folder as the binary file, then it will store all configuration and data files in the program's folder, instead of the folder in a user profile.
