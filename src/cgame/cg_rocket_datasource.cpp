/*
 * Daemon GPL source code
 * Copyright (C) 2015  Unreal Arena
 * Copyright (C) 2012  Unvanquished Developers
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include "cg_local.h"

static void AddToServerList( const char *name, const char *label, int clients, int bots, int ping, int maxClients, char *mapName, char *addr, int netSrc )
{
	server_t *node;

	if ( rocketInfo.data.serverCount[ netSrc ] == MAX_SERVERS )
	{
		return;
	}

	if ( !*name || !*mapName )
	{
		return;
	}

	node = &rocketInfo.data.servers[ netSrc ][ rocketInfo.data.serverCount[ netSrc ] ];

	node->name = BG_strdup( name );
	node->clients = clients;
	node->bots = bots;
	node->ping = ping;
	node->maxClients = maxClients;
	node->addr = BG_strdup( addr );
	node->label = BG_strdup( label );
	node->mapName = BG_strdup( mapName );

	rocketInfo.data.serverCount[ netSrc ]++;
}

static void CG_Rocket_SetServerListServer( const char *table, int index )
{
	int netSrc = CG_StringToNetSource( table );

	if ( !Q_stricmp( table, "severInfo" ) || !Q_stricmp( table, "serverPlayers" ) )
	{
		return;
	}

	rocketInfo.data.serverIndex[ netSrc ] = index;
	rocketInfo.currentNetSrc = netSrc;
	CG_Rocket_BuildServerInfo();
}
#define MAX_SERVERSTATUS_LINES 4096
void CG_Rocket_BuildServerInfo()
{
	static char serverInfoText[ MAX_SERVERSTATUS_LINES ];
	char buf[ MAX_INFO_STRING ];
	const char *p;
	server_t *server;
	int netSrc = rocketInfo.currentNetSrc;

	int serverIndex = rocketInfo.data.serverIndex[ netSrc ];

	if ( serverIndex >= rocketInfo.data.serverCount[ netSrc ] || serverIndex < 0 )
	{
		return;
	}

	if ( rocketInfo.realtime < rocketInfo.serverStatusLastRefresh + 500 )
	{
		return;
	}

	buf[0] = 0;

	rocketInfo.serverStatusLastRefresh = rocketInfo.realtime;

	if ( !rocketInfo.data.buildingServerInfo )
	{
		rocketInfo.data.buildingServerInfo = true;
	}

	server = &rocketInfo.data.servers[ netSrc ][ serverIndex ];


	if ( trap_LAN_ServerStatus( server->addr, serverInfoText, sizeof( serverInfoText ) ) )
	{
		int i = 0, score, ping;
		const char *start, *end;
		static char key[BIG_INFO_VALUE], value[ BIG_INFO_VALUE ];
		char name[ MAX_STRING_CHARS ];

		Rocket_DSClearTable( "server_browser", "serverInfo" );
		Rocket_DSClearTable( "server_browser", "serverPlayers" );

		p = serverInfoText;

		while ( *p )
		{
			Info_NextPair( &p, key, value );

			if ( key[ 0 ] )
			{
				Info_SetValueForKey( buf, "cvar", key, false );
				Info_SetValueForKey( buf, "value", value, false );
				Rocket_DSAddRow( "server_browser", "serverInfo", buf );
				*buf = '\0';
			}

			else
			{
				break;
			}
		}

		// Parse first set of players
		sscanf( value, "%d %d", &score, &ping );
		start = strchr( value, '"' );

		if ( !start )
		{
			trap_LAN_ResetServerStatus();
			rocketInfo.data.buildingServerInfo = false;
			return;
		}

		end = strchr( start + 1, '"' );

		if ( !end )
		{
			trap_LAN_ResetServerStatus();
			rocketInfo.data.buildingServerInfo = false;
			return;
		}

		Q_strncpyz( name, start + 1, end - start );
		start = end = NULL;
		Info_SetValueForKey( buf, "num", va( "%d", i++ ), false );
		Info_SetValueForKeyRocket( buf, "name", name, false );
		Info_SetValueForKey( buf, "score", va( "%d", score ), false );
		Info_SetValueForKey( buf, "ping", va( "%d", ping ), false );
		Rocket_DSAddRow( "server_browser", "serverPlayers", buf );

		while ( *p )
		{
			Info_NextPair( &p, key, value );

			if ( key[ 0 ] )
			{
				sscanf( key, "%d %d", &score, &ping );
				start = strchr( key, '"' );

				if ( !start )
				{
					break;
				}

				end = strchr( start + 1, '"' );

				if ( !end )
				{
					break;
				}

				Q_strncpyz( name, start + 1, end - start );
				start = end = NULL;
				Info_SetValueForKey( buf, "num", va( "%d", i++ ), false );
				Info_SetValueForKeyRocket( buf, "name", name, false );
				Info_SetValueForKey( buf, "score", va( "%d", score ), false );
				Info_SetValueForKey( buf, "ping", va( "%d", ping ), false );
				Rocket_DSAddRow( "server_browser", "serverPlayers", buf );
			}

			if ( value[ 0 ] )
			{
				sscanf( value, "%d %d", &score, &ping );
				start = strchr( value, '"' );

				if ( !start )
				{
					break;
				}

				end = strchr( start + 1, '"' );

				if ( !end )
				{
					break;
				}

				Q_strncpyz( name, start + 1, end - start );
				start = end = NULL;
				Info_SetValueForKey( buf, "num", va( "%d", i++ ), false );
				Info_SetValueForKeyRocket( buf, "name", name, false );
				Info_SetValueForKey( buf, "score", va( "%d", score ), false );
				Info_SetValueForKey( buf, "ping", va( "%d", ping ), false );
				Rocket_DSAddRow( "server_browser", "serverPlayers", buf );
			}
		}

		trap_LAN_ResetServerStatus();
		rocketInfo.data.buildingServerInfo = false;
	}

}

void CG_Rocket_BuildServerList( const char *args )
{
	char data[ MAX_INFO_STRING ] = { 0 };
	int netSrc = CG_StringToNetSource( args );
	int i;

	// Only refresh once every second
	if ( rocketInfo.realtime < 1000 + rocketInfo.serversLastRefresh )
	{
		return;
	}

	rocketInfo.serversLastRefresh = rocketInfo.realtime;
	rocketInfo.currentNetSrc = netSrc;

	if ( netSrc != AS_FAVORITES )
	{
		int numServers;

		rocketInfo.data.retrievingServers = true;

		Rocket_DSClearTable( "server_browser", args );
		CG_Rocket_CleanUpServerList( args );

		trap_LAN_MarkServerVisible( netSrc, -1, true );

		numServers = trap_LAN_GetServerCount( netSrc );

		// Still waiting for a response...
		if ( numServers == -1 )
		{
			return;
		}

		for ( i = 0; i < numServers; ++i )
		{
			char info[ MAX_STRING_CHARS ];
			int ping, bots, clients, maxClients;

			Com_Memset( &data, 0, sizeof( data ) );

			if ( !trap_LAN_ServerIsVisible( netSrc, i ) )
			{
				continue;
			}

			ping = trap_LAN_GetServerPing( netSrc, i );

			if ( ping >= 0 || !Q_stricmp( args, "favorites" ) )
			{
				char addr[ 50 ]; // long enough for IPv6 literal plus port no.
				char mapname[ 256 ];
				trap_LAN_GetServerInfo( netSrc, i, info, sizeof( info ) );

				bots = atoi( Info_ValueForKey( info, "bots" ) );
				clients = atoi( Info_ValueForKey( info, "clients" ) );
				maxClients = atoi( Info_ValueForKey( info, "sv_maxclients" ) );
				Q_strncpyz( addr, Info_ValueForKey( info, "addr" ), sizeof( addr ) );
				Q_strncpyz( mapname, Info_ValueForKey( info, "mapname" ), sizeof( mapname ) );
				AddToServerList( Info_ValueForKey( info, "hostname" ), Info_ValueForKey( info, "label" ), clients, bots, ping, maxClients, mapname, addr, netSrc );
			}
		}

		for ( i = 0; i < rocketInfo.data.serverCount[ netSrc ]; ++i )
		{
			if ( rocketInfo.data.servers[ netSrc ][ i ].ping <= 0 )
			{
				continue;
			}

			Info_SetValueForKey( data, "name", rocketInfo.data.servers[ netSrc ][ i ].name, false );
			Info_SetValueForKey( data, "players", va( "%d", rocketInfo.data.servers[ netSrc ][ i ].clients ), false );
			Info_SetValueForKey( data, "bots", va( "%d", rocketInfo.data.servers[ netSrc ][ i ].bots ), false );
			Info_SetValueForKey( data, "ping", va( "%d", rocketInfo.data.servers[ netSrc ][ i ].ping ), false );
			Info_SetValueForKey( data, "maxClients", va( "%d", rocketInfo.data.servers[ netSrc ][ i ].maxClients ), false );
			Info_SetValueForKey( data, "addr", rocketInfo.data.servers[ netSrc ][ i ].addr, false );
			Info_SetValueForKey( data, "label", rocketInfo.data.servers[ netSrc ][ i ].label, false );
			Info_SetValueForKey( data, "map", rocketInfo.data.servers[ netSrc ][ i ].mapName, false );

			Rocket_DSAddRow( "server_browser", args, data );

			if ( rocketInfo.data.retrievingServers )
			{
				rocketInfo.data.retrievingServers = false;
			}
		}

	}

	else if ( !Q_stricmp( args, "serverInfo" ) )
	{
		CG_Rocket_BuildServerInfo();
	}
}

static int ServerListCmpByPing( const void *one, const void *two )
{
	server_t* a = ( server_t* ) one;
	server_t* b = ( server_t* ) two;

	if ( a->ping > b->ping ) return 1;

	if ( b->ping > a->ping ) return -1;

	if ( a->ping == b->ping )  return 0;

	return 0; // silence compiler
}

static int ServerListCmpByName( const void* one, const void* two )
{
	static char cleanName1[ MAX_STRING_CHARS ] = { 0 };
	static char cleanName2[ MAX_STRING_CHARS ] = { 0 };
	server_t* a = ( server_t* ) one;
	server_t* b = ( server_t* ) two;

	Q_strncpyz( cleanName1, a->name, sizeof( cleanName1 ) );
	Q_strncpyz( cleanName2, b->name, sizeof( cleanName2 ) );

	Q_CleanStr( cleanName1 );
	Q_CleanStr( cleanName2 );

	return Q_stricmp( cleanName1, cleanName2 );
}

static int ServerListCmpByMap( const void* one, const void* two )
{
	server_t* a = ( server_t* ) one;
	server_t* b = ( server_t* ) two;

	return Q_stricmp( a->mapName, b->mapName );
}

static int ServerListCmpByPlayers( const void* one, const void* two )
{
	server_t* a = ( server_t* ) one;
	server_t* b = ( server_t* ) two;

	if ( a->clients > b->clients ) return 1;

	if ( b->clients > a->clients ) return -1;

	if ( a->clients == b->clients )  return 0;

	return 0; // silence compiler
}

static void CG_Rocket_SortServerList( const char *name, const char *sortBy )
{
	char data[ MAX_INFO_STRING ] = { 0 };
	int netSrc = CG_StringToNetSource( name );
	int  i;

	if ( !Q_stricmp( sortBy, "ping" ) )
	{
		qsort( rocketInfo.data.servers[ netSrc ], rocketInfo.data.serverCount[ netSrc ], sizeof( server_t ), &ServerListCmpByPing );
	}
	else if ( !Q_stricmp( sortBy, "name" ) )
	{
		qsort( rocketInfo.data.servers[ netSrc ], rocketInfo.data.serverCount[ netSrc ], sizeof( server_t ), &ServerListCmpByName );
	}
	else if ( !Q_stricmp( sortBy, "players" ) )
	{
		qsort( rocketInfo.data.servers[ netSrc ], rocketInfo.data.serverCount[ netSrc ], sizeof( server_t ), &ServerListCmpByPlayers );
	}
	else if ( !Q_stricmp( sortBy, "map" ) )
	{
		qsort( rocketInfo.data.servers[ netSrc ], rocketInfo.data.serverCount[ netSrc ], sizeof( server_t ), &ServerListCmpByMap );
	}

	Rocket_DSClearTable( "server_browser", name );

	for ( i = 0; i < rocketInfo.data.serverCount[ netSrc ]; ++i )
	{
		if ( rocketInfo.data.servers[ netSrc ][ i ].ping <= 0 )
		{
			continue;
		}

		Info_SetValueForKey( data, "name", rocketInfo.data.servers[ netSrc ][ i ].name, false );
		Info_SetValueForKey( data, "players", va( "%d", rocketInfo.data.servers[ netSrc ][ i ].clients ), false );
		Info_SetValueForKey( data, "bots", va( "%d", rocketInfo.data.servers[ netSrc ][ i ].bots ), false );
		Info_SetValueForKey( data, "ping", va( "%d", rocketInfo.data.servers[ netSrc ][ i ].ping ), false );
		Info_SetValueForKey( data, "maxClients", va( "%d", rocketInfo.data.servers[ netSrc ][ i ].maxClients ), false );
		Info_SetValueForKey( data, "addr", rocketInfo.data.servers[ netSrc ][ i ].addr, false );
		Info_SetValueForKey( data, "label", rocketInfo.data.servers[ netSrc ][ i ].label, false );
		Info_SetValueForKey( data, "map", rocketInfo.data.servers[ netSrc ][ i ].mapName, false );

		Rocket_DSAddRow( "server_browser", name, data );
	}
}

void CG_Rocket_CleanUpServerList( const char *table )
{
	int i;
	int j;
	int netSrc = CG_StringToNetSource( table );

	for ( i = AS_LOCAL; i <= AS_FAVORITES; ++i )
	{
		if ( !table || !*table || i == netSrc )
		{
			for ( j = 0; j < rocketInfo.data.serverCount[ i ]; ++j )
			{
				BG_Free( rocketInfo.data.servers[ i ][ j ].name );
				BG_Free( rocketInfo.data.servers[ i ][ j ].label );
				BG_Free( rocketInfo.data.servers[ i ][ j ].addr );
				BG_Free( rocketInfo.data.servers[ i ][ j ].mapName );
				rocketInfo.data.serverCount[ i ] = 0;
			}
		}
	}
}

static void CG_Rocket_FilterServerList( const char *table, const char *filter )
{
	const char *str = ( table && *table ) ? table : CG_NetSourceToString( rocketInfo.currentNetSrc );
	int netSrc = CG_StringToNetSource( str );
	int i;

	Rocket_DSClearTable( "server_browser", str );

	for ( i = 0; i < rocketInfo.data.serverCount[ netSrc ]; ++i )
	{
		char name[ MAX_INFO_VALUE ];

		Q_strncpyz( name, rocketInfo.data.servers[ netSrc ][ i ].name, sizeof( name ) );

		if ( Q_stristr( Q_CleanStr( name ), filter ) )
		{
			char data[ MAX_INFO_STRING ] = { 0 };

			Info_SetValueForKey( data, "name", rocketInfo.data.servers[ netSrc ][ i ].name, false );
			Info_SetValueForKey( data, "players", va( "%d", rocketInfo.data.servers[ netSrc ][ i ].clients ), false );
			Info_SetValueForKey( data, "bots", va( "%d", rocketInfo.data.servers[ netSrc ][ i ].bots ), false );
			Info_SetValueForKey( data, "ping", va( "%d", rocketInfo.data.servers[ netSrc ][ i ].ping ), false );
			Info_SetValueForKey( data, "maxClients", va( "%d", rocketInfo.data.servers[ netSrc ][ i ].maxClients ), false );
			Info_SetValueForKey( data, "addr", rocketInfo.data.servers[ netSrc ][ i ].addr, false );
			Info_SetValueForKey( data, "label", rocketInfo.data.servers[ netSrc ][ i ].label, false );

			Rocket_DSAddRow( "server_browser", str, data );
		}
	}
}

void CG_Rocket_ExecServerList( const char *table )
{
	int netSrc = CG_StringToNetSource( table );
	trap_SendConsoleCommand( va( "connect %s", rocketInfo.data.servers[ netSrc ][ rocketInfo.data.serverIndex[ netSrc ] ].addr ) );
}

static bool Parse( const char **p, char **out )
{
	char *token;

	token = COM_ParseExt( p, false );

	if ( token && token[ 0 ] != 0 )
	{
		* ( out ) = BG_strdup( token );
		return true;
	}

	return false;
}

static void AddToResolutionList( int w, int h )
{
	resolution_t *node;

	if ( rocketInfo.data.resolutionCount == MAX_RESOLUTIONS )
	{
		return;
	}

	node = &rocketInfo.data.resolutions[ rocketInfo.data.resolutionCount ];

	node->width = w;
	node->height = h;
	rocketInfo.data.resolutionCount++;
}

void CG_Rocket_SetResolutionListResolution( const char*, int index )
{
	if ( index < rocketInfo.data.resolutionCount && index >= 0 )
	{
		trap_Cvar_Set("r_customwidth", va( "%d", rocketInfo.data.resolutions[ index ].width ) );
		trap_Cvar_Set("r_customheight", va( "%d", rocketInfo.data.resolutions[ index ].height ) );
		trap_Cvar_Set("r_mode", "-1" );
		rocketInfo.data.resolutionIndex = index;
	}
}

void CG_Rocket_BuildResolutionList( const char* )
{
	char        buf[ MAX_STRING_CHARS ];
	int         w, h, currentW, currentH;
	const char        *p;
	char        *out;
	int          i;

	currentW = trap_Cvar_VariableIntegerValue( "r_customwidth" );
	currentH = trap_Cvar_VariableIntegerValue( "r_customheight" );
	trap_Cvar_VariableStringBuffer( "r_availableModes", buf, sizeof( buf ) );
	p = buf;
	rocketInfo.data.resolutionCount = 0;
	rocketInfo.data.resolutionIndex = -1;

	while ( Parse( &p, &out ) )
	{

		sscanf( out, "%dx%d", &w, &h );
		AddToResolutionList( w, h );
		BG_Free( out );
	}

	buf[ 0 ] = '\0';

	Rocket_DSClearTable( "resolutions", "default" );

	for ( i = 0; i < rocketInfo.data.resolutionCount; ++i )
	{
		w = rocketInfo.data.resolutions[ i ].width;
		h = rocketInfo.data.resolutions[ i ].height;
		if ( w == currentW && h == currentH )
		{
			rocketInfo.data.resolutionIndex = i;
		}
		Info_SetValueForKey( buf, "width", va( "%d", w ), false );
		Info_SetValueForKey( buf, "height", va( "%d", h ), false );
		Rocket_DSAddRow( "resolutions", "default", buf );
	}

	if ( rocketInfo.data.resolutionIndex == -1 )
	{
		Info_SetValueForKey( buf, "width", va( "%d", -1 ), false );
		Info_SetValueForKey( buf, "height", va( "%d", -1 ), false );
		Rocket_DSAddRow( "resolutions", "default", buf );
		rocketInfo.data.resolutionIndex = rocketInfo.data.resolutionCount;
	}

}

static int ResolutionListCmpByWidth( const void *one, const void *two )
{
	resolution_t *a = ( resolution_t * ) one;
	resolution_t *b = ( resolution_t * ) two;

	if ( a->width > b->width ) return -1;

	if ( b->width > a->width ) return 1;

	if ( a->width == b->width )  return 0;

	return 0; // silence compiler
}

void CG_Rocket_SortResolutionList( const char*, const char *sortBy )
{
	static char buf[ MAX_STRING_CHARS ];
	int i;

	if ( !Q_stricmp( sortBy, "width" ) )
	{
		qsort( rocketInfo.data.resolutions, rocketInfo.data.resolutionCount, sizeof( resolution_t ), &ResolutionListCmpByWidth );
	}

	Rocket_DSClearTable( "resolutions", "default" );

	for ( i = 0; i < rocketInfo.data.resolutionCount; ++i )
	{
		Info_SetValueForKey( buf, "width", va( "%d", rocketInfo.data.resolutions[ i ].width ), false );
		Info_SetValueForKey( buf, "height", va( "%d", rocketInfo.data.resolutions[ i ].height ), false );

		Rocket_DSAddRow( "resolutions", "default", buf );
	}
}

int CG_Rocket_GetResolutionListIndex( const char* )
{
	if ( !rocketInfo.data.resolutionCount)
	{
		CG_Rocket_BuildResolutionList( nullptr );
	}
	return rocketInfo.data.resolutionIndex;
}

void CG_Rocket_CleanUpResolutionList( const char* )
{
	rocketInfo.data.resolutionCount = 0;
}

static void AddToLanguageList( char *name, char *lang )
{
	language_t *node;

	if ( rocketInfo.data.languageCount == MAX_LANGUAGES )
	{
		return;
	}

	node = &rocketInfo.data.languages[ rocketInfo.data.languageCount ];

	node->name = name;
	node->lang = lang;
	rocketInfo.data.languageCount++;
}

void CG_Rocket_SetLanguageListLanguage( const char*, int index )
{
	if ( index > 0 && index < rocketInfo.data.languageCount )
	{
		rocketInfo.data.languageIndex = index;
		trap_Cvar_Set( "language", rocketInfo.data.languages[ index ].lang );
	}
}

// FIXME: use COM_Parse or something instead of this way
void CG_Rocket_BuildLanguageList( const char* )
{
	char        buf[ MAX_STRING_CHARS ], temp[ MAX_TOKEN_CHARS ], language[ 32 ];
	int         index = 0, lang = 0;
	bool    quoted = false;
	char        *p;

	trap_Cvar_VariableStringBuffer( "trans_languages", buf, sizeof( buf ) );
	trap_Cvar_VariableStringBuffer( "language", language, sizeof( language ) );

	p = buf;
	memset( &temp, 0, sizeof( temp ) );

	while ( p && *p )
	{
		if ( *p == '"' && quoted )
		{
			temp[ index ] = '\0';
			AddToLanguageList( BG_strdup( temp ), nullptr );
			quoted = false;
			index = 0;
		}

		else if ( *p == '"' || quoted )
		{
			if ( !quoted )
			{
				p++;
			}

			quoted = true;
			temp[ index++ ] = *p;
		}

		p++;
	}

	trap_Cvar_VariableStringBuffer( "trans_encodings", buf, sizeof( buf ) );
	p = buf;
	memset( &temp, 0, sizeof( temp ) );

	while ( p && *p )
	{
		if ( *p == '"' && quoted )
		{
			temp[ index ] = '\0';

			// Set the current language index
			if( !Q_stricmp( temp, language ) )
			{
				rocketInfo.data.languageIndex = lang;
			}

			rocketInfo.data.languages[ lang++ ].lang = BG_strdup( temp );
			quoted = false;
			index = 0;
		}

		else if ( *p == '"' || quoted )
		{
			if ( !quoted )
			{
				p++;
			}

			quoted = true;
			temp[ index++ ] = *p;
		}

		p++;
	}

	buf[ 0 ] = '\0';

	for ( index = 0; index < rocketInfo.data.languageCount; ++index )
	{
		Info_SetValueForKey( buf, "name", rocketInfo.data.languages[ index ].name, false );
		Info_SetValueForKey( buf, "lang", rocketInfo.data.languages[ index ].lang, false );

		Rocket_DSAddRow( "languages", "default", buf );
	}
}

int CG_Rocket_GetLanguageListIndex( const char* )
{
	if ( !rocketInfo.data.languageCount )
	{
		CG_Rocket_BuildLanguageList( nullptr );
	}

	return rocketInfo.data.languageIndex;
}

void CG_Rocket_CleanUpLanguageList( const char* )
{
	int i;

	for ( i = 0; i < rocketInfo.data.languageCount; ++i )
	{
		BG_Free( rocketInfo.data.languages[ i ].lang );
		BG_Free( rocketInfo.data.languages[ i ].name );
	}

	rocketInfo.data.languageCount = 0;
}

static void AddToVoipInputs( char *name )
{
	if ( rocketInfo.data.voipInputsCount == MAX_INPUTS )
	{
		return;
	}

	rocketInfo.data.voipInputs[ rocketInfo.data.voipInputsCount++ ] = name;
}

void CG_Rocket_SetVoipInputsInput( const char*, int index )
{
	rocketInfo.data.voipInputIndex = index;
}

void CG_Rocket_BuildVoIPInputs( const char* )
{
	char buf[ MAX_STRING_CHARS ];
	char *p, *head;
	int inputs = 0;

	trap_Cvar_VariableStringBuffer( "audio.al.availableCaptureDevices", buf, sizeof( buf ) );
	head = buf;

	while ( ( p = strchr( head, '\n' ) ) )
	{
		*p = '\0';
		AddToVoipInputs( BG_strdup( head ) );
		head = p + 1;
	}

	buf[ 0 ] = '\0';

	for ( inputs = 0; inputs < rocketInfo.data.voipInputsCount; ++inputs )
	{
		Info_SetValueForKey( buf, "name", rocketInfo.data.voipInputs[ inputs ], false );

		Rocket_DSAddRow( "voipInputs", "default", buf );
	}
}

void CG_Rocket_CleanUpVoIPInputs( const char* )
{
	int i;

	for ( i = 0; i < rocketInfo.data.voipInputsCount; ++i )
	{
		BG_Free( rocketInfo.data.voipInputs[ i ] );
	}

	rocketInfo.data.voipInputsCount = 0;
}

static void AddToAlOutputs( char *name )
{
	if ( rocketInfo.data.alOutputsCount == MAX_OUTPUTS )
	{
		return;
	}

	rocketInfo.data.alOutputs[ rocketInfo.data.alOutputsCount++ ] = name;
}

void CG_Rocket_SetAlOutputsOutput( const char*, int index )
{
	rocketInfo.data.alOutputIndex = index;
	trap_Cvar_Set( "audio.al.device", rocketInfo.data.alOutputs[ index ] );
	trap_Cvar_AddFlags( "audio.al.device", CVAR_ARCHIVE );
}

void CG_Rocket_BuildAlOutputs( const char* )
{
	char buf[ MAX_STRING_CHARS ], currentDevice[ MAX_STRING_CHARS ];
	char *p, *head;
	int outputs = 0;

	trap_Cvar_VariableStringBuffer( "audio.al.device", currentDevice, sizeof( currentDevice ) );
	trap_Cvar_VariableStringBuffer( "audio.al.availableDevices", buf, sizeof( buf ) );
	head = buf;

	while ( ( p = strchr( head, '\n' ) ) )
	{
		*p = '\0';

		// Set current device
		if ( !Q_stricmp( currentDevice, head ) )
		{
			rocketInfo.data.alOutputIndex = rocketInfo.data.alOutputsCount;
		}

		AddToAlOutputs( BG_strdup( head ) );
		head = p + 1;
	}

	buf[ 0 ] = '\0';

	for ( outputs = 0; outputs < rocketInfo.data.alOutputsCount; ++outputs )
	{
		Info_SetValueForKey( buf, "name", rocketInfo.data.alOutputs[ outputs ], false );

		Rocket_DSAddRow( "alOutputs", "default", buf );
	}
}

void CG_Rocket_CleanUpAlOutputs( const char* )
{
	int i;

	for ( i = 0; i < rocketInfo.data.alOutputsCount; ++i )
	{
		BG_Free( rocketInfo.data.alOutputs[ i ] );
	}

	rocketInfo.data.alOutputsCount = 0;
}

void CG_Rocket_SetModListMod( const char*, int index )
{
	rocketInfo.data.modIndex = index;
}

void CG_Rocket_BuildModList( const char* )
{
	int   numdirs;
	char  dirlist[ 2048 ];
	char  *dirptr;
	char  *descptr;
	int   i;
	int   dirlen;

	rocketInfo.data.modCount = 0;
	numdirs = trap_FS_GetFileList( "$modlist", "", dirlist, sizeof( dirlist ) );
	dirptr = dirlist;

	for ( i = 0; i < numdirs; i++ )
	{
		dirlen = strlen( dirptr ) + 1;
		descptr = dirptr + dirlen;
		rocketInfo.data.modList[ rocketInfo.data.modCount ].name = BG_strdup( dirptr );
		rocketInfo.data.modList[ rocketInfo.data.modCount ].description = BG_strdup( descptr );
		dirptr += dirlen + strlen( descptr ) + 1;
		rocketInfo.data.modCount++;

		if ( rocketInfo.data.modCount >= MAX_MODS )
		{
			break;
		}
	}

	dirlist[ 0 ] = '\0';

	for ( i = 0; i < rocketInfo.data.modCount; ++i )
	{
		Info_SetValueForKey( dirlist, "name", rocketInfo.data.modList[ i ].name, false );
		Info_SetValueForKey( dirlist, "description", rocketInfo.data.modList[ i ].description, false );

		Rocket_DSAddRow( "modList", "default", dirlist );
	}
}

void CG_Rocket_CleanUpModList( const char* )
{
	int i;

	for ( i = 0; i < rocketInfo.data.modCount; ++i )
	{
		BG_Free( rocketInfo.data.modList[ i ].name );
		BG_Free( rocketInfo.data.modList[ i ].description );
	}

	rocketInfo.data.modCount = 0;
}

void CG_Rocket_SetDemoListDemo( const char*, int index )
{
	rocketInfo.data.demoIndex = index;
}

void CG_Rocket_ExecDemoList( const char* )
{
	trap_SendConsoleCommand( va( "demo %s", rocketInfo.data.demoList[ rocketInfo.data.demoIndex ] ) );
}

void CG_Rocket_BuildDemoList( const char* )
{
	char  demolist[ 4096 ];
	char demoExt[ 32 ];
	char  *demoname;
	int   i, len;

	Com_sprintf( demoExt, sizeof( demoExt ), "dm_%d", ( int ) trap_Cvar_VariableIntegerValue( "protocol" ) );

	rocketInfo.data.demoCount = trap_FS_GetFileList( "demos", demoExt, demolist, 4096 );

	Com_sprintf( demoExt, sizeof( demoExt ), ".dm_%d", ( int ) trap_Cvar_VariableIntegerValue( "protocol" ) );

	if ( rocketInfo.data.demoCount )
	{
		if ( rocketInfo.data.demoCount > MAX_DEMOS )
		{
			rocketInfo.data.demoCount = MAX_DEMOS;
		}

		demoname = demolist;

		for ( i = 0; i < rocketInfo.data.demoCount; i++ )
		{
			len = strlen( demoname );

			if ( !Q_stricmp( demoname +  len - strlen( demoExt ), demoExt ) )
			{
				demoname[ len - strlen( demoExt ) ] = '\0';
			}

			rocketInfo.data.demoList[ i ] = BG_strdup( demoname );
			demoname += len + 1;
		}
	}

	demolist[ 0 ] = '\0';

	for ( i = 0; i < rocketInfo.data.demoCount; ++i )
	{
		Info_SetValueForKey( demolist, "name", rocketInfo.data.demoList[ i ], false );

		Rocket_DSAddRow( "demoList", "default", demolist );
	}
}

void CG_Rocket_CleanUpDemoList( const char* )
{
	int i;

	for ( i = 0; i < rocketInfo.data.demoCount; ++i )
	{
		BG_Free( rocketInfo.data.demoList[ i ] );
	}

	rocketInfo.data.demoCount = 0;
}

void CG_Rocket_BuildPlayerList( const char* )
{
	char buf[ MAX_INFO_STRING ] = { 0 };
	clientInfo_t *ci;
	score_t *score;
	int i;

	//CG_RequestScores();

	// Do not build list if not currently playing
	if ( rocketInfo.cstate.connState < CA_ACTIVE )
	{
		return;
	}

	// Clear old values. Always build all three teams.
	Rocket_DSClearTable( "playerList", "spectators" );
	Rocket_DSClearTable( "playerList", "q" );
	Rocket_DSClearTable( "playerList", "u" );

	for ( i = 0; i < cg.numScores; ++i )
	{
		score = &cg.scores[ i ];
		ci = &cgs.clientinfo[ score->client ];

		if ( !ci->infoValid )
		{
			continue;
		}

		Info_SetValueForKey( buf, "num", va( "%d", score->client ), false );
		Info_SetValueForKey( buf, "score", va( "%d", score->score ), false );
		Info_SetValueForKey( buf, "ping", va( "%d", score->ping ), false );
		Info_SetValueForKey( buf, "weapon", va( "%d", score->weapon ), false );
		Info_SetValueForKey( buf, "upgrade", va( "%d", score->upgrade ), false );
		Info_SetValueForKey( buf, "time", va( "%d", score->time ), false );
		Info_SetValueForKey( buf, "credits", va( "%d", ci->credit ), false );
		Info_SetValueForKey( buf, "location", CG_ConfigString( CS_LOCATIONS + ci->location ), false );

		switch ( score->team )
		{
			case TEAM_Q:
				rocketInfo.data.playerList[ score->team ][ rocketInfo.data.playerCount[ TEAM_Q ]++ ] = i;
				Rocket_DSAddRow( "playerList", "q", buf );
				break;

			case TEAM_U:
				rocketInfo.data.playerList[ score->team ][ rocketInfo.data.playerIndex[ TEAM_U ]++ ] = i;
				Rocket_DSAddRow( "playerList", "u", buf );
				break;

			case TEAM_NONE:
				rocketInfo.data.playerList[ score->team ][ rocketInfo.data.playerCount[ TEAM_NONE ]++ ] = i;
				Rocket_DSAddRow( "playerList", "spectators", buf );
				break;
		}
	}

}

static int PlayerListCmpByScore( const void *one, const void *two )
{
	int *a = ( int * ) one;
	int *b = ( int * ) two;

	if ( cg.scores[ *a ].score > cg.scores[ *b ].score ) return 1;

	if ( cg.scores[ *b ].score > cg.scores[ *a ].score ) return -1;

	if ( cg.scores[ *a ].score == cg.scores[ *b ].score )  return 0;

	return 0; // silence compiler
}

void CG_Rocket_SortPlayerList( const char*, const char *sortBy )
{
	int i;
	clientInfo_t *ci;
	score_t *score;
	char buf[ MAX_INFO_STRING ];

	// Do not sort list if not currently playing
	if ( rocketInfo.cstate.connState < CA_ACTIVE )
	{
		return;
	}



	if ( !Q_stricmp( "score", sortBy ) )
	{
		qsort( rocketInfo.data.playerList[ TEAM_NONE ], rocketInfo.data.playerCount[ TEAM_NONE ], sizeof( int ), &PlayerListCmpByScore );
		qsort( rocketInfo.data.playerList[ TEAM_Q ], rocketInfo.data.playerCount[ TEAM_Q ], sizeof( int ), &PlayerListCmpByScore );
		qsort( rocketInfo.data.playerList[ TEAM_U ], rocketInfo.data.playerIndex[ TEAM_U ], sizeof( int ), &PlayerListCmpByScore );
	}

	// Clear old values. Always build all three teams.
	Rocket_DSClearTable( "playerList", "spectators" );
	Rocket_DSClearTable( "playerList", "q" );
	Rocket_DSClearTable( "playerList", "u" );

	for ( i = 0; i < rocketInfo.data.playerCount[ TEAM_NONE ]; ++i )
	{
		score = &cg.scores[ rocketInfo.data.playerList[ TEAM_NONE ][ i ] ];
		ci = &cgs.clientinfo[ score->client ];

		if ( !ci->infoValid )
		{
			continue;
		}

		Info_SetValueForKey( buf, "num", va( "%d", score->client ), false );
		Info_SetValueForKey( buf, "score", va( "%d", score->score ), false );
		Info_SetValueForKey( buf, "ping", va( "%d", score->ping ), false );
		Info_SetValueForKey( buf, "weapon", va( "%d", score->weapon ), false );
		Info_SetValueForKey( buf, "upgrade", va( "%d", score->upgrade ), false );
		Info_SetValueForKey( buf, "time", va( "%d", score->time ), false );
		Info_SetValueForKey( buf, "credits", va( "%d", ci->credit ), false );
		Info_SetValueForKey( buf, "location", CG_ConfigString( CS_LOCATIONS + ci->location ), false );

		Rocket_DSAddRow( "playerList", "spectators", buf );
	}

	for ( i = 0; i < rocketInfo.data.playerIndex[ TEAM_U ]; ++i )
	{
		score = &cg.scores[ rocketInfo.data.playerList[ TEAM_U ][ i ] ];
		ci = &cgs.clientinfo[ score->client ];

		if ( !ci->infoValid )
		{
			continue;
		}

		Info_SetValueForKey( buf, "num", va( "%d", score->client ), false );
		Info_SetValueForKey( buf, "score", va( "%d", score->score ), false );
		Info_SetValueForKey( buf, "ping", va( "%d", score->ping ), false );
		Info_SetValueForKey( buf, "weapon", va( "%d", score->weapon ), false );
		Info_SetValueForKey( buf, "upgrade", va( "%d", score->upgrade ), false );
		Info_SetValueForKey( buf, "time", va( "%d", score->time ), false );
		Info_SetValueForKey( buf, "credits", va( "%d", ci->credit ), false );
		Info_SetValueForKey( buf, "location", CG_ConfigString( CS_LOCATIONS + ci->location ), false );
		Rocket_DSAddRow( "playerList", "u", buf );
	}

	for ( i = 0; i < rocketInfo.data.playerCount[ TEAM_Q ]; ++i )
	{
		ci = &cgs.clientinfo[ rocketInfo.data.playerList[ TEAM_Q ][ i ] ];
		score = &cg.scores[ rocketInfo.data.playerList[ TEAM_Q ][ i ] ];

		if ( !ci->infoValid )
		{
			continue;
		}

		Info_SetValueForKey( buf, "num", va( "%d", score->client ), false );
		Info_SetValueForKey( buf, "score", va( "%d", score->score ), false );
		Info_SetValueForKey( buf, "ping", va( "%d", score->ping ), false );
		Info_SetValueForKey( buf, "weapon", va( "%d", score->weapon ), false );
		Info_SetValueForKey( buf, "upgrade", va( "%d", score->upgrade ), false );
		Info_SetValueForKey( buf, "time", va( "%d", score->time ), false );
		Info_SetValueForKey( buf, "credits", va( "%d", ci->credit ), false );
		Info_SetValueForKey( buf, "location", CG_ConfigString( CS_LOCATIONS + ci->location ), false );

		Rocket_DSAddRow( "playerList", "q", buf );
	}
}

void CG_Rocket_BuildMapList( const char* )
{
	int i;

	Rocket_DSClearTable( "mapList", "default" );
	trap_FS_LoadAllMapMetadata();
	CG_LoadArenas();

	for ( i = 0; i < rocketInfo.data.mapCount; ++i )
	{
		char buf[ MAX_INFO_STRING ] = { 0 };
		Info_SetValueForKey( buf, "num", va( "%d", i ), false );
		Info_SetValueForKey( buf, "mapName", rocketInfo.data.mapList[ i ].mapName, false );
		Info_SetValueForKey( buf, "mapLoadName", rocketInfo.data.mapList[ i ].mapLoadName, false );

		Rocket_DSAddRow( "mapList", "default", buf );
	}

}

void CG_Rocket_CleanUpMapList( const char* )
{
	int i;

	for ( i = 0; i < rocketInfo.data.mapCount; ++i )
	{
		BG_Free( rocketInfo.data.mapList[ i ].mapLoadName );
		BG_Free( rocketInfo.data.mapList[ i ].mapName );
	}

	rocketInfo.data.mapCount = 0;
}

void CG_Rocket_SetMapListIndex( const char*, int index )
{
	rocketInfo.data.mapIndex = index;
}


void CG_Rocket_CleanUpPlayerList( const char* )
{
	rocketInfo.data.playerCount[ TEAM_Q ] = 0;
	rocketInfo.data.playerIndex[ TEAM_U ] = 0;
	rocketInfo.data.playerCount[ TEAM_NONE ] = 0;
}

void CG_Rocket_SetPlayerListPlayer( const char*, int )
{
}

void CG_Rocket_BuildTeamList( const char* )
{
	static const char *data[] =
	{
		"\\name\\Q\\description\\The Q Team\n\n"
		"The Q team strengths are in movement and the ability to "
		"quickly construct new bases quickly. They possess a range "
		"of abilities including basic melee attacks, movement-"
		"crippling poisons and more.\\"
		,
		"\\name\\U\\description\\The U Team\n\n"
		"The U team is the masters of technology. Although their "
		"bases take long to construct, their automated defense "
		"ensures they stay built. A wide range of upgrades and "
		"weapons are available to the U team, each contributing "
		"to eradicate the Q threat.\\"
		,
		"\\name\\Spectate\\description\\Watch the game without playing.\\"
		,
		"\\name\\Auto Select\\description\\Join the team with the least players.\\"
		,
		nullptr
	};

	int i = 0;

	Rocket_DSClearTable( "teamList", "default" );

	while ( data[ i ] )
	{
		Rocket_DSAddRow( "teamList", "default", data[ i++ ] );
	}
}

void CG_Rocket_SetTeamList( const char*, int index )
{
	rocketInfo.data.selectedTeamIndex = index;
}

void CG_Rocket_ExecTeamList( const char* )
{
	const char *cmd = nullptr;

	switch ( rocketInfo.data.selectedTeamIndex )
	{
		case 0:
			cmd = "team q";
			break;

		case 1:
			cmd = "team u";
			break;

		case 2:
			cmd = "team spectate";
			break;

		case 3:
			cmd = "team auto";
			break;
	}

	if ( cmd )
	{
		trap_SendConsoleCommand( cmd );
		Rocket_DocumentAction( rocketInfo.menu[ ROCKETMENU_TEAMSELECT ].id, "hide" );
	}
}

void CG_Rocket_CleanUpTeamList( const char* )
{
	rocketInfo.data.selectedTeamIndex = -1;
}


enum
{
    ROCKETDS_BOTH,
    ROCKETDS_WEAPONS,
    ROCKETDS_UPGRADES
};

//////// beacon shit


void CG_Rocket_CleanUpBeaconList( const char* )
{
	rocketInfo.data.selectedBeacon = -1;
	rocketInfo.data.beaconListCount = 0;
}

void CG_Rocket_BuildBeaconList( const char *table )
{
	static char buf[ MAX_STRING_CHARS ];

	if ( rocketInfo.cstate.connState < CA_ACTIVE )
	{
		return;
	}

	if ( !Q_stricmp( table, "default" ) )
	{
		int i;
		const beaconAttributes_t *ba;

		Rocket_DSClearTable( "beaconList", "default" );
		CG_Rocket_CleanUpBeaconList( "default" );

		for ( i = BCT_NONE + 1; i < NUM_BEACON_TYPES; i++ )
		{
			ba = BG_Beacon( i );

			if( ba->flags & BCF_RESERVED )
				continue;

			buf[ 0 ] = '\0';

			Info_SetValueForKey( buf, "num", va( "%d", i ), false );
			Info_SetValueForKey( buf, "name", ba->humanName, false );
			Info_SetValueForKey( buf, "desc", ba->desc, false );

			Rocket_DSAddRow( "beaconList", "default", buf );

			rocketInfo.data.beaconList[ rocketInfo.data.beaconListCount++ ] = i;
		}
	}
}

void CG_Rocket_SetBeaconList( const char*, int index )
{
	rocketInfo.data.selectedBeacon = index;
}

void CG_Rocket_ExecBeaconList( const char* )
{
	const beaconAttributes_t *ba;

	ba = BG_Beacon( rocketInfo.data.beaconList[ rocketInfo.data.selectedBeacon ] );

	if( !ba )
		return;

	trap_SendClientCommand( va( "beacon %s", ba->name ) );
	Rocket_DocumentAction( rocketInfo.menu[ ROCKETMENU_BEACONS ].id, "hide" );
}

static void nullSortFunc( const char*, const char* )
{
}

static void nullExecFunc( const char* )
{
}

static void nullFilterFunc( const char*, const char* )
{
}

static int nullGetFunc( const char* )
{
	return -1;
}

typedef struct
{
	const char *name;
	void ( *build )( const char *args );
	void ( *sort )( const char *name, const char *sortBy );
	void ( *cleanup )( const char *table );
	void ( *set )( const char *table, int index );
	void ( *filter )( const char *table, const char *filter );
	void ( *exec )( const char *table );
	int  ( *get )( const char *table );
} dataSourceCmd_t;

static const dataSourceCmd_t dataSourceCmdList[] =
{
	{ "alOutputs", &CG_Rocket_BuildAlOutputs, &nullSortFunc, &CG_Rocket_CleanUpAlOutputs, &CG_Rocket_SetAlOutputsOutput, &nullFilterFunc, &nullExecFunc, &nullGetFunc },
	{ "beaconList", &CG_Rocket_BuildBeaconList, &nullSortFunc, &CG_Rocket_CleanUpBeaconList, &CG_Rocket_SetBeaconList, &nullFilterFunc, &CG_Rocket_ExecBeaconList, &nullGetFunc },
	{ "demoList", &CG_Rocket_BuildDemoList, &nullSortFunc, &CG_Rocket_CleanUpDemoList, &CG_Rocket_SetDemoListDemo, &nullFilterFunc, &CG_Rocket_ExecDemoList, &nullGetFunc },
	{ "languages", &CG_Rocket_BuildLanguageList, &nullSortFunc, &CG_Rocket_CleanUpLanguageList, &CG_Rocket_SetLanguageListLanguage, &nullFilterFunc, &nullExecFunc, &CG_Rocket_GetLanguageListIndex },
	{ "mapList", &CG_Rocket_BuildMapList, &nullSortFunc, &CG_Rocket_CleanUpMapList, &CG_Rocket_SetMapListIndex, &nullFilterFunc, &nullExecFunc, &nullGetFunc },
	{ "modList", &CG_Rocket_BuildModList, &nullSortFunc, &CG_Rocket_CleanUpModList, &CG_Rocket_SetModListMod, &nullFilterFunc, &nullExecFunc, &nullGetFunc },
	{ "playerList", &CG_Rocket_BuildPlayerList, &CG_Rocket_SortPlayerList, &CG_Rocket_CleanUpPlayerList, &CG_Rocket_SetPlayerListPlayer, &nullFilterFunc, &nullExecFunc, &nullGetFunc },
	{ "resolutions", &CG_Rocket_BuildResolutionList, &CG_Rocket_SortResolutionList, &CG_Rocket_CleanUpResolutionList, &CG_Rocket_SetResolutionListResolution, &nullFilterFunc, &nullExecFunc, &CG_Rocket_GetResolutionListIndex},
	{ "server_browser", &CG_Rocket_BuildServerList, &CG_Rocket_SortServerList, &CG_Rocket_CleanUpServerList, &CG_Rocket_SetServerListServer, &CG_Rocket_FilterServerList, &CG_Rocket_ExecServerList, &nullGetFunc },
	{ "teamList", &CG_Rocket_BuildTeamList, &nullSortFunc, &CG_Rocket_CleanUpTeamList, &CG_Rocket_SetTeamList, &nullFilterFunc, &CG_Rocket_ExecTeamList, &nullGetFunc },
	{ "voipInputs", &CG_Rocket_BuildVoIPInputs, &nullSortFunc, &CG_Rocket_CleanUpVoIPInputs, &CG_Rocket_SetVoipInputsInput, &nullFilterFunc, &nullExecFunc, &nullGetFunc },

};

static const size_t dataSourceCmdListCount = ARRAY_LEN( dataSourceCmdList );

static int dataSourceCmdCmp( const void *a, const void *b )
{
	return Q_stricmp( ( const char * ) a, ( ( dataSourceCmd_t * ) b )->name );
}

void CG_Rocket_BuildDataSource( const char *dataSrc, const char *table )
{
	dataSourceCmd_t *cmd;

	// No data
	if ( !dataSrc )
	{
		return;

	}

	cmd = ( dataSourceCmd_t * ) bsearch( dataSrc, dataSourceCmdList, dataSourceCmdListCount, sizeof( dataSourceCmd_t ), dataSourceCmdCmp );

	if ( cmd )
	{
		cmd->cleanup( table );
		cmd->build( table );
	}
}

void CG_Rocket_SortDataSource( const char *dataSource, const char *name, const char *sortBy )
{
	dataSourceCmd_t *cmd;

	cmd = ( dataSourceCmd_t * ) bsearch( dataSource, dataSourceCmdList, dataSourceCmdListCount, sizeof( dataSourceCmd_t ), dataSourceCmdCmp );

	if ( cmd && cmd->sort )
	{
		cmd->sort( name, sortBy );
	}
}

void CG_Rocket_SetDataSourceIndex( const char *dataSource, const char *table, int index )
{
	dataSourceCmd_t *cmd;

	cmd = ( dataSourceCmd_t * ) bsearch( dataSource, dataSourceCmdList, dataSourceCmdListCount, sizeof( dataSourceCmd_t ), dataSourceCmdCmp );

	if ( cmd && cmd->set )
	{
		cmd->set( table, index );
	}
}

void CG_Rocket_FilterDataSource( const char *dataSource, const char *table, const char *filter )
{
	dataSourceCmd_t *cmd;

	cmd = ( dataSourceCmd_t * ) bsearch( dataSource, dataSourceCmdList, dataSourceCmdListCount, sizeof( dataSourceCmd_t ), dataSourceCmdCmp );

	if ( cmd && cmd->filter )
	{
		cmd->filter( table, filter );
	}
}

void CG_Rocket_ExecDataSource( const char *dataSource, const char *table )
{
	dataSourceCmd_t *cmd;

	cmd = ( dataSourceCmd_t * ) bsearch( dataSource, dataSourceCmdList, dataSourceCmdListCount, sizeof( dataSourceCmd_t ), dataSourceCmdCmp );

	if ( cmd && cmd->exec )
	{
		cmd->exec( table );
	}
}

int CG_Rocket_GetDataSourceIndex( const char *dataSource, const char *table )
{
	dataSourceCmd_t *cmd;

	cmd = ( dataSourceCmd_t * ) bsearch( dataSource, dataSourceCmdList, dataSourceCmdListCount, sizeof( dataSourceCmd_t ), dataSourceCmdCmp );

	if ( cmd && cmd->get )
	{
		return cmd->get( table );
	}

	return -1;
}

void CG_Rocket_RegisterDataSources()
{
	for ( unsigned i = 0; i < dataSourceCmdListCount; ++i )
	{
		// Check that the commands are in increasing order so that it can be used by bsearch
		if ( i != 0 && Q_stricmp( dataSourceCmdList[ i - 1 ].name, dataSourceCmdList[ i ].name ) > 0 )
		{
			CG_Printf( "CGame dataSourceCmdList is in the wrong order for %s and %s\n", dataSourceCmdList[i - 1].name, dataSourceCmdList[ i ].name );
		}

		Rocket_RegisterDataSource( dataSourceCmdList[ i ].name );
	}
}

void CG_Rocket_CleanUpDataSources()
{
	for ( unsigned i = 0; i < dataSourceCmdListCount; ++i )
	{
		dataSourceCmdList[ i ].cleanup( nullptr );
	}
}