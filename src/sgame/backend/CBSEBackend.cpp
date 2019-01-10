/*
 * CBSE GPL Source Code
 * Copyright (C) 2016-2019  Unreal Arena
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


// THIS FILE IS AUTO GENERATED, EDIT AT YOUR OWN RISK

/*
 * This file contains:
 *   - Implementation of the base entity.
 *   - Implementations of the specific entities and related helpers.
 */

#include "CBSEEntities.h"
#include <tuple>

#define myoffsetof(st, m) static_cast<int>((size_t)(&((st *)1)->m))-1

// /////////// //
// Base entity //
// /////////// //

// Base entity constructor.
Entity::Entity(const MessageHandler *messageHandlers, const int* componentOffsets, gentity_t* oldEnt)
	: messageHandlers(messageHandlers), componentOffsets(componentOffsets), oldEnt(oldEnt)
{}

// Base entity's message dispatcher.
bool Entity::SendMessage(EntityMessage msg, const void* data) {
	MessageHandler handler = messageHandlers[static_cast<int>(msg)];
	if (handler) {
		handler(this, data);
		return true;
	}
	return false;
}

// /////////////// //
// Message helpers //
// /////////////// //

bool Entity::PrepareNetCode() {
	return SendMessage(EntityMessage::PrepareNetCode, nullptr);
}

bool Entity::Heal(int amount, gentity_t* source) {
	std::tuple<int, gentity_t*> data(amount, source);
	return SendMessage(EntityMessage::Heal, &data);
}

bool Entity::Damage(int amount, gentity_t* source, Util::optional<Vec3> location, Util::optional<Vec3> direction, int flags, meansOfDeath_t meansOfDeath) {
	std::tuple<int, gentity_t*, Util::optional<Vec3>, Util::optional<Vec3>, int, meansOfDeath_t> data(amount, source, location, direction, flags, meansOfDeath);
	return SendMessage(EntityMessage::Damage, &data);
}

// ///////////////////////// //
// Component implementations //
// ///////////////////////// //


std::set<TeamComponent*> TeamComponentBase::allSet;

/**
 * @return A reference to the TeamComponent of the owning entity.
 */
TeamComponent& ClientComponentBase::GetTeamComponent() {
    return r_TeamComponent;
}
const TeamComponent& ClientComponentBase::GetTeamComponent() const {
    return r_TeamComponent;
}



std::set<ClientComponent*> ClientComponentBase::allSet;

/**
 * @return A reference to the ClientComponent of the owning entity.
 */
ClientComponent& SpectatorComponentBase::GetClientComponent() {
    return r_ClientComponent;
}
const ClientComponent& SpectatorComponentBase::GetClientComponent() const {
    return r_ClientComponent;
}


/**
 * @return A reference to the TeamComponent of the owning entity.
 */
TeamComponent& SpectatorComponentBase::GetTeamComponent() {
    return r_ClientComponent.GetTeamComponent();
}
const TeamComponent& SpectatorComponentBase::GetTeamComponent() const {
    return r_ClientComponent.GetTeamComponent();
}


std::set<SpectatorComponent*> SpectatorComponentBase::allSet;



std::set<HealthComponent*> HealthComponentBase::allSet;



std::set<KnockbackComponent*> KnockbackComponentBase::allSet;


// ////////////////////// //
// Entity implementations //
// ////////////////////// //

// EmptyEntity
// ---------

// EmptyEntity's component offset vtable.
const int EmptyEntity::componentOffsets[] = {
	0,
	0,
	0,
	0,
	0,
};

// EmptyEntity's message dispatcher vtable.
const MessageHandler EmptyEntity::messageHandlers[] = {
	nullptr,
	nullptr,
	nullptr,
};

// EmptyEntity's constructor.
EmptyEntity::EmptyEntity(Params params)
	: Entity(messageHandlers, componentOffsets, params.oldEnt)
{}

// ClientEntity
// ---------

// ClientEntity's component offset vtable.
const int ClientEntity::componentOffsets[] = {
	myoffsetof(ClientEntity, c_TeamComponent),
	myoffsetof(ClientEntity, c_ClientComponent),
	0,
	0,
	0,
};

