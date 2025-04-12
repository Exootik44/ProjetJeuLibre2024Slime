#pragma once

#include "Components.h"

class Entity
{
public:
	int32_t id = 0;
	uint32_t tab_index = 0;
	bool m_isDestroyed = false;

	//std::vector<std::string>tag = {};
	//virtual void OnCollision(Entity* entity1, Entity* entity2) {};
	//std::stack<int> m_tagList;
	int m_tag;

	virtual void OnCollision(Entity* otherEntity) {};
};

struct EntityComponents
{
	uint32_t tab_index = 0;
	ComponentMask mask = 0;
	std::vector<Component*> vec_components;

	~EntityComponents() 
	{
		for (auto* comp : vec_components) 
		{
			delete comp;
		}
		vec_components.clear();
	}
};


class EntityManager
{
private:

	uint32_t entityNb = 0; // Positif, ce sont les entity qui existent actuellement dans le jeu
	int32_t entitiesToAddIndex = 0; // Negatif pour savoir qu'elles ne sont pas encore ajoutees 

	Entity* tab_entity[64000] = { nullptr };

	EntityComponents* tab_Components[64000] = { nullptr };

	//std::vector<Entity*> tab_toDestroy;
	std::vector<Entity> tab_toDestroy;
	std::vector<Entity*> tab_entitiesToAdd;
	std::vector<EntityComponents*> tab_compToAdd;

public:
	~EntityManager();

	Entity* (&GetEntityTab())[64000] { return tab_entity; }

	EntityComponents* (&GetComponentsTab())[64000] {	return tab_Components; }

	//std::vector<Entity*>& GetToDestroyTab() { return tab_toDestroy; }
	std::vector<Entity>& GetToDestroyTab() { return tab_toDestroy; }
	std::vector<Entity*>& GetEntityToAddTab() { return tab_entitiesToAdd; }
	std::vector<EntityComponents*>& GetComponentToAddTab() { return tab_compToAdd; }

	uint32_t GetNbEntity() { return entityNb; }

	Entity* CreateEntity();

	void ToDestroy(Entity* entity);
	void DestroyEntity(Entity* entity);

	bool HasComponent(Entity* entity, ComponentMask componentMask) const;

	void ResetEntitiesToAdd() { entitiesToAddIndex = 0; }

	void AddEntityToTab(Entity* entity, EntityComponents* components);

	template <typename T>
	void AddComponent(Entity* entity)
	{
		T* newComp = new T;


		if (entity->id < 0) // Entity a ajouter
		{
			for (auto* component : tab_compToAdd[entity->tab_index]->vec_components)
			{
				if (newComp->ID == component->ID)
				{
					std::cout << "Entity already has this component" << std::endl;
					delete newComp;
					return;
				}
			}
			tab_compToAdd[entity->tab_index]->mask |= newComp->mask;

			tab_compToAdd[entity->tab_index]->vec_components.push_back(newComp);
		}
		else // entity existante
		{
			for (auto* component : tab_Components[entity->tab_index]->vec_components)
			{
				if (newComp->ID == component->ID)
				{
					delete newComp;
					return;
				}
			}
			tab_Components[entity->tab_index]->mask |= newComp->mask;

			tab_Components[entity->tab_index]->vec_components.push_back(newComp);
		}
	}

	template <typename T>
	void RemoveComponent(Entity* entity)
	{
		T* compToRemove = new T;

		if (entity->id < 0) // Entity a ajouter
		{
			int i = 0;
			for (auto* component : tab_compToAdd[entity->tab_index]->vec_components)
			{
				if (compToRemove->ID == component->ID)
				{
					tab_compToAdd[entity->tab_index]->mask &= ~compToRemove->mask;

					if (component != nullptr)
					{
						delete component;
						component = nullptr;
						tab_compToAdd[entity->tab_index]->vec_components.erase(tab_compToAdd[entity->tab_index]->vec_components.begin() + i);
					}
					return;
				}
				i++;
			}
		}
		else // entity existante
		{
			int i = 0;
			for (auto* component : tab_Components[entity->tab_index]->vec_components)
			{
				if (compToRemove->ID == component->ID)
				{
					tab_Components[entity->tab_index]->mask &= ~compToRemove->mask;

					if (component != nullptr)
					{
						delete component;
						component = nullptr;
						tab_Components[entity->tab_index]->vec_components.erase(tab_Components[entity->tab_index]->vec_components.begin() + i);
					}
					return;
				}
				i++;
			}
		}
	}
};