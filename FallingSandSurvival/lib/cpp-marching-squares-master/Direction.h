/**
 * A direction in the plane. As a convenience, directions provide unit vector
 * components (manhattan metric) for both the conventional plane and screen
 * coordinates (y axis reversed).
 * 
 * @author Tom Gibara
 * Ported to C++ by Juha Reunanen
 *
 */

#pragma once

#include <utility>
#include <box2d/b2_math.h>

namespace MarchingSquares {
    struct Direction {
        Direction() : x(0), y(0) {}
        Direction(int x, int y) : x(x), y(y) {}
		Direction(b2Vec2 vec) : x(vec.x), y(vec.y) {}
        int x;
        int y;
    };

    bool operator== (const Direction& a, const Direction& b) {
        return a.x == b.x && a.y == b.y;
    }

    Direction operator* (const Direction& direction, int multiplier) {
        return Direction(direction.x * multiplier, direction.y * multiplier);
    }

    Direction operator+ (const Direction& a, const Direction& b) {
        return Direction(a.x + b.x, a.y + b.y);
    }

    Direction& operator+= (Direction& a, const Direction& b) {
        a.x += b.x;
        a.y += b.y;
        return a;
    }

    Direction MakeDirection(int x, int y) { return Direction(x, y); }

    Direction East()      { return MakeDirection( 1,  0); }
    Direction Northeast() { return MakeDirection( 1,  1); }
    Direction North()     { return MakeDirection( 0,  1); }
    Direction Northwest() { return MakeDirection(-1,  1); }
    Direction West()      { return MakeDirection(-1,  0); }
    Direction Southwest() { return MakeDirection(-1, -1); }
    Direction South()     { return MakeDirection( 0, -1); }
    Direction Southeast() { return MakeDirection( 1, -1); }
}
