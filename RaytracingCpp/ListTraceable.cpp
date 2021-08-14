#include "ListTraceable.h"

bool ListTraceable::hit(const Ray& ray, real t_min, real t_max, HitData& hitData) const
{
	bool hit_anything = false;
	real closestHit = t_max;
	HitData tmpHitData;

	for (const auto& object : objects)
	{
		if (object->hit(ray, t_min, closestHit, tmpHitData))
		{
			hit_anything = true;
			closestHit = tmpHitData.t;
			hitData = tmpHitData;
		}
	}

	return hit_anything;
}

void ListTraceable::clear() 
{ 
	objects.clear(); 
}

void ListTraceable::add(std::shared_ptr<Traceable> object) 
{ 
	objects.push_back(object); 
}
