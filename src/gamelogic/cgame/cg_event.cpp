/*
 * Daemon GPL source code
 * Copyright (C) 2015  Unreal Arena
 * Copyright (C) 2000-2009  Darklegion Development
 * Copyright (C) 1999-2005  Id Software, Inc.
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


// cg_event.c -- handle entity events at snapshot or playerstate transitions

#include "cg_local.h"

/*
=============
CG_Obituary
=============
*/
static const char teamTag[][8] = { "^2●^7", "^1●^7", "^4●^7" };

#define LONGFORM ">"
static const struct {
	char     icon[16];
	bool envKill;
	bool showAssist;
	team_t   team;
} meansOfDeath[] = {
	// Icon            Envkill Assist? (Team)
	{ "☠",             false, false },
	{ "[shotgun]",     false, true  },
	{ "[blaster]",     false, true  },
	{ "[painsaw]",     false, true  },
	{ "[rifle]",       false, true  },
	{ "[chaingun]",    false, true  },
	{ "[prifle]",      false, true  },
	{ "[mdriver]",     false, true  },
	{ "[lasgun]",      false, true  },
	{ "[lcannon]",     false, true  },
	{ "[lcannon]",     false, true  }, // splash
	{ "[flamer]",      false, true  },
	{ "[flamer]",      false, true  }, // splash
	{ "[flamer]",      false, true  }, // burn
	{ "[grenade]",     false, true  },
	{ "[firebomb]",    false, true  },
	{ "crushed",       true,  false }, // weight (H) // FIXME
	{ LONGFORM,        true,  false }, // water
	{ LONGFORM,        true,  false }, // slime
	{ LONGFORM,        true,  false }, // lava
	{ LONGFORM,        true,  false }, // crush
	{ "[telenode]",    false, false }, // telefrag
	{ LONGFORM,        true,  false }, // falling
	{ "☠",             false, false }, // suicide
	{ LONGFORM,        true,  false }, // target laser - shouldn't happen
	{ LONGFORM,        true,  false }, // trigger hurt

	{ "[granger]",     false, true  },
	{ "[dretch]",      false, true  },
	{ "[basilisk]",    false, true  },
	{ "[dragoon]",     false, true  },
	{ "[dragoon]",     false, true  }, // pounce
	{ "[advdragoon]",  false, true  },
	{ "[marauder]",    false, true  },
	{ "[advmarauder]", false, true  },
	{ "[tyrant]",      false, true  },
	{ "[tyrant]",      false, true  }, // trample
	{ "crushed",       false, true  }, // weight (A) // FIXME

	{ LONGFORM,        true,  false, TEAM_Q }, // Q spawn
	{ LONGFORM,        true,  false, TEAM_U }, // U spawn
};

