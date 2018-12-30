/*
 * CBSE GPL Source Code
 * Copyright (C) 2016-2018  Unreal Arena
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
 *   - Declaration of the base entity.
 *   - Implementations of the base components.
 *   - Helpers to access entities and components.
 */

#ifndef CBSE_BACKEND_H_
#define CBSE_BACKEND_H_

#include <set>

#define CBSE_INCLUDE_TYPES_ONLY
#include "../CBSE.h"
#undef CBSE_INCLUDE_TYPES_ONLY

// /////////// //
// Message IDs //
// /////////// //

enum class EntityMessage {
	PrepareNetCode,
	Heal,
	Damage,
};

// //////////////////// //
// Forward declarations //
// //////////////////// //

class Entity;

class TeamComponent;
class ClientComponent;
class SpectatorComponent;
class HealthComponent;
class KnockbackComponent;

/** Message handler declaration. */
using MessageHandler = void (*)(Entity*, const void* /*_data*/);

// //////////////////// //
// Component priorities //
// //////////////////// //

namespace detail {
	template<typename T> struct ComponentPriority;

	template<> struct ComponentPriority<TeamComponent> {
		static const int value = 0;
	};

	template<> struct ComponentPriority<ClientComponent> {
		static const int value = 1;
	};

	template<> struct ComponentPriority<SpectatorComponent> {
		static const int value = 2;
	};

	template<> struct ComponentPriority<HealthComponent> {
		static const int value = 3;
	};

	template<> struct ComponentPriority<KnockbackComponent> {
		static const int value = 4;
	};
}

// ////////////////////////////// //
// Declaration of the base Entity //
// ////////////////////////////// //

/** Base entity class. */
class Entity {
	public:
		/**
		 * @brief Base entity constructor.
		 * @param messageHandlers Message handler vtable.
		 * @param componentOffsets Component offset vtable.
		 */
		Entity(const MessageHandler* messageHandlers, const int* componentOffsets, gentity_t* oldEnt);

		/**
		 * @brief Base entity deconstructor.
		 */
		virtual ~Entity() = default;

		// /////////////// //
		// Message helpers //
		// /////////////// //

		bool PrepareNetCode(); /**< Sends the PrepareNetCode message to all interested components. */
		bool Heal(int amount, gentity_t* source); /**< Sends the Heal message to all interested components. */
		bool Damage(int amount, gentity_t* source, Util::optional<Vec3> location, Util::optional<Vec3> direction, int flags, meansOfDeath_t meansOfDeath); /**< Sends the Damage message to all interested components. */

		/**
		 * @brief Returns a component of this entity, if available.
		 * @tparam T Type of component to ask for.
		 * @return Pointer to component of type T or nullptr.
		 */
		template<typename T> const T* Get() const {
			int index = detail::ComponentPriority<T>::value;
			int offset = componentOffsets[index];
			if (offset) {
				return (const T*) (((char*) this) + offset);
			} else {
				return nullptr;
			}
		}

		/**
		 * @brief Returns a component of this entity, if available.
		 * @tparam T Type of component to ask for.
		 * @return Pointer to component of type T or nullptr.
		 */
		template<typename T> T* Get() {
			return const_cast<T*>(static_cast<const Entity*>(this)->Get<T>());
		}

	private:
		/** Message handler vtable. */
		const MessageHandler* messageHandlers;

		/** Component offset vtable. */
		const int* componentOffsets;

		/**
		 * @brief Generic message dispatcher.
		 * @note Should not be called directly, use message helpers instead.
		 */
		bool SendMessage(EntityMessage msg, const void* data);

    public:
		// ///////////////////// //
		// Shared public members //
		// ///////////////////// //

		gentity_t* oldEnt;
};

// ////////////////////////// //
// Base component definitions //
// ////////////////////////// //

template<typename C>
class AllComponents {
	public:
		AllComponents(std::set<C*>& all): all(all) {}

		typename std::set<C*>::iterator begin() {
			return all.begin();
		}

		typename std::set<C*>::iterator end() {
			return all.end();
		}

	private:
		std::set<C*>& all;
};

/** Base class of TeamComponent. */
class TeamComponentBase {
	public:
		/**
		 * @brief TeamComponentBase constructor.
		 * @param entity The entity that owns this component.
		 * @param team An initialization parameter.
		 */
		TeamComponentBase(Entity& entity, team_t team)
			: entity(entity), team(team){
			allSet.insert((TeamComponent*)((char*) this - (char*) (TeamComponentBase*) (TeamComponent*) nullptr));
		}

		~TeamComponentBase() {
			allSet.erase((TeamComponent*)((char*) this - (char*) (TeamComponentBase*) (TeamComponent*) nullptr));
		}

		/** A reference to the entity that owns the component instance. Allows sending back messages. */
		Entity& entity;

		static AllComponents<TeamComponent> GetAll() {
			return {allSet};
		}

	protected:
		team_t team; /**< An initialization parameter. */

	private:

		static std::set<TeamComponent*> allSet;
};

