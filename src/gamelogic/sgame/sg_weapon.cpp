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


// sg_weapon.c
// perform the server side effects of a weapon firing

#include "sg_local.h"

static vec3_t forward, right, up;
static vec3_t muzzle;

void G_ForceWeaponChange( gentity_t *ent, weapon_t weapon )
{
	playerState_t *ps = &ent->client->ps;

	if ( weapon == WP_NONE || !BG_InventoryContainsWeapon( weapon, ps->stats ) )
	{
		// switch to the first non blaster weapon
		ps->persistant[ PERS_NEWWEAPON ] = BG_PrimaryWeapon( ent->client->ps.stats );
	}
	else
	{
		ps->persistant[ PERS_NEWWEAPON ] = weapon;
	}

	// force this here to prevent flamer effect from continuing
	ps->generic1 = WPM_NOTFIRING;

	// The PMove will do an animated drop, raise, and set the new weapon
	ps->pm_flags |= PMF_WEAPON_SWITCH;
}

/**
 * @brief Refills spare ammo clips.
 */
static void GiveMaxClips( gentity_t *self )
{
	playerState_t *ps;
	const weaponAttributes_t *wa;

	if ( !self || !self->client )
	{
		return;
	}

	ps = &self->client->ps;
	wa = BG_Weapon( ps->stats[ STAT_WEAPON ] );

	ps->clips = wa->maxClips;
}

/**
 * @brief Refills current ammo clip/charge.
 */
static void GiveFullClip( gentity_t *self )
{
	playerState_t *ps;
	const weaponAttributes_t *wa;

	if ( !self || !self->client )
	{
		return;
	}

	ps = &self->client->ps;
	wa = BG_Weapon( ps->stats[ STAT_WEAPON ] );

	ps->ammo = wa->maxAmmo;
}

/**
 * @brief Refills both spare clips and current clip/charge.
 */
void G_GiveMaxAmmo( gentity_t *self )
{
	GiveMaxClips( self );
	GiveFullClip( self );
}

/**
 * @brief Checks the condition for G_RefillAmmo.
 */
static bool CanUseAmmoRefill( gentity_t *self )
{
	const weaponAttributes_t *wa;
	playerState_t *ps;

	if ( !self || !self->client )
	{
		return false;
	}

	ps = &self->client->ps;
	wa = BG_Weapon( ps->stats[ STAT_WEAPON ] );

	if ( wa->infiniteAmmo )
	{
		return false;
	}

	if ( wa->maxClips == 0 )
	{
		// clipless weapons can be refilled whenever they lack ammo
		return ( ps->ammo != wa->maxAmmo );
	}
	else if ( ps->clips != wa->maxClips )
	{
		// clip weapons have to miss a clip to be refillable
		return true;
	}
	else
	{
		return false;
	}
}

/**
 * @brief Refills clips on clip based weapons, refills charge on other weapons.
 * @param self
 * @param triggerEvent Trigger an event when relvant resource was modified.
 * @return Whether relevant resource was modified.
 */
bool G_RefillAmmo( gentity_t *self, bool triggerEvent )
{
	if ( !CanUseAmmoRefill( self ) )
	{
		return false;
	}

	self->client->lastAmmoRefillTime = level.time;

	if ( BG_Weapon( self->client->ps.stats[ STAT_WEAPON ] )->maxClips > 0 )
	{
		GiveMaxClips( self );

		if ( triggerEvent )
		{
			G_AddEvent( self, EV_CLIPS_REFILL, 0 );
		}
	}
	else
	{
		GiveFullClip( self );

		if ( triggerEvent )
		{
			G_AddEvent( self, EV_AMMO_REFILL, 0 );
		}
	}

	return true;
}

/**
 * @brief Refills jetpack fuel.
 * @param self
 * @param triggerEvent Trigger an event when fuel was modified.
 * @return Whether fuel was modified.
 */
bool G_RefillFuel( gentity_t *self, bool triggerEvent )
{
	if ( !self || !self->client )
	{
		return false;
	}

	// needs a human with jetpack
	if ( self->client->ps.persistant[ PERS_TEAM ] != TEAM_HUMANS ||
	     !BG_InventoryContainsUpgrade( UP_JETPACK, self->client->ps.stats ) )
	{
		return false;
	}

	if ( self->client->ps.stats[ STAT_FUEL ] != JETPACK_FUEL_MAX )
	{
		self->client->ps.stats[ STAT_FUEL ] = JETPACK_FUEL_MAX;

		self->client->lastFuelRefillTime = level.time;

		if ( triggerEvent )
		{
			G_AddEvent( self, EV_FUEL_REFILL, 0 );
		}

		return true;
	}
	else
	{
		return false;
	}
}

/**
 * @brief Attempts to refill ammo from a close source.
 * @return Whether ammo was refilled.
 */
bool G_FindAmmo( gentity_t *self )
{
	bool  foundSource = false;

	// don't search for a source if refilling isn't possible
	if ( !CanUseAmmoRefill( self ) )
	{
		return false;
	}

	if ( foundSource )
	{
		return G_RefillAmmo( self, true );
	}

	return false;
}

/**
 * @brief Attempts to refill jetpack fuel from a close source.
 * @return true if fuel was refilled.
 */
bool G_FindFuel( gentity_t *self )
{
	bool  foundSource = false;

	if ( !self || !self->client )
	{
		return false;
	}

	if ( foundSource )
	{
		return G_RefillFuel( self, true );
	}

	return false;
}

