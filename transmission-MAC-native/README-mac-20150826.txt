https://github.com/cfpp2p/transmission/commits/svn/trunk


Commits on Jul 3, 2015

    version change only: already was fixed
    https://trac.transmissionbt.com/ticket/5427
    cfpp2p authored Jul 3, 2015

    Updated DHT blocklist functionality - test nodes.
    cfpp2p authored Jul 3, 2015

Commits on Jun 28, 2015

    Fixed: limiting download to 0 in a seeding torrent prevents uploading
    https://trac.transmissionbt.com/ticket/3902
    cfpp2p authored Jun 28, 2015

    Commits on June 22, 2015

    Added zero upload torrent setting
    Added drone seed a torrent
    Added turn off torrent private flag
    Added zero peer torrent setting

    Commits on Jun 21, 2015

    Fixed: Setting Upload Limit of individual torrent to 0KB/s stops download
    https://trac.transmissionbt.com/ticket/3155
    cfpp2p authored Jun 21, 2015 

    Commits on May 6, 2015

    User agent, client version, peer id fixes private trackers
    not recognizing transmission. Added ability to change many
    new settings through RPC and the Shift web client.
    All webseed settings added to RPC. Added ability to set an
    alternate default streaming mode. Fixed transmission exits
    without informing http trackers #5477 and #5854.
    cfpp2p authored May 6, 2015

    Commits on March 31, 2015

    https://trac.transmissionbt.com/ticket/4649#comment:47
    Fix webseed bug where a corrupt block overwrites a good block
    and prevent block write if piece is already complete.
    cfpp2p authored Mar 31, 2015

    Commits on March 16, 2015
	
    add setting in settings.json "cheat-mode-default"
    for torrents only when first added.
    0, /* Normal - no cheat */
    1, /* always leecher */
    2, /* always seeder */
    3, /* report a ratio of ~2 */
    4, /* report a ratio of ~4 */

    Commits on March 04, 2015

    fix #5873 patch by x190 Corrupt webseed causes
    excessive bandwidth usage
    https://trac.transmissionbt.com/ticket/5873
    cfpp2p authored Mar 04, 2015

    Commits on Feb 21, 2015 

    3 items:
    1.) minimize wasted downloaded data when pauseing torrents with active webseeds
    2.) verbose logging for some aspects of webseeds
    3.) better implementation of "drop-interrupted-webseeds": false, mode
    cfpp2p authored Feb 21, 2015


After exhaustive testing The February 7, 2015 release
concludes the recent frequent updates for webseeds and
the new features. Please report any issues to:
http://sourceforge.net/p/transmissiondaemon/discussion/general/
Thank you for being patient :) through this.

Commits on Feb 7, 2015

    improve log message appearance
    cfpp2p authored Feb 7, 2015

    FIX rare crash: message log level 3 then user deletes data active webseed
    cfpp2p authored Feb 7, 2015

Commits on Feb 4, 2015

    Full blocklist support for webseeds and logging
    cfpp2p authored Feb 3, 2015

    Fixed issue resuming a paused torrent and connecting to webseeds when
    in the endgame after a pause
    cfpp2p authored Feb 3, 2015

    New settings: settings.json - blocklist-webseeds,
    drop-interrupted-webseeds, webseed-timeout-seconds
    cfpp2p authored Feb 3, 2015

Commits on Jan 31, 2015

    close webseed connection on pause and add logging, do properly and
    immediately on all concurrent webseed connections for the torrent.
    Patch by x190, thanks.
    cfpp2p authored Jan 31, 2015

Commits on Jan 29, 2015

    Fixed slow to pause/stop webseed issues and possible crashes.
    cfpp2p authored Jan 29, 2015

    Only set CURLOPT_COOKIEFILE,
    if it exists https://trac.transmissionbt.com/ticket/5356
    cfpp2p authored Jan 29, 2015

    tr-getopt code': fixed https://trac.transmissionbt.com/changeset/14067
    cfpp2p authored Jan 29, 2015

    tracker message warning appearance
    cfpp2p authored Jan 29, 2015

Commits on Jan 27, 2015

    fix a variety of webseed issues: crashes, hangs and
    unnecessary bandwidth.
    https://forum.transmissionbt.com/viewtopic.php?f=4&t=16724&start=15#p70316
    cfpp2p authored Jan 26, 2015

Commits on Dec 14, 2014

    Show tracker ip to user. Facilitate whitelisting.
    cfpp2p authored Dec 14, 2014

Commits on Dec 10, 2014

    Improved blocklist support: only warn user if tracker in blocklist(s),
    don't automatically block tracker. Added hook for full TCP blocklist support.
    Now complete UDP and TCP blocklist support is available for all connection types.
    cfpp2p authored Dec 10, 2014
	
Commits on Dec 03, 2014

    allow whitelists, with precedence over blocklists
    cfpp2p authored Dec 3, 2014 

Commits on Oct 27, 2014

    Prevent relative or invalid paths to public functions #5825, #5802
    cfpp2p authored Oct 27, 2014

    Automatically parse a text file in "watched" folder for magnet links #4710
    cfpp2p authored Oct 27, 2014


Commits on Sep 19, 2014

    run script when torrent added, prevent zombies from child scripts,
    quick verify do not queue-verify empty torrents when adding,
    set prefetch magnet as default.
    #5586, #5452, #4034
    cfpp2p authored September 19, 2014

