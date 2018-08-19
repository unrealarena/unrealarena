/*
 * Daemon GPL Source Code
 * Copyright (C) 2016  Unreal Arena
 * Copyright (C) 1999-2010  id Software LLC, a ZeniMax Media company.
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


#include "server.h"
#include "framework/CvarSystem.h"

/*
===============================================================================

OPERATOR CONSOLE ONLY COMMANDS

These commands can only be entered from stdin or by a remote operator datagram
===============================================================================
*/

class MapCmd: public Cmd::StaticCmd {
    public:
        MapCmd(Str::StringRef name, Str::StringRef description, bool cheat):
            Cmd::StaticCmd(name, Cmd::SYSTEM, description), cheat(cheat) {
        }

        void Run(const Cmd::Args& args) const OVERRIDE {
            if (args.Argc() < 2) {
                PrintUsage(args, "<mapname> (layoutname)", "loads a new map");
                return;
            }

            const std::string& mapName = args.Argv(1);

            //Detect any newly added paks
            FS::RefreshPaks();

            //Make sure the map exists to avoid typos that would kill the game
            if (!FS::FindPak("map-" + mapName)) {
                Print("Can't find map %s", mapName);
                return;
            }

            //Gets the layout list from the command but do not override if there is nothing
            std::string layouts = args.ConcatArgs(2);
            if (not layouts.empty()) {
                Cvar::SetValue("g_layouts", layouts);
            }

            Cvar::SetValueForce("sv_cheats", cheat ? "1" : "0");
            SV_SpawnServer(mapName.c_str());
        }

        Cmd::CompletionResult Complete(int argNum, const Cmd::Args& args, Str::StringRef prefix) const OVERRIDE {
            if (argNum == 1) {
                Cmd::CompletionResult out;
                for (auto& x: FS::GetAvailablePaks()) {
                    if (Str::IsPrefix("map-" + prefix, x.name))
                        out.push_back({x.name.substr(4), ""});
                }
                return out;
            } else if (argNum > 1) {
                return FS::HomePath::CompleteFilename(prefix, "game/layouts/" + args.Argv(1), ".dat", false, true);
            }

            return {};
        }

    private:
        bool cheat;
};
static MapCmd MapCmdRegistration("map", "starts a new map", false);
static MapCmd DevmapCmdRegistration("devmap", "starts a new map with cheats enabled", true);

void MSG_PrioritiseEntitystateFields();
void MSG_PrioritisePlayerStateFields();

static void SV_FieldInfo_f()
{
	MSG_PrioritiseEntitystateFields();
	MSG_PrioritisePlayerStateFields();
}

/*
================
SV_MapRestart_f

Completely restarts a level, but doesn't send a new gamestate to the clients.
This allows fair starts with variable load times.
================
*/
static void SV_MapRestart_f()
{
	int         i;
	client_t    *client;
	bool    denied;
	char        reason[ MAX_STRING_CHARS ];
	bool    isBot;

	// make sure we aren't restarting twice in the same frame
	if ( com_frameTime == sv.serverId )
	{
		return;
	}

	// make sure server is running
	if ( !com_sv_running->integer )
	{
		Log::Notice( "Server is not running.\n" );
		return;
	}

	// check for changes in variables that can't just be restarted
	// check for maxclients change
	if ( sv_maxclients->modified )
	{
		char mapname[ MAX_QPATH ];

#ifdef UNREALARENA
		Log::Notice( "sv_maxclients variable change - restarting.\n" );
#else
		Log::Notice( "sv_maxclients variable change — restarting.\n" );
#endif
		// restart the map the slow way
		Q_strncpyz( mapname, Cvar_VariableString( "mapname" ), sizeof( mapname ) );

		SV_SpawnServer( mapname );
		return;
	}

	// toggle the server bit so clients can detect that a
	// map_restart has happened
	svs.snapFlagServerBit ^= SNAPFLAG_SERVERCOUNT;

	// generate a new serverid
	// TTimo - don't update restartedserverId there, otherwise we won't deal correctly with multiple map_restart
	sv.serverId = com_frameTime;
	Cvar_Set( "sv_serverid", va( "%i", sv.serverId ) );

	// reset all the VM data in place without changing memory allocation
	// note that we do NOT set sv.state = SS_LOADING, so configstrings that
	// had been changed from their default values will generate broadcast updates
	sv.state = serverState_t::SS_LOADING;
	sv.restarting = true;

	SV_RestartGameProgs();

	// run a few frames to allow everything to settle
	for ( i = 0; i < GAME_INIT_FRAMES; i++ )
	{
		gvm.GameRunFrame( sv.time );
		svs.time += FRAMETIME;
		sv.time += FRAMETIME;
	}

	// create a baseline for more efficient communications
	// Gordon: meh, this won't work here as the client doesn't know it has happened
//  SV_CreateBaseline ();

	sv.state = serverState_t::SS_GAME;
	sv.restarting = false;

	// connect and begin all the clients
	for ( i = 0; i < sv_maxclients->integer; i++ )
	{
		client = &svs.clients[ i ];

		// send the new gamestate to all connected clients
		if ( client->state < clientState_t::CS_CONNECTED )
		{
			continue;
		}

		isBot = SV_IsBot(client);

		// add the map_restart command
		SV_AddServerCommand( client, "map_restart\n" );

		// connect the client again, without the firstTime flag
		denied = gvm.GameClientConnect( reason, sizeof( reason ), i, false, isBot );

		if ( denied )
		{
			// this generally shouldn't happen, because the client
			// was connected before the level change
			SV_DropClient( client, reason );

			if ( !isBot )
			{
				Log::Notice( "SV_MapRestart_f: dropped client %i: denied!\n", i );
			}

			continue;
		}

		client->state = clientState_t::CS_ACTIVE;

		SV_ClientEnterWorld( client, &client->lastUsercmd );
	}

	// run another frame to allow things to look at all the players
	gvm.GameRunFrame( sv.time );
	svs.time += FRAMETIME;
	sv.time += FRAMETIME;
}

