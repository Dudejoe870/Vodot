#pragma once

#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/variant/typed_array.hpp>

#include <godot_cpp/templates/local_vector.hpp>

#include "voxel_attribute_descriptor.hpp"

using namespace godot;

class VoxelAttributeObject : public Resource
{
	GDCLASS(VoxelAttributeObject, Resource);

protected:
	static void _bind_methods();
public:
	LocalVector<Ref<VoxelAttributeDescriptor>> descriptors;

	TypedArray<VoxelAttributeDescriptor> get_descriptors() const;
	void set_descriptors(const TypedArray<VoxelAttributeDescriptor> &p_descriptors);

	VoxelAttributeObject();
	~VoxelAttributeObject();
};
