#ifndef NDRNP_COORDINATE_H
#define NDRNP_COORDINATE_H

#include <iostream>
#include <utility>
#include <cmath>

namespace ndrnp {
    // type declarations.
    class Coordinate;

    // function declarations.

    /* @class Coordinate
     * The coordinate of a point in 3-D Euclidean space.
     */
    class Coordinate {
    public:
        typedef double    coordinate_type;

        Coordinate(const coordinate_type& x = 0.0,
                   const coordinate_type& y = 0.0,
                   const coordinate_type& z = 0.0)
        : _x(x), _y(y), _z(z) {}

        Coordinate(const Coordinate&);
        Coordinate(Coordinate&&);

        Coordinate& operator=(const Coordinate&);
        Coordinate& operator=(Coordinate&&);

        Coordinate& operator+=(const Coordinate&);
        Coordinate& operator-=(const Coordinate&);

        Coordinate operator+(const Coordinate&) const;
        Coordinate operator-(const Coordinate&) const;

        bool operator==(const Coordinate&) const;
        bool operator!=(const Coordinate&) const;

        bool operator>(const Coordinate&) const = delete;
        bool operator>=(const Coordinate&) const = delete;
        bool operator<(const Coordinate&) const = delete;
        bool operator<=(const Coordinate&) const = delete;

        coordinate_type x() const { return _x; }
        coordinate_type y() const { return _y; }
        coordinate_type z() const { return _z; }

        void setX(const coordinate_type& x) { _x = x; }
        void setY(const coordinate_type& y) { _y = y; }
        void setZ(const coordinate_type& z) { _z = z; }
    private:
        coordinate_type    _x;
        coordinate_type    _y;
        coordinate_type    _z;
    };

    Coordinate::Coordinate(const Coordinate& coor)
    : _x(coor._x), _y(coor._y), _z(coor._z) {}
    Coordinate::Coordinate(Coordinate&& coor)
    : _x(std::move(coor._x)), _y(std::move(coor._y)),
      _z(std::move(coor._z)) {}

    Coordinate&
    Coordinate::operator=(const Coordinate& co) {
        _x = co._x; _y = co._y; _z = co._z;
        return *this;
    }
    Coordinate&
    Coordinate::operator=(Coordinate&& co) {
        _x = std::move(co._x);
        _y = std::move(co._y);
        _z = std::move(co._z);
        return *this;
    }

    Coordinate&
    Coordinate::operator+=(const Coordinate& rhs) {
        _x += rhs._x; _y += rhs._y; _z += rhs._z;
        return *this;
    }
    Coordinate&
    Coordinate::operator-=(const Coordinate& rhs) {
        _x -= rhs._x; _y -= rhs._y; _z -= rhs._z;
        return *this;
    }

    Coordinate
    Coordinate::operator+(const Coordinate& rhs) const {
        return Coordinate(_x + rhs._x, _y + rhs._y,
                          _z + rhs._z);
    }
    Coordinate
    Coordinate::operator-(const Coordinate& rhs) const {
        return Coordinate(_x - rhs._x, _y - rhs._y,
                          _z - rhs._z);
    }

    bool
    Coordinate::operator==(const Coordinate& rhs) const {
        return _x == rhs._x && _y == rhs._y && _z == rhs._z;
    }
    bool
    Coordinate::operator!=(const Coordinate& rhs) const {
        return !(*this == rhs);
    }

    // function definitions.
    Coordinate::coordinate_type
    distance(const Coordinate& c1, const Coordinate& c2) {
        Coordinate::coordinate_type x, y, z;

        x = std::fabs(c1.x() - c2.x());
        y = std::fabs(c1.y() - c2.y());
        z = std::fabs(c1.z() - c2.z());

        return std::sqrt(std::pow(x, 2) + std::pow(y, 2) +
                         std::pow(z, 2));
    }

    std::ostream&
    operator<<(std::ostream& os, const Coordinate& co) {
        os << "(" << co.x() << "," << co.y() << ","
           << co.z() << ")";
        return os;
    }
}

#endif
