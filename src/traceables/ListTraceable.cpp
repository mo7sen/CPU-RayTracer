#include <traceables/ListTraceable.h>

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

bool ListTraceable::bounding_box(real t0, real t1, AABB& aabb) const
{
	if(objects.size() == 0) {
		return false;
	}

	AABB tmp_aabb;
	bool first_iter = true;
	for (const auto& object : objects)
	{
		if(!object->bounding_box(t0, t1, tmp_aabb)) {
			return false;
		}
		if(first_iter) {
			aabb = tmp_aabb;
		} else {
			aabb = AABB::surrounding(aabb, tmp_aabb);
		}

		first_iter = false;
	}

	return true;
}

void ListTraceable::clear() 
{ 
	objects.clear(); 
}

void ListTraceable::add(std::shared_ptr<Traceable> object) 
{ 
	objects.push_back(object); 
}