/*
================
Trace a bounding box against entities, but not the world
Also check there is a line of sight between the start and end point
================
*/
static void G_WideTrace( trace_t *tr, gentity_t *ent, const float range,
                         const float width, const float height, gentity_t **target )
{
	vec3_t mins, maxs, end;
	float  halfDiagonal;

	*target = nullptr;

	if ( !ent->client )
	{
		return;
	}

	// Calculate box to use for trace
	VectorSet( maxs, width, width, height );
	VectorNegate( maxs, mins );
	halfDiagonal = VectorLength( maxs );

	G_UnlaggedOn( ent, muzzle, range + halfDiagonal );

	// Trace box against entities
	VectorMA( muzzle, range, forward, end );
	trap_Trace( tr, muzzle, mins, maxs, end, ent->s.number, CONTENTS_BODY, 0 );

	if ( tr->entityNum != ENTITYNUM_NONE )
	{
		*target = &g_entities[ tr->entityNum ];
	}

	// Line trace against the world, so we never hit through obstacles.
	// The range is reduced according to the former trace so we don't hit something behind the
	// current target.
	VectorMA( muzzle, Distance( muzzle, tr->endpos ) + halfDiagonal, forward, end );
	trap_Trace( tr, muzzle, nullptr, nullptr, end, ent->s.number, CONTENTS_SOLID, 0 );

	// In case we hit a different target, which can happen if two potential targets are close,
	// switch to it, so we will end up with the target we were looking at.
	if ( tr->entityNum != ENTITYNUM_NONE )
	{
		*target = &g_entities[ tr->entityNum ];
	}

	G_UnlaggedOff();
}

/*
======================
Round a vector to integers for more efficient network
transmission, but make sure that it rounds towards a given point
rather than blindly truncating.  This prevents it from truncating
into a wall.
======================
*/
void G_SnapVectorTowards( vec3_t v, vec3_t to )
{
	int i;

	for ( i = 0; i < 3; i++ )
	{
		if ( v[ i ] >= 0 )
		{
			v[ i ] = ( int )( v[ i ] + ( to[ i ] <= v[ i ] ? 0 : 1 ) );
		}
		else
		{
			v[ i ] = ( int )( v[ i ] + ( to[ i ] <= v[ i ] ? -1 : 0 ) );
		}
	}
}

/*
======================
Round a vector to integers for more efficient network
transmission, but make sure that it rounds towards a given point
rather than blindly truncating.  This prevents it from truncating
into a wall.
======================
*/
/*
static void SnapVectorNormal( vec3_t v, vec3_t normal )
{
	int i;

	for ( i = 0; i < 3; i++ )
	{
		if ( v[ i ] >= 0 )
		{
			v[ i ] = ( int )( v[ i ] + ( normal[ i ] <= 0 ? 0 : 1 ) );
		}
		else
		{
			v[ i ] = ( int )( v[ i ] + ( normal[ i ] <= 0 ? -1 : 0 ) );
		}
	}
}
*/

static void SendRangedHitEvent( gentity_t *attacker, gentity_t *target, trace_t *tr )
{
	gentity_t *event;

	// snap the endpos to integers, but nudged towards the line
	G_SnapVectorTowards( tr->endpos, muzzle );

	if ( target->takedamage && target->s.eType == ET_PLAYER )
	{
		event = G_NewTempEntity( tr->endpos, EV_WEAPON_HIT_ENTITY );
	}
	else
	{
		event = G_NewTempEntity( tr->endpos, EV_WEAPON_HIT_ENVIRONMENT );
	}

	// normal
	event->s.eventParm = DirToByte( tr->plane.normal );

	// victim
	event->s.otherEntityNum = target->s.number;

	// attacker
	event->s.otherEntityNum2 = attacker->s.number;

	// weapon
	event->s.weapon = attacker->s.weapon;

	// weapon mode
	event->s.generic1 = attacker->s.generic1;
}

static void SendMeleeHitEvent( gentity_t *attacker, gentity_t *target, trace_t *tr )
{
	gentity_t *event;
	vec3_t    normal, origin;
	float     mag, radius;

	if ( !attacker->client )
	{
		return;
	}

	if ( target->health <= 0 )
	{
		return;
	}

	if ( tr )
	{
		VectorSubtract( tr->endpos, target->s.origin, normal );
	}
	else
	{
		VectorSubtract( attacker->client->ps.origin, target->s.origin, normal );
	}

	// Normalize the horizontal components of the vector difference to the "radius" of the bounding box
	mag = sqrt( normal[ 0 ] * normal[ 0 ] + normal[ 1 ] * normal[ 1 ] );
	radius = target->r.maxs[ 0 ] * 1.21f;

	if ( mag > radius )
	{
		normal[ 0 ] = normal[ 0 ] / mag * radius;
		normal[ 1 ] = normal[ 1 ] / mag * radius;
	}

	// Clamp origin to be within bounding box vertically
	if ( normal[ 2 ] > target->r.maxs[ 2 ] )
	{
		normal[ 2 ] = target->r.maxs[ 2 ];
	}

	if ( normal[ 2 ] < target->r.mins[ 2 ] )
	{
		normal[ 2 ] = target->r.mins[ 2 ];
	}

	VectorAdd( target->s.origin, normal, origin );
	VectorNegate( normal, normal );
	VectorNormalize( normal );

	event = G_NewTempEntity( origin, EV_WEAPON_HIT_ENTITY );

	// normal
	event->s.eventParm = DirToByte( normal );

	// victim
	event->s.otherEntityNum = target->s.number;

	// attacker
	event->s.otherEntityNum2 = attacker->s.number;

	// weapon
	event->s.weapon = attacker->s.weapon;

	// weapon mode
	event->s.generic1 = attacker->s.generic1;
}

