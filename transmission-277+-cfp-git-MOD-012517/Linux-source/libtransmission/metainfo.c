/*
 * This file Copyright (C) Mnemosyne LLC
 *
 * This file is licensed by the GPL version 2. Works owned by the
 * Transmission project are granted a special exemption to clause 2(b)
 * so that the bulk of its code can remain under the MIT license.
 * This exemption does not extend to derived works not owned by
 * the Transmission project.
 *
 * $Id: metainfo.c 14305 2014-07-21 12:27:38Z cfpp2p $
 */

#include <assert.h>
#include <errno.h>
#include <stdio.h> /* fopen(), fwrite(), fclose() */
#include <string.h> /* strlen() */

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h> /* unlink, stat */

#include <event2/buffer.h>

#include "transmission.h"
#include "session.h"
#include "bencode.h"
#include "crypto.h" /* tr_sha1 */
#include "metainfo.h"
#include "platform.h" /* tr_getTorrentDir() */
#include "utils.h"

/***
****
***/

#define MAX_NAME_LENGTH 219
// 219 + 36 = 255
// . + 16_byte_hash + .torrent + .tmp.XXXXXX = 36 bytes
// . + 16_byte_hash + .resume + .tmp.XXXXXX = 35 bytes
// see tr_bencToFile() for more explanation


static char*
metainfoGetBasenameNameAndPartialHash( const tr_info * inf, const tr_session * session )
{
    size_t i;
    char * shortName;
    char * cleanName;

    if( !strlen( inf->name ) )
        // allow empty name -- substitute
        shortName = tr_strdup( "Hash-Name-" );
    else if( strlen( inf->name ) <= MAX_NAME_LENGTH )
        shortName = tr_strdup( inf->name );
    else
        // truncate
        shortName = tr_strndup( inf->name, MAX_NAME_LENGTH );

    if( session && session->cleanUTFenabled )
        cleanName = tr_utf8clean( shortName, -1 );
    else
        cleanName = tr_strdup( shortName );
    tr_free( shortName );

    char * ret = tr_strdup_printf( "%s.%16.16s", cleanName, inf->hashString );

    const size_t name_len = strlen( cleanName );
    tr_free( cleanName );

    for( i=0; i<name_len; ++i )
        if( ret[i] == '/' )
            ret[i] = '_';


    return ret;
}

static char* metainfoGetBasenameHashOnly(tr_info const* inf)
{
    return tr_strdup(inf->hashString);
}

char* tr_metainfoGetBasename(tr_info const* inf, enum tr_metainfo_basename_format format, const tr_session * session)
{
    switch (format)
    {
    case TR_METAINFO_BASENAME_NAME_AND_PARTIAL_HASH:
        return metainfoGetBasenameNameAndPartialHash(inf, session);

    case TR_METAINFO_BASENAME_HASH:
        return metainfoGetBasenameHashOnly(inf);

    default:
        assert( 0 );
        return NULL;
    }
}

static char*
getTorrentFilename(tr_session const* session, tr_info const* inf, enum tr_metainfo_basename_format format)
{
    char * base = tr_metainfoGetBasename(inf, format, session);
    char * filename = tr_strdup_printf( "%s" TR_PATH_DELIMITER_STR "%s.torrent",
                                        tr_getTorrentDir( session ), base );
    tr_free( base );
    return filename;
}

static char*
getOldTorrentFilename( const tr_session * session, const tr_info * inf )
{
    int i;
    char * path;
    struct stat sb;
    const int tagCount = 5;
    const char * tags[] = { "beos", "cli", "daemon", "macosx", "wx" };

    /* test the beos, cli, daemon, macosx, wx tags */
    for( i=0; i<tagCount; ++i ) {
        path = tr_strdup_printf( "%s%c%s-%s", tr_getTorrentDir( session ), '/', inf->hashString, tags[i] );
        if( !stat( path, &sb ) && ( ( sb.st_mode & S_IFMT ) == S_IFREG ) )
            return path;
        tr_free( path );
    }

    /* test a non-tagged file */
    path = tr_buildPath( tr_getTorrentDir( session ), inf->hashString, NULL );
    if( !stat( path, &sb ) && ( ( sb.st_mode & S_IFMT ) == S_IFREG ) )
        return path;
    tr_free( path );

    /* return the -gtk form by default, since that's the most common case.
       don't bother testing stat() on it since this is the last candidate
       and we don't want to return NULL anyway */
    return tr_strdup_printf( "%s%c%s-%s", tr_getTorrentDir( session ), '/', inf->hashString, "gtk" );
}

