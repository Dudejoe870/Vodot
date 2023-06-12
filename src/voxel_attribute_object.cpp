#include "voxel_attribute_object.hpp"

#include <godot_cpp/core/class_db.hpp>

using namespace godot;

TypedArray<VoxelAttributeDescriptor> VoxelAttributeObject::get_descriptors() const {
    TypedArray<VoxelAttributeDescriptor> result;
    for (const Ref<VoxelAttributeDescriptor> &desc : descriptors) {
        result.append(desc);
    }
    return result;
}

void VoxelAttributeObject::set_descriptors(const TypedArray<VoxelAttributeDescriptor> &p_descriptors) {
    descriptors.resize(p_descriptors.size());
    for (size_t i = 0; i < p_descriptors.size(); i++) {
        descriptors[i] = p_descriptors[i];
    }
    emit_changed();
}

void VoxelAttributeObject::_bind_methods() {
    ClassDB::bind_method(D_METHOD("get_descriptors"), &VoxelAttributeObject::get_descriptors);
    ClassDB::bind_method(D_METHOD("set_descriptors", "descriptors"), &VoxelAttributeObject::set_descriptors);
    ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "descriptors", 
            PROPERTY_HINT_ARRAY_TYPE, VoxelAttributeDescriptor::get_class_static(), 
            PROPERTY_USAGE_DEFAULT), 
            "set_descriptors", "get_descriptors");
}

VoxelAttributeObject::VoxelAttributeObject() {
}

VoxelAttributeObject::~VoxelAttributeObject() {
}