static gentity_t *FireMelee( gentity_t *self, float range, float width, float height,
                             int damage, meansOfDeath_t mod )
{
	trace_t   tr;
	gentity_t *traceEnt;

	G_WideTrace( &tr, self, range, width, height, &traceEnt );

	if ( traceEnt != nullptr && traceEnt->takedamage )
	{
		SendMeleeHitEvent( self, traceEnt, &tr );

		G_Damage( traceEnt, self, self, forward, tr.endpos, damage, 0, mod );
	}

	return traceEnt;
}

static void FireLevel1Melee( gentity_t *self )
{
	gentity_t *target;

	target = FireMelee( self, LEVEL1_CLAW_RANGE, LEVEL1_CLAW_WIDTH, LEVEL1_CLAW_WIDTH,
	                    LEVEL1_CLAW_DMG, MOD_LEVEL1_CLAW );

	if ( target && target->client && target->takedamage )
	{
		target->client->ps.stats[ STAT_STATE2 ] |= SS2_LEVEL1SLOW;
		target->client->lastLevel1SlowTime = level.time;
	}
}

/*
======================================================================

MACHINEGUN

======================================================================
*/

static void FireBullet( gentity_t *self, float spread, int damage, int mod )
{
	// TODO: Merge this with other *Fire functions

	trace_t   tr;
	vec3_t    end;
	float     r, u;
	gentity_t *target;

	r = random() * M_PI * 2.0f;
	u = sin( r ) * crandom() * spread * 16;
	r = cos( r ) * crandom() * spread * 16;
	VectorMA( muzzle, 8192 * 16, forward, end );
	VectorMA( end, r, right, end );
	VectorMA( end, u, up, end );

	// don't use unlagged if this is not a client (e.g. turret)
	if ( self->client )
	{
		G_UnlaggedOn( self, muzzle, 8192 * 16 );
		trap_Trace( &tr, muzzle, nullptr, nullptr, end, self->s.number, MASK_SHOT, 0 );
		G_UnlaggedOff();
	}
	else
	{
		trap_Trace( &tr, muzzle, nullptr, nullptr, end, self->s.number, MASK_SHOT, 0 );
	}

	if ( tr.surfaceFlags & SURF_NOIMPACT )
	{
		return;
	}

	target = &g_entities[ tr.entityNum ];

	SendRangedHitEvent( self, target, &tr );

	if ( target->takedamage )
	{
		G_Damage( target, self, self, forward, tr.endpos, damage, 0, mod );
	}
}

/*
======================================================================

SHOTGUN

======================================================================
*/

/*
================
Keep this in sync with ShotgunPattern in CGAME!
================
*/
static void ShotgunPattern( vec3_t origin, vec3_t origin2, int seed, gentity_t *self )
{
	int       i;
	float     r, u, a;
	vec3_t    end;
	vec3_t    forward, right, up;
	trace_t   tr;
	gentity_t *traceEnt;

	// derive the right and up vectors from the forward vector, because
	// the client won't have any other information
	VectorNormalize2( origin2, forward );
	PerpendicularVector( right, forward );
	CrossProduct( forward, right, up );

	// generate the "random" spread pattern
	for ( i = 0; i < SHOTGUN_PELLETS; i++ )
	{
		r = Q_crandom( &seed ) * M_PI;
		a = Q_random( &seed ) * SHOTGUN_SPREAD * 16;

		u = sin( r ) * a;
		r = cos( r ) * a;

		VectorMA( origin, SHOTGUN_RANGE, forward, end );
		VectorMA( end, r, right, end );
		VectorMA( end, u, up, end );

		trap_Trace( &tr, origin, nullptr, nullptr, end, self->s.number, MASK_SHOT, 0 );
		traceEnt = &g_entities[ tr.entityNum ];

		// do the damage
		if ( !( tr.surfaceFlags & SURF_NOIMPACT ) )
		{
			if ( traceEnt->takedamage )
			{
				G_Damage( traceEnt, self, self, forward, tr.endpos, SHOTGUN_DMG, 0, MOD_SHOTGUN );
			}
		}
	}
}

static void FireShotgun( gentity_t *self )
{
	gentity_t *tent;

	// instead of an EV_WEAPON_HIT_* event, send this so client can generate the same spread pattern
	tent = G_NewTempEntity( muzzle, EV_SHOTGUN );
	VectorScale( forward, 4096, tent->s.origin2 );
	SnapVector( tent->s.origin2 );
	tent->s.eventParm = rand() / ( RAND_MAX / 0x100 + 1 ); // seed for spread pattern
	tent->s.otherEntityNum = self->s.number;

	// caclulate the pattern and do the damage
	G_UnlaggedOn( self, muzzle, SHOTGUN_RANGE );
	ShotgunPattern( tent->s.pos.trBase, tent->s.origin2, tent->s.eventParm, self );
	G_UnlaggedOff();
}

/*
======================================================================

MASS DRIVER

======================================================================
*/

