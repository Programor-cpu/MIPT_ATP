#define _USE_MATH_DEFINES
#include <stdlib.h>

#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>

namespace tools {
const double cDelta = 1e-10;

bool EqualDouble(double one, double two) { return (abs(one - two) <= cDelta); }
}  // namespace tools
class Line;
struct Point {
  double x;
  double y;
  Point(double x, double y) : x(x), y(y) {}
  Point(const Point& one, const Point& two)
      : x(two.x - one.x), y(two.y - one.y) {}
  Point() : x(0), y(0) {}
  bool operator==(const Point& another) const {
    return (tools::EqualDouble(x, another.x) &&
            tools::EqualDouble(y, another.y));
  }
  bool operator!=(const Point& another) const { return (!(*this == another)); }
  Point& operator+=(const Point& another) {
    x += another.x;
    y += another.y;
    return *this;
  }
  Point operator+(const Point& another) const {
    Point inreturn = *this;
    inreturn += another;
    return inreturn;
  }
  Point& operator*=(double multi) {
    x *= multi;
    y *= multi;
    return *this;
  }
  Point operator*(double multi) const {
    Point inreturn = *this;
    inreturn *= multi;
    return inreturn;
  }
  Point& operator-=(const Point& another) {
    x -= another.x;
    y -= another.y;
    return *this;
  }
  Point operator-(const Point& another) const {
    Point inreturn = *this;
    inreturn -= another;
    return inreturn;
  }
  Point& operator/=(double multi) {
    x /= multi;
    y /= multi;
    return *this;
  }
  Point operator/(double multi) const {
    Point inreturn = *this;
    inreturn /= multi;
    return inreturn;
  }
  void RotatePoint(const Point& center, double angle) {
    angle *= (M_PI / 180);
    Point vect = Point(center, *this);
    double new_x = vect.x * cos(angle) - vect.y * sin(angle);
    double new_y = vect.x * sin(angle) + vect.y * cos(angle);
    vect.x = new_x;
    vect.y = new_y;
    x = new_x + center.x;
    y = new_y + center.y;
  }
  double Cross(const Point& one, const Point& two) {
    return (one.x * two.x + one.y * two.y);
  }
  void ReflectPointByPoint(const Point& center) {
    *this += Point(*this, center) * 2;
  }
  void ReflectPointByLine(const Line& axis);
  void ScalePoint(const Point& center, double coefficient) {
    Point vect(center, *this);
    vect *= coefficient;
    vect += center;
    *this = vect;
  }
};
double GetDistance(const Point& one, const Point& two) {
  return (sqrt((one.x - two.x) * (one.x - two.x) +
               (one.y - two.y) * (one.y - two.y)));
}
Point GetMid(const Point& one, const Point& two) {
  return Point((one.x + two.x) / 2, (one.y + two.y) / 2);
}
Point GetOrthogonal(const Point& another) {
  Point inreturn = another;
  std::swap(inreturn.x, inreturn.y);
  inreturn.y *= -1;
  return inreturn;
}
double Dot(const Point& one, const Point& two) {
  return (one.y * two.x - one.x * two.y);
}
double Angle(const Point& a, const Point& b, const Point& c) {
  Point vector_one(b, a);
  Point vector_two(b, c);
  return acos(((vector_one.x * vector_two.x + vector_one.y * vector_two.y) /
               (GetDistance(a, b) * GetDistance(c, b))));
}

// POINT END

class Line {
 private:
  Point first_ = Point(0, 0);
  Point second_ = Point(0, 0);

 public:
  Line() {}
  Line(const Point& one, const Point& two) : first_(one), second_(two) {}
  Line(double k, double b)
      : first_(0, b), second_(Point(cos(atan(k)), b + sin(atan(k)))) {}
  Line(const Point& one, double k)
      : first_(one), second_(one.x + cos(atan(k)), one.y + sin(atan(k))) {}
  bool operator==(const Line& another) const {
    return (tools::EqualDouble(
                (another.first_.x - first_.x) / (second_.x - first_.x),
                (another.first_.y - first_.y) / (second_.y - first_.y)) &&
            tools::EqualDouble(
                (another.second_.x - first_.x) / (second_.x - first_.x),
                (another.second_.y - first_.y) / (second_.y - first_.y)));
  }
  std::pair<Point, Point> points() const { return {first_, second_}; }
};
void Point::ReflectPointByLine(const Line& axis) {
  Point A(*this, axis.points().first);
  Point B(*this, axis.points().second);
  Point C(A, B);
  *this += ((A + C * ((Cross(A, A) - Cross(A, B)) / Cross(C, C))) * 2);
}

