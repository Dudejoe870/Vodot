#include "voxel_attribute_descriptor.hpp"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/math.hpp>

using namespace godot;

String VoxelAttributeDescriptor::get_name() const {
    return name;
}

VoxelAttributeDescriptor::Type VoxelAttributeDescriptor::get_type() const {
    return type;
}

size_t VoxelAttributeDescriptor::get_num_components() const {
    return num_components;
}

size_t VoxelAttributeDescriptor::get_component_size() const {
    return component_size;
}

bool VoxelAttributeDescriptor::get_sync_with_gpu() const {
    return sync_with_gpu;
}

void VoxelAttributeDescriptor::set_name(const String &p_name) {
    name = p_name;
    emit_changed();
}

void VoxelAttributeDescriptor::set_type(Type p_type) {
    type = p_type;
    emit_changed();
}

void VoxelAttributeDescriptor::set_num_components(size_t p_num_components) {
    num_components = CLAMP(p_num_components, 1, 4);
    emit_changed();
}

void VoxelAttributeDescriptor::set_component_size(size_t p_component_size) {
    component_size = MAX(get_minimum_component_size(), p_component_size);
    emit_changed();
}

void VoxelAttributeDescriptor::set_sync_with_gpu(bool p_sync_with_gpu) {
    sync_with_gpu = p_sync_with_gpu;
    emit_changed();
}

size_t VoxelAttributeDescriptor::get_type_size(Type p_type) {
    switch (p_type) {
        default:
        case TYPE_INTEGER8:
            return 1;
        case TYPE_INTEGER16:
            return 2;
        case TYPE_FLOAT32:
        case TYPE_INTEGER32:
            return 4;
        case TYPE_FLOAT64:
        case TYPE_INTEGER64:
            return 8;
    }
}

void VoxelAttributeDescriptor::_bind_methods() {
    ClassDB::bind_static_method(get_class_static(), D_METHOD("get_type_size", "type"), &VoxelAttributeDescriptor::get_type_size);
    ClassDB::bind_method(D_METHOD("get_minimum_component_size"), &VoxelAttributeDescriptor::get_minimum_component_size);

    ClassDB::bind_method(D_METHOD("get_name"), &VoxelAttributeDescriptor::get_name);
    ClassDB::bind_method(D_METHOD("set_name", "name"), &VoxelAttributeDescriptor::set_name);
    ADD_PROPERTY(
        	PropertyInfo(Variant::STRING, "name"), 
        	"set_name", "get_name");

    ClassDB::bind_method(D_METHOD("get_type"), &VoxelAttributeDescriptor::get_type);
    ClassDB::bind_method(D_METHOD("set_type", "type"), &VoxelAttributeDescriptor::set_type);
    ADD_PROPERTY(
        	PropertyInfo(Variant::INT, "type", PROPERTY_HINT_ENUM, "Float,Double,u8,u16,u32,u64"), 
        	"set_type", "get_type");

    ClassDB::bind_method(D_METHOD("get_num_components"), &VoxelAttributeDescriptor::get_num_components);
    ClassDB::bind_method(D_METHOD("set_num_components", "num_components"), &VoxelAttributeDescriptor::set_num_components);
    ADD_PROPERTY(
        	PropertyInfo(Variant::INT, "num_components", PROPERTY_HINT_RANGE, "1,4,1"), 
        	"set_num_components", "get_num_components");

    ClassDB::bind_method(D_METHOD("get_component_size"), &VoxelAttributeDescriptor::get_component_size);
    ClassDB::bind_method(D_METHOD("set_component_size", "component_size"), &VoxelAttributeDescriptor::set_component_size);
    ADD_PROPERTY(
        	PropertyInfo(Variant::INT, "component_size", PROPERTY_HINT_RANGE, "1,8,1,or_greater"), 
        	"set_component_size", "get_component_size");

    ClassDB::bind_method(D_METHOD("get_sync_with_gpu"), &VoxelAttributeDescriptor::get_sync_with_gpu);
    ClassDB::bind_method(D_METHOD("set_sync_with_gpu", "sync_with_gpu"), &VoxelAttributeDescriptor::set_sync_with_gpu);
    ADD_PROPERTY(
        	PropertyInfo(Variant::BOOL, "sync_with_gpu"), 
        	"set_sync_with_gpu", "get_sync_with_gpu");

    BIND_ENUM_CONSTANT(TYPE_FLOAT32)
    BIND_ENUM_CONSTANT(TYPE_FLOAT64)
    BIND_ENUM_CONSTANT(TYPE_INTEGER8)
    BIND_ENUM_CONSTANT(TYPE_INTEGER16)
    BIND_ENUM_CONSTANT(TYPE_INTEGER32)
    BIND_ENUM_CONSTANT(TYPE_INTEGER64)
}

VoxelAttributeDescriptor::VoxelAttributeDescriptor(const String &p_name) {
    name = p_name;
}

VoxelAttributeDescriptor::~VoxelAttributeDescriptor() {
}