static void FireMassdriver( gentity_t *self )
{
	// TODO: Merge this with other *Fire functions

	trace_t   tr;
	vec3_t    end;
	gentity_t *target;

	VectorMA( muzzle, 8192.0f * 16.0f, forward, end );

	G_UnlaggedOn( self, muzzle, 8192.0f * 16.0f );
	trap_Trace( &tr, muzzle, nullptr, nullptr, end, self->s.number, MASK_SHOT, 0 );
	G_UnlaggedOff();

	if ( tr.surfaceFlags & SURF_NOIMPACT )
	{
		return;
	}

	target = &g_entities[ tr.entityNum ];

	// snap the endpos to integers, but nudged towards the line
	G_SnapVectorTowards( tr.endpos, muzzle );

	SendRangedHitEvent( self, target, &tr );

	if ( target->takedamage )
	{
		G_Damage( target, self, self, forward, tr.endpos, MDRIVER_DMG, DAMAGE_KNOCKBACK,
		          MOD_MDRIVER );
	}
}

/*
======================================================================

BLASTER PISTOL

======================================================================
*/

static void FireBlaster( gentity_t *self )
{
	G_SpawnMissile( MIS_BLASTER, self, muzzle, forward, nullptr, G_ExplodeMissile, level.time + 10000 );
}

/*
======================================================================

PULSE RIFLE

======================================================================
*/

static void FirePrifle( gentity_t *self )
{
	G_SpawnMissile( MIS_PRIFLE, self, muzzle, forward, nullptr, G_ExplodeMissile, level.time + 10000 );
}

/*
======================================================================

FLAME THROWER

======================================================================
*/

static void FireFlamer( gentity_t *self )
{
	G_SpawnMissile( MIS_FLAMER, self, muzzle, forward, nullptr, G_FreeEntity, level.time + FLAMER_LIFETIME );
}

/*
======================================================================

GRENADE

======================================================================
*/

static void FireGrenade( gentity_t *self )
{
	G_SpawnMissile( MIS_GRENADE, self, muzzle, forward, nullptr, G_ExplodeMissile, level.time + 5000 );
}

/*
======================================================================

FIREBOMB

======================================================================
*/

#define FIREBOMB_SUBMISSILE_COUNT 15
#define FIREBOMB_IGNITE_RANGE     192
#define FIREBOMB_TIMER            4000

static void FirebombMissileThink( gentity_t *self )
{
	gentity_t *m;
	int       subMissileNum;
	vec3_t    dir;

	// spam fire
	for ( subMissileNum = 0; subMissileNum < FIREBOMB_SUBMISSILE_COUNT; subMissileNum++ )
	{
		dir[ 0 ] = ( rand() / ( float )RAND_MAX ) - 0.5f;
		dir[ 1 ] = ( rand() / ( float )RAND_MAX ) - 0.5f;
		dir[ 2 ] = ( rand() / ( float )RAND_MAX ) * 0.5f;

		VectorNormalize( dir );

		// the submissile's parent is the attacker
		m = G_SpawnMissile( MIS_FIREBOMB_SUB, self->parent, self->s.origin, dir, nullptr, G_FreeEntity, level.time + 10000 );

		// randomize missile speed
		VectorScale( m->s.pos.trDelta, ( rand() / ( float )RAND_MAX ) + 0.5f, m->s.pos.trDelta );
	}

	// explode
	G_ExplodeMissile( self );
}

void FireFirebomb( gentity_t *self )
{
	G_SpawnMissile( MIS_FIREBOMB, self, muzzle, forward, nullptr, FirebombMissileThink, level.time + FIREBOMB_TIMER );
}

/*
======================================================================

LAS GUN

======================================================================
*/

static void FireLasgun( gentity_t *self )
{
	// TODO: Merge this with other *Fire functions

	trace_t   tr;
	vec3_t    end;
	gentity_t *target;

	VectorMA( muzzle, 8192 * 16, forward, end );

	G_UnlaggedOn( self, muzzle, 8192 * 16 );
	trap_Trace( &tr, muzzle, nullptr, nullptr, end, self->s.number, MASK_SHOT, 0 );
	G_UnlaggedOff();

	if ( tr.surfaceFlags & SURF_NOIMPACT )
	{
		return;
	}

	target = &g_entities[ tr.entityNum ];

	// snap the endpos to integers, but nudged towards the line
	G_SnapVectorTowards( tr.endpos, muzzle );

	SendRangedHitEvent( self, target, &tr );

	if ( target->takedamage )
	{
		G_Damage( target, self, self, forward, tr.endpos, LASGUN_DAMAGE, 0, MOD_LASGUN );
	}
}

/*
======================================================================

PAIN SAW

======================================================================
*/

static void FirePainsaw( gentity_t *self )
{
	trace_t   tr;
	gentity_t *target;

	G_WideTrace( &tr, self, PAINSAW_RANGE, PAINSAW_WIDTH, PAINSAW_HEIGHT, &target );

	if ( !target || !target->takedamage )
	{
		return;
	}

	// not really a "ranged" weapon, but this is still the right call
	SendRangedHitEvent( self, target, &tr );

	G_Damage( target, self, self, forward, tr.endpos, PAINSAW_DAMAGE, 0, MOD_PAINSAW );
}

/*
======================================================================

LUCIFER CANNON

======================================================================
*/