class Shape {
 public:
  virtual double perimeter() const = 0;
  virtual double area() const { return 0; }
  virtual bool containsPoint(const Point& point) const = 0;
  virtual bool isCongruentTo(const Shape& another) const = 0;
  virtual bool isSimilarTo(const Shape& another) const = 0;
  virtual bool operator==(const Shape& another) const = 0;
  virtual bool operator!=(const Shape& another) const = 0;
  virtual void rotate(const Point& center, double angle) = 0;
  virtual void reflect(const Point& center) = 0;
  virtual void reflect(const Line& axis) = 0;
  virtual void scale(const Point& center, double coefficient) = 0;
  virtual ~Shape() {}
};

class Ellipse : public Shape {
 protected:
  Point first_center_;
  Point second_center_;
  double distance_;
  long double a_;
  long double b_;

 public:
  Ellipse(const Point& one, const Point& two, double dist)
      : first_center_(one), second_center_(two), distance_(dist) {
    if (one != two) {
      a_ = GetDistance(GetMid(first_center_, second_center_), second_center_) +
           (distance_ - GetDistance(first_center_, second_center_)) / 2;
      b_ = GetDistance(GetMid(first_center_, second_center_), second_center_) *
           tan(acos(2 *
                    GetDistance(GetMid(first_center_, second_center_),
                                second_center_) /
                    distance_));
    } else {
      a_ = distance_ / 2;
      b_ = distance_ / 2;
    }
  }
  double perimeter() const final {
    double h = pow(a_ - b_, 2) / pow(a_ + b_, 2);
    return M_PI * (a_ + b_) * (1 + 3 * h / (10 + sqrt(4 - 3 * h)));
  }
  double area() const final { return M_PI * a_ * b_; }
  Point center() const { return GetMid(first_center_, second_center_); }
  std::pair<Point, Point> focuses() const {
    return {first_center_, second_center_};
  }
  std::pair<Line, Line> directrices() const {
    Point ed = Point(first_center_, second_center_) /
               GetDistance(first_center_, second_center_);
    Point dir_first_one =
        GetMid(first_center_, second_center_) + ed * a_ / eccentricity();
    Point dir_second_one =
        GetMid(first_center_, second_center_) + (ed * -a_ / eccentricity());
    Point dir_first_two = dir_first_one + GetOrthogonal(ed);
    Point dir_second_two = dir_second_one + GetOrthogonal(ed);
    Line first_dir(dir_first_one, dir_first_two);
    Line second_dir(dir_second_one, dir_second_two);
    return {first_dir, second_dir};
  }
  double eccentricity() const { return sqrt(1 - b_ * b_ / (a_ * a_)); }
  bool containsPoint(const Point& point) const final {
    return (GetDistance(first_center_, point) +
                GetDistance(second_center_, point) <=
            distance_);
  }
  void rotate(const Point& center, double angle) final {
    first_center_.RotatePoint(center, angle);
    second_center_.RotatePoint(center, angle);
  }
  void reflect(const Point& center) final {
    first_center_.ReflectPointByPoint(center);
    second_center_.ReflectPointByPoint(center);
  }
  void reflect(const Line& axis) final {
    first_center_.ReflectPointByLine(axis);
    second_center_.ReflectPointByLine(axis);
  }
  void scale(const Point& center, double coefficient) final {
    first_center_.ScalePoint(center, coefficient);
    second_center_.ScalePoint(center, coefficient);
    distance_ *= (abs(coefficient));
    a_ *= (abs(coefficient));
    b_ *= (abs(coefficient));
  }
  bool isCongruentTo(const Shape& another) const final {
    const Ellipse* another_ellipse = dynamic_cast<const Ellipse*>(&another);
    if (another_ellipse != nullptr) {
      if (isSimilarTo(another) &&
          tools::EqualDouble(distance_, another_ellipse->distance_)) {
        return true;
      }
    }
    return false;
  }
  bool isSimilarTo(const Shape& another) const final {
    const Ellipse* another_ellipse = dynamic_cast<const Ellipse*>(&another);
    if (another_ellipse) {
      if (tools::EqualDouble(GetDistance(first_center_, second_center_) *
                                 another_ellipse->distance_,
                             GetDistance(another_ellipse->first_center_,
                                         another_ellipse->second_center_) *
                                 distance_)) {
        return true;
      }
    }
    return false;
  }
  bool operator==(const Shape& another) const final {
    const Ellipse* another_ellipse = dynamic_cast<const Ellipse*>(&another);
    if (another_ellipse) {
      if ((tools::EqualDouble(distance_, another_ellipse->distance_)) &&
          ((first_center_ == another_ellipse->first_center_ &&
            second_center_ == another_ellipse->second_center_) ||
           (first_center_ == another_ellipse->second_center_ &&
            second_center_ == another_ellipse->first_center_))) {
        return true;
      }
    }

    return false;
  }
  bool operator!=(const Shape& another) const final {
    return (!(*this == another));
  }
};