/* this is for really old versions of T and will probably be removed someday */
void
tr_metainfoMigrate( tr_session * session,
                    tr_info *   inf )
{
    struct stat new_sb;
    char *      name = getTorrentFilename(session, inf, TR_METAINFO_BASENAME_NAME_AND_PARTIAL_HASH);

    if( stat( name, &new_sb ) || ( ( new_sb.st_mode & S_IFMT ) != S_IFREG ) )
    {
        char *    old_name = getOldTorrentFilename( session, inf );
        size_t    contentLen;
        uint8_t * content;

        tr_mkdirp( tr_getTorrentDir( session ), 0777 );
        if( ( content = tr_loadFile( old_name, &contentLen ) ) )
        {
            FILE * out;
            errno = 0;
            out = fopen( name, "wb+" );
            if( !out )
            {
                tr_nerr( inf->name, _( "Couldn't create \"%1$s\": %2$s" ),
                        name, tr_strerror( errno ) );
            }
            else
            {
                if( fwrite( content, sizeof( uint8_t ), contentLen, out )
                    == contentLen )
                {
                    tr_free( inf->torrent );
                    inf->torrent = tr_strdup( name );
                    tr_sessionSetTorrentFile( session, inf->hashString, name );
                    unlink( old_name );
                }
                fclose( out );
            }
        }

        tr_free( content );
        tr_free( old_name );
    }

    tr_free( name );
}

/***
****
***/

static bool
path_is_harmful( const char * path )
{
    if( ( path == NULL ) || ( *path == '\0' ) ) return true;
	
    //do NOT allow backward traverse
    if( !strncmp( path, "../", 3 ) ) return true;
    if( ( strstr( path, "/../" ) != NULL ) ) return true;

    const char * endOfString = strrchr( path, '\0' );

    // - error for all below will be -->  Is a direcrory  <-- by OS

    // illegal - slash as final
    if( !strcmp( --endOfString, "/" ) ) return true;

    // check for filename is just one or two dots
    if( strlen( path ) > 2 ) {
        if( ( !strcmp( --endOfString, "/." ) ) ) return true;
        if( ( !strcmp( --endOfString, "/.." ) ) ) return true;
        return false;
    }
    if( strlen( path ) == 2 ) {
        if( ( !strcmp( path, ".." ) ) || ( !strcmp( path, "/." ) ) ) return true;
        return false;
    }
    if( ( strlen( path ) == 1 )
        && ( !strcmp( path, "." ) ) ) return true;

    return false;
}

static bool
getfile( char ** setme, tr_file_index_t fileIndex, const char * root, tr_benc * path, struct evbuffer * buf, const bool cleanFiles, const bool noPath )
{
    bool success = false;
    int n = 0;

    if( !noPath && tr_bencIsList( path ) )
    {
        int i;
        n = tr_bencListSize( path );

        if( n > 0 )
        {
            evbuffer_drain( buf, evbuffer_get_length( buf ) );
            evbuffer_add( buf, root, strlen( root ) );

            for( i = 0; i < n; ++i )
            {
                const char * str;
                if( tr_bencGetStr( tr_bencListChild( path, i ), &str ) )
                {
                    evbuffer_add( buf, TR_PATH_DELIMITER_STR, 1 );
                    evbuffer_add( buf, str, strlen( str ) );
                }
            }

            if( cleanFiles )
                *setme = tr_utf8clean( (char*)evbuffer_pullup( buf, -1 ), evbuffer_get_length( buf ) );
            else
                *setme = tr_strndup( (char*)evbuffer_pullup( buf, -1 ), evbuffer_get_length( buf ) );

            /* fprintf( stderr, "[%s]\n", *setme ); */
            success = true;
        }
    }

    if( ( ( *setme != NULL ) && path_is_harmful( *setme ) ) || ( n < 1 ) )
    {
        char * nowName;
        nowName = tr_strdup_printf( "%s" TR_PATH_DELIMITER_STR "file-number-%d", root, fileIndex );
        tr_free( *setme );
        if( cleanFiles )
            *setme = tr_utf8clean( nowName, -1 );
        else
            *setme = tr_strdup( nowName );
        tr_free( nowName );
        success = true;
    }

    return success;
}

