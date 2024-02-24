#pragma once

#include <cstdint>

struct Canvas
{
    uint32_t* frame_buf = nullptr;
    int64_t frame_size_x = 0;
    int64_t frame_size_y = 0;
    int64_t lower_bound_x = 0;
    int64_t lower_bound_y = 0;
    uint32_t pen_color = 0x000000ff;
};
