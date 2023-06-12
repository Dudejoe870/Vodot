#pragma once

namespace util {

_ALWAYS_INLINE_ size_t index_3d(size_t x, size_t y, size_t z, size_t width, size_t height, size_t depth) {
    return (z * width * height) + (y * width) + x;
}

template <class T, typename TO_TYPE>
struct VectorComponentUtilProxy {
	static TO_TYPE get_vector_component_as_type(size_t p_component_index, T p_vector) {
		return (TO_TYPE)p_vector.coord[p_component_index];
	}
};

// Literally why is this inconsistent???
// also WHY THE HELL DOESN'T PARTIAL TEMPLATE SPECIFICATION WORK WITH FUNCTIONS????
//
//
//
// I hate C++
template <typename TO_TYPE>
struct VectorComponentUtilProxy<Vector4, TO_TYPE> {
    static TO_TYPE get_vector_component_as_type(size_t p_component_index, Vector4 p_vector) {
		return (TO_TYPE)p_vector.components[p_component_index];
	}
};

}