class Circle : public Ellipse {
 public:
  Circle(const Point& centre, double radius)
      : Ellipse(centre, centre, 2 * radius) {}
  double radius() const { return distance_ / 2; }
};

class Polygon : public Shape {
 protected:
  bool SimilarCheck(const std::vector<Point>& another_points) const {
    bool flag = false;
    for (size_t i = 0; i != points_.size(); i++) {
      size_t sign = 0;
      double k = 0;
      Point one(points_[(i - 1 + points_.size()) % points_.size()],
                points_[(i + points_.size()) % points_.size()]);
      Point two(points_[(i + points_.size()) % points_.size()],
                points_[(i + 1 + points_.size()) % points_.size()]);
      if (tools::EqualDouble(
              Angle(points_[(i - 1 + points_.size()) % points_.size()],
                    points_[(i + points_.size()) % points_.size()],
                    points_[(i + 1 + points_.size()) % points_.size()]),
              Angle(another_points[(-1 + points_.size()) % points_.size()],
                    another_points[(points_.size()) % points_.size()],
                    another_points[(1 + points_.size()) % points_.size()]))) {
        if (one.x * two.y - one.y * two.x > 0) {
          sign = 1;
        } else {
          sign = -1;
        }
        for (size_t j = 0; j != points_.size(); ++j) {
          Point vec_one(points_[(i + j - 1 + points_.size()) % points_.size()],
                        points_[(i + j + points_.size()) % points_.size()]);
          Point vec_two(points_[(i + j + points_.size()) % points_.size()],
                        points_[(i + j + 1 + points_.size()) % points_.size()]);
          double first_angle =
              Angle(points_[(i + j - 1 + points_.size()) % points_.size()],
                    points_[(i + j + points_.size()) % points_.size()],
                    points_[(i + j + 1 + points_.size()) % points_.size()]);
          double second_angle =
              Angle(another_points[(j - 1 + points_.size()) % points_.size()],
                    another_points[(j + points_.size()) % points_.size()],
                    another_points[(j + 1 + points_.size()) % points_.size()]);
          if (sign * (vec_one.x * vec_two.y - vec_one.y * vec_two.x) < 0) {
            first_angle = 2 * M_PI - first_angle;
            second_angle = 2 * M_PI - second_angle;
          }
          if (tools::EqualDouble(first_angle, second_angle)) {
            if (j == 0) {
              k = GetDistance(
                      points_[(i + j + points_.size()) % points_.size()],
                      points_[(i + j + 1 + points_.size()) % points_.size()]) /
                  GetDistance(
                      another_points[(j + points_.size()) % points_.size()],
                      another_points[(j + 1 + points_.size()) %
                                     points_.size()]);
            } else {
              if (tools::EqualDouble(
                      GetDistance(
                          points_[(i + j + points_.size()) % points_.size()],
                          points_[(i + j + 1 + points_.size()) %
                                  points_.size()]) /
                          GetDistance(another_points[(j + points_.size()) %
                                                     points_.size()],
                                      another_points[(j + 1 + points_.size()) %
                                                     points_.size()]),
                      k)) {
                if (j == points_.size() - 1) {
                  flag = true;
                  break;
                }
              } else {
                break;
              }
            }
          } else {
            break;
          }
        }
      }
      if (flag) {
        return true;
      }
    }
    return false;
  }
  bool CheckPoints(const std::vector<Point>& another_points) const {
    bool flag = false;
    for (size_t i = 0; i != points_.size(); ++i) {
      if (points_[i] == another_points[0]) {
        for (size_t j = 0; j != points_.size(); ++j) {
          if (points_[(i + j + points_.size()) % points_.size()] ==
              another_points[(j + points_.size()) % points_.size()]) {
            if (j == points_.size() - 1) {
              flag = true;
            }
          } else {
            break;
          }
        }
      }
      if (flag) {
        return true;
      }
    }
    return false;
  }
  std::vector<Point> points_;

