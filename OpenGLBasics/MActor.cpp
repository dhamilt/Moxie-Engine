#include "glPCH.h"
#include "MActor.h"

void MActor::AddComponent(EntityComponent* component)
{
	components.push_back(component);

	component->parent = &this->transform;
}

void MActor::RemoveComponent(EntityComponent* component)
{
	int i = 0;
	while (i < components.size())
	{
		if (component == components[i])
			break;
		
		i++;
	}

	// if component was found in the collection
	if (i < components.size())
		// remove it
		components.erase(components.begin() + i);
}
