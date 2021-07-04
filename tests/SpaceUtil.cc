/**
 * @file        SpaceUtil.cc
 * @author      Argishti Ayvazyan (ayvazyan.argishti@gmail.com)
 * @brief       Tests implementation for unti tests for space::utils.
 * @date        20-03-2021
 * @copyright   Copyright (c) 2021
 */


#include <gtest/gtest.h>

#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/geometries/box.hpp>
#include <boost/geometry/geometries/segment.hpp>
#include <boost/geometry/algorithms/intersection.hpp>

#include <boost/geometry/index/rtree.hpp>


#include "Rect.h"
#include "Square.h"
#include "SimplePolygon.h"
#include "Polygon.h"
#include "Segment.h"
#include "Utility.h"


int rand(int from, int to)
{
    return (std::rand() % (to - from)) + from;
}

template <typename TCrt>
auto spaceToBoostPoint(const space::Point<TCrt>& point)
{
    using BPoint = boost::geometry::model::point<TCrt, 2, boost::geometry::cs::cartesian>;
    const auto[x, y] = point;
    return BPoint {x, y};
}

template <typename TCrt>
auto spaceToBoostRect(const space::Rect<TCrt>& rect)
{
    using point = boost::geometry::model::point<TCrt, 2, boost::geometry::cs::cartesian>;
    using box = boost::geometry::model::box<point>;

    const auto[x1, y1] = space::util::bottomLeftOf(rect);
    const auto[x2, y2] = space::util::topRightOf(rect);
    return box(point(x1, y1), point(x2, y2));
}

TEST(ComparePoint, space_Point)
{
    space::Point<int32_t> point {50, 13};
    space::Point<int32_t> point1 {0, 0};

    ASSERT_TRUE(point == point);
    ASSERT_FALSE(point != point);
    ASSERT_FALSE(point == point1);
    ASSERT_TRUE(point != point1);
}

TEST(CoverRectPoint, space_util)
{
    space::Rect<int32_t> rect {{0, 0}, 100, 100};
    space::Point<int32_t> point {50, 50};
    ASSERT_TRUE (space::util::contains(rect, point));
    space::util::move(point, 100, 100);
    ASSERT_FALSE (space::util::contains(rect, point));
}

TEST(CoverRect, space_util)
{
    space::Rect<int32_t> rect1 {{0, 0}, 100, 100};
    space::Rect<int32_t> rect2 {{50, 50}, 10, 10};
    ASSERT_TRUE (space::util::contains(rect1, rect2));
    space::util::move(rect2, 100, 100);
    ASSERT_FALSE (space::util::contains(rect1, rect2));
}

TEST(CreateRect, space_util)
{
    space::Point<int32_t> leftBottom {1, 1};
    space::Point<int32_t> rightTop {13, 13};

    space::Rect<int32_t> rect {leftBottom, rightTop};

    ASSERT_TRUE (space::util::bottomLeftOf(rect) == leftBottom);
    ASSERT_TRUE (space::util::topRightOf(rect) == rightTop);
}


TEST(CompareRect, space_Rect)
{
    space::Rect<int32_t> rect {{50, 13}, 100, 100};
    space::Rect<int32_t> rect1 {{0, 0}, 123, 123};

    ASSERT_TRUE(rect == rect);
    ASSERT_FALSE(rect != rect);
    ASSERT_FALSE(rect == rect1);
    ASSERT_TRUE(rect != rect1);
}


TEST(CoverSquarePoint, space_util)
{
    space::Square<int32_t> rect {{0, 0}, 100};
    space::Point<int32_t> point {50, 50};
    ASSERT_TRUE (space::util::contains(rect, point));
    space::util::move(point, 100, 100);
    ASSERT_FALSE (space::util::contains(rect, point));
}

TEST(CoverSquare, space_util)
{
    space::Square<int32_t> rect1 {{0, 0}, 100};
    space::Rect<int32_t> rect2 {{50, 50}, 10, 10};
    ASSERT_TRUE (space::util::contains(rect1, rect2));
    space::util::move(rect2, 100, 100);
    ASSERT_FALSE (space::util::contains(rect1, rect2));
}