static void CG_Obituary( entityState_t *ent )
{
	int          mod;
	int          target, attacker, assistant;
	int          attackerClass = -1;
	const char   *message;
	const char   *messageAssisted = nullptr;
	const char   *messageSuicide = nullptr;
	const char   *targetInfo;
	const char   *attackerInfo;
	const char   *assistantInfo;
	char         targetName[ MAX_NAME_LENGTH ];
	char         attackerName[ MAX_NAME_LENGTH ];
	char         assistantName[ MAX_NAME_LENGTH ];
	gender_t     gender;
	clientInfo_t *ci;
	team_t       attackerTeam, assistantTeam = TEAM_NONE;

	target = ent->otherEntityNum;
	attacker = ent->otherEntityNum2;
	assistant = ent->otherEntityNum3;
	assistantTeam = (team_t) ( ent->generic1 & 0xFF ); // ugly hack allowing for future expansion(!)
	mod = ent->eventParm;

	if ( target < 0 || target >= MAX_CLIENTS )
	{
		CG_Error( "CG_Obituary: target out of range" );
	}

	ci = &cgs.clientinfo[ target ];
	gender = ci->gender;

	if ( attacker < 0 || attacker >= MAX_CLIENTS )
	{
		attacker = ENTITYNUM_WORLD;
		attackerInfo = nullptr;
		attackerTeam = TEAM_NONE;
		strcpy( attackerName, "noname" );
	}
	else
	{
		attackerInfo = CG_ConfigString( CS_PLAYERS + attacker );
		attackerTeam = cgs.clientinfo[ attacker ].team;
		Q_strncpyz( attackerName, Info_ValueForKey( attackerInfo, "n" ), sizeof( attackerName ) );

		// check for kill messages about the current clientNum
		if ( target == cg.snap->ps.clientNum )
		{
			Q_strncpyz( cg.killerName, attackerName, sizeof( cg.killerName ) );
		}
	}

	if ( assistant < 0 || assistant >= MAX_CLIENTS )
	{
		assistantInfo = nullptr;
	}
	else
	{
		assistantInfo = CG_ConfigString( CS_PLAYERS + assistant );
	}

	if ( assistantTeam < TEAM_NONE || assistantTeam >= NUM_TEAMS )
	{
		assistantTeam = TEAM_NONE;
	}

	if ( !assistantInfo )
	{
		strcpy( assistantName, "noname" );
	}
	else
	{
		Q_strncpyz( assistantName, Info_ValueForKey( assistantInfo, "n" ), sizeof( assistantName ) );
	}

	targetInfo = CG_ConfigString( CS_PLAYERS + target );

	if ( !targetInfo )
	{
		return;
	}

	Q_strncpyz( targetName, Info_ValueForKey( targetInfo, "n" ), sizeof( targetName ) );

	// check for single client messages

	if ( cg_emoticonsInMessages.integer )
	{
		if ( mod < MOD_UNKNOWN || mod >= ARRAY_LEN( meansOfDeath ) )
		{
			mod = MOD_UNKNOWN;
		}

		if ( meansOfDeath[ mod ].team )
		{
			attackerTeam = meansOfDeath[ mod ].team;
		}

		// if the long form is needed, show it; but we need a kill icon for this kill type
		if ( *meansOfDeath[ mod ].icon == '>' )
		{
				goto is_long_kill_message;
		}

		// if there's text for the kill type, print a kill message
		if ( *meansOfDeath[ mod ].icon )
		{
			if ( meansOfDeath[ mod ].envKill )
			{
				if ( meansOfDeath[ mod ].showAssist && assistantInfo )
				{
					CG_Printf( "%s (+ %s%s^7) %s %s%s\n", teamTag[ attackerTeam ], teamTag[ assistantTeam ], assistantName, meansOfDeath[ mod ].icon, teamTag[ ci->team ], targetName );
				}
				else
				{
					CG_Printf( "%s %s %s%s\n", teamTag[ attackerTeam ], meansOfDeath[ mod ].icon, teamTag[ ci->team ], targetName );
				}
			}
			else if ( attacker == target )
			{
				CG_Printf( "%s %s%s\n", meansOfDeath[ mod ].icon, teamTag[ ci->team ], targetName );
			}
			else
			{
				if ( meansOfDeath[ mod ].showAssist && assistantInfo )
				{
					CG_Printf( "%s%s^7 (+ %s%s^7) %s %s%s\n", teamTag[ attackerTeam ], attackerName, teamTag[ assistantTeam ], assistantName, meansOfDeath[ mod ].icon, teamTag[ ci->team ], targetName );
				}
				else
				{
					CG_Printf( "%s%s^7 %s %s%s\n", teamTag[ attackerTeam ], attackerName, meansOfDeath[ mod ].icon, teamTag[ ci->team ], targetName );
				}

				// nice big message for teamkills
				if ( attackerTeam == ci->team && attacker == cg.clientNum )
				{
					CG_CenterPrint( va( _("You killed ^1TEAMMATE^7 %s"), targetName ),
							SCREEN_HEIGHT * 0.30, BIGCHAR_WIDTH );
				}
			}
		}
	}
	else // Long form
	{
		is_long_kill_message: // <- use of this label == needs a kill icon

		// Messages which contain no killer name
		switch ( mod )
		{
			// Environmental kills

			case MOD_FALLING:
				message = G_( "%s%s ^7fell to death\n" );
				break;

			case MOD_CRUSH:
				message = G_( "%s%s ^7was crushed to death\n" );
				break;

			case MOD_WATER:
				message = G_( "%s%s ^7drowned\n" );
				break;

			case MOD_SLIME:
				message = G_( "%s%s ^7melted\n" );
				break;

			case MOD_LAVA:
				message = G_( "%s%s ^7took a dip in some lava\n" );
				break;

			case MOD_TRIGGER_HURT:
				message = G_( "%s%s ^7was in the wrong place\n" );
				break;

			// Shouldn't happen

			case MOD_TARGET_LASER:
				message = G_( "%s%s ^7saw the light\n" );
				messageAssisted = G_( "%s%s ^7saw the light; %s%s^7 assisted\n" );
				break;

			default:
				message = nullptr;
				break;
		}

		if ( message )
		{
			if ( messageAssisted && assistantInfo )
			{
				CG_Printf( messageAssisted, teamTag[ ci->team ], targetName , teamTag[ assistantTeam ], assistantName);
			}
			else
			{
				CG_Printf( message, teamTag[ ci->team ], targetName );
			}

			return;
		}

		// Messages which contain the killer's name
		switch ( mod )
		{
			case MOD_PAINSAW:
				message = G_( "%s%s ^7was sawn by %s%s\n" );
				messageAssisted = G_( "%s%s ^7was sawn by %s%s^7; %s%s^7 assisted\n" );
				break;

			case MOD_BLASTER:
				message = G_( "%s%s ^7was blasted by %s%s\n" );
				messageAssisted = G_( "%s%s ^7was blasted by %s%s^7; %s%s^7 assisted\n" );
				break;

			case MOD_MACHINEGUN:
				message = G_( "%s%s ^7was machinegunned by %s%s\n" );
				messageAssisted = G_( "%s%s ^7was machinegunned by %s%s^7; %s%s^7 assisted\n" );
				break;

			case MOD_CHAINGUN:
				message = G_( "%s%s ^7was mowed down by %s%s\n" );
				messageAssisted = G_( "%s%s ^7was mowed down by %s%s^7; %s%s^7 assisted\n" );
				break;

			case MOD_SHOTGUN:
				message = G_( "%s%s ^7was gunned down by %s%s\n" );
				messageAssisted = G_( "%s%s ^7was gunned down by %s%s^7; %s%s^7 assisted\n" );
				break;

			case MOD_PRIFLE:
				message = G_( "%s%s ^7was seared by %s%s^7's pulse blast\n" );
				messageAssisted = G_( "%s%s ^7was seared by %s%s^7's pulse blast; %s%s^7 assisted\n" );
				break;

			case MOD_MDRIVER:
				message = G_( "%s%s ^7was sniped by %s%s\n" );
				messageAssisted = G_( "%s%s ^7was sniped by %s%s^7; %s%s^7 assisted\n" );
				break;

			case MOD_LASGUN:
				message = G_( "%s%s ^7was lasered by %s%s\n" );
				messageAssisted = G_( "%s%s ^7was lasered by %s%s^7; %s%s^7 assisted\n" );
				break;

			case MOD_FLAMER:
			case MOD_FLAMER_SPLASH:
				message = G_( "%s%s ^7was grilled by %s%s^7's flame\n" );
				messageAssisted = G_( "%s%s ^7was grilled by %s%s^7's flame; %s%s^7 assisted\n" );
				messageSuicide = G_( "%s%s ^7was charred to a crisp" );
				break;

			case MOD_LCANNON:
				message = G_( "%s%s ^7was annihilated by %s%s^7's plasma blast\n" );
				messageAssisted = G_( "%s%s ^7was annihilated by %s%s^7's plasma blast; %s%s^7 assisted\n" );
				break;

			case MOD_LCANNON_SPLASH:
				message = G_( "%s%s ^7was irradiated by %s%s^7's plasma blast\n" );
				messageAssisted = G_( "%s%s ^7was irradiated by %s%s^7's plasma blast; %s%s^7 assisted\n" );
				messageSuicide = G_( "%s%s ^7was irradiated" );
				break;

			case MOD_GRENADE:
				message = G_( "%s%s ^7was blown up by %s%s^7's grenade\n" );
				messageAssisted = G_( "%s%s ^7was blown up by %s%s^7's grenade; %s%s^7 assisted\n" );
				messageSuicide = G_( "%s%s ^7was blown up" );
				break;

			case MOD_FIREBOMB:
				message = G_( "%s%s ^7was incinerated by %s%s^7's firebomb\n" );
				messageAssisted = G_( "%s%s ^7was incinerated by %s%s^7's firebomb; %s%s^7 assisted\n" );
				messageSuicide = G_( "%s%s ^7was incinerated" );
				break;

			case MOD_WEIGHT:
				message = G_( "%s%s ^7was crushed under %s%s^7's weight\n" );
				messageAssisted = G_( "%s%s ^7was crushed under %s%s^7's weight; %s%s^7 assisted\n" );
				break;

			case MOD_POISON:
				message = G_( "%s%s ^7should have used a medkit against %s%s^7's poison\n" );
				messageAssisted = G_( "%s%s ^7should have used a medkit against %s%s^7's poison; %s%s^7 assisted\n" );
				break;

			case MOD_TELEFRAG:
				message = G_( "%s%s ^7tried to invade %s%s^7's personal space\n" );
				break;

			default:
				message = G_( "%s%s ^7was killed by %s%s\n" );
				messageAssisted = G_( "%s%s ^7was killed by %s%s^7 and %s%s\n" );
				messageSuicide = G_( "%s%s ^7committed suicide\n" );
				break;
		}

		if ( message )
		{
			// shouldn't need to do this here, but it avoids
			char attackerClassName[ 64 ];

			if ( attackerClass == -1 )
			{
				*attackerClassName = 0;
			}
			else
			{
				Q_strncpyz( attackerClassName, _( BG_ClassModelConfig( attackerClass )->humanName ), sizeof( attackerClassName ) );
			}

			// Argument order: victim, attacker, [class,] [assistant]. Each has team tag first.
			if ( messageSuicide && attacker == target )
			{
				CG_Printf( messageSuicide, teamTag[ ci->team ], targetName );
			}
			else if ( messageAssisted && assistantInfo )
			{
				if ( attackerClass != -1 )
				{
					CG_Printf( messageAssisted, teamTag[ ci->team ], targetName, teamTag[ attackerTeam ], attackerName, attackerClassName, teamTag[ assistantTeam ], assistantName );
				}
				else
				{
					CG_Printf( messageAssisted, teamTag[ ci->team ], targetName, teamTag[ attackerTeam ], attackerName, teamTag[ assistantTeam ], assistantName );
				}
			}
			else
			{
				CG_Printf( message, teamTag[ ci->team ], targetName, teamTag[ attackerTeam ], attackerName, attackerClassName );
			}

			if ( attackerTeam == ci->team && attacker == cg.clientNum && attacker != target )
			{
				CG_CenterPrint( va( _("You killed ^1TEAMMATE^7 %s"), targetName ),
						SCREEN_HEIGHT * 0.30, BIGCHAR_WIDTH );
			}

			return;
		}

		// we don't know what it was
		CG_Printf( G_( "%s%s^7 died\n" ), teamTag[ ci->team ], targetName );
	}
}