static const char*
parseFiles( tr_info * inf, tr_benc * files, const tr_benc * length, const bool cleanFiles )
{
    int64_t len;
    bool noPath;

    inf->totalSize = 0;

    if( path_is_harmful( inf->name ) )
    {
        char * nowName;
        nowName = tr_strdup_printf( "hash-name.%16.16s", inf->hashString );
        tr_free( inf->name );
        if( cleanFiles )
            inf->name = tr_utf8clean( nowName, -1 );
        else
            inf->name = tr_strdup( nowName );
        tr_free( nowName );
    }

    if( tr_bencIsList( files ) ) /* multi-file mode */
    {
        tr_file_index_t i;
        struct evbuffer * buf = evbuffer_new( );

        inf->isMultifile = 1;
        inf->fileCount   = tr_bencListSize( files );
        inf->files       = tr_new0( tr_file, inf->fileCount );

        for( i = 0; i < inf->fileCount; ++i )
        {
            tr_benc * file;
            tr_benc * path;

            file = tr_bencListChild( files, i );
            if( !tr_bencIsDict( file ) ) {
                evbuffer_free( buf );
                return "files";
            }

            noPath = false;
            if( !tr_bencDictFindList( file, "path.utf-8", &path ) )
                if( !tr_bencDictFindList( file, "path", &path ) )
                    noPath = true;

            if( !getfile( &inf->files[i].name, i, inf->name, path, buf, cleanFiles, noPath ) ) {
                evbuffer_free( buf );
                return "path";
            }

            if( !tr_bencDictFindInt( file, "length", &len ) ) {
                evbuffer_free( buf );
                return "length";
            }

            inf->files[i].length = len;
            inf->totalSize      += len;
        }

        evbuffer_free( buf );
    }
    else if( tr_bencGetInt( length, &len ) ) /* single-file mode */
    {
        inf->isMultifile      = 0;
        inf->fileCount        = 1;
        inf->files            = tr_new0( tr_file, 1 );
        inf->files[0].name    = tr_strdup( inf->name );
        inf->files[0].length  = len;
        inf->totalSize       += len;
    }
    else
    {
        return "length";
    }

    return NULL;
}

static char *
tr_convertAnnounceToScrape( const char * announce )
{
    char *       scrape = NULL;
    const char * s;

    /* To derive the scrape URL use the following steps:
     * Begin with the announce URL. Find the last '/' in it.
     * If the text immediately following that '/' isn't 'announce'
     * it will be taken as a sign that that tracker doesn't support
     * the scrape convention. If it does, substitute 'scrape' for
     * 'announce' to find the scrape page. */
    if( ( ( s = strrchr( announce, '/' ) ) ) && !strncmp( ++s, "announce", 8 ) )
    {
        const char * prefix = announce;
        const size_t prefix_len = s - announce;
        const char * suffix = s + 8;
        const size_t suffix_len = strlen( suffix );
        const size_t alloc_len = prefix_len + 6 + suffix_len + 1;
        char * walk = scrape = tr_new( char, alloc_len );
        memcpy( walk, prefix, prefix_len ); walk += prefix_len;
        memcpy( walk, "scrape", 6 );        walk += 6;
        memcpy( walk, suffix, suffix_len ); walk += suffix_len;
        *walk++ = '\0';
        assert( walk - scrape == (int)alloc_len );
    }
    /* Some torrents with UDP announce URLs don't have /announce. */
    else if ( !strncmp( announce, "udp:", 4 ) )
    {
        scrape = tr_strdup( announce );
    }

    return scrape;
}

