#include "line_segment.hpp"

#include <cmath>
#include <iostream>
#include <limits>

LineSegment::LineSegment(const Point& a, const Point& b) : a_(a), b_(b)
{
    if (not (std::fabs(a_.x - b_.x) < std::numeric_limits<float>::epsilon()))
    {
        calculateSlope();
    }
}

LineSegment::Orientation LineSegment::getOrientation() const
{
    if (a_.x == b_.x and a_.y == b_.y)
    {
        return LineSegment::Orientation::Point;
    }
    if (std::fabs(a_.x - b_.x) < std::numeric_limits<float>::epsilon())
    {
        return LineSegment::Orientation::Vertical;
    }
    if (std::fabs(slope_) < std::numeric_limits<float>::epsilon())
    {
        return LineSegment::Orientation::Horizontal;
    }
    if (std::fabs(slope_ - 1.f) < std::numeric_limits<float>::epsilon())
    {
        return LineSegment::Orientation::Diagonal;
    }
    if (std::fabs(slope_) > 1.f)
    {
        return LineSegment::Orientation::MostlyVertical;
    }
    return LineSegment::Orientation::MostlyHorizontal;
}

void LineSegment::draw(const Canvas& canvas)
{
    const LineSegment::Orientation orientation = getOrientation();
    clipBoundaries(canvas, orientation);
    if (not isLineSegmentInFrame(canvas))
    {
        return;
    }
    const float frame_limit_x = canvas.frame_size_x - 0.5f;
    const float frame_limit_y = canvas.frame_size_y - 0.5f;
    a_.x = std::min(a_.x, frame_limit_x);
    a_.y = std::min(a_.y, frame_limit_y);
    b_.x = std::min(b_.x, frame_limit_x);
    b_.y = std::min(b_.y, frame_limit_y);
    alignPixelCenter();
    const float x1 = std::min(a_.x, b_.x);
    const float x2 = std::max(a_.x, b_.x);
    const float y1 = std::min(a_.y, b_.y);
    const float y2 = std::max(a_.y, b_.y);
    switch (orientation)
    {
    case LineSegment::Orientation::Point:
        if (isInFrame(canvas, x1, y1))
        {
            drawPoint(canvas, x1, y1);
        }
        break;
    case LineSegment::Orientation::Vertical:
        drawVertical(canvas, x1, y1, y2);
        break;
    case LineSegment::Orientation::Horizontal:
        drawHorizontal(canvas, y1, x1, x2);
        break;
    case LineSegment::Orientation::Diagonal:
        drawDiagonal(canvas, x1, x2);
        break;
    case LineSegment::Orientation::MostlyVertical:
        drawMostlyVertical(canvas, y1, y2);
        break;
    case LineSegment::Orientation::MostlyHorizontal:
        drawMostlyHorizontal(canvas, x1, x2);
        break;
    default:
        std::cerr << "[ERROR] Failed to draw a line segment\n";
        break;
    }
}

void LineSegment::calculateSlope()
{
    slope_ = (b_.y - a_.y) / (b_.x - a_.x);
}

void LineSegment::clipBoundaries(const Canvas& canvas, LineSegment::Orientation orientation)
{
    Point a = a_;
    Point b = b_;

    if (a.x < canvas.lower_bound_x) clipX(a, canvas.lower_bound_x, orientation);
    else if (a.x > canvas.frame_size_x) clipX(a, canvas.frame_size_x, orientation);

    if (b.x < canvas.lower_bound_x) clipX(b, canvas.lower_bound_x, orientation);
    else if (b.x > canvas.frame_size_x) clipX(b, canvas.frame_size_x, orientation);

    if (a.y < canvas.lower_bound_y) clipY(a, canvas.lower_bound_y, orientation);
    else if (a.y > canvas.frame_size_y) clipY(a, canvas.frame_size_y, orientation);

    if (b.y < canvas.lower_bound_y) clipY(b, canvas.lower_bound_y, orientation);
    else if (b.y > canvas.frame_size_y) clipY(b, canvas.frame_size_y, orientation);

    if (isWithinLineSegment(a) and isWithinLineSegment(b))
    {
        a_ = a;
        b_ = b;
    }
}