//==========================================================================

/*
================
CG_PainEvent

Also called by playerstate transition
================
*/
void CG_PainEvent( centity_t *cent, int health )
{
	const char *snd;

	// don't do more than two pain sounds a second
	if ( cg.time - cent->pe.painTime < 500 )
	{
		return;
	}

	if ( health < 25 )
	{
		snd = "*pain25_1.wav";
	}
	else if ( health < 50 )
	{
		snd = "*pain50_1.wav";
	}
	else if ( health < 75 )
	{
		snd = "*pain75_1.wav";
	}
	else
	{
		snd = "*pain100_1.wav";
	}

	trap_S_StartSound( nullptr, cent->currentState.number, CHAN_VOICE,
	                   CG_CustomSound( cent->currentState.number, snd ) );

	// save pain time for programitic twitch animation
	cent->pe.painTime = cg.time;
	cent->pe.painDirection ^= 1;
}

/*
=========================
CG_OnPlayerWeaponChange

Called on weapon change
=========================
*/
void CG_OnPlayerWeaponChange( weapon_t oldWeapon )
{
	playerState_t *ps = &cg.snap->ps;

	// Change the HUD to match the weapon. Close the old hud first
	Rocket_ShowHud( ps->weapon );

	cg.weaponOffsetsFilter.Reset( );

	cg.predictedPlayerEntity.pe.weapon.animationNumber = -1; //force weapon lerpframe recalculation
}

