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


#include "HealthComponent.h"
#include "math.h"

static Log::Logger healthLogger("sgame.health");

#ifdef UNREALARENA
HealthComponent::HealthComponent(Entity& entity)
	: HealthComponentBase(entity), health(MAX_HEALTH)
#else
HealthComponent::HealthComponent(Entity& entity, float maxHealth)
	: HealthComponentBase(entity, maxHealth), health(maxHealth)
#endif
{}

// TODO: Handle rewards array.
HealthComponent& HealthComponent::operator=(const HealthComponent& other) {
#ifdef UNREALARENA
	health = other.health;
#else
	health = (other.health / other.maxHealth) * maxHealth;
#endif
	return *this;
}

void HealthComponent::HandlePrepareNetCode() {
	int transmittedHealth = Math::Clamp((int)std::ceil(health), -999, 999);
	gclient_t *client = entity.oldEnt->client;

	if (client) {
		if (client->ps.stats[STAT_HEALTH] != transmittedHealth) {
			client->pers.infoChangeTime = level.time;
		}
		client->ps.stats[STAT_HEALTH] = transmittedHealth;
#ifdef UNREALARENA
		client->ps.stats[STAT_MAX_HEALTH] = (int)std::ceil(MAX_HEALTH);
	}
#else
		client->ps.stats[STAT_MAX_HEALTH] = (int)std::ceil(maxHealth);
	} else if (entity.oldEnt->s.eType == ET_BUILDABLE) {
		entity.oldEnt->s.generic1 = std::max(transmittedHealth, 0);
	}
#endif
}

