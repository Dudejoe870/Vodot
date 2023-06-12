#pragma once

#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>

#include <godot_cpp/templates/vector.hpp>

#include "voxel_attribute_object.hpp"
#include "util.hpp"

using namespace godot;

// A Dynamic Voxel Container object is a container object that stores Voxels in an optimized dynamic way.
// It uses a volumetric grid of "Chunks" that contain Voxel data.
//
// This is ideal for large Voxel models that can be dynamically edited.
class DynamicVoxelStorage : public RefCounted
{
	GDCLASS(DynamicVoxelStorage, RefCounted);

protected:
	static void _bind_methods();

	Ref<VoxelAttributeObject> voxel_attribute_object;

	// The Chunk size to use. Describes all axes (width, height, depth).
	size_t chunk_size = 32;

	// These are aligned up to the nearest multiple of "chunk_size".
	size_t width = 256;
	size_t height = 256;
	size_t depth = 256;

	enum {
		EMPTY_CHUNK = UINT32_MAX
	};

	// Stores chunk indexes within the attribute buffers where the data for certain "chunks" lie
	// in a 3D volumetric grid.
	// A chunk that is set to "UINT32_MAX" is empty.
	TightLocalVector<uint32_t> _chunk_buffer;

	// Stores a buffer per-attribute (in the order they appear in the descriptors array within the Attribute Object)
	// that contains all of the Voxel data for the non-empty chunks.
	LocalVector<LocalVector<uint8_t>> _attribute_buffers;

	// While the other data can be directly uploaded to the GPU, this is data that only the CPU needs to keep track of.
	struct AllocatedChunkInfo {
		uint32_t voxel_counter = 0; // Voxel counter so we know when to free the chunk.
	};
	TightLocalVector<AllocatedChunkInfo> _allocated_chunk_info;

	// This is also CPU only.
	// Stores a list of chunk indexes that were previously marked as reusable by being empty.
	// Used to prevent unnecessary buffer growth.
	LocalVector<uint32_t> _reusable_chunk_queue;

	void _init_buffers();

	// Allocates the memory required for a new chunk by appending extra room on the end of each of the attribute buffers.
	_ALWAYS_INLINE_ uint32_t _allocate_new_chunk() {
		if (_attribute_buffers.is_empty()) return EMPTY_CHUNK;
		uint32_t allocated_chunk_index = 0;
		{
			// We get the index of the chunk we are about to allocate by dividing the size of the first attribute buffer by the byte size of each chunk.
			const Ref<VoxelAttributeDescriptor> &attribute_info = get_voxel_attribute_object()->descriptors[0];
			allocated_chunk_index = _attribute_buffers[0].size() / ((chunk_size * chunk_size * chunk_size) * 
					attribute_info->get_component_size() * attribute_info->get_num_components());
			
			 // Allocate a new info struct.
			_allocated_chunk_info.reserve(allocated_chunk_index+1);
			_allocated_chunk_info.resize(allocated_chunk_index+1);
		}

		for (size_t i = 0; i < _attribute_buffers.size(); i++) {
			LocalVector<uint8_t> &buffer = _attribute_buffers[i];
			const Ref<VoxelAttributeDescriptor> &attribute_info = get_voxel_attribute_object()->descriptors[i];
			buffer.resize(buffer.size() + ((chunk_size * chunk_size * chunk_size) * 
					attribute_info->get_component_size() * attribute_info->get_num_components()));
		}
		return allocated_chunk_index;
	}

	_ALWAYS_INLINE_ uint32_t _get_next_chunk() {
		uint32_t chunk_index = 0;
		if (_reusable_chunk_queue.is_empty()) {
			// If there are no reusable chunks in the middle of the buffers then allocate a new one on the end.
			chunk_index = _allocate_new_chunk();
			ERR_FAIL_COND_V_MSG(chunk_index == EMPTY_CHUNK, EMPTY_CHUNK, "No attribute buffers defined, cannot allocate Voxel Chunk.");
		} else {
			// Otherwise (there *are* reusable chunks in the middle), pop one off the queue and use that index. 
			// (we can assume it's already initialized to empty, as otherwise it wouldn't have been freed in the first place)
			chunk_index = _reusable_chunk_queue[_reusable_chunk_queue.size()-1];
			_reusable_chunk_queue.resize(_reusable_chunk_queue.size()-1);
		}
		return chunk_index;
	}

	_ALWAYS_INLINE_ uint32_t &_get_chunk_index(size_t p_x, size_t p_y, size_t p_z) {
		size_t chunk_buffer_index = util::index_3d(
				p_x / chunk_size, p_y / chunk_size, p_z / chunk_size, 
				width / chunk_size, height / chunk_size, depth / chunk_size);
		return _chunk_buffer[chunk_buffer_index];
	}

	_ALWAYS_INLINE_ bool _init_chunk_index(uint32_t &p_chunk_index, size_t p_attribute_index, size_t p_x, size_t p_y, size_t p_z, bool p_is_zero_write) {
		if (p_chunk_index == EMPTY_CHUNK) {
			if (p_is_zero_write) return false;

			p_chunk_index = _get_next_chunk();
		}
		return true;
	}