/*
=========================
CG_OnPlayerUpgradeChange

Called on upgrade change
=========================
*/

void CG_OnPlayerUpgradeChange()
{
	playerState_t *ps = &cg.snap->ps;
}

/*
=========================
CG_OnMapRestart

Called whenever the map is restarted
via map_restart
=========================
*/
void CG_OnMapRestart()
{
	// if scoreboard is showing, hide it
	CG_HideScores_f();

	// hide any other menus
	Rocket_DocumentAction( "", "blurall" );
}

/*
==============
CG_Momentum

Notify player of generated momentum
==============
*/
void CG_Momentum( entityState_t *es )
{
	float                  momentum;
	bool               negative;

	negative   = es->groundEntityNum;
	momentum = ( negative ? -es->otherEntityNum2 : es->otherEntityNum2 ) / 10.0f;

	cg.momentumGained     = momentum;
	cg.momentumGainedTime = cg.time;
}

/*
==============
CG_EntityEvent

An entity has an event value
also called by CG_CheckPlayerstateEvents
==============
*/
void CG_EntityEvent( centity_t *cent, vec3_t position )
{
	entityState_t *es;
	int           event;
	const char    *s;
	int           clientNum;
	clientInfo_t  *ci;
	int           steptime;

	if ( cg.snap->ps.persistant[ PERS_SPECSTATE ] != SPECTATOR_NOT )
	{
		steptime = 200;
	}
	else
	{
		steptime = BG_Class( cg.snap->ps.stats[ STAT_CLASS ] )->steptime;
	}

	es = &cent->currentState;
	event = es->event & ~EV_EVENT_BITS;

	if ( cg_debugEvents.integer )
	{
		CG_Printf( "ent:%3i  event:%3i %s\n", es->number, event,
		           BG_EventName( event ) );
	}

	if ( !event )
	{
		return;
	}

	clientNum = es->clientNum;

	if ( clientNum < 0 || clientNum >= MAX_CLIENTS )
	{
		clientNum = 0;
	}

	ci = &cgs.clientinfo[ clientNum ];

	switch ( event )
	{
		case EV_FOOTSTEP:
			if ( cg_footsteps.integer && ci->footsteps != FOOTSTEP_NONE )
			{
				if ( ci->footsteps == FOOTSTEP_CUSTOM )
				{
					trap_S_StartSound( nullptr, es->number, CHAN_BODY,
					                   ci->customFootsteps[ rand() & 3 ] );
				}
				else
				{
					trap_S_StartSound( nullptr, es->number, CHAN_BODY,
					                   cgs.media.footsteps[ ci->footsteps ][ rand() & 3 ] );
				}
			}

			break;

		case EV_FOOTSTEP_METAL:
			if ( cg_footsteps.integer && ci->footsteps != FOOTSTEP_NONE )
			{
				if ( ci->footsteps == FOOTSTEP_CUSTOM )
				{
					trap_S_StartSound( nullptr, es->number, CHAN_BODY,
					                   ci->customMetalFootsteps[ rand() & 3 ] );
				}
				else
				{
					trap_S_StartSound( nullptr, es->number, CHAN_BODY,
					                   cgs.media.footsteps[ FOOTSTEP_METAL ][ rand() & 3 ] );
				}
			}

			break;

		case EV_FOOTSTEP_SQUELCH:
			if ( cg_footsteps.integer && ci->footsteps != FOOTSTEP_NONE )
			{
				trap_S_StartSound( nullptr, es->number, CHAN_BODY,
				                   cgs.media.footsteps[ FOOTSTEP_FLESH ][ rand() & 3 ] );
			}

			break;

		case EV_FOOTSPLASH:
			if ( cg_footsteps.integer && ci->footsteps != FOOTSTEP_NONE )
			{
				trap_S_StartSound( nullptr, es->number, CHAN_BODY,
				                   cgs.media.footsteps[ FOOTSTEP_SPLASH ][ rand() & 3 ] );
			}

			break;

		case EV_FOOTWADE:
			if ( cg_footsteps.integer && ci->footsteps != FOOTSTEP_NONE )
			{
				trap_S_StartSound( nullptr, es->number, CHAN_BODY,
				                   cgs.media.footsteps[ FOOTSTEP_SPLASH ][ rand() & 3 ] );
			}

			break;

		case EV_SWIM:
			if ( cg_footsteps.integer && ci->footsteps != FOOTSTEP_NONE )
			{
				trap_S_StartSound( nullptr, es->number, CHAN_BODY,
				                   cgs.media.footsteps[ FOOTSTEP_SPLASH ][ rand() & 3 ] );
			}

			break;

		case EV_FALL_SHORT:
			trap_S_StartSound( nullptr, es->number, CHAN_AUTO, cgs.media.landSound );

			if ( clientNum == cg.predictedPlayerState.clientNum )
			{
				// smooth landing z changes
				cg.landChange = -8;
				cg.landTime = cg.time;
			}

			break;

		case EV_FALL_MEDIUM:
			// use a general pain sound
			trap_S_StartSound( nullptr, es->number, CHAN_VOICE, CG_CustomSound( es->number, "*pain100_1.wav" ) );

			if ( clientNum == cg.predictedPlayerState.clientNum )
			{
				// smooth landing z changes
				cg.landChange = -16;
				cg.landTime = cg.time;
			}

			break;

		case EV_FALL_FAR:
			trap_S_StartSound( nullptr, es->number, CHAN_AUTO, CG_CustomSound( es->number, "*fall1.wav" ) );
			cent->pe.painTime = cg.time; // don't play a pain sound right after this

			if ( clientNum == cg.predictedPlayerState.clientNum )
			{
				// smooth landing z changes
				cg.landChange = -24;
				cg.landTime = cg.time;
			}

			break;

		case EV_FALLING:
			trap_S_StartSound( nullptr, es->number, CHAN_AUTO, CG_CustomSound( es->number, "*falling1.wav" ) );
			break;

		case EV_STEP_4:
		case EV_STEP_8:
		case EV_STEP_12:
		case EV_STEP_16: // smooth out step up transitions
		case EV_STEPDN_4:
		case EV_STEPDN_8:
		case EV_STEPDN_12:
		case EV_STEPDN_16: // smooth out step down transitions
			{
				float oldStep;
				int   delta;
				int   step;

				if ( clientNum != cg.predictedPlayerState.clientNum )
				{
					break;
				}

				// if we are interpolating, we don't need to smooth steps
				if ( cg.demoPlayback || ( cg.snap->ps.pm_flags & PMF_FOLLOW ) ||
				     cg_nopredict.integer || cg.pmoveParams.synchronous )
				{
					break;
				}

				// check for stepping up before a previous step is completed
				delta = cg.time - cg.stepTime;

				if ( delta < steptime )
				{
					oldStep = cg.stepChange * ( steptime - delta ) / steptime;
				}
				else
				{
					oldStep = 0;
				}

				// add this amount
				if ( event >= EV_STEPDN_4 )
				{
					step = 4 * ( event - EV_STEPDN_4 + 1 );
					cg.stepChange = oldStep - step;
				}
				else
				{
					step = 4 * ( event - EV_STEP_4 + 1 );
					cg.stepChange = oldStep + step;
				}

				if ( cg.stepChange > MAX_STEP_CHANGE )
				{
					cg.stepChange = MAX_STEP_CHANGE;
				}
				else if ( cg.stepChange < -MAX_STEP_CHANGE )
				{
					cg.stepChange = -MAX_STEP_CHANGE;
				}

				cg.stepTime = cg.time;
				break;
			}

		case EV_JUMP:
			trap_S_StartSound( nullptr, es->number, CHAN_VOICE, CG_CustomSound( es->number, "*jump1.wav" ) );

			if ( BG_ClassHasAbility( cg.predictedPlayerState.stats[ STAT_CLASS ], SCA_WALLJUMPER ) )
			{
				vec3_t surfNormal, refNormal = { 0.0f, 0.0f, 1.0f };
				vec3_t rotAxis;

				if ( clientNum != cg.predictedPlayerState.clientNum )
				{
					break;
				}

				//set surfNormal
				VectorCopy( cg.predictedPlayerState.grapplePoint, surfNormal );

				//if we are moving from one surface to another smooth the transition
				if ( !VectorCompare( surfNormal, cg.lastNormal ) && surfNormal[ 2 ] != 1.0f )
				{
					CrossProduct( refNormal, surfNormal, rotAxis );
					VectorNormalize( rotAxis );

					//add the op
					CG_addSmoothOp( rotAxis, 15.0f, 1.0f );
				}

				//copy the current normal to the lastNormal
				VectorCopy( surfNormal, cg.lastNormal );
			}

			break;

		case EV_TAUNT:
			if ( !cg_noTaunt.integer )
			{
				trap_S_StartSound( nullptr, es->number, CHAN_VOICE, CG_CustomSound( es->number, "*taunt.wav" ) );
			}

			break;

		case EV_WATER_TOUCH:
			trap_S_StartSound( nullptr, es->number, CHAN_AUTO, cgs.media.watrInSound );
			break;

		case EV_WATER_LEAVE:
			trap_S_StartSound( nullptr, es->number, CHAN_AUTO, cgs.media.watrOutSound );
			break;

		case EV_WATER_UNDER:
			trap_S_StartSound( nullptr, es->number, CHAN_AUTO, cgs.media.watrUnSound );
			break;

		case EV_WATER_CLEAR:
			trap_S_StartSound( nullptr, es->number, CHAN_AUTO, CG_CustomSound( es->number, "*gasp.wav" ) );
			break;

		case EV_JETPACK_ENABLE:
      cent->jetpackAnim = JANIM_SLIDEOUT;
			break;

		case EV_JETPACK_DISABLE:
      cent->jetpackAnim = JANIM_SLIDEIN;
			break;

		case EV_JETPACK_IGNITE:
			// TODO: Play jetpack ignite gfx/sfx
			break;

		case EV_JETPACK_START:
			// TODO: Start jetpack thrust gfx/sfx
			break;

		case EV_JETPACK_STOP:
			// TODO: Stop jetpack thrust gfx/sfx
			break;

		case EV_NOAMMO:
			trap_S_StartSound( nullptr, es->number, CHAN_WEAPON, cgs.media.weaponEmptyClick );
			break;

		case EV_CHANGE_WEAPON:
			trap_S_StartSound( nullptr, es->number, CHAN_AUTO, cgs.media.selectSound );
			break;

		case EV_FIRE_WEAPON:
			CG_HandleFireWeapon( cent, WPM_PRIMARY );
			break;

		case EV_FIRE_WEAPON2:
			CG_HandleFireWeapon( cent, WPM_SECONDARY );
			break;

		case EV_FIRE_WEAPON3:
			CG_HandleFireWeapon( cent, WPM_TERTIARY );
			break;

		case EV_WEAPON_RELOAD:
			if ( cg_weapons[ es->eventParm ].wim[ WPM_PRIMARY ].reloadSound )
			{
				trap_S_StartSound( nullptr, es->number, CHAN_WEAPON, cg_weapons[ es->eventParm ].wim[ WPM_PRIMARY ].reloadSound );
			}
			break;

		case EV_PLAYER_TELEPORT_IN:
			//deprecated
			break;

		case EV_PLAYER_TELEPORT_OUT:
			CG_PlayerDisconnect( position );
			break;

		case EV_AMMO_REFILL:
		case EV_CLIPS_REFILL:
		case EV_FUEL_REFILL:
			// TODO: Add different sounds for EV_AMMO_REFILL, EV_CLIPS_REFILL, EV_FUEL_REFILL
			break;

		case EV_GRENADE_BOUNCE:
			if ( rand() & 1 )
			{
				trap_S_StartSound( nullptr, es->number, CHAN_AUTO, cgs.media.hardBounceSound1 );
			}
			else
			{
				trap_S_StartSound( nullptr, es->number, CHAN_AUTO, cgs.media.hardBounceSound2 );
			}
			break;

		case EV_WEAPON_HIT_ENTITY:
			CG_HandleWeaponHitEntity( es, position );
			break;

		case EV_WEAPON_HIT_ENVIRONMENT:
			CG_HandleWeaponHitWall( es, position );
			break;

		case EV_MISSILE_HIT_ENTITY:
			CG_HandleMissileHitEntity( es, position );
			break;

		// currently there is no support for metal sounds
		case EV_MISSILE_HIT_ENVIRONMENT:
		case EV_MISSILE_HIT_METAL:
			CG_HandleMissileHitWall( es, position );
			break;

		case EV_SHOTGUN:
			CG_HandleFireShotgun( es );
			break;

		case EV_TESLATRAIL:
			{
				centity_t *source = &cg_entities[ es->generic1 ];
				centity_t *target = &cg_entities[ es->clientNum ];

				if ( !CG_IsTrailSystemValid( &source->muzzleTS ) )
				{
					source->muzzleTS = CG_SpawnNewTrailSystem( cgs.media.teslaZapTS );

					if ( CG_IsTrailSystemValid( &source->muzzleTS ) )
					{
						CG_SetAttachmentCent( &source->muzzleTS->frontAttachment, source );
						CG_SetAttachmentCent( &source->muzzleTS->backAttachment, target );
						CG_AttachToCent( &source->muzzleTS->frontAttachment );
						CG_AttachToCent( &source->muzzleTS->backAttachment );

						source->muzzleTSDeathTime = cg.time + cg_teslaTrailTime.integer;
					}
				}
			}
			break;

		case EV_GENERAL_SOUND:
			if ( cgs.gameSounds[ es->eventParm ] )
			{
				trap_S_StartSound( nullptr, es->number, CHAN_VOICE, cgs.gameSounds[ es->eventParm ] );
			}
			else
			{
				s = CG_ConfigString( CS_SOUNDS + es->eventParm );
				trap_S_StartSound( nullptr, es->number, CHAN_VOICE, CG_CustomSound( es->number, s ) );
			}

			break;

		case EV_GLOBAL_SOUND: // play from the player's head so it never diminishes
			if ( cgs.gameSounds[ es->eventParm ] )
			{
				trap_S_StartSound( nullptr, cg.snap->ps.clientNum, CHAN_AUTO, cgs.gameSounds[ es->eventParm ] );
			}
			else
			{
				s = CG_ConfigString( CS_SOUNDS + es->eventParm );
				trap_S_StartSound( nullptr, cg.snap->ps.clientNum, CHAN_AUTO, CG_CustomSound( es->number, s ) );
			}

			break;

		case EV_PAIN:
			// local player sounds are triggered in CG_CheckLocalSounds,
			// so ignore events on the player
			if ( cent->currentState.number != cg.snap->ps.clientNum )
			{
				CG_PainEvent( cent, es->eventParm );
			}

			break;

		case EV_DEATH1:
		case EV_DEATH2:
		case EV_DEATH3:
			trap_S_StartSound( nullptr, es->number, CHAN_VOICE,
			                   CG_CustomSound( es->number, va( "*death%i.wav", event - EV_DEATH1 + 1 ) ) );
			break;

		case EV_OBITUARY:
			CG_Obituary( es );
			break;

		case EV_GIB_PLAYER:
			// no gibbing
			break;

		case EV_STOPLOOPINGSOUND:
			trap_S_StopLoopingSound( es->number );
			es->loopSound = 0;
			break;

		case EV_MEDKIT_USED:
			trap_S_StartSound( nullptr, es->number, CHAN_AUTO, cgs.media.medkitUseSound );
			break;

		case EV_PLAYER_RESPAWN:
			if ( es->number == cg.clientNum )
			{
				cg.spawnTime = cg.time;
			}

			break;

		case EV_HIT:
			cg.hitTime = cg.time;
			break;

		case EV_MOMENTUM:
			CG_Momentum( es );
			break;

		default:
			CG_Error( "Unknown event: %i", event );
	}
}