static gentity_t *FireLcannonHelper( gentity_t *self, vec3_t start, vec3_t dir,
                                     int damage, int radius, int speed )
{
	// TODO: Tidy up this and lcannonFire

	gentity_t *m;
	int       nextthink;
	float     charge;

	// explode in front of player when overcharged
	if ( damage == LCANNON_DAMAGE )
	{
		nextthink = level.time;
	}
	else
	{
		nextthink = level.time + 10000;
	}

	if ( self->s.generic1 == WPM_PRIMARY )
	{
		m = G_SpawnMissile( MIS_LCANNON, self, start, dir, nullptr, G_ExplodeMissile, nextthink );

		// some values are set in the code
		m->damage       = damage;
		m->splashDamage = damage / 2;
		m->splashRadius = radius;
		VectorScale( dir, speed, m->s.pos.trDelta );
		SnapVector( m->s.pos.trDelta ); // save net bandwidth

		// pass the missile charge through
		charge = ( float )( damage - LCANNON_SECONDARY_DAMAGE ) / LCANNON_DAMAGE;

		m->s.torsoAnim = charge * 255;

		if ( m->s.torsoAnim < 0 )
		{
			m->s.torsoAnim = 0;
		}
	}
	else
	{
		m = G_SpawnMissile( MIS_LCANNON2, self, start, dir, nullptr, G_ExplodeMissile, nextthink );
	}

	return m;
}

static void FireLcannon( gentity_t *self, bool secondary )
{
	if ( secondary && self->client->ps.stats[ STAT_MISC ] <= 0 )
	{
		FireLcannonHelper( self, muzzle, forward, LCANNON_SECONDARY_DAMAGE,
		                   LCANNON_SECONDARY_RADIUS, LCANNON_SECONDARY_SPEED );
	}
	else
	{
		FireLcannonHelper( self, muzzle, forward,
		                   self->client->ps.stats[ STAT_MISC ] * LCANNON_DAMAGE / LCANNON_CHARGE_TIME_MAX,
		                   LCANNON_RADIUS, LCANNON_SPEED );
	}

	self->client->ps.stats[ STAT_MISC ] = 0;
}

/*
======================================================================

LEVEL0

======================================================================
*/

bool G_CheckVenomAttack( gentity_t *self )
{
	trace_t   tr;
	gentity_t *traceEnt;
	int       damage = LEVEL0_BITE_DMG;

	if ( self->client->ps.weaponTime )
	{
		return false;
	}

	// Calculate muzzle point
	AngleVectors( self->client->ps.viewangles, forward, right, up );
	G_CalcMuzzlePoint( self, forward, right, up, muzzle );

	G_WideTrace( &tr, self, LEVEL0_BITE_RANGE, LEVEL0_BITE_WIDTH, LEVEL0_BITE_WIDTH, &traceEnt );

	if ( !traceEnt || !traceEnt->takedamage || traceEnt->health <= 0 ||
	     G_OnSameTeam( self, traceEnt ) )
	{
		return false;
	}

	SendMeleeHitEvent( self, traceEnt, &tr );

	G_Damage( traceEnt, self, self, forward, tr.endpos, damage, 0, MOD_LEVEL0_BITE );

	self->client->ps.weaponTime += LEVEL0_BITE_REPEAT;

	return true;
}

/*
======================================================================

LEVEL2

======================================================================
*/
#define MAX_ZAPS MAX_CLIENTS

static zap_t zaps[ MAX_ZAPS ];

static void FindZapChainTargets( zap_t *zap )
{
	gentity_t *ent = zap->targets[ 0 ]; // the source
	int       entityList[ MAX_GENTITIES ];
	vec3_t    range;
	vec3_t    mins, maxs;
	int       i, num;
	gentity_t *enemy;
	trace_t   tr;
	float     distance;

	VectorSet(range, LEVEL2_AREAZAP_CHAIN_RANGE, LEVEL2_AREAZAP_CHAIN_RANGE, LEVEL2_AREAZAP_CHAIN_RANGE);

	VectorAdd( ent->s.origin, range, maxs );
	VectorSubtract( ent->s.origin, range, mins );

	num = trap_EntitiesInBox( mins, maxs, entityList, MAX_GENTITIES );

	for ( i = 0; i < num; i++ )
	{
		enemy = &g_entities[ entityList[ i ] ];

		// don't chain to self; noclippers can be listed, don't chain to them either
		if ( enemy == ent || ( enemy->client && enemy->client->noclip ) )
		{
			continue;
		}

		distance = Distance( ent->s.origin, enemy->s.origin );

		if ( enemy->client &&
		     enemy->client->pers.team == TEAM_HUMANS &&
		     enemy->health > 0 && // only chain to living targets
		     distance <= LEVEL2_AREAZAP_CHAIN_RANGE )
		{
			// world-LOS check: trace against the world, ignoring other BODY entities
			trap_Trace( &tr, ent->s.origin, nullptr, nullptr,
			            enemy->s.origin, ent->s.number, CONTENTS_SOLID, 0 );

			if ( tr.entityNum == ENTITYNUM_NONE )
			{
				zap->targets[ zap->numTargets ] = enemy;
				zap->distances[ zap->numTargets ] = distance;

				if ( ++zap->numTargets >= LEVEL2_AREAZAP_MAX_TARGETS )
				{
					return;
				}
			}
		}
	}
}

static void UpdateZapEffect( zap_t *zap )
{
	int i;
	int entityNums[ LEVEL2_AREAZAP_MAX_TARGETS + 1 ];

	entityNums[ 0 ] = zap->creator->s.number;

	assert( zap->numTargets <= LEVEL2_AREAZAP_MAX_TARGETS );

	for ( i = 0; i < zap->numTargets; i++ )
	{
		entityNums[ i + 1 ] = zap->targets[ i ]->s.number;
	}

	BG_PackEntityNumbers( &zap->effectChannel->s,
	                      entityNums, zap->numTargets + 1 );


	G_SetOrigin( zap->effectChannel, muzzle );
	trap_LinkEntity( zap->effectChannel );
}