static const char*
getannounce( tr_info * inf, tr_benc * meta )
{
    const char *      str;
    tr_tracker_info * trackers = NULL;
    int               trackerCount = 0;
    tr_benc *         tiers;

    /* Announce-list */
    if( tr_bencDictFindList( meta, "announce-list", &tiers ) )
    {
        int       n;
        int       i, j, validTiers;
        const int numTiers = tr_bencListSize( tiers );

        n = 0;
        for( i = 0; i < numTiers; ++i )
            n += tr_bencListSize( tr_bencListChild( tiers, i ) );

        trackers = tr_new0( tr_tracker_info, n );

        for( i = 0, validTiers = 0; i < numTiers; ++i )
        {
            tr_benc * tier = tr_bencListChild( tiers, i );
            const int tierSize = tr_bencListSize( tier );
            bool anyAdded = false;
            for( j = 0; j < tierSize; ++j )
            {
                if( tr_bencGetStr( tr_bencListChild( tier, j ), &str ) )
                {
                    char * url = tr_strstrip( tr_strdup( str ) );
                    if( !tr_urlIsValidTracker( url ) )
                        tr_free( url );
                    else {
                        tr_tracker_info * t = trackers + trackerCount;
                        t->tier = validTiers;
                        t->announce = url;
                        t->scrape = tr_convertAnnounceToScrape( url );
                        t->id = trackerCount;

                        anyAdded = true;
                        ++trackerCount;
                    }
                }
            }

            if( anyAdded )
                ++validTiers;
        }

        /* did we use any of the tiers? */
        if( !trackerCount )
        {
            tr_free( trackers );
            trackers = NULL;
        }
    }

    /* Regular announce value */
    if( !trackerCount
      && tr_bencDictFindStr( meta, "announce", &str ) )
    {
        char * url = tr_strstrip( tr_strdup( str ) );
        if( !tr_urlIsValidTracker( url ) )
            tr_free( url );
        else {
            trackers = tr_new0( tr_tracker_info, 1 );
            trackers[trackerCount].tier = 0;
            trackers[trackerCount].announce = url;
            trackers[trackerCount].scrape = tr_convertAnnounceToScrape( url );
            trackers[trackerCount].id = 0;
            trackerCount++;
            /*fprintf( stderr, "single announce: [%s]\n", url );*/
        }
    }

    inf->trackers = trackers;
    inf->trackerCount = trackerCount;

    return NULL;
}

/**
 * @brief Ensure that the URLs for multfile torrents end in a slash.
 *
 * See http://bittorrent.org/beps/bep_0019.html#metadata-extension
 * for background on how the trailing slash is used for "url-list"
 * fields.
 *
 * This function is to workaround some .torrent generators, such as
 * mktorrent and very old versions of utorrent, that don't add the
 * trailing slash for multifile torrents if omitted by the end user.
 */
static char*
fix_webseed_url( const tr_info * inf, const char * url )
{
    char * ret = NULL;
    /* strip trailing whitespace */
    const char * url_nows = tr_strstrip( tr_strndup( url, strlen( url ) ) );
    const size_t len = strlen( url_nows );

    if( tr_urlIsValid( url_nows, len ) )
    {
        if( ( inf->fileCount > 1 ) && ( len > 0 ) && ( url_nows[len-1] != '/' ) )
            ret = tr_strdup_printf( "%*.*s/", (int)len, (int)len, url_nows );
        else
            ret = tr_strndup( url_nows, len );
    }

    return ret;
}

static void
geturllist( tr_info    * inf,
            tr_benc    * meta,
            int          maxWebseeders )
{
    tr_benc * urls;
    const char * url;

    if( tr_bencDictFindList( meta, "url-list", &urls ) )
    {
        int          i;
        const int    n = MIN( tr_bencListSize( urls ), maxWebseeders );

        inf->webseedCount = 0;
        inf->webseeds = tr_new0( char*, n );

        for( i = 0; i < n; ++i )
        {
            if( tr_bencGetStr( tr_bencListChild( urls, i ), &url ) )
            {
                char * fixed_url = fix_webseed_url( inf, url );

                if( fixed_url != NULL )
                    inf->webseeds[inf->webseedCount++] = fixed_url;
            }
        }
    }
    else if( tr_bencDictFindStr( meta, "url-list", &url ) && ( maxWebseeders > 0 ) ) /* handle single items in webseeds */
    {
        char * fixed_url = fix_webseed_url( inf, url );

        if( fixed_url != NULL )
        {
            inf->webseedCount = 1;
            inf->webseeds = tr_new0( char*, 1 );
            inf->webseeds[0] = fixed_url;
        }
    }
}

