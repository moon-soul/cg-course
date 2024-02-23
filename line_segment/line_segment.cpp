#include "line_segment.hpp"

#include <cmath>
#include <iostream>
#include <limits>

int64_t LineSegment::frame_size_x_ = 0;
int64_t LineSegment::frame_size_y_ = 0;
int64_t LineSegment::lower_bound_x_ = 0;
int64_t LineSegment::lower_bound_y_ = 0;
uint32_t* LineSegment::frame_buf_ = nullptr;

LineSegment::LineSegment(const Point& a, const Point& b, uint32_t color) : color_(color), a_(a), b_(b)
{
    if (not (std::fabs(a_.x - b_.x) < std::numeric_limits<float>::epsilon()))
    {
        calculateSlope();
    }
}

void LineSegment::setFrameSizeX(int64_t frame_size_x)
{
    frame_size_x_ = frame_size_x;
}

void LineSegment::setFrameSizeY(int64_t frame_size_y)
{
    frame_size_y_ = frame_size_y;
}

void LineSegment::setFrameBuf(uint32_t* frame_buf)
{
    frame_buf_ = frame_buf;
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

void LineSegment::setColor(uint32_t color)
{
    color_ = color;
}

void LineSegment::draw()
{
    const LineSegment::Orientation orientation = getOrientation();
    clipBoundaries(orientation);
    if (not isLineSegmentInFrame())
    {
        std::cerr << "[ERROR] Line segment is out of boundaries\n";
        return;
    }
    alignPixelCenter();
    const float x1 = std::min(a_.x, b_.x);
    const float x2 = std::max(a_.x, b_.x);
    const float y1 = std::min(a_.y, b_.y);
    const float y2 = std::max(a_.y, b_.y);
    switch (orientation)
    {
    case LineSegment::Orientation::Point:
        if (isInFrame(x1, y1))
        {
            drawPoint(x1, y1);
        }
        break;
    case LineSegment::Orientation::Vertical:
        drawVertical(x1, y1, y2);
        break;
    case LineSegment::Orientation::Horizontal:
        drawHorizontal(y1, x1, x2);
        break;
    case LineSegment::Orientation::Diagonal:
        drawDiagonal(x1, x2);
        break;
    case LineSegment::Orientation::MostlyVertical:
        drawMostlyVertical(y1, y2);
        break;
    case LineSegment::Orientation::MostlyHorizontal:
        drawMostlyHorizontal(x1, x2);
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

void LineSegment::clipBoundaries(LineSegment::Orientation orientation)
{
    Point a = a_;
    Point b = b_;

    if (a.x < lower_bound_x_) clipX(a, lower_bound_x_, orientation);
    else if (a.x >= frame_size_x_) clipX(a, frame_size_x_ - 1, orientation);

    if (b.x < lower_bound_x_) clipX(b, lower_bound_x_, orientation);
    else if (b.x >= frame_size_x_) clipX(b, frame_size_x_ - 1, orientation);

    if (a.y < lower_bound_y_) clipY(a, lower_bound_y_, orientation);
    else if (a.y >= frame_size_y_) clipY(a, frame_size_y_ - 1, orientation);

    if (b.y < lower_bound_y_) clipY(b, lower_bound_y_, orientation);
    else if (b.y >= frame_size_y_) clipY(b, frame_size_y_ - 1, orientation);

    if (isWithinLineSegment(a) and isWithinLineSegment(b))
    {
        a_ = a;
        b_ = b;
    }
}

void LineSegment::clipX(Point& p, int64_t x_bound, LineSegment::Orientation orientation)
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

void LineSegment::clipY(Point& p, int64_t y_bound, LineSegment::Orientation orientation)
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

bool LineSegment::isLineSegmentInFrame() const
{
    return (isInFrame(a_.x, a_.y) or isInFrame(b_.x, b_.y));
}

bool LineSegment::isInFrame(float x, float y) const
{
    return (x >= 0 and y >= 0 and x < frame_size_x_ and y < frame_size_y_);
}

void LineSegment::alignPixelCenter()
{
    a_.x = std::floor(a_.x) + 0.5f;
    a_.y = std::floor(a_.y) + 0.5f;
    b_.x = std::floor(b_.x) + 0.5f;
    b_.y = std::floor(b_.y) + 0.5f;
}

inline void LineSegment::drawPoint(float x, float y)
{
    frame_buf_[static_cast<int64_t>(x) + static_cast<int64_t>(y) * frame_size_x_] = color_;
}

void LineSegment::drawVertical(float x, float start_y, float end_y)
{
    const int64_t end_y_int = static_cast<int64_t>(end_y);
    for (int64_t y = start_y; y <= end_y_int; ++y)
    {
        drawPoint(x, y);
    }
}

void LineSegment::drawHorizontal(float y, float start_x, float end_x)
{
    const int64_t end_x_int = static_cast<int64_t>(end_x);
    for (int64_t x = start_x; x <= end_x_int; ++x)
    {
        drawPoint(x, y);
    }
}

void LineSegment::drawDiagonal(float start_x, float end_x)
{
    const int64_t end_x_int = static_cast<int64_t>(end_x);
    for (int64_t x = start_x; x <= end_x_int; ++x)
    {
        const float y = getYifDiagonalX(x + 0.5f);
        drawPoint(x, y);
    }
}

void LineSegment::drawMostlyVertical(float start_y, float end_y)
{
    const int64_t end_y_int = static_cast<int64_t>(end_y);
    for (int64_t y = start_y; y <= end_y_int; ++y)
    {
        const float x = getXifY(y + 0.5f);
        drawPoint(x, y);
    }
}

void LineSegment::drawMostlyHorizontal(float start_x, float end_x)
{
    const int64_t end_x_int = static_cast<int64_t>(end_x);
    for (int64_t x = start_x; x <= end_x_int; ++x)
    {
        const float y = getYifX(x + 0.5f);
        drawPoint(x, y);
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