static void CreateNewZap( gentity_t *creator, gentity_t *target )
{
	int   i;
	zap_t *zap;

	for ( i = 0; i < MAX_ZAPS; i++ )
	{
		zap = &zaps[ i ];

		if ( zap->used )
		{
			continue;
		}

		zap->used = true;
		zap->timeToLive = LEVEL2_AREAZAP_TIME;

		zap->creator = creator;
		zap->targets[ 0 ] = target;
		zap->numTargets = 1;

		// the zap chains only through living entities
		if ( target->health > 0 )
		{
			G_Damage( target, creator, creator, forward, target->s.origin, LEVEL2_AREAZAP_DMG,
			          DAMAGE_NO_LOCDAMAGE, MOD_LEVEL2_ZAP );

			FindZapChainTargets( zap );

			for ( i = 1; i < zap->numTargets; i++ )
			{
				G_Damage( zap->targets[ i ], target, zap->creator, forward, target->s.origin,
				          LEVEL2_AREAZAP_DMG * ( 1 - powf( ( zap->distances[ i ] /
				                                 LEVEL2_AREAZAP_CHAIN_RANGE ), LEVEL2_AREAZAP_CHAIN_FALLOFF ) ) + 1,
				          DAMAGE_NO_LOCDAMAGE, MOD_LEVEL2_ZAP );
			}
		}

		zap->effectChannel = G_NewEntity();
		zap->effectChannel->s.eType = ET_LEV2_ZAP_CHAIN;
		zap->effectChannel->classname = "lev2zapchain";
		UpdateZapEffect( zap );

		return;
	}
}

void G_UpdateZaps( int msec )
{
	int   i;
	zap_t *zap;

	for ( i = 0; i < MAX_ZAPS; i++ )
	{
		zap = &zaps[ i ];

		if ( !zap->used )
		{
			continue;
		}

		zap->timeToLive -= msec;

		// the disappearance of players is handled immediately in G_ClearPlayerZapEffects()

		UpdateZapEffect( zap );
	}
}

/*
===============
Called from G_LeaveTeam() and TeleportPlayer().
===============
*/
void G_ClearPlayerZapEffects( gentity_t *player )
{
	int   i, j;
	zap_t *zap;

	for ( i = 0; i < MAX_ZAPS; i++ )
	{
		zap = &zaps[ i ];

		if ( !zap->used )
		{
			continue;
		}

		// the disappearance of the creator or the first target destroys the whole zap effect
		if ( zap->creator == player || zap->targets[ 0 ] == player )
		{
			G_FreeEntity( zap->effectChannel );
			zap->used = false;
			continue;
		}

		// the disappearance of chained players destroy the appropriate beams
		for ( j = 1; j < zap->numTargets; j++ )
		{
			if ( zap->targets[ j ] == player )
			{
				zap->targets[ j-- ] = zap->targets[ --zap->numTargets ];
			}
		}
	}
}

static void FireAreaZap( gentity_t *ent )
{
	trace_t   tr;
	gentity_t *traceEnt;

	G_WideTrace( &tr, ent, LEVEL2_AREAZAP_RANGE, LEVEL2_AREAZAP_WIDTH, LEVEL2_AREAZAP_WIDTH, &traceEnt );

	if ( traceEnt == nullptr )
	{
		return;
	}

	if ( traceEnt->client && traceEnt->client->pers.team == TEAM_HUMANS )
	{
		CreateNewZap( ent, traceEnt );
	}
}

/*
======================================================================

LEVEL3

======================================================================
*/

bool G_CheckPounceAttack( gentity_t *self )
{
	trace_t   tr;
	gentity_t *traceEnt;
	int       damage, timeMax, pounceRange, payload;

	if ( self->client->pmext.pouncePayload <= 0 )
	{
		return false;
	}

	// In case the goon lands on his target, he gets one shot after landing
	payload = self->client->pmext.pouncePayload;

	if ( !( self->client->ps.pm_flags & PMF_CHARGE ) )
	{
		self->client->pmext.pouncePayload = 0;
	}

	// Calculate muzzle point
	AngleVectors( self->client->ps.viewangles, forward, right, up );
	G_CalcMuzzlePoint( self, forward, right, up, muzzle );

	// Trace from muzzle to see what we hit
	pounceRange = self->client->ps.weapon == WP_ALEVEL3 ? LEVEL3_POUNCE_RANGE :
	              LEVEL3_POUNCE_UPG_RANGE;
	G_WideTrace( &tr, self, pounceRange, LEVEL3_POUNCE_WIDTH,
	             LEVEL3_POUNCE_WIDTH, &traceEnt );

	if ( traceEnt == nullptr )
	{
		return false;
	}

	// Send blood impact
	if ( traceEnt->takedamage )
	{
		SendMeleeHitEvent( self, traceEnt, &tr );
	}

	if ( !traceEnt->takedamage )
	{
		return false;
	}

	// Deal damage
	timeMax = self->client->ps.weapon == WP_ALEVEL3 ? LEVEL3_POUNCE_TIME :
	          LEVEL3_POUNCE_TIME_UPG;
	damage = payload * LEVEL3_POUNCE_DMG / timeMax;
	self->client->pmext.pouncePayload = 0;
	G_Damage( traceEnt, self, self, forward, tr.endpos, damage,
	          DAMAGE_NO_LOCDAMAGE, MOD_LEVEL3_POUNCE );

	return true;
}