TEST(IntersectsRect, space_util)
{
    space::Rect<int32_t> rect {{50, 13}, 100, 100};
    space::Rect<int32_t> rect1 {{0, 0}, 123, 123};

    ASSERT_TRUE(space::util::hesIntersect(rect, rect1));
    ASSERT_TRUE(space::util::hesIntersect(rect1, rect));

    space::util::move(rect1, 149, 110);

    ASSERT_TRUE(space::util::hesIntersect(rect, rect1));
    ASSERT_TRUE(space::util::hesIntersect(rect1, rect));

    space::util::move(rect1, 100000, 100000);

    ASSERT_FALSE(space::util::hesIntersect(rect, rect1));
    ASSERT_FALSE(space::util::hesIntersect(rect1, rect));

    for (int64_t i = 0; i < 1'000'000; ++i)
    {
        space::Rect<int32_t> spaceRect1 {{std::rand() % 1000, std::rand() % 1000}, std::rand() % 1000,
                                         std::rand() % 1000};
        space::Rect<int32_t> spaceRect2 {{std::rand() % 1000, std::rand() % 1000}, std::rand() % 1000,
                                         std::rand() % 1000};
        const auto boostBox1 = spaceToBoostRect(spaceRect1);
        const auto boostBox2 = spaceToBoostRect(spaceRect2);
        ASSERT_TRUE(space::util::hesIntersect(spaceRect1, spaceRect2)
                == boost::geometry::intersects(boostBox1, boostBox2));
    }
}

TEST(IntersectsSquare, space_Square)
{
    space::Rect<int32_t> rect {{50, 13}, 100, 100};
    space::Square<int32_t> rect1 {{0, 0}, 123};

    ASSERT_TRUE(space::util::hesIntersect(rect, rect1));
    ASSERT_TRUE(space::util::hesIntersect(rect1, rect));

    space::util::move(rect1, 149, 110);

    ASSERT_TRUE(space::util::hesIntersect(rect, rect1));
    ASSERT_TRUE(space::util::hesIntersect(rect1, rect));

    space::util::move(rect1.pos(), 100000, 100000);

    ASSERT_FALSE(space::util::hesIntersect(rect, rect1));
    ASSERT_FALSE(space::util::hesIntersect(rect1, rect));
}

TEST(CompareSquare, space_Square)
{
    space::Square<int32_t> square {{50, 13}, 100};
    space::Square<int32_t> square1 {{0, 0}, 123};

    ASSERT_TRUE(square == square);
    ASSERT_FALSE(square != square);
    ASSERT_FALSE(square == square1);
    ASSERT_TRUE(square != square1);
}

TEST(EmptySimplePolygon, space_SimplePolygon)
{
    using Poly = space::SimplePolygon<int32_t>;
    Poly poly;
    ASSERT_TRUE (poly.empty());
    Poly::TPiecewiseLinearCurve boundary {{0, 0},
                                          {1, 1},
                                          {2, 2}};
    Poly poly1 {boundary};
    ASSERT_FALSE (poly1.empty());
}

TEST(BoundaryCurveSimplePolygon, space_SimplePolygon)
{
    using Poly = space::SimplePolygon<int32_t>;
    Poly poly;
    EXPECT_THROW ((void)poly.boundaryCurve(), std::out_of_range);

    Poly::TPiecewiseLinearCurve boundary {{0, 0},
                                          {1, 1},
                                          {2, 2}};

    Poly poly1 {boundary};
    ASSERT_TRUE(boundary == poly1.boundaryCurve());
}

TEST(MoveSimplePolygon, space_SimplePolygon)
{
    using Poly = space::SimplePolygon<int32_t>;

    Poly::TPiecewiseLinearCurve boundary {{0, 0},
                                          {1, 1},
                                          {2, 2}};

    Poly poly {boundary};

    space::util::move(poly, 12, 12);
    const auto& changedBoundary = poly.boundaryCurve();
    ASSERT_TRUE (std::ranges::equal(boundary, changedBoundary
                                , [](auto point1, const auto& point2)
        {
            space::util::move(point1, 12, 12);
            return point1 == point2;
        }
                               ));
}

TEST(BoundaryBoxOfSimplePolygon, space_SimplePolygon)
{
    using Poly = space::SimplePolygon<int32_t>;
    using Rect = space::Rect<int32_t>;

    Poly::TPiecewiseLinearCurve boundary {{0,   0},
                                          {1,   1},
                                          {12,  14},
                                          {124, 444},
                                          {2,   2}};

    Poly poly {boundary};
    const auto bBox = space::util::boundaryBoxOf(poly);
    ASSERT_TRUE((bBox == Rect {{0,   0}, {124, 444}}));
}

