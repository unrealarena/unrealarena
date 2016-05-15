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


#include "sg_local.h"
#include "sg_spawn.h"

/*
=================================================================================

game_score

=================================================================================
*/

void game_score_act( gentity_t *self, gentity_t*, gentity_t *activator )
{
	if ( !activator )
	{
		return;
	}

	G_AddCreditsToScore( activator, self->config.amount );
}

void SP_game_score( gentity_t *self )
{
	if ( !self->config.amount )
	{
		if( G_SpawnInt( "count", "0", &self->config.amount) )
		{
			G_WarnAboutDeprecatedEntityField( self, "amount", "count", ENT_V_RENAMED );
		}
		else
		{
			self->config.amount = 1;
		}
	}

	self->act = game_score_act;
}

/*
=================================================================================

game_end

=================================================================================
*/
void game_end_act( gentity_t *self, gentity_t*, gentity_t* )
{
	if ( level.unconditionalWin == TEAM_NONE ) // only if not yet triggered
	{
		level.unconditionalWin = self->conditions.team;
	}
}

void SP_game_end( gentity_t *self )
{
#ifdef UNREALARENA
	if(!Q_stricmp(self->classname, "target_u_win"))
	{
		self->conditions.team = TEAM_U;
	}
	else if(!Q_stricmp(self->classname, "target_q_win"))
	{
		self->conditions.team = TEAM_Q;
	}
#else
	if(!Q_stricmp(self->classname, "target_human_win"))
	{
		self->conditions.team = TEAM_HUMANS;
	}
	else if(!Q_stricmp(self->classname, "target_alien_win"))
	{
		self->conditions.team = TEAM_ALIENS;
	}
#endif

	self->act = game_end_act;
}

/*
=================================================================================

game_funds

=================================================================================
*/

void game_funds_act( gentity_t *self, gentity_t*, gentity_t *activator )
{
	if( !activator )
	{
		return;
	}

	G_AddCreditToClient( activator->client, self->amount, true );
}

void game_funds_reset( gentity_t *self )
{
	G_ResetIntField( &self->amount, false, self->config.amount, self->eclass->config.amount, 0);
}

void SP_game_funds( gentity_t *self )
{
	self->act = game_funds_act;
	self->reset = game_funds_reset;
}


/*
=================================================================================

game_kill

=================================================================================
*/
void game_kill_act( gentity_t*, gentity_t*, gentity_t *activator )
{
	if ( !activator )
	{
		return;
	}

	G_Damage( activator, nullptr, nullptr, nullptr, nullptr, 100000, DAMAGE_NO_PROTECTION, MOD_TELEFRAG );
}

void SP_game_kill( gentity_t *self )
{
	self->act = game_kill_act;
}
