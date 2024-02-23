#pragma once

#include <cstdint>

#include "point.hpp"

class LineSegment
{
public:
    enum class Orientation
    {
        Point,
        Vertical,
        Horizontal,
        Diagonal,
        MostlyVertical,
        MostlyHorizontal
    };

    LineSegment(const Point& a, const Point& b, uint32_t color = 0x000000ff);

    static void setFrameSizeX(int64_t frame_size_x);
    static void setFrameSizeY(int64_t frame_size_y);
    static void setFrameBuf(uint32_t* frame_buf);

    Orientation getOrientation() const;
    void setColor(uint32_t color);
    void draw();

private:
    void calculateSlope();
    void clipBoundaries(LineSegment::Orientation orientation);
    void clipX(Point& p, int64_t x_bound, LineSegment::Orientation orientation);
    void clipY(Point& p, int64_t y_bound, LineSegment::Orientation orientation);
    bool isWithinLineSegment(const Point& p) const;
    bool isLineSegmentInFrame() const;
    bool isInFrame(float x, float y) const;
    void alignPixelCenter();
    inline void drawPoint(float x, float y);
    void drawVertical(float x, float start_y, float end_y);
    void drawHorizontal(float y, float start_x, float end_x);
    void drawDiagonal(float start_x, float end_x);
    void drawMostlyVertical(float start_y, float end_y);
    void drawMostlyHorizontal(float start_x, float end_x);
    inline float getYifDiagonalX(float x) const;
    inline float getXifY(float y) const;
    inline float getYifX(float x) const;

    static int64_t frame_size_x_;
    static int64_t frame_size_y_;
    static int64_t lower_bound_x_;
    static int64_t lower_bound_y_;
    static uint32_t* frame_buf_;
    uint32_t color_;
    Point a_;
    Point b_;
    float slope_;
};
