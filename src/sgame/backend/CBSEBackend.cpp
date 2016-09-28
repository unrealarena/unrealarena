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

// Base entity deconstructor.
Entity::~Entity()
{}

// Base entity's message dispatcher.
bool Entity::SendMessage(int msg, const void* data) {
	MessageHandler handler = messageHandlers[msg];
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
	return SendMessage(MSG_PREPARENETCODE, nullptr);
}

bool Entity::Heal(int amount, gentity_t* source) {
	std::tuple<int, gentity_t*> data(amount, source);
	return SendMessage(MSG_HEAL, &data);
}

bool Entity::Damage(int amount, gentity_t* source, Util::optional<Vec3> location, Util::optional<Vec3> direction, int flags, meansOfDeath_t meansOfDeath) {
	std::tuple<int, gentity_t*, Util::optional<Vec3>, Util::optional<Vec3>, int, meansOfDeath_t> data(amount, source, location, direction, flags, meansOfDeath);
	return SendMessage(MSG_DAMAGE, &data);
}

std::set<ClientComponent*> ClientComponentBase::allSet;
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

// EmptyEntity's deconstructor.
EmptyEntity::~EmptyEntity()
{}

// ClientEntity
// ---------

// ClientEntity's component offset vtable.
const int ClientEntity::componentOffsets[] = {
	myoffsetof(ClientEntity, c_ClientComponent),
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
	, c_ClientComponent(*this, params.Client_clientData)
{}

// ClientEntity's deconstructor.
ClientEntity::~ClientEntity()
{}

// QPlayerEntity
// ---------

// QPlayerEntity's component offset vtable.
const int QPlayerEntity::componentOffsets[] = {
	myoffsetof(QPlayerEntity, c_ClientComponent),
	myoffsetof(QPlayerEntity, c_HealthComponent),
	myoffsetof(QPlayerEntity, c_KnockbackComponent),
};

// QPlayerEntity's PrepareNetCode message dispatcher.
void h_QPlayer_PrepareNetCode(Entity* _entity, const void* ) {
	QPlayerEntity* entity = (QPlayerEntity*) _entity;
	entity->c_HealthComponent.HandlePrepareNetCode();
}

// QPlayerEntity's Heal message dispatcher.
void h_QPlayer_Heal(Entity* _entity, const void*  _data) {
	QPlayerEntity* entity = (QPlayerEntity*) _entity;
	const std::tuple<int, gentity_t*>* data = (const std::tuple<int, gentity_t*>*) _data;
	entity->c_HealthComponent.HandleHeal(std::get<0>(*data), std::get<1>(*data));
}

// QPlayerEntity's Damage message dispatcher.
void h_QPlayer_Damage(Entity* _entity, const void*  _data) {
	QPlayerEntity* entity = (QPlayerEntity*) _entity;
	const std::tuple<int, gentity_t*, Util::optional<Vec3>, Util::optional<Vec3>, int, meansOfDeath_t>* data = (const std::tuple<int, gentity_t*, Util::optional<Vec3>, Util::optional<Vec3>, int, meansOfDeath_t>*) _data;
	entity->c_HealthComponent.HandleDamage(std::get<0>(*data), std::get<1>(*data), std::get<2>(*data), std::get<3>(*data), std::get<4>(*data), std::get<5>(*data));
	entity->c_KnockbackComponent.HandleDamage(std::get<0>(*data), std::get<1>(*data), std::get<2>(*data), std::get<3>(*data), std::get<4>(*data), std::get<5>(*data));
}

// QPlayerEntity's message dispatcher vtable.
const MessageHandler QPlayerEntity::messageHandlers[] = {
	h_QPlayer_PrepareNetCode,
	h_QPlayer_Heal,
	h_QPlayer_Damage,
};

// QPlayerEntity's constructor.
QPlayerEntity::QPlayerEntity(Params params)
	: Entity(messageHandlers, componentOffsets, params.oldEnt)
	, c_ClientComponent(*this, params.Client_clientData)
	, c_HealthComponent(*this)
	, c_KnockbackComponent(*this)
{}

// QPlayerEntity's deconstructor.
QPlayerEntity::~QPlayerEntity()
{}

// UPlayerEntity
// ---------

// UPlayerEntity's component offset vtable.
const int UPlayerEntity::componentOffsets[] = {
	myoffsetof(UPlayerEntity, c_ClientComponent),
	myoffsetof(UPlayerEntity, c_HealthComponent),
	myoffsetof(UPlayerEntity, c_KnockbackComponent),
};

// UPlayerEntity's PrepareNetCode message dispatcher.
void h_UPlayer_PrepareNetCode(Entity* _entity, const void* ) {
	UPlayerEntity* entity = (UPlayerEntity*) _entity;
	entity->c_HealthComponent.HandlePrepareNetCode();
}

// UPlayerEntity's Heal message dispatcher.
void h_UPlayer_Heal(Entity* _entity, const void*  _data) {
	UPlayerEntity* entity = (UPlayerEntity*) _entity;
	const std::tuple<int, gentity_t*>* data = (const std::tuple<int, gentity_t*>*) _data;
	entity->c_HealthComponent.HandleHeal(std::get<0>(*data), std::get<1>(*data));
}

// UPlayerEntity's Damage message dispatcher.
void h_UPlayer_Damage(Entity* _entity, const void*  _data) {
	UPlayerEntity* entity = (UPlayerEntity*) _entity;
	const std::tuple<int, gentity_t*, Util::optional<Vec3>, Util::optional<Vec3>, int, meansOfDeath_t>* data = (const std::tuple<int, gentity_t*, Util::optional<Vec3>, Util::optional<Vec3>, int, meansOfDeath_t>*) _data;
	entity->c_HealthComponent.HandleDamage(std::get<0>(*data), std::get<1>(*data), std::get<2>(*data), std::get<3>(*data), std::get<4>(*data), std::get<5>(*data));
	entity->c_KnockbackComponent.HandleDamage(std::get<0>(*data), std::get<1>(*data), std::get<2>(*data), std::get<3>(*data), std::get<4>(*data), std::get<5>(*data));
}

// UPlayerEntity's message dispatcher vtable.
const MessageHandler UPlayerEntity::messageHandlers[] = {
	h_UPlayer_PrepareNetCode,
	h_UPlayer_Heal,
	h_UPlayer_Damage,
};

// UPlayerEntity's constructor.
UPlayerEntity::UPlayerEntity(Params params)
	: Entity(messageHandlers, componentOffsets, params.oldEnt)
	, c_ClientComponent(*this, params.Client_clientData)
	, c_HealthComponent(*this)
	, c_KnockbackComponent(*this)
{}

// UPlayerEntity's deconstructor.
UPlayerEntity::~UPlayerEntity()
{}

#undef myoffsetof
