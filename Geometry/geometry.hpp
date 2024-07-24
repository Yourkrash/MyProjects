#include <iostream>

class Vector {
 private:
  int64_t xcrd_;
  int64_t ycrd_;

 public:
  Vector();
  Vector(const Vector& vctr) noexcept;
  Vector(const int64_t& xcrd, const int64_t& ycrd);
  ~Vector();
  int64_t& GetX();
  int64_t& GetY();
  const int64_t& GetX() const;
  const int64_t& GetY() const;
  Vector& operator=(const Vector& vctr);
  Vector& operator+=(const Vector& vctr);
  Vector& operator-=(const Vector& vctr);
  Vector& operator*=(const int64_t& num);
  Vector& operator-();
};

int64_t operator*(const Vector& vctr1, const Vector& vctr2);

int64_t operator^(const Vector& vctr1, const Vector& vctr2);

Vector operator+(const Vector& vctr1, const Vector& vctr2);

Vector operator-(const Vector& vctr1, const Vector& vctr2);

Vector operator*(const int64_t& num, const Vector& vctr);

Vector operator*(const Vector& vctr, const int64_t& num);

class Point;
class Segment;

class IShape {
 public:
  virtual void Move(const Vector& vcrt) = 0;
  virtual bool ContainsPoint(const Point& point) const = 0;
  virtual bool CrossSegment(const Segment& segm) const = 0;
  virtual IShape* Clone() = 0;
  virtual ~IShape() = default;
};

class Point : public IShape {
 private:
  int64_t xpnt_;
  int64_t ypnt_;

 public:
  Point(const int64_t& xpnt, const int64_t& ypnt);
  Point(const Point& point);
  int64_t& GetX();
  int64_t& GetY();
  const int64_t& GetX() const;
  const int64_t& GetY() const;
  void Move(const Vector& vcrt);
  bool ContainsPoint(const Point& point) const;
  bool CrossSegment(const Segment& segm) const;
  IShape* Clone();
};

Vector operator-(const Point& point1, const Point& point2);

class Segment : public IShape {
 private:
  Point apnt_;
  Point bpnt_;

 public:
  Segment(const Point& point1, const Point& point2);
  Segment(const Segment& segm);
  Point& GetA();
  Point& GetB();
  const Point& GetA() const;
  const Point& GetB() const;
  void Move(const Vector& vcrt);
  bool ContainsPoint(const Point& point) const;
  bool CrossSegment(const Segment& segm) const;
  IShape* Clone();
};

class Line : public IShape {
 private:
  int64_t axc_;
  int64_t byc_;
  int64_t con_;

 public:
  Line(const Point& point1, const Point& point2);
  Line(const Line& line);
  int64_t& GetA();
  int64_t& GetB();
  int64_t& GetC();
  const int64_t& GetA() const;
  const int64_t& GetB() const;
  const int64_t& GetC() const;
  void Move(const Vector& vcrt);
  bool ContainsPoint(const Point& point) const;
  bool CrossSegment(const Segment& segm) const;
  IShape* Clone();
};

class Ray : public IShape {
 private:
  Point start_;
  Vector vector_;

 public:
  Ray(const Point& point1, const Point& point2);
  Ray(const Ray& ray);
  Point& GetA();
  Vector& GetVector();
  const Point& GetA() const;
  const Vector& GetVector() const;
  void Move(const Vector& vcrt);
  bool ContainsPoint(const Point& point) const;
  bool CrossSegment(const Segment& segm) const;
  IShape* Clone();
};

class Circle : public IShape {
 private:
  Point centre_;
  int64_t radius_;

 public:
  Circle(const Point& centr, const int64_t& rad);
  Circle(const Circle& circ);
  Point& GetCentre();
  int64_t& GetRadius();
  const Point& GetCentre() const;
  const int64_t& GetRadius() const;
  void Move(const Vector& vcrt);
  bool ContainsPoint(const Point& point) const;
  bool ContainsPointNoCircle(const Point& point) const;
  bool CrossSegment(const Segment& segm) const;
  IShape* Clone();
};
