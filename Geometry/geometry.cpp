#include "geometry.hpp"

bool Check(int64_t x1crd, int64_t x2crd, int64_t x3crd, int64_t x4crd) {
  if (x1crd > x2crd) {
    std::swap(x1crd, x2crd);
  }
  if (x3crd > x4crd) {
    std::swap(x3crd, x4crd);
  }
  return std::max(x1crd, x3crd) <= std::min(x2crd, x4crd);
}

bool RayCheck(Vector vec_a, Vector vec_b, Vector vector) {
  if ((((vec_a ^ vector) == 0) && ((vec_a * vector) < 0)) &&
      (vec_b ^ vector) != 0) {
    return false;
  }
  if ((((vec_b ^ vector) == 0) && ((vec_b * vector) < 0)) &&
      (vec_a ^ vector) != 0) {
    return false;
  }
  return !((((vec_a ^ vector) == 0) && ((vec_a * vector) < 0)) &&
           (((vec_b ^ vector) == 0) && ((vec_b * vector) < 0)));
}

Vector::Vector() : xcrd_(0), ycrd_(0) {}

Vector::~Vector() {}

Vector::Vector(const int64_t& xcrd, const int64_t& ycrd)
    : xcrd_(xcrd), ycrd_(ycrd) {}

Vector::Vector(const Vector& vctr) noexcept
    : xcrd_(vctr.xcrd_), ycrd_(vctr.ycrd_) {}

int64_t& Vector::GetX() { return xcrd_; }

int64_t& Vector::GetY() { return ycrd_; }

const int64_t& Vector::GetX() const { return xcrd_; }

const int64_t& Vector::GetY() const { return ycrd_; }

int64_t operator*(const Vector& vctr1, const Vector& vctr2) {
  int64_t multy = vctr1.GetX() * vctr2.GetX() + vctr1.GetY() * vctr2.GetY();
  return multy;
}

int64_t operator^(const Vector& vctr1, const Vector& vctr2) {
  int64_t multy = vctr1.GetX() * vctr2.GetY() - vctr1.GetY() * vctr2.GetX();
  return multy;
}

Vector& Vector::operator=(const Vector& vctr) {
  xcrd_ = vctr.xcrd_;
  ycrd_ = vctr.ycrd_;
  return *this;
}

Vector& Vector::operator+=(const Vector& vctr) {
  xcrd_ += vctr.xcrd_;
  ycrd_ += vctr.ycrd_;
  return *this;
}

Vector& Vector::operator-=(const Vector& vctr) {
  xcrd_ -= vctr.xcrd_;
  ycrd_ -= vctr.ycrd_;
  return *this;
}

Vector operator+(const Vector& vctr1, const Vector& vctr2) {
  Vector sum = vctr1;
  sum += vctr2;
  return sum;
}

Vector operator-(const Vector& vctr1, const Vector& vctr2) {
  Vector sum = vctr1;
  sum -= vctr2;
  return sum;
}

Vector& Vector::operator*=(const int64_t& num) {
  xcrd_ *= num;
  ycrd_ *= num;
  return *this;
}

Vector operator*(const int64_t& num, const Vector& vctr) {
  Vector sum = vctr;
  sum *= num;
  return sum;
}

Vector& Vector::operator-() {
  xcrd_ = -xcrd_;
  ycrd_ = -ycrd_;
  return *this;
}

Point::Point(const int64_t& xpnt, const int64_t& ypnt)
    : xpnt_(xpnt), ypnt_(ypnt) {}

Point::Point(const Point& point) : xpnt_(point.xpnt_), ypnt_(point.ypnt_) {}

int64_t& Point::GetX() { return xpnt_; }

int64_t& Point::GetY() { return ypnt_; }

const int64_t& Point::GetX() const { return xpnt_; }

const int64_t& Point::GetY() const { return ypnt_; }

void Point::Move(const Vector& vcrt) {
  xpnt_ += vcrt.GetX();
  ypnt_ += vcrt.GetY();
}

bool Point::ContainsPoint(const Point& point) const {
  return (xpnt_ == point.GetX()) && (ypnt_ == point.GetY());
}

bool Point::CrossSegment(const Segment& segm) const {
  return segm.ContainsPoint(*this);
}

IShape* Point::Clone() { return new Point(*this); }

Vector operator-(const Point& point1, const Point& point2) {
  return Vector(point1.GetX() - point2.GetX(), point1.GetY() - point2.GetY());
}

Segment::Segment(const Point& point1, const Point& point2)
    : apnt_(point1), bpnt_(point2) {}

Segment::Segment(const Segment& segm)
    : apnt_(segm.GetA()), bpnt_(segm.GetB()) {}

Point& Segment::GetA() { return apnt_; }

Point& Segment::GetB() { return bpnt_; }

const Point& Segment::GetA() const { return apnt_; }

const Point& Segment::GetB() const { return bpnt_; }

void Segment::Move(const Vector& vcrt) {
  apnt_.Move(vcrt);
  bpnt_.Move(vcrt);
}

bool Segment::ContainsPoint(const Point& point) const {
  return (((point - apnt_) ^ (bpnt_ - apnt_)) == 0) &&
         (((point - apnt_) * (bpnt_ - apnt_)) >= 0) &&
         ((point - apnt_) * (point - apnt_) <=
          (bpnt_ - apnt_) * (bpnt_ - apnt_));
}