static void FireBounceball( gentity_t *self )
{
	G_SpawnMissile( MIS_BOUNCEBALL, self, muzzle, forward, nullptr, G_ExplodeMissile, level.time + 3000 );
}

/*
======================================================================

LEVEL4

======================================================================
*/

void G_ChargeAttack( gentity_t *self, gentity_t *victim )
{
	int    damage;
	vec3_t forward;

	if ( !self->client || self->client->ps.stats[ STAT_MISC ] <= 0 ||
	     !( self->client->ps.stats[ STAT_STATE ] & SS_CHARGING ) ||
	     self->client->ps.weaponTime )
	{
		return;
	}

	VectorSubtract( victim->s.origin, self->s.origin, forward );
	VectorNormalize( forward );

	if ( !victim->takedamage )
	{
		return;
	}

	SendMeleeHitEvent( self, victim, nullptr );

	damage = LEVEL4_TRAMPLE_DMG * self->client->ps.stats[ STAT_MISC ] /
	         LEVEL4_TRAMPLE_DURATION;

	G_Damage( victim, self, self, forward, victim->s.origin, damage,
	          DAMAGE_NO_LOCDAMAGE, MOD_LEVEL4_TRAMPLE );

	self->client->ps.weaponTime += LEVEL4_TRAMPLE_REPEAT;
}

/*
======================================================================

GENERIC

======================================================================
*/

static INLINE meansOfDeath_t ModWeight( const gentity_t *self )
{
	return self->client->pers.team == TEAM_HUMANS ? MOD_WEIGHT_H : MOD_WEIGHT_A;
}

void G_ImpactAttack( gentity_t *self, gentity_t *victim )
{
	float  impactVelocity, impactEnergy;
	vec3_t knockbackDir;
	int    attackerMass, impactDamage;

	// self must be a client
	if ( !self->client )
	{
		return;
	}

	// ignore invincible targets
	if ( !victim->takedamage )
	{
		return;
	}

	// don't do friendly fire
	if ( G_OnSameTeam( self, victim ) )
	{
		return;
	}

	// attacker must be above victim
	if ( self->client->ps.origin[ 2 ] + self->r.mins[ 2 ] <
	     victim->s.origin[ 2 ] + victim->r.maxs[ 2 ] )
	{
		return;
	}

	// allow the granger airlifting ritual
	if ( victim->client && victim->client->ps.stats[ STAT_STATE2 ] & SS2_JETPACK_ACTIVE &&
	     ( self->client->pers.classSelection == PCL_ALIEN_BUILDER0 ||
	       self->client->pers.classSelection == PCL_ALIEN_BUILDER0_UPG ) )
	{
		return;
	}

	// calculate impact damage
	attackerMass = BG_Class( self->client->pers.classSelection )->mass;
	impactVelocity = fabs( self->client->pmext.fallImpactVelocity[ 2 ] ) * QU_TO_METER; // in m/s
	impactEnergy = attackerMass * impactVelocity * impactVelocity; // in J
	impactDamage = ( int )( impactEnergy * IMPACTDMG_JOULE_TO_DAMAGE );

	// deal impact damage to clients, use a threshold for friendly fire
	if ( impactDamage > 0 )
	{
		// calculate knockback direction
		VectorSubtract( victim->s.origin, self->client->ps.origin, knockbackDir );
		VectorNormalize( knockbackDir );

		G_Damage( victim, self, self, knockbackDir, victim->s.origin, impactDamage,
		          DAMAGE_NO_LOCDAMAGE, ModWeight( self ) );
	}
}

void G_WeightAttack( gentity_t *self, gentity_t *victim )
{
	float  weightDPS;
	int    attackerMass, victimMass, weightDamage;

	// weigth damage is only dealt between clients
	if ( !self->client || !victim->client )
	{
		return;
	}

	// don't do friendly fire
	if ( G_OnSameTeam( self, victim ) )
	{
		return;
	}

	// ignore invincible targets
	if ( !victim->takedamage )
	{
		return;
	}

	// attacker must be above victim
	if ( self->client->ps.origin[ 2 ] + self->r.mins[ 2 ] <
	     victim->s.origin[ 2 ] + victim->r.maxs[ 2 ] )
	{
		return;
	}

	// victim must be on the ground
	if ( victim->client->ps.groundEntityNum == ENTITYNUM_NONE )
	{
		return;
	}

	// check timer
	if ( victim->client->nextCrushTime > level.time )
	{
		return;
	}

	attackerMass = BG_Class( self->client->pers.classSelection )->mass;
	victimMass = BG_Class( victim->client->pers.classSelection )->mass;
	weightDPS = WEIGHTDMG_DMG_MODIFIER * MAX( attackerMass - victimMass, 0 );

	if ( weightDPS > WEIGHTDMG_DPS_THRESHOLD )
	{
		weightDamage = ( int )( weightDPS * ( WEIGHTDMG_REPEAT / 1000.0f ) );

		if ( weightDamage > 0 )
		{
			G_Damage( victim, self, self, nullptr, victim->s.origin, weightDamage,
					  DAMAGE_NO_LOCDAMAGE, ModWeight( self ) );
		}
	}

	victim->client->nextCrushTime = level.time + WEIGHTDMG_REPEAT;
}

//======================================================================

