/*
 * Daemon GPL source code
 * Copyright (C) 2015-2016  Unreal Arena
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

#ifndef UNREALARENA
static float CG_Rocket_GetBuildableLoadProgress()
{
	return cg.buildablesFraction;
}
#endif

static float CG_Rocket_GetCharLoadProgress()
{
	return cg.charModelFraction;
}

static float CG_Rocket_GetMediaLoadProgress()
{
	return cg.mediaFraction;
}

static float CG_Rocket_GetOverallLoadProgress()
{
#ifdef UNREALARENA
	return ( cg.mediaFraction + cg.charModelFraction ) / 2.0f;
#else
	return ( cg.mediaFraction + cg.charModelFraction + cg.buildablesFraction ) / 3.0f;
#endif
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

#ifndef UNREALARENA
static float CG_Rocket_GetStaminaProgress()
{
	playerState_t *ps = &cg.snap->ps;
	float         stamina = ps->stats[ STAT_STAMINA ];

	return ( stamina / ( float ) STAMINA_MAX );
}
#endif

static float CG_Rocket_GetPoisonProgress()
{
#ifdef UNREALARENA
	return 0;
#else
	static int time = -1;

	if ( cg.snap->ps.stats[ STAT_STATE ] & SS_BOOSTED )
	{
		if ( time == -1 || cg.snap->ps.stats[ STAT_STATE ] & SS_BOOSTEDNEW )
		{
			time = cg.time;
		}

		return 1 - ( ( ( float )cg.time - time ) / BOOST_TIME );
	}

	else
	{
		time = -1;
		return 0;
	}

#endif
}

static float CG_Rocket_GetPlayerHealthProgress()
{
	playerState_t *ps = &cg.snap->ps;

#ifdef UNREALARENA
	return ( float )ps->stats[ STAT_HEALTH ] / ( float )BG_Class( ( team_t ) ps->persistant[ PERS_TEAM ] )->health;
#else
	return ( float )ps->stats[ STAT_HEALTH ] / ( float )BG_Class( ps->stats[ STAT_CLASS ] )->health;
#endif
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

#ifndef UNREALARENA
float CG_Rocket_FuelProgress()
{
	int   fuel;

	if ( !BG_InventoryContainsUpgrade( UP_JETPACK, cg.snap->ps.stats ) )
	{
		return 0;
	}

	fuel     = cg.snap->ps.stats[ STAT_FUEL ];
	return ( float )fuel / ( float )JETPACK_FUEL_MAX;
}
#endif

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
#ifdef UNREALARENA
	{ "ammo", &CG_Rocket_GetPlayerAmmoProgress, ELEMENT_U },
#else
	{ "ammo", &CG_Rocket_GetPlayerAmmoProgress, ELEMENT_HUMANS },
#endif
	{ "btimer", &CG_Rocket_GetBuildTimerProgress, ELEMENT_BOTH },
#ifndef UNREALARENA
	{ "buildables", &CG_Rocket_GetBuildableLoadProgress, ELEMENT_LOADING },
#endif
	{ "characters", &CG_Rocket_GetCharLoadProgress, ELEMENT_LOADING },
	{ "charge", &CG_ChargeProgress, ELEMENT_BOTH },
	{ "download", &CG_Rocket_DownloadProgress, ELEMENT_ALL },
#ifndef UNREALARENA
	{ "fuel", &CG_Rocket_FuelProgress, ELEMENT_HUMANS },
#endif
	{ "health", &CG_Rocket_GetPlayerHealthProgress, ELEMENT_BOTH },
	{ "media", &CG_Rocket_GetMediaLoadProgress, ELEMENT_LOADING },
	{ "overall", &CG_Rocket_GetOverallLoadProgress, ELEMENT_LOADING },
#ifdef UNREALARENA
	{ "poison", &CG_Rocket_GetPoisonProgress, ELEMENT_Q },
#else
	{ "poison", &CG_Rocket_GetPoisonProgress, ELEMENT_ALIENS },
#endif
#ifndef UNREALARENA
	{ "stamina", &CG_Rocket_GetStaminaProgress, ELEMENT_HUMANS },
#endif
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
