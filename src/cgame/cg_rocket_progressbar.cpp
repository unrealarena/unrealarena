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

static float CG_Rocket_GetCharLoadProgress()
{
	return cg.charModelFraction;
}

static float CG_Rocket_GetMediaLoadProgress()
{
	return cg.mediaFraction;
}

static float CG_Rocket_GetBuildTimerProgress()
{
	static int misc = 0;
	static int max;
	playerState_t *ps = &cg.snap->ps;
	weapon_t weapon = BG_GetPlayerWeapon( ps );

	// Not building anything
	if ( weapon != WP_HBUILD && weapon != WP_ABUILD && weapon != WP_ABUILD2 )
	{
		return 0;
	}

	// Building something new. Note max value.
	if ( ps->stats[ STAT_MISC ] > 0 && misc <= 0 )
	{
		max = ps->stats[ STAT_MISC ];
	}

	misc = ps->stats[ STAT_MISC ];

	return ( float ) misc / ( float ) max;
}

static float CG_Rocket_GetPoisonProgress()
{
	static int time = -1;

	time = -1;
	return 0;
}

static float CG_Rocket_GetPlayerHealthProgress()
{
	playerState_t *ps = &cg.snap->ps;

	return ( float )ps->stats[ STAT_HEALTH ] / ( float )BG_Class( ( team_t ) ps->persistant[ PERS_TEAM ] )->health;
}

static float CG_Rocket_GetPlayerAmmoProgress()
{
	if ( cg.snap->ps.weaponstate == WEAPON_RELOADING )
	{
		float maxDelay;
		playerState_t *ps = &cg.snap->ps;
		centity_t *cent = &cg_entities[ ps->clientNum ];

		maxDelay = ( float ) BG_Weapon( cent->currentState.weapon )->reloadTime;
		return ( maxDelay - ( float ) ps->weaponTime ) / maxDelay;

	}

	else
	{
		int      maxAmmo;
		weapon_t weapon;

		weapon = BG_PrimaryWeapon( cg.snap->ps.stats );
		maxAmmo = BG_Weapon( weapon )->maxAmmo;

		if ( maxAmmo <= 0 )
		{
			return 0;
		}

		return ( float )cg.snap->ps.ammo / ( float )maxAmmo;
	}
}

float CG_Rocket_DownloadProgress()
{
	return trap_Cvar_VariableValue( "cl_downloadCount" ) / trap_Cvar_VariableValue( "cl_downloadSize" );
}


typedef struct progressBarCmd_s
{
	const char *command;
	float( *get )();
	rocketElementType_t type;
} progressBarCmd_t;

static const progressBarCmd_t progressBarCmdList[] =
{
	{ "ammo", &CG_Rocket_GetPlayerAmmoProgress, ELEMENT_U },
	{ "btimer", &CG_Rocket_GetBuildTimerProgress, ELEMENT_BOTH },
	{ "characters", &CG_Rocket_GetCharLoadProgress, ELEMENT_LOADING },
	{ "charge", &CG_ChargeProgress, ELEMENT_BOTH },
	{ "download", &CG_Rocket_DownloadProgress, ELEMENT_ALL },
	{ "health", &CG_Rocket_GetPlayerHealthProgress, ELEMENT_BOTH },
	{ "media", &CG_Rocket_GetMediaLoadProgress, ELEMENT_LOADING },
	{ "poison", &CG_Rocket_GetPoisonProgress, ELEMENT_Q },
};

static const size_t progressBarCmdListCount = ARRAY_LEN( progressBarCmdList );

static int progressBarCmdCmp( const void *a, const void *b )
{
	return Q_stricmp( ( const char * ) a, ( ( progressBarCmd_t * ) b )->command );
}

float CG_Rocket_ProgressBarValue( Str::StringRef name )
{
	progressBarCmd_t *cmd;

	// Get the progressbar command
	cmd = ( progressBarCmd_t * ) bsearch( name.c_str(), progressBarCmdList, progressBarCmdListCount, sizeof( progressBarCmd_t ), progressBarCmdCmp );

	if ( cmd && CG_Rocket_IsCommandAllowed( cmd->type ) )
	{
		return cmd->get();
	}

	return 0;
}
