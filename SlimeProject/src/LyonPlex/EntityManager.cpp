#include "pch.h"

#include "EntityManager.h"

EntityManager::~EntityManager()
{
	for (auto& entity : tab_entity)
	{
		if (entity)
		{
			delete entity;
			entity = nullptr;
		}
	}

	for (auto& components : tab_Components)
	{
		delete components;
		components = nullptr;
	}

	for (auto& entity : tab_toDestroy)
	{
		//delete entity;
		//entity = nullptr;
	}
	tab_toDestroy.clear();

	for (auto& entity : tab_entitiesToAdd)
	{
		delete entity;
		entity = nullptr;
	}
	tab_entitiesToAdd.clear();
	for (auto& components : tab_compToAdd)
	{
		delete components;
		components = nullptr;
	}
	tab_compToAdd.clear();
}

Entity* EntityManager::CreateEntity() {

	Entity* entity = new Entity;

	entity->tab_index = entitiesToAddIndex * -1;
	entity->id = entitiesToAddIndex - 1;
	tab_entitiesToAdd.push_back(entity);

	EntityComponents* newComponents = new EntityComponents;
	newComponents->mask = COMPONENT_NONE;
	newComponents->tab_index = entitiesToAddIndex - 1;

	tab_compToAdd.push_back(newComponents);

	entitiesToAddIndex--;
	return entity;
}

void EntityManager::AddEntityToTab(Entity* entity, EntityComponents* components)
{
	entity->tab_index = entityNb;
	entity->id = entityNb + 1;
	tab_entity[entity->tab_index] = entity; // Ajout de l'entity dans le tableau d'entities

	components->tab_index = entityNb;
	tab_Components[entityNb] = components; // Ajout des components (vides) de l'entity dans la case correspondant du tableau de composants

	entityNb++;
}

void EntityManager::DestroyEntity(Entity* entity) {
	int index = entity->tab_index;
	int lastIndex = entityNb - 1;

	if (index > lastIndex) {
		std::cerr << "Index of entity to destroy invalid." << std::endl;
		return;
	}

	int i = 0;

	for (auto& entityToDestroy : tab_toDestroy)
	{
		if (tab_Components[lastIndex]->tab_index == entityToDestroy.tab_index)
			tab_toDestroy.at(i).tab_index = index;
		i++;
	}

	tab_entity[lastIndex]->tab_index = index;
	*tab_entity[index] = *tab_entity[lastIndex];


	for (auto* component : tab_Components[index]->vec_components)
	{
		if (component)
		{
			if (component->ID == Mesh_ID)
			{
				auto* meshComp = static_cast<MeshComponent*>(component);
				delete meshComp->m_mesh;
				meshComp->m_mesh = nullptr;
				delete meshComp;
				continue;
			}
			delete component;
			component = nullptr;
		}
	}
	tab_Components[lastIndex]->tab_index = index;
	*tab_Components[index] = *tab_Components[lastIndex];


	tab_Components[lastIndex]->vec_components.clear();

	delete tab_entity[lastIndex];
	tab_entity[lastIndex] = nullptr;

	delete tab_Components[lastIndex];
	tab_Components[lastIndex] = nullptr;

	//int newEntityToPointAt = 
	entityNb--;
}

void EntityManager::ToDestroy(Entity* entity)
{
	if (entity->m_isDestroyed == false)
	{
		entity->m_isDestroyed = true;

		Entity entityToDestroy;
		entityToDestroy.id = entity->id;
		entityToDestroy.tab_index = entity->tab_index;
		entityToDestroy.m_isDestroyed = entity->m_isDestroyed;

		tab_toDestroy.push_back(entityToDestroy);
	}
}

bool EntityManager::HasComponent(Entity* entity, ComponentMask componentMask) const {

	return (tab_Components[entity->tab_index]->mask & componentMask) == componentMask;
}