TEST(CompareSimplePolygon, space_SimplePolygon)
{
    using Poly = space::SimplePolygon<int32_t>;

    Poly::TPiecewiseLinearCurve boundary {{{0, 0},
                                              {1, 1},
                                              {12, 14},
                                              {124, 444},
                                              {2, 2}}};

    Poly poly {boundary};
    Poly poly1;

    ASSERT_TRUE(poly == poly);
    ASSERT_FALSE(poly != poly);
    ASSERT_FALSE(poly == poly1);
    ASSERT_TRUE(poly != poly1);
}

TEST(EmptyPolygon, space_Polygon)
{
    using Poly = space::Polygon<int32_t>;
    using SimplePoly = space::SimplePolygon<int32_t>;
    Poly poly;
    ASSERT_TRUE (poly.empty());
    ASSERT_FALSE (poly.hasHoles());
    Poly::TSimplePolugon boundary {{{0, 0},
                                       {1, 1},
                                       {2, 2}}};
    Poly poly1 {boundary};
    ASSERT_FALSE (poly1.empty());
    ASSERT_FALSE (poly1.hasHoles());

    space::Vector<SimplePoly> holes;
    holes.push_back(SimplePoly {{{3, 3}, {1, 1}, {2, 2}}});
    holes.push_back(SimplePoly {{{6, 6}, {3, 3}, {9, 9}}});

    Poly poly2 {boundary, holes};
    ASSERT_FALSE (poly2.empty());
    ASSERT_TRUE (poly2.hasHoles());
}

TEST(HasHolesPolygon, space_Polygon)
{
    using Poly = space::Polygon<int32_t>;
    using SimplePoly = space::SimplePolygon<int32_t>;

    Poly poly;
    ASSERT_FALSE (poly.hasHoles());
    Poly::TSimplePolugon boundary {{{0, 0},
                                       {1, 1},
                                       {2, 2}}};
    space::Vector<SimplePoly> holes;
    holes.push_back(SimplePoly {{{3, 3}, {1, 1}, {2, 2}}});
    holes.push_back(SimplePoly {{{6, 6}, {3, 3}, {9, 9}}});

    Poly poly1 {boundary, holes};
    ASSERT_FALSE (poly1.empty());
    ASSERT_TRUE (poly1.hasHoles());
}

TEST(boundary_Polygon, space_Polygon)
{
    using Poly = space::Polygon<int32_t>;

    Poly poly;
    EXPECT_THROW((void)poly.boundary(), std::out_of_range);

    Poly::TSimplePolugon boundary {{{0, 0},
                                       {1, 1},
                                       {2, 2}}};


    Poly poly1 {boundary};
    ASSERT_TRUE(boundary == poly1.boundary());
}

TEST(HolesPolygon, space_Polygon)
{
    using Poly = space::Polygon<int32_t>;
    using SimplePoly = space::SimplePolygon<int32_t>;

    Poly poly;
    ASSERT_TRUE (poly.holes().empty());

    Poly::TSimplePolugon boundary {{{0, 0},
                                       {1, 1},
                                       {2, 2}}};
    Poly poly1 {boundary};
    ASSERT_TRUE (poly1.holes().empty());

    space::Vector<SimplePoly> holes;
    holes.push_back(SimplePoly {{{3, 3}, {1, 1}, {2, 2}}});
    holes.push_back(SimplePoly {{{6, 6}, {3, 3}, {9, 9}}});

    Poly poly2 {boundary, holes};
    auto spanHoles = poly2.holes();
    ASSERT_FALSE (spanHoles.empty());
    ASSERT_TRUE(std::size(spanHoles) == std::size(holes));
}

TEST(MovePolygon, space_Polygon)
{
    using Poly = space::Polygon<int32_t>;
    using SimplePoly = space::SimplePolygon<int32_t>;

    Poly::TSimplePolugon boundary {{{0, 0},
                                       {1, 1},
                                       {2, 2}}};
    space::Vector<SimplePoly> holes;
    holes.push_back(SimplePoly {{{3, 3}, {1, 1}, {2, 2}}});
    holes.push_back(SimplePoly {{{6, 6}, {3, 3}, {9, 9}}});

    Poly poly {boundary, holes};

    space::util::move(poly, 12, 13);

    auto equal = [](auto point1, const auto& point2)
    {
        space::util::move(point1, 12, 13);
        return point1 == point2;
    };

    ASSERT_TRUE (std::ranges::equal(boundary.boundaryCurve(), poly.boundary().boundaryCurve(), equal));

    ASSERT_TRUE (std::ranges::equal(holes, poly.holes(), equal));
}

