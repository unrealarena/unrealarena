/*
 * CBSE Source Code
 * Copyright (C) 2018  Unreal Arena
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


#include "SpectatorComponent.h"

SpectatorComponent::SpectatorComponent(Entity& entity, ClientComponent& r_ClientComponent)
	: SpectatorComponentBase(entity, r_ClientComponent)
{}

void SpectatorComponent::HandlePrepareNetCode() {
	gclient_t* cl = entity.oldEnt->client;

	if (!cl
		|| cl->sess.spectatorState != SPECTATOR_FOLLOW
		|| cl->sess.spectatorClient < 0
		|| cl->sess.spectatorClient >= level.maxclients
		|| level.clients[cl->sess.spectatorClient].pers.connected != CON_CONNECTED) {
		return;
	}

	// Save
	int score = cl->ps.persistant[PERS_SCORE];
	int ping = cl->ps.ping;

	// Copy
	cl->ps = level.clients[cl->sess.spectatorClient].ps;

	// Restore
	cl->ps.persistant[PERS_SCORE] = score;
	cl->ps.ping = ping;

	cl->ps.pm_flags |= PMF_FOLLOW;
	cl->ps.pm_flags &= ~PMF_QUEUED;
}
