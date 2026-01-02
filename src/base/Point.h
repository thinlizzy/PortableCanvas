#ifndef PORTABLE_CANVAS_POINT_2024_01_26_H
#define PORTABLE_CANVAS_POINT_2024_01_26_H

namespace pc {

struct Point {
	int x,y;

	Point & operator+=(Point const & p) {
		this->x += p.x;
		this->y += p.y;
		return *this;
	}

	Point & operator-=(Point const & p) {
		this->x -= p.x;
		this->y -= p.y;
		return *this;
	}
};

constexpr Point operator+(Point const & p1, Point const & p2) {
	return Point{ .x = p1.x + p2.x, .y = p1.y + p2.y };
}

constexpr Point operator-(Point const & p1, Point const & p2) {
	return Point{ .x = p1.x - p2.x, .y = p1.y - p2.y };
}

constexpr Point operator*(Point const & p1, int m) {
	return Point{ .x = p1.x*m, .y = p1.y*m };
}

namespace literals {

constexpr Point operator""_x(unsigned long long v) {
	return Point{ .x = int(v) };
}

constexpr Point operator""_y(unsigned long long v) {
	return Point{ .y = int(v) };
}

}

}

#endif