 public:
  Polygon(const std::vector<Point>& points) : points_(points) {}
  template <typename... Args>
  explicit Polygon(const Args&... args) : points_({args...}) {}
  const std::vector<Point>& getVertices() const { return points_; }
  bool isConvex() const {
    double sign = 0;
    size_t n = points_.size();
    for (size_t i = 0; i != n; ++i) {
      Point one = points_[i] - points_[(i + n - 1) % n];
      Point two = points_[(i + 1 + n) % n] - points_[i];
      if (i == 0) {
        if (one.x * two.y - one.y * two.x > 0) {
          sign = 1;
        } else {
          sign = -1;
        }
      } else {
        if (sign * (one.x * two.y - one.y * two.x) < 0) {
          return false;
        }
      }
    }
    return true;
  }
  bool containsPoint(const Point& point) const final {
    bool inside = false;
    for (size_t i = 0; i < points_.size(); ++i) {
      Point one = points_[i];
      Point two = points_[(i + 1 + points_.size()) % points_.size()];
      bool check = (one.y > point.y) != (two.y > point.y);

      double intersect =
          (two.x - one.x) * (point.y - one.y) / (two.y - one.y) + one.x;
      if (check && point.x < intersect) {
        inside = !inside;
      }
    }
    return inside;
  }
  double perimeter() const final {
    double p = 0;
    for (size_t i = 0; i != points_.size(); ++i) {
      p += GetDistance(points_[i],
                       points_[(i + 1 + points_.size()) % points_.size()]);
    }
    return p;
  }
  double area() const final {
    double s = 0;
    for (size_t i = 0; i != points_.size(); ++i) {
      s += points_[i].x * points_[(i + 1 + points_.size()) % points_.size()].y;
      s -= points_[i].y * points_[(i + 1 + points_.size()) % points_.size()].x;
    }
    return 0.5 * abs(s);
  }
  void rotate(const Point& center, double angle) final {
    for (size_t i = 0; i != points_.size(); ++i) {
      points_[i].RotatePoint(center, angle);
    }
  }
  void reflect(const Point& center) final {
    for (size_t i = 0; i != points_.size(); ++i) {
      points_[i].ReflectPointByPoint(center);
    }
  }
  void reflect(const Line& axis) final {
    for (size_t i = 0; i != points_.size(); ++i) {
      points_[i].ReflectPointByLine(axis);
    }
  }
  void scale(const Point& center, double coefficient) final {
    for (size_t i = 0; i != points_.size(); ++i) {
      points_[i].ScalePoint(center, coefficient);
    }
  }
  bool isCongruentTo(const Shape& another) const final {
    const Polygon* another_polygon = dynamic_cast<const Polygon*>(&another);
    if (another_polygon != nullptr &&
        another_polygon->points_.size() == points_.size()) {
      if (isSimilarTo(another) &&
          tools::EqualDouble(perimeter(),
                             Polygon(another_polygon->points_).perimeter())) {
        return true;
      }
    }
    return false;
  }
  bool isSimilarTo(const Shape& another) const final {
    const Polygon* another_polygon = dynamic_cast<const Polygon*>(&another);
    if (another_polygon != nullptr &&
        another_polygon->points_.size() == points_.size()) {
      std::vector<Point> another_points = another_polygon->points_;
      if (!SimilarCheck(another_points)) {
        std::reverse(another_points.begin(), another_points.end());
        return SimilarCheck(another_points);
      }
      return true;
    }
    return false;
  }
  bool operator==(const Shape& another) const final {
    const Polygon* another_polygon = dynamic_cast<const Polygon*>(&another);
    if (another_polygon != nullptr &&
        another_polygon->points_.size() == points_.size()) {
      std::vector<Point> another_points = another_polygon->points_;
      if (!CheckPoints(another_points)) {
        std::reverse(another_points.begin(), another_points.end());
        return CheckPoints(another_points);
      }
      return true;
    }
    return false;
  }
  bool operator!=(const Shape& another) const final {
    return (!(*this == another));
  }
};
class Rectangle : public Polygon {
 private:
  std::vector<Point> HelpGenerate(const Point& one, const Point& two,
                                  double k) const {
    Point A = one;
    Point B = A;
    Point C = two;
    Point D = C;
    if (k > 1) {
      k = 1 / k;
    }
    Point center = GetMid(A, C);
    B.RotatePoint(center, (-2 * atan(k)) * 180 / M_PI);
    D.RotatePoint(center, (-2 * atan(k)) * 180 / M_PI);
    return {A, B, C, D};
  }

