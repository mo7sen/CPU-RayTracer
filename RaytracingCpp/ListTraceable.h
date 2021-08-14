#pragma once
#include "Traceable.h"

#include <vector>

class ListTraceable : public Traceable
{
public:
	std::vector<std::shared_ptr<Traceable>> objects;

public:
	ListTraceable() = default;
	~ListTraceable() = default;

	void clear();
	void add(std::shared_ptr<Traceable> object);

	virtual bool hit(const Ray& ray, real t_min, real t_max, HitData& hitData) const override;
};

