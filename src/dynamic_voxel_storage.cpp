#include "dynamic_voxel_storage.hpp"

#include <godot_cpp/core/class_db.hpp>

#include "util.hpp"

using namespace godot;

Ref<VoxelAttributeObject> DynamicVoxelStorage::get_voxel_attribute_object() const {
	return voxel_attribute_object;
}

void DynamicVoxelStorage::set_voxel_attribute_object(const Ref<VoxelAttributeObject> &p_voxel_attribute_object) {
	voxel_attribute_object = p_voxel_attribute_object;
	_init_buffers();
}

size_t DynamicVoxelStorage::get_chunk_size() const {
	return chunk_size;
}

size_t DynamicVoxelStorage::get_width() const {
	return width;
}

size_t DynamicVoxelStorage::get_height() const {
	return height;
}

size_t DynamicVoxelStorage::get_depth() const {
	return depth;
}

// TODO: Add a new method that keeps the original Voxel data
void DynamicVoxelStorage::resize_and_clear(size_t p_width, size_t p_height, size_t p_depth, size_t p_chunk_size) {
	chunk_size = p_chunk_size;

	width = p_width + (chunk_size - (p_width % chunk_size));
	height = p_height + (chunk_size - (p_height % chunk_size));
	depth = p_depth + (chunk_size - (p_depth % chunk_size));

	size_t chunk_buffer_size = (width / chunk_size) * (height / chunk_size) * (depth / chunk_size);

	_chunk_buffer.reset();
	// Ensures this TightLocalVector only allocates *absolutely* what is necessary.
	// Contrary to what you might think, this won't happen if you don't reserve first.
	_chunk_buffer.reserve(chunk_buffer_size);
	_chunk_buffer.resize(chunk_buffer_size);
	for (uint32_t &chunk_index : _chunk_buffer) {
		chunk_index = EMPTY_CHUNK;
	}

	_init_buffers();
}

void DynamicVoxelStorage::_init_buffers() {
	_attribute_buffers.reset();
	if (get_voxel_attribute_object().is_valid()) {
		_attribute_buffers.resize(get_voxel_attribute_object()->get_descriptors().size());
	}
}

void DynamicVoxelStorage::clear() {
	resize_and_clear(chunk_size, width, height, depth);
}