	_ALWAYS_INLINE_ bool _check_voxel(uint32_t p_chunk_index, size_t p_chunk_voxel_index) {
		for (size_t attribute_index = 0; attribute_index < _attribute_buffers.size(); attribute_index++) {
			const Ref<VoxelAttributeDescriptor> &attribute_info = get_voxel_attribute_object()->descriptors[attribute_index];
			const LocalVector<uint8_t> &buffer = _attribute_buffers[attribute_index];
			const uint8_t *attribute_ptr = &buffer[
				((p_chunk_index * (chunk_size * chunk_size * chunk_size)) + p_chunk_voxel_index)
					* attribute_info->get_component_size() * attribute_info->get_num_components()];
			for (size_t i = 0; i < attribute_info->get_component_size() * attribute_info->get_num_components(); i++) {
				if (attribute_ptr[i] != 0) return true;
			}
		}
		return false;
	}

	_ALWAYS_INLINE_ void _mark_chunk_as_reusable_if_chunk_is_now_empty(uint32_t &p_chunk_index, size_t p_chunk_voxel_index, bool p_is_zero_write) {
		if (p_is_zero_write) {
			if (!_check_voxel(p_chunk_index, p_chunk_voxel_index)) {
				_allocated_chunk_info[p_chunk_index].voxel_counter--;
				if (_allocated_chunk_info[p_chunk_index].voxel_counter == 0) {
					_reusable_chunk_queue.push_back(p_chunk_index);
					p_chunk_index = EMPTY_CHUNK;
				}
			}
		}
	}
public:
	Ref<VoxelAttributeObject> get_voxel_attribute_object() const;
	void set_voxel_attribute_object(const Ref<VoxelAttributeObject> &p_voxel_attribute_object);

	size_t get_chunk_size() const;
	size_t get_width() const;
	size_t get_height() const;
	size_t get_depth() const;

	void resize_and_clear(size_t p_width, size_t p_height, size_t p_depth, size_t p_chunk_size);
	void clear();

	template <class T, size_t num_components, typename COMPONENT_T, VoxelAttributeDescriptor::Type COMPONENT_TYPE, bool unchecked = false>
	void set_voxel_attribute_vector(size_t p_attribute_index, size_t p_x, size_t p_y, size_t p_z, T p_value) {
		const Ref<VoxelAttributeDescriptor> &attribute_info = get_voxel_attribute_object()->descriptors[p_attribute_index];
		LocalVector<uint8_t> &attribute_buffer = _attribute_buffers[p_attribute_index];
		if constexpr (!unchecked) {
			ERR_FAIL_COND_MSG(attribute_info->get_type() == COMPONENT_TYPE, "Attribute component type doesn't match Vector component type.");
		}
		bool is_zero_write = p_value == T();

		uint32_t &chunk_index = _get_chunk_index(p_x, p_y, p_z);
		if (!_init_chunk_index(chunk_index, p_attribute_index, p_x, p_y, p_z, is_zero_write)) return;

		size_t chunk_voxel_index = util::index_3d(
				p_x % chunk_size, p_y % chunk_size, p_z % chunk_size,
				chunk_size, chunk_size, chunk_size);
		uint8_t *attribute_ptr = &attribute_buffer[
				((chunk_index * (chunk_size * chunk_size * chunk_size)) + chunk_voxel_index)
					* attribute_info->get_component_size() * attribute_info->get_num_components()];
#ifdef REAL_T_IS_DOUBLE
		if constexpr (COMPONENT_TYPE == VoxelAttributeDescriptor::TYPE_FLOAT64
#else
		if constexpr (COMPONENT_TYPE == VoxelAttributeDescriptor::TYPE_FLOAT32
#endif
		|| COMPONENT_TYPE == VoxelAttributeDescriptor::TYPE_INTEGER32) {
			*reinterpret_cast<T*>(attribute_ptr) = p_value;
		} else {
			for (size_t i = 0; i < num_components; i++) {
				*reinterpret_cast<COMPONENT_T*>(attribute_ptr + (i * attribute_info->get_component_size())) = util::VectorComponentUtilProxy<T, COMPONENT_T>::get_vector_component_as_type(i, p_value);
			}
		}

		_mark_chunk_as_reusable_if_chunk_is_now_empty(chunk_index, chunk_voxel_index, is_zero_write);
	}

	template <typename T, VoxelAttributeDescriptor::Type COMPONENT_TYPE, bool unchecked = false, typename PARAMETER_T = T>
	void set_voxel_attribute_component(size_t p_attribute_index, size_t p_x, size_t p_y, size_t p_z, size_t p_component, PARAMETER_T p_value) {
		const Ref<VoxelAttributeDescriptor> &attribute_info = get_voxel_attribute_object()->descriptors[p_attribute_index];
		LocalVector<uint8_t> &attribute_buffer = _attribute_buffers[p_attribute_index];
		if constexpr (!unchecked) {
			ERR_FAIL_COND_MSG(attribute_info->get_type() == COMPONENT_TYPE, "Attribute component type doesn't match value type.");
		}
		bool is_zero_write = p_value == 0;

		uint32_t &chunk_index = _get_chunk_index(p_x, p_y, p_z);
		if (!_init_chunk_index(chunk_index, p_attribute_index, p_x, p_y, p_z, is_zero_write)) return;

		size_t chunk_voxel_index = util::index_3d(
				p_x % chunk_size, p_y % chunk_size, p_z % chunk_size,
				chunk_size, chunk_size, chunk_size);
		uint8_t *component_ptr = &attribute_buffer[
				((((chunk_index * (chunk_size * chunk_size * chunk_size)) + chunk_voxel_index) 
					* attribute_info->get_num_components()) + p_component) 
					* attribute_info->get_component_size()];
		*reinterpret_cast<T*>(component_ptr) = (T)p_value;
		_mark_chunk_as_reusable_if_chunk_is_now_empty(chunk_index, chunk_voxel_index, is_zero_write);
	}

	DynamicVoxelStorage();
	~DynamicVoxelStorage();
};
