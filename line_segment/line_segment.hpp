#pragma once

#include <cstdint>

#include "canvas.hpp"
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

    LineSegment(const Point& a, const Point& b);

    Orientation getOrientation() const;
    void draw(const Canvas& canvas);

private:
    void calculateSlope();

    void clipBoundaries(const Canvas& canvas, LineSegment::Orientation orientation);
    void clipX(Point& p, int64_t x_bound, LineSegment::Orientation orientation) const;
    void clipY(Point& p, int64_t y_bound, LineSegment::Orientation orientation) const;
    bool isWithinLineSegment(const Point& p) const;

    bool isLineSegmentInFrame(const Canvas& canvas) const;
    bool isInFrame(const Canvas& canvas, float x, float y) const;
    void alignPixelCenter();

    inline void drawPoint(const Canvas& canvas, float x, float y) const;
    void drawVertical(const Canvas& canvas, float x, float start_y, float end_y) const;
    void drawHorizontal(const Canvas& canvas, float y, float start_x, float end_x) const;
    void drawDiagonal(const Canvas& canvas, float start_x, float end_x) const;
    void drawMostlyVertical(const Canvas& canvas, float start_y, float end_y) const;
    void drawMostlyHorizontal(const Canvas& canvas, float start_x, float end_x) const;

    inline float getYifDiagonalX(float x) const;
    inline float getXifY(float y) const;
    inline float getYifX(float x) const;

    Point a_;
    Point b_;
    float slope_;
};
