/*
 * CBSE Source Code
 * Copyright (C) 2016  Unreal Arena
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


#include "KnockbackComponent.h"

static Log::Logger knockbackLogger("sgame.knockback");

KnockbackComponent::KnockbackComponent(Entity& entity)
	: KnockbackComponentBase(entity)
{}

// TODO: Consider location as well as direction when both given.
#ifdef UNREALARENA
void KnockbackComponent::HandleDamage(int amount, gentity_t* source, Util::optional<Vec3> location,
                                      Util::optional<Vec3> direction, int flags, meansOfDeath_t meansOfDeath) {
#else
void KnockbackComponent::HandleDamage(float amount, gentity_t* source, Util::optional<Vec3> location,
                                      Util::optional<Vec3> direction, int flags, meansOfDeath_t meansOfDeath) {
#endif
	if (!(flags & DAMAGE_KNOCKBACK)) return;
#ifdef UNREALARENA
	if (amount <= 0) return;
#else
	if (amount <= 0.0f) return;
#endif

	if (!direction) {
		knockbackLogger.Warn("Received damage message with knockback flag set but no direction.");
		return;
	}

	if (Math::Length(direction.value()) == 0.0f) {
		knockbackLogger.Warn("Attempt to do knockback with null vector direction.");
		return;
	}

	// TODO: Remove dependency on client.
	gclient_t *client = entity.oldEnt->client;
	ASSERT(client != nullptr);

	// Check for immunity.
	if (client->noclip) return;
	if (client->sess.spectatorState != SPECTATOR_NOT) return;

#ifdef UNREALARENA
	int knockback = std::min(amount, (const int)KNOCKBACK_NORMAL_MASS);
	int strength = knockback * DAMAGE_TO_KNOCKBACK;
#else
	float mass = (float)BG_Class(client->ps.stats[ STAT_CLASS ])->mass;

	if (mass <= 0.0f) {
		knockbackLogger.Warn("Attempt to do knockback against target with no mass, assuming normal mass.");
		mass = KNOCKBACK_NORMAL_MASS;
	}

	float massMod  = Math::Clamp(KNOCKBACK_NORMAL_MASS / mass, KNOCKBACK_MIN_MASSMOD, KNOCKBACK_MAX_MASSMOD);
	float strength = amount * DAMAGE_TO_KNOCKBACK * massMod;
#endif

	// Change client velocity.
	Vec3 clientVelocity = Vec3::Load(client->ps.velocity);
	clientVelocity += Math::Normalize(direction.value()) * strength;
	clientVelocity.Store(client->ps.velocity);

	// Set pmove timer so that the client can't cancel out the movement immediately.
	if (!client->ps.pm_time) {
#ifdef UNREALARENA
		client->ps.pm_time = Math::Clamp(knockback * 2, KNOCKBACK_MIN_PMOVE_TIME, KNOCKBACK_MAX_PMOVE_TIME);
#else
		client->ps.pm_time = KNOCKBACK_PMOVE_TIME;
#endif
		client->ps.pm_flags |= PMF_TIME_KNOCKBACK;
	}

#ifdef UNREALARENA
	knockbackLogger.Debug("Knockback: client: %i (strength: %.1f)",
	                      entity.oldEnt->s.number, strength);
#else
	knockbackLogger.Debug("Knockback: client: %i, strength: %.1f (massMod: %.1f).",
	                      entity.oldEnt->s.number, strength, massMod);
#endif
}