/** Base class of ClientComponent. */
class ClientComponentBase {
	public:
		/**
		 * @brief ClientComponentBase constructor.
		 * @param entity The entity that owns this component.
		 * @param clientData An initialization parameter.
		 * @param r_TeamComponent A TeamComponent instance that this component depends on.
		 */
		ClientComponentBase(Entity& entity, gclient_t* clientData, TeamComponent& r_TeamComponent)
			: entity(entity), clientData(clientData), r_TeamComponent(r_TeamComponent){
			allSet.insert((ClientComponent*)((char*) this - (char*) (ClientComponentBase*) (ClientComponent*) nullptr));
		}

		~ClientComponentBase() {
			allSet.erase((ClientComponent*)((char*) this - (char*) (ClientComponentBase*) (ClientComponent*) nullptr));
		}

		/**
		 * @return A reference to the TeamComponent of the owning entity.
		 */
		TeamComponent& GetTeamComponent();
		const TeamComponent& GetTeamComponent() const;

		/** A reference to the entity that owns the component instance. Allows sending back messages. */
		Entity& entity;

		static AllComponents<ClientComponent> GetAll() {
			return {allSet};
		}

	protected:
		gclient_t* clientData; /**< An initialization parameter. */

	private:
		TeamComponent& r_TeamComponent; /**< A component of the owning entity that this component depends on. */

		static std::set<ClientComponent*> allSet;
};

/** Base class of SpectatorComponent. */
class SpectatorComponentBase {
	public:
		/**
		 * @brief SpectatorComponentBase constructor.
		 * @param entity The entity that owns this component.
		 * @param r_ClientComponent A ClientComponent instance that this component depends on.
		 */
		SpectatorComponentBase(Entity& entity, ClientComponent& r_ClientComponent)
			: entity(entity), r_ClientComponent(r_ClientComponent){
			allSet.insert((SpectatorComponent*)((char*) this - (char*) (SpectatorComponentBase*) (SpectatorComponent*) nullptr));
		}

		~SpectatorComponentBase() {
			allSet.erase((SpectatorComponent*)((char*) this - (char*) (SpectatorComponentBase*) (SpectatorComponent*) nullptr));
		}

		/**
		 * @return A reference to the ClientComponent of the owning entity.
		 */
		ClientComponent& GetClientComponent();
		const ClientComponent& GetClientComponent() const;

		/**
		 * @return A reference to the TeamComponent of the owning entity.
		 */
		TeamComponent& GetTeamComponent();
		const TeamComponent& GetTeamComponent() const;

		/** A reference to the entity that owns the component instance. Allows sending back messages. */
		Entity& entity;

		static AllComponents<SpectatorComponent> GetAll() {
			return {allSet};
		}

	protected:

	private:
		ClientComponent& r_ClientComponent; /**< A component of the owning entity that this component depends on. */

		static std::set<SpectatorComponent*> allSet;
};

/** Base class of HealthComponent. */
class HealthComponentBase {
	public:
		/**
		 * @brief HealthComponentBase constructor.
		 * @param entity The entity that owns this component.
		 */
		HealthComponentBase(Entity& entity)
			: entity(entity){
			allSet.insert((HealthComponent*)((char*) this - (char*) (HealthComponentBase*) (HealthComponent*) nullptr));
		}

		~HealthComponentBase() {
			allSet.erase((HealthComponent*)((char*) this - (char*) (HealthComponentBase*) (HealthComponent*) nullptr));
		}

		/** A reference to the entity that owns the component instance. Allows sending back messages. */
		Entity& entity;

		static AllComponents<HealthComponent> GetAll() {
			return {allSet};
		}

	protected:

	private:

		static std::set<HealthComponent*> allSet;
};

/** Base class of KnockbackComponent. */
class KnockbackComponentBase {
	public:
		/**
		 * @brief KnockbackComponentBase constructor.
		 * @param entity The entity that owns this component.
		 */
		KnockbackComponentBase(Entity& entity)
			: entity(entity){
			allSet.insert((KnockbackComponent*)((char*) this - (char*) (KnockbackComponentBase*) (KnockbackComponent*) nullptr));
		}

		~KnockbackComponentBase() {
			allSet.erase((KnockbackComponent*)((char*) this - (char*) (KnockbackComponentBase*) (KnockbackComponent*) nullptr));
		}

		/** A reference to the entity that owns the component instance. Allows sending back messages. */
		Entity& entity;

		static AllComponents<KnockbackComponent> GetAll() {
			return {allSet};
		}

	protected:

	private:

		static std::set<KnockbackComponent*> allSet;
};


// ////////////////////////// //
// Definitions of ForEntities //
// ////////////////////////// //

template <typename Component> bool HasComponents(const Entity& ent) {
    return ent.Get<Component>() != nullptr;
}

template <typename Component1, typename Component2, typename ... Components> bool HasComponents(const Entity& ent) {
    return HasComponents<Component1>(ent) && HasComponents<Component2, Components...>(ent);
}

template <typename Component1, typename ... Components, typename FuncType>
void ForEntities(FuncType f) {
    for(auto* component1: Component1::GetAll()) {
        Entity& ent = component1->entity;

        if (HasComponents<Component1, Components...>(ent)) {
            f(ent, *component1, *ent.Get<Components>()...);
        }
    }
}

#endif // CBSE_BACKEND_H_