void DynamicVoxelStorage::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_voxel_attribute_object"), &DynamicVoxelStorage::get_voxel_attribute_object);
	ClassDB::bind_method(D_METHOD("set_voxel_attribute_object", "voxel_attribute_object"), &DynamicVoxelStorage::set_voxel_attribute_object);
	ADD_PROPERTY(
			PropertyInfo(Variant::OBJECT, "voxel_attribute_object", 
			PROPERTY_HINT_RESOURCE_TYPE, VoxelAttributeObject::get_class_static(),
			PROPERTY_USAGE_EDITOR | PROPERTY_USAGE_READ_ONLY), 
			"set_voxel_attribute_object", "get_voxel_attribute_object");
	
	ClassDB::bind_method(D_METHOD("get_chunk_size"), &DynamicVoxelStorage::get_chunk_size);
	ADD_PROPERTY(
			PropertyInfo(Variant::INT, "chunk_size", PROPERTY_HINT_NONE, "", 
			PROPERTY_USAGE_EDITOR | PROPERTY_USAGE_READ_ONLY), 
			"", "get_chunk_size");
	ClassDB::bind_method(D_METHOD("get_width"), &DynamicVoxelStorage::get_width);
	ADD_PROPERTY(
			PropertyInfo(Variant::INT, "width", PROPERTY_HINT_NONE, "", 
			PROPERTY_USAGE_EDITOR | PROPERTY_USAGE_READ_ONLY), 
			"", "get_width");
	ClassDB::bind_method(D_METHOD("get_height"), &DynamicVoxelStorage::get_height);
	ADD_PROPERTY(
			PropertyInfo(Variant::INT, "height", PROPERTY_HINT_NONE, "", 
			PROPERTY_USAGE_EDITOR | PROPERTY_USAGE_READ_ONLY), 
			"", "get_height");
	ClassDB::bind_method(D_METHOD("get_depth"), &DynamicVoxelStorage::get_depth);
	ADD_PROPERTY(
			PropertyInfo(Variant::INT, "depth", PROPERTY_HINT_NONE, "", 
			PROPERTY_USAGE_EDITOR | PROPERTY_USAGE_READ_ONLY), 
			"", "get_depth");

	ClassDB::bind_method(D_METHOD("resize_and_clear", "width", "height", "depth", "chunk_size"), &DynamicVoxelStorage::resize_and_clear);
	ClassDB::bind_method(D_METHOD("clear"), &DynamicVoxelStorage::clear);

	ClassDB::bind_method(D_METHOD("set_voxel_attribute_v2f32", "attribute_index", "x", "y", "z", "value"), 
			&DynamicVoxelStorage::set_voxel_attribute_vector<Vector2, 2, float, VoxelAttributeDescriptor::TYPE_FLOAT32, false>);
	ClassDB::bind_method(D_METHOD("set_voxel_attribute_v2f64", "attribute_index", "x", "y", "z", "value"), 
			&DynamicVoxelStorage::set_voxel_attribute_vector<Vector2, 2, float, VoxelAttributeDescriptor::TYPE_FLOAT64, false>);
	ClassDB::bind_method(D_METHOD("set_voxel_attribute_v2i8", "attribute_index", "x", "y", "z", "value"), 
			&DynamicVoxelStorage::set_voxel_attribute_vector<Vector2i, 2, int8_t, VoxelAttributeDescriptor::TYPE_INTEGER8, false>);
	ClassDB::bind_method(D_METHOD("set_voxel_attribute_v2i16", "attribute_index", "x", "y", "z", "value"), 
			&DynamicVoxelStorage::set_voxel_attribute_vector<Vector2i, 2, int16_t, VoxelAttributeDescriptor::TYPE_INTEGER16, false>);
	ClassDB::bind_method(D_METHOD("set_voxel_attribute_v2i32", "attribute_index", "x", "y", "z", "value"), 
			&DynamicVoxelStorage::set_voxel_attribute_vector<Vector2i, 2, int32_t, VoxelAttributeDescriptor::TYPE_INTEGER32, false>);

	ClassDB::bind_method(D_METHOD("set_voxel_attribute_v2u8", "attribute_index", "x", "y", "z", "value"), 
			&DynamicVoxelStorage::set_voxel_attribute_vector<Vector2i, 2, uint8_t, VoxelAttributeDescriptor::TYPE_INTEGER8, false>);
	ClassDB::bind_method(D_METHOD("set_voxel_attribute_v2u16", "attribute_index", "x", "y", "z", "value"), 
			&DynamicVoxelStorage::set_voxel_attribute_vector<Vector2i, 2, uint16_t, VoxelAttributeDescriptor::TYPE_INTEGER16, false>);

	ClassDB::bind_method(D_METHOD("set_voxel_attribute_v2f32_unchecked", "attribute_index", "x", "y", "z", "value"), 
			&DynamicVoxelStorage::set_voxel_attribute_vector<Vector2, 2, float, VoxelAttributeDescriptor::TYPE_FLOAT32, true>);
	ClassDB::bind_method(D_METHOD("set_voxel_attribute_v2f64_unchecked", "attribute_index", "x", "y", "z", "value"), 
			&DynamicVoxelStorage::set_voxel_attribute_vector<Vector2, 2, float, VoxelAttributeDescriptor::TYPE_FLOAT64, true>);
	ClassDB::bind_method(D_METHOD("set_voxel_attribute_v2i8_unchecked", "attribute_index", "x", "y", "z", "value"), 
			&DynamicVoxelStorage::set_voxel_attribute_vector<Vector2i, 2, int8_t, VoxelAttributeDescriptor::TYPE_INTEGER8, true>);
	ClassDB::bind_method(D_METHOD("set_voxel_attribute_v2i16_unchecked", "attribute_index", "x", "y", "z", "value"), 
			&DynamicVoxelStorage::set_voxel_attribute_vector<Vector2i, 2, int16_t, VoxelAttributeDescriptor::TYPE_INTEGER16, true>);
	ClassDB::bind_method(D_METHOD("set_voxel_attribute_v2i32_unchecked", "attribute_index", "x", "y", "z", "value"), 
			&DynamicVoxelStorage::set_voxel_attribute_vector<Vector2i, 2, int32_t, VoxelAttributeDescriptor::TYPE_INTEGER32, true>);

	ClassDB::bind_method(D_METHOD("set_voxel_attribute_v2u8_unchecked", "attribute_index", "x", "y", "z", "value"), 
			&DynamicVoxelStorage::set_voxel_attribute_vector<Vector2i, 2, uint8_t, VoxelAttributeDescriptor::TYPE_INTEGER8, true>);
	ClassDB::bind_method(D_METHOD("set_voxel_attribute_v2u16_unchecked", "attribute_index", "x", "y", "z", "value"), 
			&DynamicVoxelStorage::set_voxel_attribute_vector<Vector2i, 2, uint16_t, VoxelAttributeDescriptor::TYPE_INTEGER16, true>);

	ClassDB::bind_method(D_METHOD("set_voxel_attribute_v3f32", "attribute_index", "x", "y", "z", "value"), 
			&DynamicVoxelStorage::set_voxel_attribute_vector<Vector3, 3, float, VoxelAttributeDescriptor::TYPE_FLOAT32, false>);
	ClassDB::bind_method(D_METHOD("set_voxel_attribute_v3f64", "attribute_index", "x", "y", "z", "value"), 
			&DynamicVoxelStorage::set_voxel_attribute_vector<Vector3, 3, float, VoxelAttributeDescriptor::TYPE_FLOAT64, false>);
	ClassDB::bind_method(D_METHOD("set_voxel_attribute_v3i8", "attribute_index", "x", "y", "z", "value"), 
			&DynamicVoxelStorage::set_voxel_attribute_vector<Vector3i, 3, int8_t, VoxelAttributeDescriptor::TYPE_INTEGER8, false>);
	ClassDB::bind_method(D_METHOD("set_voxel_attribute_v3i16", "attribute_index", "x", "y", "z", "value"), 
			&DynamicVoxelStorage::set_voxel_attribute_vector<Vector3i, 3, int16_t, VoxelAttributeDescriptor::TYPE_INTEGER16, false>);
	ClassDB::bind_method(D_METHOD("set_voxel_attribute_v3i32", "attribute_index", "x", "y", "z", "value"), 
			&DynamicVoxelStorage::set_voxel_attribute_vector<Vector3i, 3, int32_t, VoxelAttributeDescriptor::TYPE_INTEGER32, false>);

	ClassDB::bind_method(D_METHOD("set_voxel_attribute_v3u8", "attribute_index", "x", "y", "z", "value"), 
			&DynamicVoxelStorage::set_voxel_attribute_vector<Vector3i, 3, uint8_t, VoxelAttributeDescriptor::TYPE_INTEGER8, false>);
	ClassDB::bind_method(D_METHOD("set_voxel_attribute_v3u16", "attribute_index", "x", "y", "z", "value"), 
			&DynamicVoxelStorage::set_voxel_attribute_vector<Vector3i, 3, uint16_t, VoxelAttributeDescriptor::TYPE_INTEGER16, false>);

	ClassDB::bind_method(D_METHOD("set_voxel_attribute_v3f32_unchecked", "attribute_index", "x", "y", "z", "value"), 
			&DynamicVoxelStorage::set_voxel_attribute_vector<Vector3, 3, float, VoxelAttributeDescriptor::TYPE_FLOAT32, true>);
	ClassDB::bind_method(D_METHOD("set_voxel_attribute_v3f64_unchecked", "attribute_index", "x", "y", "z", "value"), 
			&DynamicVoxelStorage::set_voxel_attribute_vector<Vector3, 3, float, VoxelAttributeDescriptor::TYPE_FLOAT64, true>);
	ClassDB::bind_method(D_METHOD("set_voxel_attribute_v3i8_unchecked", "attribute_index", "x", "y", "z", "value"), 
			&DynamicVoxelStorage::set_voxel_attribute_vector<Vector3i, 3, int8_t, VoxelAttributeDescriptor::TYPE_INTEGER8, true>);
	ClassDB::bind_method(D_METHOD("set_voxel_attribute_v3i16_unchecked", "attribute_index", "x", "y", "z", "value"), 
			&DynamicVoxelStorage::set_voxel_attribute_vector<Vector3i, 3, int16_t, VoxelAttributeDescriptor::TYPE_INTEGER16, true>);
	ClassDB::bind_method(D_METHOD("set_voxel_attribute_v3i32_unchecked", "attribute_index", "x", "y", "z", "value"), 
			&DynamicVoxelStorage::set_voxel_attribute_vector<Vector3i, 3, int32_t, VoxelAttributeDescriptor::TYPE_INTEGER32, true>);

	ClassDB::bind_method(D_METHOD("set_voxel_attribute_v3u8_unchecked", "attribute_index", "x", "y", "z", "value"), 
			&DynamicVoxelStorage::set_voxel_attribute_vector<Vector3i, 3, uint8_t, VoxelAttributeDescriptor::TYPE_INTEGER8, true>);
	ClassDB::bind_method(D_METHOD("set_voxel_attribute_v3u16_unchecked", "attribute_index", "x", "y", "z", "value"), 
			&DynamicVoxelStorage::set_voxel_attribute_vector<Vector3i, 3, uint16_t, VoxelAttributeDescriptor::TYPE_INTEGER16, true>);
	
	ClassDB::bind_method(D_METHOD("set_voxel_attribute_v4f32", "attribute_index", "x", "y", "z", "value"), 
			&DynamicVoxelStorage::set_voxel_attribute_vector<Vector4, 4, float, VoxelAttributeDescriptor::TYPE_FLOAT32, false>);
	ClassDB::bind_method(D_METHOD("set_voxel_attribute_v4f64", "attribute_index", "x", "y", "z", "value"), 
			&DynamicVoxelStorage::set_voxel_attribute_vector<Vector4, 4, float, VoxelAttributeDescriptor::TYPE_FLOAT64, false>);
	ClassDB::bind_method(D_METHOD("set_voxel_attribute_v4i8", "attribute_index", "x", "y", "z", "value"), 
			&DynamicVoxelStorage::set_voxel_attribute_vector<Vector4i, 4, int8_t, VoxelAttributeDescriptor::TYPE_INTEGER8, false>);
	ClassDB::bind_method(D_METHOD("set_voxel_attribute_v4i16", "attribute_index", "x", "y", "z", "value"), 
			&DynamicVoxelStorage::set_voxel_attribute_vector<Vector4i, 4, int16_t, VoxelAttributeDescriptor::TYPE_INTEGER16, false>);
	ClassDB::bind_method(D_METHOD("set_voxel_attribute_v4i32", "attribute_index", "x", "y", "z", "value"), 
			&DynamicVoxelStorage::set_voxel_attribute_vector<Vector4i, 4, int32_t, VoxelAttributeDescriptor::TYPE_INTEGER32, false>);

	ClassDB::bind_method(D_METHOD("set_voxel_attribute_v4u8", "attribute_index", "x", "y", "z", "value"), 
			&DynamicVoxelStorage::set_voxel_attribute_vector<Vector4i, 4, uint8_t, VoxelAttributeDescriptor::TYPE_INTEGER8, false>);
	ClassDB::bind_method(D_METHOD("set_voxel_attribute_v4u16", "attribute_index", "x", "y", "z", "value"), 
			&DynamicVoxelStorage::set_voxel_attribute_vector<Vector4i, 4, uint16_t, VoxelAttributeDescriptor::TYPE_INTEGER16, false>);

	ClassDB::bind_method(D_METHOD("set_voxel_attribute_v4f32_unchecked", "attribute_index", "x", "y", "z", "value"), 
			&DynamicVoxelStorage::set_voxel_attribute_vector<Vector4, 4, float, VoxelAttributeDescriptor::TYPE_FLOAT32, true>);
	ClassDB::bind_method(D_METHOD("set_voxel_attribute_v4f64_unchecked", "attribute_index", "x", "y", "z", "value"), 
			&DynamicVoxelStorage::set_voxel_attribute_vector<Vector4, 4, float, VoxelAttributeDescriptor::TYPE_FLOAT64, true>);
	ClassDB::bind_method(D_METHOD("set_voxel_attribute_v4i8_unchecked", "attribute_index", "x", "y", "z", "value"), 
			&DynamicVoxelStorage::set_voxel_attribute_vector<Vector4i, 4, int8_t, VoxelAttributeDescriptor::TYPE_INTEGER8, true>);
	ClassDB::bind_method(D_METHOD("set_voxel_attribute_v4i16_unchecked", "attribute_index", "x", "y", "z", "value"), 
			&DynamicVoxelStorage::set_voxel_attribute_vector<Vector4i, 4, int16_t, VoxelAttributeDescriptor::TYPE_INTEGER16, true>);
	ClassDB::bind_method(D_METHOD("set_voxel_attribute_v4i32_unchecked", "attribute_index", "x", "y", "z", "value"), 
			&DynamicVoxelStorage::set_voxel_attribute_vector<Vector4i, 4, int32_t, VoxelAttributeDescriptor::TYPE_INTEGER32, true>);

	ClassDB::bind_method(D_METHOD("set_voxel_attribute_v4u8_unchecked", "attribute_index", "x", "y", "z", "value"), 
			&DynamicVoxelStorage::set_voxel_attribute_vector<Vector4i, 4, uint8_t, VoxelAttributeDescriptor::TYPE_INTEGER8, true>);
	ClassDB::bind_method(D_METHOD("set_voxel_attribute_v4u16_unchecked", "attribute_index", "x", "y", "z", "value"), 
			&DynamicVoxelStorage::set_voxel_attribute_vector<Vector4i, 4, uint16_t, VoxelAttributeDescriptor::TYPE_INTEGER16, true>);

	ClassDB::bind_method(D_METHOD("set_voxel_attribute_component_f32", "attribute_index", "x", "y", "z", "component_index", "value"), 
			&DynamicVoxelStorage::set_voxel_attribute_component<float, VoxelAttributeDescriptor::TYPE_FLOAT32, false>);
	ClassDB::bind_method(D_METHOD("set_voxel_attribute_component_f64", "attribute_index", "x", "y", "z", "component_index", "value"), 
			&DynamicVoxelStorage::set_voxel_attribute_component<double, VoxelAttributeDescriptor::TYPE_FLOAT64, false>);
	ClassDB::bind_method(D_METHOD("set_voxel_attribute_component_i8", "attribute_index", "x", "y", "z", "component_index", "value"), 
			&DynamicVoxelStorage::set_voxel_attribute_component<int8_t, VoxelAttributeDescriptor::TYPE_INTEGER8, false, int32_t>);
	ClassDB::bind_method(D_METHOD("set_voxel_attribute_component_i16", "attribute_index", "x", "y", "z", "component_index", "value"), 
			&DynamicVoxelStorage::set_voxel_attribute_component<int16_t, VoxelAttributeDescriptor::TYPE_INTEGER16, false, int32_t>);
	ClassDB::bind_method(D_METHOD("set_voxel_attribute_component_i32", "attribute_index", "x", "y", "z", "component_index", "value"), 
			&DynamicVoxelStorage::set_voxel_attribute_component<int32_t, VoxelAttributeDescriptor::TYPE_INTEGER32, false>);
	ClassDB::bind_method(D_METHOD("set_voxel_attribute_component_i64", "attribute_index", "x", "y", "z", "component_index", "value"), 
			&DynamicVoxelStorage::set_voxel_attribute_component<int64_t, VoxelAttributeDescriptor::TYPE_INTEGER64, false>);
	ClassDB::bind_method(D_METHOD("set_voxel_attribute_component_u8", "attribute_index", "x", "y", "z", "component_index", "value"), 
			&DynamicVoxelStorage::set_voxel_attribute_component<uint8_t, VoxelAttributeDescriptor::TYPE_INTEGER8, false, uint32_t>);
	ClassDB::bind_method(D_METHOD("set_voxel_attribute_component_u16", "attribute_index", "x", "y", "z", "component_index", "value"), 
			&DynamicVoxelStorage::set_voxel_attribute_component<uint16_t, VoxelAttributeDescriptor::TYPE_INTEGER16, false, uint32_t>);
	ClassDB::bind_method(D_METHOD("set_voxel_attribute_component_u32", "attribute_index", "x", "y", "z", "component_index", "value"), 
			&DynamicVoxelStorage::set_voxel_attribute_component<uint32_t, VoxelAttributeDescriptor::TYPE_INTEGER32, false>);
	ClassDB::bind_method(D_METHOD("set_voxel_attribute_component_u64", "attribute_index", "x", "y", "z", "component_index", "value"), 
			&DynamicVoxelStorage::set_voxel_attribute_component<uint64_t, VoxelAttributeDescriptor::TYPE_INTEGER64, false>);

	ClassDB::bind_method(D_METHOD("set_voxel_attribute_component_f32_unchecked", "attribute_index", "x", "y", "z", "component_index", "value"), 
			&DynamicVoxelStorage::set_voxel_attribute_component<float, VoxelAttributeDescriptor::TYPE_FLOAT32, true>);
	ClassDB::bind_method(D_METHOD("set_voxel_attribute_component_f64_unchecked", "attribute_index", "x", "y", "z", "component_index", "value"), 
			&DynamicVoxelStorage::set_voxel_attribute_component<double, VoxelAttributeDescriptor::TYPE_FLOAT64, true>);
	ClassDB::bind_method(D_METHOD("set_voxel_attribute_component_i8_unchecked", "attribute_index", "x", "y", "z", "component_index", "value"), 
			&DynamicVoxelStorage::set_voxel_attribute_component<int8_t, VoxelAttributeDescriptor::TYPE_INTEGER8, true, int32_t>);
	ClassDB::bind_method(D_METHOD("set_voxel_attribute_component_i16_unchecked", "attribute_index", "x", "y", "z", "component_index", "value"), 
			&DynamicVoxelStorage::set_voxel_attribute_component<int16_t, VoxelAttributeDescriptor::TYPE_INTEGER16, true, int32_t>);
	ClassDB::bind_method(D_METHOD("set_voxel_attribute_component_i32_unchecked", "attribute_index", "x", "y", "z", "component_index", "value"), 
			&DynamicVoxelStorage::set_voxel_attribute_component<int32_t, VoxelAttributeDescriptor::TYPE_INTEGER32, true>);
	ClassDB::bind_method(D_METHOD("set_voxel_attribute_component_i64_unchecked", "attribute_index", "x", "y", "z", "component_index", "value"), 
			&DynamicVoxelStorage::set_voxel_attribute_component<int64_t, VoxelAttributeDescriptor::TYPE_INTEGER64, true>);
	ClassDB::bind_method(D_METHOD("set_voxel_attribute_component_u8_unchecked", "attribute_index", "x", "y", "z", "component_index", "value"), 
			&DynamicVoxelStorage::set_voxel_attribute_component<uint8_t, VoxelAttributeDescriptor::TYPE_INTEGER8, true, uint32_t>);
	ClassDB::bind_method(D_METHOD("set_voxel_attribute_component_u16_unchecked", "attribute_index", "x", "y", "z", "component_index", "value"), 
			&DynamicVoxelStorage::set_voxel_attribute_component<uint16_t, VoxelAttributeDescriptor::TYPE_INTEGER16, true, uint32_t>);
	ClassDB::bind_method(D_METHOD("set_voxel_attribute_component_u32_unchecked", "attribute_index", "x", "y", "z", "component_index", "value"), 
			&DynamicVoxelStorage::set_voxel_attribute_component<uint32_t, VoxelAttributeDescriptor::TYPE_INTEGER32, true>);
	ClassDB::bind_method(D_METHOD("set_voxel_attribute_component_u64_unchecked", "attribute_index", "x", "y", "z", "component_index", "value"), 
			&DynamicVoxelStorage::set_voxel_attribute_component<uint64_t, VoxelAttributeDescriptor::TYPE_INTEGER64, true>);
}

DynamicVoxelStorage::DynamicVoxelStorage() {
	resize_and_clear(chunk_size, width, height, depth);
}

DynamicVoxelStorage::~DynamicVoxelStorage() {
}
