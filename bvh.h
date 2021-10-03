#ifndef BVH_H
#define BVH_H

#include "rtweekend.h"

#include "hittable.h"
#include "hittable_list.h"
#include <algorithm>

class bvh_node : public hittable {
    public:
	bvh_node();

	bvh_node(const hittable_list& list, double time0, double time1)
	    : bvh_node(list.objects, 0, list.objects.size(), time0, time1)
	{}		// bvh can be called with either list + time0 time1 or with listobjects 0, size + time0 time1

	bvh_node(
	    const std::vector<shared_ptr<hittable>>& src_objects,		// Constructor that matches the declaration of bvh_node with 5 arguments
	    size_t start, size_t end, double time0, double time1);

	virtual bool hit(
	    const ray& r, double t_min, double t_max, hit_record& rec) const override;

	virtual bool bounding_box(double time0, double time1, aabb& output_box) const override;

    public:
	shared_ptr<hittable> left;
	shared_ptr<hittable> right;
	aabb box;
};

bool bvh_node::bounding_box(double time0, double time1, aabb& output_box) const {
	output_box = box;
	return true;
}

bool bvh_node::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
    if (!box.hit(r, t_min, t_max)) {
	return false;
    }

    bool hit_left = left->hit(r, t_min, t_max, rec);	// True when left points on hit function
    bool hit_right = right->hit(r, t_min, hit_left ? rec.t : t_max, rec);  //True when points on hit and use rec t if hit left true, otherwise t_max - This function is not to decide right or left as much as determining if hit when coming from right or left. This is only to define hit in every aspect

    return hit_left || hit_right;
}

inline bool box_compare(const shared_ptr<hittable> a, const shared_ptr<hittable> b, int axis) {
	aabb box_a;
	aabb box_b;

	if (!a->bounding_box(0,0,box_a) || !b->bounding_box(0,0,box_b)) {
	    std::cerr << "No bounding box in bvh_node constructor.\n";
	}

	return box_a.min().e[axis] < box_b.min().e[axis];		// box_a.min value on a specific axes is less than box_b.min value on the same
}

bool box_x_compare (const shared_ptr<hittable> a, const shared_ptr<hittable> b) {
    return box_compare(a, b, 0);
}

bool box_y_compare (const shared_ptr<hittable> a, const shared_ptr<hittable> b) {
    return box_compare(a, b, 1);
}

bool box_z_compare (const shared_ptr<hittable> a, const shared_ptr<hittable> b) {
    return box_compare(a, b, 2);
}

bvh_node::bvh_node(
    const std::vector<shared_ptr<hittable>>& src_objects,
    size_t start, size_t end, double time0, double time1) {
	auto objects = src_objects; // Create a modifiable array of the source scene objects

    int axis = random_int(0,2);			// 0 for x, 1 for y and 2 for z
    auto comparator = (axis == 0) ? box_x_compare
		    : (axis == 1) ? box_y_compare
		    		  : box_z_compare;

    size_t object_span = end - start;

    if (object_span == 1) {
	left = right = objects[start];
    } else if (object_span == 2) {
	if (comparator(objects[start], objects[start+1])) {	
	    left = objects[start];
	    right = objects[start+1];
	} else {
	    left = objects[start+1];
	    right = objects[start];
	}
    } else {
	std::sort(objects.begin() + start, objects.begin() + end, comparator);

	auto mid = start + object_span/2;
	left = make_shared<bvh_node>(objects, start, mid, time0, time1);
	right = make_shared<bvh_node>(objects, mid, end, time0, time1);
    }

    aabb box_left, box_right;

    if ( !left->bounding_box (time0, time1, box_left)
      || !right->bounding_box(time0, time1, box_right)) {
	std::cerr << "Nou bounding box in bvh_node constructor.\n";
    }

    box = surrounding_box(box_left, box_right);
}

#endif