static const char*
tr_metainfoParseImpl( const tr_session  * session,
                      tr_info           * inf,
                      bool              * hasInfoDict,
                      int               * infoDictLength,
                      const tr_benc     * meta_in )
{
    int64_t         i;
    size_t          raw_len;
    const char *    str;
    const uint8_t * raw;
    tr_benc *       d;
    tr_benc *       infoDict = NULL;
    tr_benc *       meta = (tr_benc *) meta_in;
    bool            b;
    bool            isMagnet = false;

    /* info_hash: urlencoded 20-byte SHA1 hash of the value of the info key
     * from the Metainfo file. Note that the value will be a bencoded
     * dictionary, given the definition of the info key above. */
    b = tr_bencDictFindDict( meta, "info", &infoDict );
    if( hasInfoDict != NULL )
        *hasInfoDict = b;
    if( !b )
    {
        /* no info dictionary... is this a magnet link? */
        if( tr_bencDictFindDict( meta, "magnet-info", &d ) )
        {
            isMagnet = true;

            /* get the info-hash */
            if( !tr_bencDictFindRaw( d, "info_hash", &raw, &raw_len ) )
                return "info_hash";
            if( raw_len != SHA_DIGEST_LENGTH )
                return "info_hash";
            memcpy( inf->hash, raw, raw_len );
            tr_sha1_to_hex( inf->hashString, inf->hash );

            /* maybe get the display name */
            if( tr_bencDictFindStr( d, "display-name", &str ) ) {
                tr_free( inf->name );
                if( strlen( str ) > 50 )
                    inf->name = tr_strndup( str, 50 );
                else
                    inf->name = tr_strdup( str );
            }

            if( !inf->name )
                inf->name = tr_strdup( inf->hashString );
        }
        else /* not a magnet link and has no info dict... */
        {
            return "info";
        }
    }
    else
    {
        int len;
        char * bstr = tr_bencToStr( infoDict, TR_FMT_INFO_DICT, &len );
        tr_sha1( inf->hash, bstr, len, NULL );
        tr_sha1_to_hex( inf->hashString, inf->hash );

        if( infoDictLength != NULL )
            *infoDictLength = len;

        tr_free( bstr );
    }

    /* name */
    if( !isMagnet ) {
        if( !tr_bencDictFindStr( infoDict, "name.utf-8", &str ) )
            if( !tr_bencDictFindStr( infoDict, "name", &str ) )
                str = "";
        if( !str || !*str )
        {
            char * nowName;
            nowName = tr_strdup_printf( "no-name.%16.16s", inf->hashString );
            tr_free( inf->name );
            if( session && session->cleanUTFenabled )
                inf->name = tr_utf8clean( nowName, -1 );
            else
                inf->name = tr_strdup( nowName );
            tr_free( nowName );
        }
        else
        {
            tr_free( inf->name );
            if( session && session->cleanUTFenabled )
                inf->name = tr_utf8clean( str, -1 );
            else
                inf->name = tr_strdup( str );
        }
    }

    /* comment */
    if( !tr_bencDictFindStr( meta, "comment.utf-8", &str ) )
        if( !tr_bencDictFindStr( meta, "comment", &str ) )
            str = "";
    tr_free( inf->comment );
    if( session && session->cleanUTFenabled )
        inf->comment = tr_utf8clean( str, -1 );
    else
        inf->comment = tr_strdup( str );

    /* created by */
    if( !tr_bencDictFindStr( meta, "created by.utf-8", &str ) )
        if( !tr_bencDictFindStr( meta, "created by", &str ) )
            str = "";
    tr_free( inf->creator );
    if( session && session->cleanUTFenabled )
        inf->creator = tr_utf8clean( str, -1 );
    else
        inf->creator = tr_strdup( str );

    /* creation date */
    if( !tr_bencDictFindInt( meta, "creation date", &i ) )
        i = 0;
    inf->dateCreated = i;

    /* private */
    if( !tr_bencDictFindInt( infoDict, "private", &i ) )
        if( !tr_bencDictFindInt( meta, "private", &i ) )
            i = 0;
    inf->isPrivate = i != 0;

    /* piece length */
    if( !isMagnet ) {
        if( !tr_bencDictFindInt( infoDict, "piece length", &i ) || ( i < 1 ) || ( i >= 536870912 ) )
            return "piece length";
        inf->pieceSize = i;
    }

    /* pieces */
    if( !isMagnet ) {
        if( !tr_bencDictFindRaw( infoDict, "pieces", &raw, &raw_len ) )
            return "pieces";
        if( raw_len % SHA_DIGEST_LENGTH )
            return "pieces";
        inf->pieceCount = raw_len / SHA_DIGEST_LENGTH;
        inf->pieces = tr_new0( tr_piece, inf->pieceCount );
        for( i = 0; i < inf->pieceCount; ++i )
            memcpy( inf->pieces[i].hash, &raw[i * SHA_DIGEST_LENGTH],
                    SHA_DIGEST_LENGTH );
    }

    /* files */
    if( !isMagnet ) {
        if( ( str = parseFiles( inf, tr_bencDictFind( infoDict, "files" ),
                                     tr_bencDictFind( infoDict, "length" ), ( session ? session->cleanUTFenabled : false ) ) ) )
            return str;
        if( !inf->fileCount || !inf->totalSize )
            return "files";
        if( (uint64_t) inf->pieceCount !=
           ( inf->totalSize + inf->pieceSize - 1 ) / inf->pieceSize )
            return "files";
    }

    /* get announce or announce-list */
    if( ( str = getannounce( inf, meta ) ) )
        return str;

    /* get the url-list */
    if( !isMagnet && session )
        geturllist( inf, meta, session->maxWebseeds );

    /* filename of Transmission's copy */
    tr_free( inf->torrent );
    inf->torrent = session ? getTorrentFilename(session, inf, TR_METAINFO_BASENAME_NAME_AND_PARTIAL_HASH) : NULL;

    return NULL;
}

