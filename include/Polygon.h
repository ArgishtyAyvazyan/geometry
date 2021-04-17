/**
 * @file        Polygon.h
 * @author      Argishti Ayvazyan (ayvazyan.argishti@gmail.com)
 * @brief       Declaring the Polygon class.
 * @date        03-04-2021
 * @copyright   Copyright (c) 2021
 */


#pragma once

#include <algorithm>

#include "Definitions.h"
#include "Point.h"

namespace space
{

/**
 * @class   Polygon
 * @brief   The c++ representation of polygon.
 *          the polygon is a region of the plane bounded with the self-intersecting
 *          or non-self-intersecting piecewise linear curve.
 *
 * @details The polygon is represented by a collection of contours (\ref TContour).
 *
 * @tparam  TCrd The type of coordinate.
 */
template <typename TCrd>
class Polygon
{
public:

    /**
     * @brief   The type of coordinate.
     */
    using TCoordinate = TCrd;

    /**
     * @brief   Contour is a set of points places on a plane.
     *
     * @details Points in Contour are always in clockwise order.
     */
    using TContour = space::Vector<space::Point<TCoordinate>>;

public:


    constexpr Polygon() = default;

    constexpr ~Polygon() = default;

    constexpr Polygon(Polygon&&) noexcept = default;

    constexpr Polygon(const Polygon&) noexcept = default;

    constexpr Polygon& operator=(Polygon&&) noexcept = default;

    constexpr Polygon& operator=(const Polygon&) noexcept = default;

    /**
     * @brief           Initializes a new instance of the Polygon structure
     *                  that has the specified boundary and holes.
     *
     * @param boundary  The boundary of the polygon.
     * @param holes     The collections of holes boundary.
     */
    constexpr explicit Polygon(TContour boundary, space::Vector<TContour> holes = {})
        : m_arrContours {}
    {
        m_arrContours.reserve(std::size(holes) + 1);
        m_arrContours.push_back(std::move(boundary));
        std::ranges::move(holes, std::back_inserter(m_arrContours));
    }


    /**
     * @brief   Checks the polygon has contours or not.
     *
     * @return  true if has, otherwise false.
     */
    [[nodiscard]]
    bool empty() const noexcept
    {
        return std::empty(m_arrContours);
    }

    /**
     * @brief   Gets the const reference to polygon boundary contour.
     *
     * @throws  std::out_of_range if the polygon is empty.
     *
     * @return  The const reference to polygon boundary contour
     */
    [[nodiscard]]
    const TContour& boundary() const
    {
        if (empty())
        {
            throw std::out_of_range {"The Polygon is empty."};
        }
        return m_arrContours.front();
    }

    /**
     * @brief   Gets the reference to polygon boundary contour.
     *
     * @throws  std::out_of_range if the polygon is empty.
     *
     * @return  The reference to polygon boundary contour
     */
    [[nodiscard]]
    TContour& boundary()
    {
        return const_cast<TContour&>(std::as_const(*this).boundary());
    }

    /**
     * @brief   Checks the polygon has holes or not.
     *
     * @return  true if has, otherwise false.
     */
    [[nodiscard]]
    bool hasHoles() const noexcept
    {
        return std::size(m_arrContours) > 1;
    }

    /**
     * @brief   Gets the const span to the holes collection.
     *          If the polygon hasn't holes returns empty span.
     *
     * @return  The const span on the holes collection.
     */
    [[nodiscard]]
    space::Span<const TContour> holes() const noexcept
    {
        if (!hasHoles())
        {
            return {};
        }
        return space::Span<const TContour> {m_arrContours}.subspan(1, std::size(m_arrContours) - 1);
    }

    /**
     * @brief   Gets the span to the holes collection.
     *          If the polygon hasn't holes returns empty span.
     *
     * @return  The span on the holes collection.
     */
    [[nodiscard]]
    space::Span<TContour> holes() noexcept
    {
        if (!hasHoles())
        {
            return {};
        }
        return space::Span<TContour> {m_arrContours}.subspan(1, std::size(m_arrContours) - 1);
    }


private:

    /**
     * @brief   The polygon representation by polygons. The first item represents
     *          the polygon outside boundaries the others represent holes.
     */
    space::Vector<TContour> m_arrContours;
}; // class Polygon

/**
 * @brief   The ostream operators for working with streams.
 *
 * @note    Templates used to avoid include stream headers.
 *
 * @tparam  TOstream The type of ostream.
 * @tparam  TCrd The type of coordinates.
 * @param   os The ostream.
 * @param   point The Rect.
 * @return  The reference to ostream.
 */
template <typename TOstream, typename TCrd>
TOstream& operator<<(TOstream& os, const space::Polygon<TCrd>& poly)
{
    os << "Polygon { Boundary: { ";
    std::ranges::copy(poly.boundary(), std::ostream_iterator<space::Point<TCrd>>(os, ", "));
    if (poly.hasHoles())
    {
        for (auto& hole : poly.holes())
        {
            os << " } Hole: { ";
            std::ranges::copy(hole, std::ostream_iterator<space::Point<TCrd>>(os, ", "));
        }
    }
    os << " } }";
    return os;
}

} // namespace space