// ClientEntity's message dispatcher vtable.
const MessageHandler ClientEntity::messageHandlers[] = {
	nullptr,
	nullptr,
	nullptr,
};

// ClientEntity's constructor.
ClientEntity::ClientEntity(Params params)
	: Entity(messageHandlers, componentOffsets, params.oldEnt)
	, c_TeamComponent(*this, TEAM_NONE)
	, c_ClientComponent(*this, params.Client_clientData, c_TeamComponent)
{}

// SpectatorEntity
// ---------

// SpectatorEntity's component offset vtable.
const int SpectatorEntity::componentOffsets[] = {
	myoffsetof(SpectatorEntity, c_TeamComponent),
	myoffsetof(SpectatorEntity, c_ClientComponent),
	myoffsetof(SpectatorEntity, c_SpectatorComponent),
	0,
	0,
};

// SpectatorEntity's PrepareNetCode message dispatcher.
void h_Spectator_PrepareNetCode(Entity* _entity, const void*) {
	auto* entity = static_cast<SpectatorEntity*>(_entity);
	entity->c_SpectatorComponent.HandlePrepareNetCode();
}

// SpectatorEntity's message dispatcher vtable.
const MessageHandler SpectatorEntity::messageHandlers[] = {
	h_Spectator_PrepareNetCode,
	nullptr,
	nullptr,
};

// SpectatorEntity's constructor.
SpectatorEntity::SpectatorEntity(Params params)
	: Entity(messageHandlers, componentOffsets, params.oldEnt)
	, c_TeamComponent(*this, params.Team_team)
	, c_ClientComponent(*this, params.Client_clientData, c_TeamComponent)
	, c_SpectatorComponent(*this, c_ClientComponent)
{}

// PlayerEntity
// ---------

// PlayerEntity's component offset vtable.
const int PlayerEntity::componentOffsets[] = {
	myoffsetof(PlayerEntity, c_TeamComponent),
	myoffsetof(PlayerEntity, c_ClientComponent),
	0,
	myoffsetof(PlayerEntity, c_HealthComponent),
	myoffsetof(PlayerEntity, c_KnockbackComponent),
};

// PlayerEntity's Damage message dispatcher.
void h_Player_Damage(Entity* _entity, const void* _data) {
	auto* entity = static_cast<PlayerEntity*>(_entity);
	const auto* data = static_cast<const std::tuple<int, gentity_t*, Util::optional<Vec3>, Util::optional<Vec3>, int, meansOfDeath_t>*>(_data);
	entity->c_HealthComponent.HandleDamage(std::get<0>(*data), std::get<1>(*data), std::get<2>(*data), std::get<3>(*data), std::get<4>(*data), std::get<5>(*data));
	entity->c_KnockbackComponent.HandleDamage(std::get<0>(*data), std::get<1>(*data), std::get<2>(*data), std::get<3>(*data), std::get<4>(*data), std::get<5>(*data));
}

// PlayerEntity's Heal message dispatcher.
void h_Player_Heal(Entity* _entity, const void* _data) {
	auto* entity = static_cast<PlayerEntity*>(_entity);
	const auto* data = static_cast<const std::tuple<int, gentity_t*>*>(_data);
	entity->c_HealthComponent.HandleHeal(std::get<0>(*data), std::get<1>(*data));
}

// PlayerEntity's PrepareNetCode message dispatcher.
void h_Player_PrepareNetCode(Entity* _entity, const void*) {
	auto* entity = static_cast<PlayerEntity*>(_entity);
	entity->c_HealthComponent.HandlePrepareNetCode();
}

// PlayerEntity's message dispatcher vtable.
const MessageHandler PlayerEntity::messageHandlers[] = {
	h_Player_PrepareNetCode,
	h_Player_Heal,
	h_Player_Damage,
};

// PlayerEntity's constructor.
PlayerEntity::PlayerEntity(Params params)
	: Entity(messageHandlers, componentOffsets, params.oldEnt)
	, c_TeamComponent(*this, params.Team_team)
	, c_ClientComponent(*this, params.Client_clientData, c_TeamComponent)
	, c_HealthComponent(*this)
	, c_KnockbackComponent(*this)
{}

#undef myoffsetof
