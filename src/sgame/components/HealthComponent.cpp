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

HealthComponent::HealthComponent(Entity& entity)
	: HealthComponentBase(entity)
{}

void HealthComponent::HandlePrepareNetCode() {
	// TODO: Implement HealthComponent::PrepareNetCode
}

void HealthComponent::HandleHeal(int amount, gentity_t* source) {
	// TODO: Implement HealthComponent::Heal
}

void HealthComponent::HandleDamage(int amount, gentity_t* source, Util::optional<Vec3> location, Util::optional<Vec3> direction, int flags, meansOfDeath_t meansOfDeath) {
	// TODO: Implement HealthComponent::Damage
}