TEST(BoundaryBoxOfPolygon, space_Polygon)
{
    using Poly = space::Polygon<int32_t>;
    using Rect = space::Rect<int32_t>;
    using SimplePoly = Poly::TSimplePolugon;

    SimplePoly boundary {{{0, 0},
                             {1, 1},
                             {12, 14},
                             {124, 444},
                             {2, 2}}};

    space::Vector<SimplePoly> holes;
    holes.push_back(SimplePoly {{{3, 3}, {1, 1}, {2, 2}}});
    holes.push_back(SimplePoly {{{6, 6}, {3, 3}, {9, 9}}});

    Poly poly {boundary, holes};
    const auto bBox = space::util::boundaryBoxOf(poly);
    Rect target {{0,   0}, {124, 444}};
    ASSERT_EQ(bBox , target);
}

TEST(ComparePolygon, space_Polygon)
{
    using Poly = space::Polygon<int32_t>;
    using SimplePoly = Poly::TSimplePolugon;

    SimplePoly boundary {{{0, 0},
                             {1, 1},
                             {12, 14},
                             {124, 444},
                             {2, 2}}};

    space::Vector<SimplePoly> holes;
    holes.push_back(SimplePoly {{{3, 3}, {1, 1}, {2, 2}}});
    holes.push_back(SimplePoly {{{6, 6}, {3, 3}, {9, 9}}});

    Poly poly {boundary, holes};

    Poly poly1;
    ASSERT_TRUE(poly == poly);
    ASSERT_FALSE(poly != poly);
    ASSERT_FALSE(poly == poly1);
    ASSERT_TRUE(poly != poly1);
}

TEST(SimpleSegment, space_Segment)
{
    using Point = space::Point<int32_t>;
    using Segment = space::Segment<int32_t>;

    Point p1 {1, 1};
    Point p2 {4, 4};
    Segment segment {p1, p2};

    ASSERT_TRUE(p1 == segment.first);
    ASSERT_TRUE(p2 == segment.second);

    Segment segment2 {{3, 3},
                      {5, 5}};
    Point target1{3, 3};
    Point target2{5, 5};
    ASSERT_EQ(target1, segment2.first);
    ASSERT_EQ(target2, segment2.second);
}


TEST(Compare_Segment, space_Segment)
{
    using Point = space::Point<int32_t>;
    using Segment = space::Segment<int32_t>;

    Point p1 {1, 1};
    Point p2 {4, 4};
    Segment segment {p1, p2};
    Segment segment2 {p2, p1};

    ASSERT_TRUE(segment == segment);
    ASSERT_FALSE(segment != segment);
    ASSERT_FALSE(segment == segment2);
    ASSERT_TRUE(segment != segment2);
}

TEST(HesIntersectSimpleSegment, space_Segment)
{
    using Point = space::Point<int32_t>;
    using Segment = space::Segment<int32_t>;

    Point p1 {1, 1};
    Point q1 {4, 4};
    Segment segment {p1, q1};
    Point p2 {1, 4};
    Point q2 {4, 1};
    Segment segment2 {p2, q2};

    ASSERT_TRUE(space::util::hesIntersect(segment, segment2));
}

TEST(hesIntersect_Segment, space_Segment)
{
    using SPoint = space::Point<int32_t>;
    using SSegment = space::Segment<int32_t>;
    using BPoint = boost::geometry::model::point<int32_t, 2, boost::geometry::cs::cartesian>;
    using BSegment = boost::geometry::model::segment<BPoint>;

    for (int32_t i = 0; i < 100'000; ++i)
    {
        SPoint p1 {rand(0, 100), rand(0, 100)};
        SPoint q1 {rand(0, 100), rand(0, 100)};
        SSegment sSegment1 {p1, q1};
        BSegment bSegment1 {spaceToBoostPoint(p1), spaceToBoostPoint(q1)};
        SPoint p2 {rand(0, 100), rand(0, 100)};
        SPoint q2 {rand(0, 100), rand(0, 100)};
        SSegment sSegment2 {p2, q2};
        BSegment bSegment2 {spaceToBoostPoint(p2), spaceToBoostPoint(q2)};

        const bool result = boost::geometry::intersects(bSegment1, bSegment2);
        ASSERT_TRUE(result == space::util::hesIntersect(sSegment1, sSegment2));
        ASSERT_TRUE(result == space::util::hesIntersect(sSegment2, sSegment1));
    }
}


int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
