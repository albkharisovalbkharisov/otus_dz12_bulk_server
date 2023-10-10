#pragma once

#include <cstddef>

namespace async {

using handle_t = void *;

handle_t connect(std::size_t bulk);
void receive(handle_t handle, const char *data, std::size_t size);
void disconnect(handle_t handle);

extern std::size_t default_bulk_size;

inline std::size_t get_default_bulk_size(void) {
    return async::default_bulk_size;
}

inline void set_default_bulk_size(std::size_t bulk_size) {
    default_bulk_size = bulk_size;
}

}