void LineSegment::clipX(Point& p, int64_t x_bound, LineSegment::Orientation orientation) const
{
    if (orientation == LineSegment::Orientation::Horizontal)
    {
        p.x = x_bound;
    }
    else if (orientation == LineSegment::Orientation::Diagonal)
    {
        p.x = x_bound;
        p.y = getYifDiagonalX(x_bound);
    }
    else
    {
        p.x = x_bound;
        p.y = getYifX(x_bound);
    }
}

void LineSegment::clipY(Point& p, int64_t y_bound, LineSegment::Orientation orientation) const
{
    if (orientation == LineSegment::Orientation::Vertical)
    {
        p.y = y_bound;
    }
    else
    {
        p.x = getXifY(y_bound);
        p.y = y_bound;
    }
}

bool LineSegment::isWithinLineSegment(const Point& p) const
{
    const bool isWithinX = (p.x >= std::min(a_.x, b_.x) and p.x <= std::max(a_.x, b_.x));
    const bool isWithinY = (p.y >= std::min(a_.y, b_.y) and p.y <= std::max(a_.y, b_.y));
    return (isWithinX and isWithinY);
}

bool LineSegment::isLineSegmentInFrame(const Canvas& canvas) const
{
    return (isInFrame(canvas, a_.x, a_.y) or isInFrame(canvas, b_.x, b_.y));
}

bool LineSegment::isInFrame(const Canvas& canvas, float x, float y) const
{
    return (x >= 0 and y >= 0 and x <= canvas.frame_size_x and y <= canvas.frame_size_y);
}

void LineSegment::alignPixelCenter()
{
    a_.x = std::floor(a_.x) + 0.5f;
    a_.y = std::floor(a_.y) + 0.5f;
    b_.x = std::floor(b_.x) + 0.5f;
    b_.y = std::floor(b_.y) + 0.5f;
}

inline void LineSegment::drawPoint(const Canvas& canvas, float x, float y) const
{
    canvas.frame_buf[static_cast<int64_t>(x) + static_cast<int64_t>(y) * canvas.frame_size_x] = canvas.pen_color;
}

void LineSegment::drawVertical(const Canvas& canvas, float x, float start_y, float end_y) const
{
    const int64_t end_y_int = static_cast<int64_t>(end_y);
    for (int64_t y = start_y; y <= end_y_int; ++y)
    {
        drawPoint(canvas, x, y);
    }
}

void LineSegment::drawHorizontal(const Canvas& canvas, float y, float start_x, float end_x) const
{
    const int64_t end_x_int = static_cast<int64_t>(end_x);
    for (int64_t x = start_x; x <= end_x_int; ++x)
    {
        drawPoint(canvas, x, y);
    }
}

void LineSegment::drawDiagonal(const Canvas& canvas, float start_x, float end_x) const
{
    const int64_t end_x_int = static_cast<int64_t>(end_x);
    for (int64_t x = start_x; x <= end_x_int; ++x)
    {
        const float y = getYifDiagonalX(x + 0.5f);
        drawPoint(canvas, x, y);
    }
}

void LineSegment::drawMostlyVertical(const Canvas& canvas, float start_y, float end_y) const
{
    const int64_t end_y_int = static_cast<int64_t>(end_y);
    for (int64_t y = start_y; y <= end_y_int; ++y)
    {
        const float x = getXifY(y + 0.5f);
        drawPoint(canvas, x, y);
    }
}

void LineSegment::drawMostlyHorizontal(const Canvas& canvas, float start_x, float end_x) const
{
    const int64_t end_x_int = static_cast<int64_t>(end_x);
    for (int64_t x = start_x; x <= end_x_int; ++x)
    {
        const float y = getYifX(x + 0.5f);
        drawPoint(canvas, x, y);
    }
}

inline float LineSegment::getYifDiagonalX(float x) const
{
    return a_.y + x - a_.x;
}

inline float LineSegment::getXifY(float y) const
{
    return (y - a_.y) / slope_ + a_.x;
}

inline float LineSegment::getYifX(float x) const
{
    return a_.y + slope_ * (x - a_.x);
}