 public:
  Rectangle(const Point& one, const Point& two, double k)
      : Polygon(HelpGenerate(one, two, k)) {}
  Point center() const {
    Point A = points_[0];
    Point C = points_[2];
    return GetMid(A, C);
  }
  std::pair<Line, Line> diagonals() const {
    Point A = points_[0];
    Point B = points_[1];
    Point C = points_[2];
    Point D = points_[3];
    return {Line(A, C), Line(B, D)};
  }
};

class Square : public Rectangle {
 public:
  Square(const Point& one, const Point& two) : Rectangle(one, two, 1) {}
  Circle inscribedCircle() {
    Point A = points_[0];
    Point C = points_[2];
    return Circle(center(), GetDistance(A, C) / (2 * sqrt(2)));
  }
  Circle circumscribedCircle() {
    Point A = points_[0];
    Point C = points_[2];
    return Circle(center(), GetDistance(A, C) / 2);
  }
};

class Triangle : public Polygon {
 public:
  Triangle(const Point& A, const Point& B, const Point& C) : Polygon(A, B, C) {}
  Triangle(const std::vector<Point>& points) : Polygon(points) {}
  Circle inscribedCircle() const {
    Point A = points_[0];
    Point B = points_[1];
    Point C = points_[2];
    double a = GetDistance(B, C);
    double b = GetDistance(A, C);
    double c = GetDistance(A, B);
    double x = (a * A.x + b * B.x + c * C.x) / (a + b + c);
    double y = (a * A.y + b * B.y + c * C.y) / (a + b + c);
    return Circle(Point(x, y), 2 * area() / perimeter());
  }
  Circle circumscribedCircle() const {
    Point A = points_[0];
    Point B = points_[1];
    Point C = points_[2];
    double x = -(0.5) *
               (A.y * (B.x * B.x + B.y * B.y - C.x * C.x - C.y * C.y) +
                B.y * (C.x * C.x + C.y * C.y - A.x * A.x - A.y * A.y) +
                C.y * (A.x * A.x + A.y * A.y - B.x * B.x - B.y * B.y)) /
               (A.x * (B.y - C.y) + B.x * (C.y - A.y) + C.x * (A.y - B.y));
    double y = (0.5) *
               (A.x * (B.x * B.x + B.y * B.y - C.x * C.x - C.y * C.y) +
                B.x * (C.x * C.x + C.y * C.y - A.x * A.x - A.y * A.y) +
                C.x * (A.x * A.x + A.y * A.y - B.x * B.x - B.y * B.y)) /
               (A.x * (B.y - C.y) + B.x * (C.y - A.y) + C.x * (A.y - B.y));
    Point cent(x, y);
    double a = GetDistance(B, C);
    double b = GetDistance(A, C);
    double c = GetDistance(A, B);
    return Circle(Point(x, y), a * b * c / (4 * area()));
  }
  Point centroid() const {
    Point A = points_[0];
    Point B = points_[1];
    Point C = points_[2];
    return Point((A.x + B.x + C.x) / 3, (A.y + B.y + C.y) / 3);
  }
  Point orthocenter() const {
    Point A = points_[0];
    Point B = points_[1];
    Point C = points_[2];
    double tanA = tan(Angle(C, A, B));
    double tanB = tan(Angle(A, B, C));
    double tanC = tan(Angle(B, C, A));
    double x = (A.x * tanA + B.x * tanB + C.x * tanC) / (tanA + tanB + tanC);
    double y = (A.y * tanA + B.y * tanB + C.y * tanC) / (tanA + tanB + tanC);
    return Point(x, y);
  }
  Line EulerLine() const { return Line(centroid(), orthocenter()); }
  Circle ninePointsCircle() const {
    Point A = points_[0];
    Point B = points_[1];
    Point C = points_[2];
    return Triangle(GetMid(A, B), GetMid(B, C), GetMid(C, A))
        .circumscribedCircle();
  }
};