#include "BVHNode.h"

bool BVHNode::bounding_box(real time0, real time1, AABB& aabb) const
{
	aabb = box;
	return true;
}

#include "SolidColorTexture.h"
#include "LambertianMaterial.h"
bool BVHNode::hit(const Ray& ray, real t_min, real t_max, HitData& hitData) const
{
	if(!box.hit(ray, t_min, t_max))
		return false;

	bool hit_left = left->hit(ray, t_min, t_max, hitData);
	
	if(hit_left) { t_max = hitData.t; }
	bool hit_right = right->hit(ray, t_min, t_max, hitData);

	return hit_left || hit_right;
}

__forceinline bool box_cmp(const std::shared_ptr<Traceable> a, const std::shared_ptr<Traceable> b, int axis)
{
	AABB box_a;
	AABB box_b;
	if(!a->bounding_box(0, 0, box_a) || !b->bounding_box(0, 0, box_b)) {
		std::cerr << "No bounding box in box_cmp.\n";
	}

	return box_a.vMin()[axis] < box_b.vMin()[axis];
}

bool box_cmp_x(const std::shared_ptr<Traceable> a, const std::shared_ptr<Traceable> b)
{
	return box_cmp(a, b, 0);
}

bool box_cmp_y(const std::shared_ptr<Traceable> a, const std::shared_ptr<Traceable> b)
{
	return box_cmp(a, b, 1);
}

bool box_cmp_z(const std::shared_ptr<Traceable> a, const std::shared_ptr<Traceable> b)
{
	return box_cmp(a, b, 2);
}

#include <algorithm>

BVHNode::BVHNode(
		const std::vector<std::shared_ptr<Traceable>>& src_objects,
		size_t start, size_t end, real time0, real time1)
{
	auto objects = src_objects;
	int axis = random_int(0, 2);

	auto comparator = (axis == 0) ? box_cmp_x
					: (axis == 1) ? box_cmp_y
								  : box_cmp_z;

	size_t object_span = end - start;

	if(object_span == 1) {
		left = right = objects[start];
	} else if (object_span == 2) {
		if(comparator(objects[start], objects[start+1])) {
			left = objects[start];
			right = objects[start+1];
		} else {
			left = objects[start+1];
			right = objects[start];
		}
	} else {
		std::sort(objects.begin() + start, objects.begin() + end, comparator);
		auto mid = start + object_span / 2;

		left = std::make_shared<BVHNode>(objects, start, mid, time0, time1);
		right = std::make_shared<BVHNode>(objects, mid, end, time0, time1);
	}

	AABB box_left, box_right;
	if(!left->bounding_box(time0, time1, box_left) || !right->bounding_box(time0, time1, box_right)) {
		std::cerr << "No bounding box in BVHNode construction.\n";
	}

	box = AABB::surrounding(box_left, box_right);
}

#define INDENT_BY(x) do { \
	for(int i = 0; i < (x); i++) \
		printf("\t"); \
} while (0)

#include <assert.h>

void print_bvh(const Traceable* bvh, int level)
{
	if(bvh == nullptr) {
		return;
	}

	INDENT_BY(level);

	auto *node = dynamic_cast<const BVHNode*>(bvh);
	if(node != nullptr) {
		printf("Level %d: Node\n", level);
		print_bvh(node->left.get(), level + 1);
		print_bvh(node->right.get(), level + 1);
	} else {
		auto *leaf = bvh;
		printf("Level %d: Leaf\n", level);
	}
	INDENT_BY(level);
	AABB aabb;
	if(!bvh->bounding_box(0, 0, aabb)) {
		std::cerr << "Weirdly, no AABB on this node.\n";
	}

	printf("Bounding Box: vMin(%f, %f, %f), vMax(%f, %f, %f)\n", aabb.vMin().x(), aabb.vMin().y(), aabb.vMin().z(), aabb.vMax().x(), aabb.vMax().y(), aabb.vMax().z());
}