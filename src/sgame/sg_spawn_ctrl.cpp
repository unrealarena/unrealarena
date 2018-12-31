/*
 * Unvanquished GPL Source Code
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


#include "sg_local.h"
#include "sg_spawn.h"

/*
=================================================================================

ctrl_relay

=================================================================================
*/

void target_relay_act( gentity_t *self, gentity_t*, gentity_t *activator )
{
#ifndef UNREALARENA
	if (!self->enabled)
		return;
#endif

#ifdef UNREALARENA
	if ( ( self->spawnflags & 1 ) && activator && activator->client &&
	     activator->client->pers.team != TEAM_U )
	{
		return;
	}

	if ( ( self->spawnflags & 2 ) && activator && activator->client &&
	     activator->client->pers.team != TEAM_Q )
	{
		return;
	}
#else
	if ( ( self->spawnflags & 1 ) && activator && activator->client &&
	     activator->client->pers.team != TEAM_HUMANS )
	{
		return;
	}

	if ( ( self->spawnflags & 2 ) && activator && activator->client &&
	     activator->client->pers.team != TEAM_ALIENS )
	{
		return;
	}
#endif

	if ( self->spawnflags & 4 )
	{
		G_FireEntityRandomly( self, activator );
		return;
	}

	if ( !self->config.wait.time )
	{
		G_FireEntity( self, activator );
	}
	else
	{
		self->nextthink = VariatedLevelTime( self->config.wait );
		self->think = think_fireDelayed;
		self->activator = activator;
	}
}

void ctrl_relay_reset( gentity_t *self )
{
#ifndef UNREALARENA
	self->enabled = !(self->spawnflags & SPF_SPAWN_DISABLED);
#endif
}

void ctrl_relay_act( gentity_t *self, gentity_t*, gentity_t *activator )
{
#ifndef UNREALARENA
	if (!self->enabled)
		return;
#endif

	if ( !self->config.wait.time )
	{
		G_EventFireEntity( self, activator, ON_ACT );
	}
	else
	{
		self->nextthink = VariatedLevelTime( self->config.wait );
		self->think = think_fireOnActDelayed;
		self->activator = activator;
	}
}

void SP_ctrl_relay( gentity_t *self )
{
	if( Q_stricmp(self->classname, S_CTRL_RELAY ) ) //if anything but ctrl_relay
	{
		if ( !self->config.wait.time ) {
			// check delay for backwards compatibility
			G_SpawnFloat( "delay", "0", &self->config.wait.time );

			//target delay had previously a default of 1 instead of 0
			if ( !self->config.wait.time && !Q_stricmp(self->classname, "target_delay") )
			{
				self->config.wait.time = 1;
			}
		}
		SP_WaitFields(self, 0, 0 );

		self->act = target_relay_act;
		return;
	}

	SP_WaitFields(self, 0, 0 );
	self->act = ctrl_relay_act;
	self->reset = ctrl_relay_reset;
}

/*
=================================================================================

ctrl_limited

=================================================================================
*/

void ctrl_limited_act(gentity_t *self, gentity_t*, gentity_t *activator)
{
#ifndef UNREALARENA
	if (!self->enabled)
		return;
#endif

	G_FireEntity( self, activator );
	if ( self->count <= 1 )
	{
		G_FreeEntity( self );
		return;
	}
	self->count--;
}

void ctrl_limited_reset( gentity_t *self )
{
#ifndef UNREALARENA
	self->enabled = !(self->spawnflags & SPF_SPAWN_DISABLED);
#endif

	G_ResetIntField(&self->count, true, self->config.amount, self->eclass->config.amount, 1);
}

void SP_ctrl_limited( gentity_t *self )
{
	self->act = ctrl_limited_act;
	self->reset = ctrl_limited_reset;
}