/*
================
SV_Status_f
================
*/
static void SV_Status_f()
{
	int           i, j, l;
	client_t      *cl;
	playerState_t *ps;
	const char    *s;
	int           ping;
	float         cpu, avg;

	// make sure server is running
	if ( !com_sv_running->integer )
	{
		Log::Notice( "Server is not running.\n" );
		return;
	}

	cpu = ( svs.stats.latched_active + svs.stats.latched_idle );

	if ( cpu )
	{
		cpu = 100 * svs.stats.latched_active / cpu;
	}

	avg = 1000 * svs.stats.latched_active / STATFRAMES;

	Log::Notice( "cpu utilization  : %3i%%\n"
	            "avg response time: %i ms\n"
	            "map: %s\n"
	            "num score ping name            lastmsg address               qport rate\n"
	            "--- ----- ---- --------------- ------- --------------------- ----- -----\n",
	           ( int ) cpu, ( int ) avg, sv_mapname->string );

	for ( i = 0, cl = svs.clients; i < sv_maxclients->integer; i++, cl++ )
	{
		if ( cl->state == clientState_t::CS_FREE )
		{
			continue;
		}

		Log::Notice( "%3i ", i );
		ps = SV_GameClientNum( i );
		Log::Notice( "%5i ", ps->persistant[ PERS_SCORE ] );

		if ( cl->state == clientState_t::CS_CONNECTED )
		{
			Log::Notice( "CNCT " );
		}
		else if ( cl->state == clientState_t::CS_ZOMBIE )
		{
			Log::Notice( "ZMBI " );
		}
		else
		{
			ping = cl->ping < 9999 ? cl->ping : 9999;
			Log::Notice( "%4i ", ping );
		}

		Log::Notice( "%s", cl->name );
		l = 16 - strlen( cl->name );

		for ( j = 0; j < l; j++ )
		{
			Log::Notice( " " );
		}

		Log::Notice( "%7i ", svs.time - cl->lastPacketTime );

		s = NET_AdrToString( cl->netchan.remoteAddress );
		Log::Notice( "%s", s );
		l = 22 - strlen( s );

		for ( j = 0; j < l; j++ )
		{
			Log::Notice( " " );
		}

		Log::Notice( "%5i", cl->netchan.qport );

		Log::Notice( " %5i", cl->rate );

		Log::Notice( "\n" );
	}

	Log::Notice( "\n" );
}

/*
==================
SV_Heartbeat_f

Also called by SV_DropClient, SV_DirectConnect, and SV_SpawnServer
==================
*/
void SV_Heartbeat_f()
{
	svs.nextHeartbeatTime = -9999999;
}

/*
===========
SV_Serverinfo_f

Examine the serverinfo string
===========
*/
static void SV_Serverinfo_f()
{
	// make sure server is running
	if ( !com_sv_running->integer )
	{
		Log::Notice( "Server is not running.\n" );
		return;
	}

	Log::Notice( "Server info settings:\n" );
	Info_Print( Cvar_InfoString( CVAR_SERVERINFO, false ) );
}

/*
===========
SV_Systeminfo_f

Examine the systeminfo string
===========
*/
static void SV_Systeminfo_f()
{
	// make sure server is running
	if ( !com_sv_running->integer )
	{
		Log::Notice( "Server is not running.\n" );
		return;
	}

	Log::Notice( "System info settings:\n" );
	Info_Print( Cvar_InfoString( CVAR_SYSTEMINFO, false ) );
}

/*
==================
SV_AddOperatorCommands
==================
*/
void SV_AddOperatorCommands()
{
	if ( com_sv_running->integer )
	{
		// These commands should only be available while the server is running.
		Cmd_AddCommand( "fieldinfo",   SV_FieldInfo_f );
		Cmd_AddCommand( "heartbeat",   SV_Heartbeat_f );
		Cmd_AddCommand( "map_restart", SV_MapRestart_f );
		Cmd_AddCommand( "serverinfo",  SV_Serverinfo_f );
		Cmd_AddCommand( "status",      SV_Status_f );
		Cmd_AddCommand( "systeminfo",  SV_Systeminfo_f );
	}
}

/*
==================
SV_RemoveOperatorCommands
==================
*/
void SV_RemoveOperatorCommands()
{
	Cmd_RemoveCommand( "fieldinfo" );
	Cmd_RemoveCommand( "heartbeat" );
	Cmd_RemoveCommand( "map_restart" );
	Cmd_RemoveCommand( "serverinfo" );
	Cmd_RemoveCommand( "status" );
	Cmd_RemoveCommand( "systeminfo" );
}
