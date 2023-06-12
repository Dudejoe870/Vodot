#pragma once

#include <godot_cpp/core/binder_common.hpp>

#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/variant/string.hpp>

using namespace godot;

class VoxelAttributeDescriptor : public Resource
{
	GDCLASS(VoxelAttributeDescriptor, Resource);

protected:
	static void _bind_methods();
public:
    enum Type {
        TYPE_FLOAT32,
        TYPE_FLOAT64,
        TYPE_INTEGER8,
        TYPE_INTEGER16,
        TYPE_INTEGER32,
        TYPE_INTEGER64
    };
    static size_t get_type_size(Type p_type);

    _ALWAYS_INLINE_ size_t get_minimum_component_size() const {
        return get_type_size(get_type());
    }

    String get_name() const;
    Type get_type() const;
    size_t get_num_components() const;
    size_t get_component_size() const;

    bool get_sync_with_gpu() const;

    void set_name(const String &p_name);
    void set_type(Type p_type);
    void set_num_components(size_t p_num_components);
    void set_component_size(size_t p_component_size);

    void set_sync_with_gpu(bool p_sync_with_gpu);

    VoxelAttributeDescriptor(const String &p_name = String());
	~VoxelAttributeDescriptor();
protected:
    String name;
    Type type = TYPE_INTEGER8;
    size_t num_components = 1;
    size_t component_size = sizeof(uint8_t);

    bool sync_with_gpu = true;
};

VARIANT_ENUM_CAST(VoxelAttributeDescriptor::Type)