/*
==============
CG_CheckEvents

==============
*/
void CG_CheckEvents( centity_t *cent )
{
	entity_event_t event;
	entity_event_t oldEvent = EV_NONE;

	// check for event-only entities
	if ( cent->currentState.eType > ET_EVENTS )
	{
		event = (entity_event_t) ( cent->currentState.eType - ET_EVENTS );

		if ( cent->previousEvent )
		{
			return; // already fired
		}

		cent->previousEvent = 1;

		cent->currentState.event = cent->currentState.eType - ET_EVENTS;

		// Move the pointer to the entity that the
		// event was originally attached to
		if ( cent->currentState.eFlags & EF_PLAYER_EVENT )
		{
			cent = &cg_entities[ cent->currentState.otherEntityNum ];
			oldEvent = (entity_event_t) cent->currentState.event;
			cent->currentState.event = event;
		}
	}
	else
	{
		// check for events riding with another entity
		if ( cent->currentState.event == cent->previousEvent )
		{
			return;
		}

		cent->previousEvent = cent->currentState.event;

		if ( ( cent->currentState.event & ~EV_EVENT_BITS ) == 0 )
		{
			return;
		}
	}

	// calculate the position at exactly the frame time
	BG_EvaluateTrajectory( &cent->currentState.pos, cg.snap->serverTime, cent->lerpOrigin );
	CG_SetEntitySoundPosition( cent );

	CG_EntityEvent( cent, cent->lerpOrigin );

	// If this was a reattached spilled event, restore the original event
	if ( oldEvent != EV_NONE )
	{
		cent->currentState.event = oldEvent;
	}
}