/*
===============
Set muzzle location relative to pivoting eye.
===============
*/
void G_CalcMuzzlePoint( gentity_t *self, vec3_t forward, vec3_t right, vec3_t up, vec3_t muzzlePoint )
{
	vec3_t normal;

	VectorCopy( self->client->ps.origin, muzzlePoint );
	BG_GetClientNormal( &self->client->ps, normal );
	VectorMA( muzzlePoint, self->client->ps.viewheight, normal, muzzlePoint );
	VectorMA( muzzlePoint, 1, forward, muzzlePoint );
	// snap to integer coordinates for more efficient network bandwidth usage
	SnapVector( muzzlePoint );
}

void G_FireWeapon( gentity_t *self, weapon_t weapon, weaponMode_t weaponMode )
{
	// calculate muzzle
	if ( self->client )
	{
		AngleVectors( self->client->ps.viewangles, forward, right, up );
		G_CalcMuzzlePoint( self, forward, right, up, muzzle );
	}

	switch ( weaponMode )
	{
		case WPM_PRIMARY:
		{
			switch ( weapon )
			{
				case WP_ALEVEL1:
					FireLevel1Melee( self );
					break;

				case WP_ALEVEL3:
					FireMelee( self, LEVEL3_CLAW_RANGE, LEVEL3_CLAW_WIDTH, LEVEL3_CLAW_WIDTH,
					           LEVEL3_CLAW_DMG, MOD_LEVEL3_CLAW );
					break;

				case WP_ALEVEL3_UPG:
					FireMelee( self, LEVEL3_CLAW_UPG_RANGE, LEVEL3_CLAW_WIDTH, LEVEL3_CLAW_WIDTH,
					           LEVEL3_CLAW_DMG, MOD_LEVEL3_CLAW );
					break;

				case WP_ALEVEL2:
					FireMelee( self, LEVEL2_CLAW_RANGE, LEVEL2_CLAW_WIDTH, LEVEL2_CLAW_WIDTH,
					           LEVEL2_CLAW_DMG, MOD_LEVEL2_CLAW );
					break;

				case WP_ALEVEL2_UPG:
					FireMelee( self, LEVEL2_CLAW_U_RANGE, LEVEL2_CLAW_WIDTH, LEVEL2_CLAW_WIDTH,
					           LEVEL2_CLAW_DMG, MOD_LEVEL2_CLAW );
					break;

				case WP_ALEVEL4:
					FireMelee( self, LEVEL4_CLAW_RANGE, LEVEL4_CLAW_WIDTH, LEVEL4_CLAW_HEIGHT,
					           LEVEL4_CLAW_DMG, MOD_LEVEL4_CLAW );
					break;

				case WP_BLASTER:
					FireBlaster( self );
					break;

				case WP_MACHINEGUN:
					FireBullet( self, RIFLE_SPREAD, RIFLE_DMG, MOD_MACHINEGUN );
					break;

				case WP_SHOTGUN:
					FireShotgun( self );
					break;

				case WP_CHAINGUN:
					FireBullet( self, CHAINGUN_SPREAD, CHAINGUN_DMG, MOD_CHAINGUN );
					break;

				case WP_FLAMER:
					FireFlamer( self );
					break;

				case WP_PULSE_RIFLE:
					FirePrifle( self );
					break;

				case WP_MASS_DRIVER:
					FireMassdriver( self );
					break;

				case WP_LUCIFER_CANNON:
					FireLcannon( self, false );
					break;

				case WP_LAS_GUN:
					FireLasgun( self );
					break;

				case WP_PAIN_SAW:
					FirePainsaw( self );
					break;

				case WP_ABUILD:
				case WP_ABUILD2:
					break;

				case WP_HBUILD:
					break;

				default:
					break;
			}
			break;
		}
		case WPM_SECONDARY:
		{
			switch ( weapon )
			{
				case WP_LUCIFER_CANNON:
					FireLcannon( self, true );
					break;

				case WP_ALEVEL2_UPG:
					FireAreaZap( self );
					break;

				case WP_ABUILD:
				case WP_ABUILD2:
				case WP_HBUILD:
					break;

				default:
					break;
			}
			break;
		}
		case WPM_TERTIARY:
		{
			switch ( weapon )
			{
				case WP_ALEVEL3_UPG:
					FireBounceball( self );
					break;

				case WP_ABUILD2:
					break;

				default:
					break;
			}
			break;
		}
		default:
		{
			break;
		}
	}

}

void G_FireUpgrade( gentity_t *self, upgrade_t upgrade )
{
	if ( !self || !self->client )
	{
		Com_Printf( S_WARNING "G_FireUpgrade: Called with non-player parameter.\n" );
		return;
	}

	if ( upgrade <= UP_NONE || upgrade >= UP_NUM_UPGRADES )
	{
		Com_Printf( S_WARNING "G_FireUpgrade: Called with unknown upgrade.\n" );
		return;
	}

	AngleVectors( self->client->ps.viewangles, forward, right, up );
	G_CalcMuzzlePoint( self, forward, right, up, muzzle );

	switch ( upgrade )
	{
		case UP_GRENADE:  FireGrenade( self );  break;
		case UP_FIREBOMB: FireFirebomb( self ); break;
		default:                                break;
	}

	switch ( upgrade )
	{
		case UP_GRENADE:
		case UP_FIREBOMB:
			trap_SendServerCommand( self->client->ps.clientNum, "vcommand grenade" );
			break;

		default:
			break;
	}
}