#ifdef UNREALARENA
void HealthComponent::HandleHeal(int amount, gentity_t* source) {
#else
void HealthComponent::HandleHeal(float amount, gentity_t* source) {
#endif
#ifdef UNREALARENA
	if (health <= 0) return;
	if (health >= MAX_HEALTH) return;
#else
	if (health <= 0.0f) return;
	if (health >= maxHealth) return;
#endif

	// Only heal up to maximum health.
#ifdef UNREALARENA
	amount = std::min(amount, MAX_HEALTH - health);
#else
	amount = std::min(amount, maxHealth - health);
#endif

#ifdef UNREALARENA
	if (amount <= 0) return;
#else
	if (amount <= 0.0f) return;
#endif

#ifdef UNREALARENA
	healthLogger.Debug("Healing: %3d (%3d --> %3d)", amount, health, health + amount);
#else
	healthLogger.Debug("Healing: %3.1f (%3.1f → %3.1f)", amount, health, health + amount);
#endif

	health += amount;
#ifndef UNREALARENA
	ScaleDamageAccounts(amount);
#endif
}

#ifdef UNREALARENA
void HealthComponent::HandleDamage(int amount, gentity_t* source, Util::optional<Vec3> location,
Util::optional<Vec3> direction, int flags, meansOfDeath_t meansOfDeath) {
#else
void HealthComponent::HandleDamage(float amount, gentity_t* source, Util::optional<Vec3> location,
Util::optional<Vec3> direction, int flags, meansOfDeath_t meansOfDeath) {
#endif
#ifdef UNREALARENA
	if (health <= 0) return;
	if (amount <= 0) return;
#else
	if (health <= 0.0f) return;
	if (amount <= 0.0f) return;
#endif

	gclient_t *client = entity.oldEnt->client;

	// Check for immunity.
	if (entity.oldEnt->flags & FL_GODMODE) return;
	if (client) {
		if (client->noclip) return;
		if (client->sess.spectatorState != SPECTATOR_NOT) return;
	}

	// Set source to world if missing.
	if (!source) source = &g_entities[ENTITYNUM_WORLD];

	// Don't handle ET_MOVER w/o die or pain function.
	// TODO: Handle mover special casing in a dedicated component.
	if (entity.oldEnt->s.eType == ET_MOVER && !(entity.oldEnt->die || entity.oldEnt->pain)) {
		// Special case for ET_MOVER with act function in initial position.
		if ((entity.oldEnt->moverState == MOVER_POS1 || entity.oldEnt->moverState == ROTATOR_POS1)
		    && entity.oldEnt->act) {
			entity.oldEnt->act(entity.oldEnt, source, source);
		}

		return;
	}

	// Check for protection.
	if (!(flags & DAMAGE_NO_PROTECTION)) {
		// Check for protection from friendly damage.
		if (entity.oldEnt != source && G_OnSameTeam(entity.oldEnt, source)) {
			// Check if friendly fire has been disabled.
			if (!g_friendlyFire.integer) return;

#ifndef UNREALARENA
			// Never do friendly damage on movement attacks.
			switch (meansOfDeath) {
				case MOD_LEVEL3_POUNCE:
				case MOD_LEVEL4_TRAMPLE:
					return;

				default:
					break;
			}

			// If dretchpunt is enabled and this is a dretch, do dretchpunt instead of damage.
			// TODO: Add a message for pushing.
			if (g_dretchPunt.integer && client && client->ps.stats[STAT_CLASS] == PCL_ALIEN_LEVEL0)
			{
				vec3_t dir, push;

				VectorSubtract(entity.oldEnt->r.currentOrigin, source->r.currentOrigin, dir);
				VectorNormalizeFast(dir);
				VectorScale(dir, (amount * 10.0f), push);
				push[ 2 ] = 64.0f;

				VectorAdd( client->ps.velocity, push, client->ps.velocity );

				return;
			}
#endif
		}

#ifndef UNREALARENA
		// Check for protection from friendly buildable damage. Never protect from building actions.
		// TODO: Use DAMAGE_NO_PROTECTION flag instead of listing means of death here.
		if (entity.oldEnt->s.eType == ET_BUILDABLE && source->client &&
		    meansOfDeath != MOD_DECONSTRUCT && meansOfDeath != MOD_SUICIDE &&
		    meansOfDeath != MOD_REPLACE     && meansOfDeath != MOD_NOCREEP) {
			if (G_OnSameTeam(entity.oldEnt, source) && !g_friendlyBuildableFire.integer) {
				return;
			}
		}
#endif
	}

#ifdef UNREALARENA
	int take = amount;
#else
	float take = amount;
#endif

	// Apply damage modifiers.
#ifdef UNREALARENA
	// [TODO] UNIMPLEMENTED + add damage modifier depending on the hit location
#else
	if (!(flags & DAMAGE_PURE)) {
		entity.ApplyDamageModifier(take, location, direction, flags, meansOfDeath);
	}
#endif

	// Update combat timers.
	// TODO: Add a message to update combat timers.
	if (client && source->client && entity.oldEnt != source) {
		client->lastCombatTime = entity.oldEnt->client->lastCombatTime = level.time;
	}

	if (client) {
		// Save damage w/o armor modifier.
#ifdef UNREALARENA
		client->damage_received += amount;
#else
		client->damage_received += (int)(amount + 0.5f);
#endif

		// Save damage direction.
		if (direction) {
			VectorCopy(direction.value().Data(), client->damage_from);
			client->damage_fromWorld = false;
		} else {
			VectorCopy(entity.oldEnt->r.currentOrigin, client->damage_from);
			client->damage_fromWorld = true;
		}

#ifndef UNREALARENA
		// Drain jetpack fuel.
		// TODO: Have another component handle jetpack fuel drain.
		client->ps.stats[STAT_FUEL] = std::max(0, client->ps.stats[STAT_FUEL] -
		                                       (int)(amount + 0.5f) * JETPACK_FUEL_PER_DMG);

		// If boosted poison every attack.
		// TODO: Add a poison message and a PoisonableComponent.
		if (source->client && (source->client->ps.stats[STAT_STATE] & SS_BOOSTED) &&
		    client->pers.team == TEAM_HUMANS && client->poisonImmunityTime < level.time) {
			switch (meansOfDeath) {
				case MOD_POISON:
				case MOD_LEVEL2_ZAP:
					break;

				default:
					client->ps.stats[STAT_STATE] |= SS_POISONED;
					client->lastPoisonTime   = level.time;
					client->lastPoisonClient = source;
					break;
			}
		}
#endif
	}

#ifdef UNREALARENA
	healthLogger.Notice("Taking damage: %3d (%3d --> %3d)", take, health, health - take);
#else
	healthLogger.Notice("Taking damage: %3.1f (%3.1f → %3.1f)", take, health, health - take);
#endif

	// Do the damage.
	health -= take;

	// Update team overlay info.
	if (client) client->pers.infoChangeTime = level.time;

	// TODO: Move lastDamageTime to HealthComponent.
	entity.oldEnt->lastDamageTime = level.time;

#ifndef UNREALARENA
	// HACK: gentity_t.nextRegenTime only affects alien clients.
	// TODO: Catch damage message in a new RegenerationComponent.
	entity.oldEnt->nextRegenTime = level.time + ALIEN_CLIENT_REGEN_WAIT;
#endif

	// Handle non-self damage.
	if (entity.oldEnt != source) {
		float loss = take;

#ifdef UNREALARENA
		if (health < 0) loss += health;
#else
		if (health < 0.0f) loss += health;
#endif

		// TODO: Use ClientComponent.
		if (source->client) {
#ifndef UNREALARENA
			// Add to the attacker's account on the target.
			// TODO: Move damage account array to HealthComponent.
			entity.oldEnt->credits[source->client->ps.clientNum].value += loss;
			entity.oldEnt->credits[source->client->ps.clientNum].time = level.time;
			entity.oldEnt->credits[source->client->ps.clientNum].team = (team_t)source->client->pers.team;
#endif

			// Notify the attacker of a hit.
			SpawnHitNotification(source);
		}
	}

	// Handle death.
	// TODO: Send a Die/Pain message and handle details where appropriate.
	if (health <= 0) {
#ifdef UNREALARENA
		healthLogger.Notice("Dying with %d health.", health);
#else
		healthLogger.Notice("Dying with %.1f health.", health);
#endif

		// Disable knockback.
		if (client) entity.oldEnt->flags |= FL_NO_KNOCKBACK;

		// Call legacy die function.
		if (entity.oldEnt->die) entity.oldEnt->die(entity.oldEnt, source, source, meansOfDeath);

#ifndef UNREALARENA
		// Send die message.
		entity.Die(source, meansOfDeath);
#endif

		// Trigger ON_DIE event.
		if(!client) G_EventFireEntity(entity.oldEnt, source, ON_DIE);
	} else if (entity.oldEnt->pain) {
		entity.oldEnt->pain(entity.oldEnt, source, (int)std::ceil(take));
	}
}

#ifndef UNREALARENA
void HealthComponent::SetHealth(float health) {
	Math::Clamp(health, FLT_EPSILON, maxHealth);

	healthLogger.Debug("Changing health: %3.1f → %3.1f.", this->health, health);

	ScaleDamageAccounts(health - this->health);
	HealthComponent::health = health;
}

void HealthComponent::SetMaxHealth(float maxHealth, bool scaleHealth) {
	ASSERT_GT(maxHealth, 0.0f);

	healthLogger.Debug("Changing maximum health: %3.1f → %3.1f.", this->maxHealth, maxHealth);

	HealthComponent::maxHealth = maxHealth;
	if (scaleHealth) SetHealth(health * (this->maxHealth / maxHealth));
}

// TODO: Move credits array to HealthComponent.
void HealthComponent::ScaleDamageAccounts(float healthRestored) {
	if (healthRestored <= 0.0f) return;

	// Get total damage account and remember relevant clients.
	float totalAccreditedDamage = 0.0f;
	std::vector<Entity*> relevantClients;
	ForEntities<ClientComponent>([&](Entity& other, ClientComponent& client) {
		float clientDamage = entity.oldEnt->credits[other.oldEnt->s.number].value;
		if (clientDamage > 0.0f) {
			totalAccreditedDamage += clientDamage;
			relevantClients.push_back(&other);
		}
	});

	if (relevantClients.empty()) return;

	// Calculate account scale factor.
	float scale;
	if (healthRestored < totalAccreditedDamage) {
		scale = (totalAccreditedDamage - healthRestored) / totalAccreditedDamage;

		healthLogger.Debug("Scaling damage accounts of %i client(s) by %.2f.",
		                   relevantClients.size(), scale);
	} else {
		// Clear all accounts.
		scale = 0.0f;

		healthLogger.Debug("Clearing damage accounts of %i client(s).", relevantClients.size());
	}

	// Scale down or clear damage accounts.
	for (Entity* other : relevantClients) {
		entity.oldEnt->credits[other->oldEnt->s.number].value *= scale;
	}
}
#endif

// TODO: Replace this with a call to a proper event factory.
void HealthComponent::SpawnHitNotification(gentity_t *attacker)
{
	if (!attacker->client) return;

	gentity_t *event = G_NewTempEntity(attacker->s.origin, EV_HIT);
	event->r.svFlags = SVF_SINGLECLIENT;
	event->r.singleClient = attacker->client->ps.clientNum;
}