Commits on Sep 13, 2014

    cfpp2p

    FIX Incorrect error message when failing to write to disk
    https://forum.transmissionbt.com/viewtopic.php?f=2&t=16422
    cfpp2p authored September 13, 2014


Commits on Jul 31, 2014

    Added 'streaming' capability to libtransmission.
	Additionally #5755, #5754, #5736, #5734
    cfpp2p authored 7 hours ago

Commits on Jul 21, 2014

    optimize block size without overflows fix #5755
    cfpp2p authored 11 days ago


July 30, 2014

Added 'streaming' capability to libtransmission.

Added rpc 'streaming' to the GUI and web clients.

Fixed several overflows and possible crashes due to
type conversions and other associated issues.
( you WON'T find correct fixes for these at "official"
https://trac.transmissionbt.com/report/12 )
#5755, #5754, #5736, #5734


Jul 08, 2014

    r14303 #5734 #5732 & #4005 -- Properly fix peer communication
    vulnerabilities (no known exploits) reported by the famous Ben Hawkes
    78c31e15f0
    cfpp2p authored July 08, 2014 

June 24, 2014

Correlate with official 2.83+ trunk relevant changes
are enhancement #5711 only.

Jun 24, 2014

    XBT is self descriptive, and new mainline client #5711
    7b3b34c35d Browse code
    cfpp2p authored June 23, 2014


May 21, 2014

This release validates all known bugs have been eliminated.

NONE of the 2.8x branch's current unsolved or past bugs have
ever been introduced to this release. All 2.8x features and
enhancements are included in this release (except rename).

This release includes updates to the DHT and fixes an insignificant
memory leak after adding a corrupt torrent.
(Yet to be fixed in the official 2.8x branch).

A multitude of new bugs were introduced with the official 2.8x branch's
quark and threading model changes. The official 2.8x branch's quark and
threading model changes introduced and continue to allow for a multitude
of unsolved past and yet undiscovered new bugs and crashes.

May 21, 2014

    Transmission
    fix insignificant after adding a corrupt torrent
    a2cf355025 Browse code
    cfpp2p authored May 21, 2014

    DHT
    Don't load function buffers and call debugf() unless debugging is active
    020f37731f Browse code
    cfpp2p authored May 21, 2014

Jan 30, 2014

    sanitize Basename
    614cb2c4d2 Browse code
    cfpp2p authored January 30, 2014

Jan 29, 2014

    test for both forward and backward slash -- #5517
    6f95f6718b Browse code
    cfpp2p authored January 29, 2014

Jan 25, 2014

    disallow contrived filenames fix #5517
    81a04f3ba4 Browse code
    cfpp2p authored January 25, 2014

Jan 21, 2014

    Don't load function buffers and call debugf() unless debugging is act� �
    bcb2898951 Browse code
    cfpp2p authored January 21, 2014

    Fixes #5517 Error adding torrent with "../" in file path
    d1356a0e2e Browse code
    cfpp2p authored January 21, 2014

Jan 14, 2014

    fixed #5583 blocklist update �
    ce82ff3bde Browse code
    cfpp2p authored January 14, 2014

    whitespace
    f214ad0633 Browse code
    cfpp2p authored January 14, 2014

Dec 27, 2013

    #5562 fixed Discrepancy in curl SSL setup between tr-daemon and tr-re� �
    91c177d2c4 Browse code
    cfpp2p authored December 27, 2013



https://github.com/cfpp2p/transmission/commits/master

Jan 14, 2014

    update SHIFT web client. Pull from killemov/Shift@97b8811
    d9a2fd239b Browse code
    cfpp2p authored January 14, 2014



December 02, 2013

web client changes ONLY:
 https://github.com/cfpp2p/transmission/commits/master

NO changes to the daemon:
 https://github.com/cfpp2p/transmission/commits/cygwin



https://github.com/cfpp2p/transmission/commits/master

Dec 02, 2013

    Multiple torrent files upload shift client. Pull from killemov/Shift@d� �
    30b0c83022 Browse code
    cfp authored December 02, 2013

Nov 29, 2013

    fix minor display issue when toggling 100% done files.
    73d554dc78 Browse code
    cfp authored November 29, 2013


	
https://github.com/cfpp2p/transmission/commits/cygwin

Oct 22, 2013

    pread with cygwin has improper access to zero byte files in unusual c� �
    051e1d1b1a Browse code
    cfpp2p authored October 22, 2013

Oct 19, 2013

    fix minor bug with magnet verify and incomplete directory enabled
    edfdbeeaed Browse code
    cfpp2p authored October 19, 2013

Oct 16, 2013

    magnet download not verified for existing data fixed #4089
    f6d6ef8dc3 Browse code
    cfp authored October 16, 2013

Oct 01, 2013

    revert 14136, correct fix for "queued torrents' state changes to paus� �
    f2a62b0485 Browse code
    cfp authored October 01, 2013

Sep 27, 2013

    Option to Automatically Verify Local Data on Completion fixes # 4649
    b911bcfe85 Browse code
    cfp authored September 27, 2013

    fixes queuing issues #4540 and #5427
    0a6e703d86 Browse code
    cfp authored September 27, 2013

Sep 20, 2013

    fix dns error with udp trackers
    60130d0d27 Browse code
    cfp authored September 20, 2013