bool Segment::CrossSegment(const Segment& segm) const {
  return Check(this->GetA().GetX(), this->GetB().GetX(), segm.GetA().GetX(),
               segm.GetB().GetX()) &&
         Check(this->GetA().GetY(), this->GetB().GetY(), segm.GetA().GetY(),
               segm.GetB().GetY()) &&
         ((((segm.GetA() - this->GetA()) ^ (this->GetB() - this->GetA())) *
           ((segm.GetB() - this->GetA()) ^ (this->GetB() - this->GetA()))) <=
          0) &&
         ((((this->GetA() - segm.GetA()) ^ (segm.GetB() - segm.GetA())) *
           ((this->GetB() - segm.GetA()) ^ (segm.GetB() - segm.GetA()))) <= 0);
}

IShape* Segment::Clone() { return new Segment(*this); }

Line::Line(const Point& point1, const Point& point2)
    : axc_(-(point2 - point1).GetY()), byc_((point2 - point1).GetX()) {
  con_ = point1.GetX() * point2.GetY() - point2.GetX() * point1.GetY();
}
int64_t& Line::GetA() { return axc_; }
int64_t& Line::GetB() { return byc_; }
int64_t& Line::GetC() { return con_; }
const int64_t& Line::GetA() const { return axc_; }
const int64_t& Line::GetB() const { return byc_; }
const int64_t& Line::GetC() const { return con_; }
void Line::Move(const Vector& vcrt) {
  con_ -= axc_ * vcrt.GetX() + byc_ * vcrt.GetY();
}

bool Line::ContainsPoint(const Point& point) const {
  return axc_ * point.GetX() + byc_ * point.GetY() + con_ == 0;
}

bool Line::CrossSegment(const Segment& segm) const {
  int64_t ptr1 = axc_ * segm.GetA().GetX() + byc_ * segm.GetA().GetY() + con_;
  int64_t ptr2 = axc_ * segm.GetB().GetX() + byc_ * segm.GetB().GetY() + con_;
  return ((ptr1 <= 0) && (ptr2 >= 0)) || ((ptr1 >= 0) && (ptr2 <= 0));
}

Line::Line(const Line& line)
    : axc_(line.GetA()), byc_(line.GetB()), con_(line.GetC()) {}

IShape* Line::Clone() { return new Line(*this); }

Ray::Ray(const Point& point1, const Point& point2)
    : start_(point1), vector_(point2 - point1) {}
Ray::Ray(const Ray& ray) : start_(ray.GetA()), vector_(ray.GetVector()) {}

Point& Ray::GetA() { return start_; }
Vector& Ray::GetVector() { return vector_; }
const Point& Ray::GetA() const { return start_; }
const Vector& Ray::GetVector() const { return vector_; }
void Ray::Move(const Vector& vcrt) { start_.Move(vcrt); }
bool Ray::ContainsPoint(const Point& point) const {
  return ((vector_ ^ (point - start_)) == 0) &&
         (vector_ * (point - start_) >= 0);
}

bool Ray::CrossSegment(const Segment& segm) const {
  Vector vec_a = segm.GetA() - start_;
  Vector vec_b = segm.GetB() - start_;
  if (((vec_a.GetX() == 0) && (vec_a.GetY() == 0)) ||
      ((vec_b.GetX() == 0) && (vec_b.GetY() == 0))) {
    return true;
  }
  return RayCheck(vec_a, vec_b, vector_) &&
         ((((vec_a ^ vector_) * (vec_b ^ vector_)) <= 0)) &&
         (((vector_ ^ vec_a) * (vec_b ^ vec_a) >= 0));
}

IShape* Ray::Clone() { return new Ray(*this); }

Circle::Circle(const Point& centr, const int64_t& rad)
    : centre_(centr), radius_(rad) {}

Circle::Circle(const Circle& circ)
    : centre_(circ.GetCentre()), radius_(circ.GetRadius()) {}

Point& Circle::GetCentre() { return centre_; }

int64_t& Circle::GetRadius() { return radius_; }

const Point& Circle::GetCentre() const { return centre_; }

const int64_t& Circle::GetRadius() const { return radius_; }

void Circle::Move(const Vector& vcrt) { centre_.Move(vcrt); }

bool Circle::ContainsPoint(const Point& point) const {
  return (centre_.GetX() - point.GetX()) * (centre_.GetX() - point.GetX()) +
             (centre_.GetY() - point.GetY()) *
                 (centre_.GetY() - point.GetY()) <=
         radius_ * radius_;
}

bool Circle::ContainsPointNoCircle(const Point& point) const {
  return (centre_.GetX() - point.GetX()) * (centre_.GetX() - point.GetX()) +
             (centre_.GetY() - point.GetY()) * (centre_.GetY() - point.GetY()) <
         radius_ * radius_;
}

bool Circle::CrossSegment(const Segment& segm) const {
  Point point1 = segm.GetA();
  Point point2 = segm.GetB();
  if (ContainsPointNoCircle(point1) && ContainsPointNoCircle(point2)) {
    return false;
  }
  if (ContainsPoint(point1) ^ ContainsPoint(point2)) {
    return true;
  }
  Vector vectrsegm = segm.GetB() - segm.GetA();
  int64_t cofa = -(segm.GetB() - segm.GetA()).GetY();
  int64_t cofb = (segm.GetB() - segm.GetA()).GetX();
  int64_t dist = (cofa * (centre_.GetX() - point1.GetX()) +
                  cofb * (centre_.GetY() - point1.GetY()));
  bool is_dist =
      (dist * dist <= radius_ * radius_ * (cofa * cofa + cofb * cofb));
  Vector normaly(-vectrsegm.GetY(), vectrsegm.GetX());
  return is_dist && ((((centre_ - segm.GetB()) ^ (normaly)) *
                      ((centre_ - segm.GetA()) ^ (normaly))) <= 0);
}

IShape* Circle::Clone() { return new Circle(*this); }