bool
tr_metainfoParse( const tr_session * session,
                  const tr_benc    * meta_in,
                  tr_info          * inf,
                  bool             * hasInfoDict,
                  int              * infoDictLength )
{
    const char * badTag = tr_metainfoParseImpl( session,
                                                inf,
                                                hasInfoDict,
                                                infoDictLength,
                                                meta_in );
    const bool success = badTag == NULL;

    if( badTag )
    {
        tr_nerr( inf->name, _( "Invalid metadata entry \"%s\"" ), badTag );
        tr_metainfoFree( inf );
    }

    return success;
}

void
tr_metainfoFree( tr_info * inf )
{
    int i;
    tr_file_index_t ff;

    for( i = 0; i < inf->webseedCount; ++i )
        tr_free( inf->webseeds[i] );

    for( ff = 0; ff < inf->fileCount; ++ff )
        tr_free( inf->files[ff].name );

    tr_free( inf->webseeds );
    tr_free( inf->pieces );
    tr_free( inf->files );
    tr_free( inf->comment );
    tr_free( inf->creator );
    tr_free( inf->torrent );
    tr_free( inf->name );

    for( i = 0; i < inf->trackerCount; ++i )
    {
        tr_free( inf->trackers[i].announce );
        tr_free( inf->trackers[i].scrape );
    }
    tr_free( inf->trackers );

    memset( inf, '\0', sizeof( tr_info ) );
}

void
tr_metainfoRemoveSaved( const tr_session * session, const tr_info * inf )
{
    char * filename;

    filename = getTorrentFilename(session, inf, TR_METAINFO_BASENAME_HASH);
    unlink( filename );
    tr_free( filename );

    filename = getTorrentFilename(session, inf, TR_METAINFO_BASENAME_NAME_AND_PARTIAL_HASH);
    unlink( filename );
    tr_free( filename );

    filename = getOldTorrentFilename( session, inf );
    unlink( filename );
    tr_free( filename );
}

void tr_metainfoMigrateFile(tr_session const* session, tr_info const* info, enum tr_metainfo_basename_format old_format,
    enum tr_metainfo_basename_format new_format)
{
    char* old_filename = getTorrentFilename(session, info, old_format);
    char* new_filename = getTorrentFilename(session, info, new_format);

    if (!rename(old_filename, new_filename))
    {
        tr_nerr( info->name, _( "Migrated torrent file from \"%s\" to \"%s\"" ),
                old_filename, new_filename );
    }

    tr_free(new_filename);
    tr_free(old_filename);
}
