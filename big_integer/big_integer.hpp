#pragma once
#include <iostream>
#include <string>
#include <vector>

class BigInt {
 private:
  std::vector<int64_t> biginteger_;
  int size_;
  bool is_negative_;
  const long long kSystcomp = 1000000000;
  const int kCellsize = 9;
  void DivisionByTwo();
  BigInt MinusPart(BigInt left, BigInt bigcopy) const;
  BigInt Multiply(BigInt multiply, const BigInt& big) const;
  static bool Zerobool(const BigInt& left, const BigInt& right);
  static BigInt Check(BigInt big, BigInt left, BigInt ans);

 public:
  BigInt();
  BigInt(std::string bigstring);
  BigInt(int64_t intbig);
  BigInt(const BigInt& copy);
  std::vector<int64_t>& Data();
  std::vector<int64_t> Data() const;
  int& Size();
  int Size() const;
  bool& Negative();
  bool Negative() const;
  BigInt& operator=(const BigInt& copy);
  bool operator==(const BigInt& big) const;
  bool operator<(const BigInt& big) const;
  bool operator<=(const BigInt& big) const;
  bool operator>(const BigInt& big) const;
  bool operator>=(const BigInt& big) const;
  bool operator!=(const BigInt& big) const;
  BigInt& operator+=(const BigInt& big);
  BigInt& operator-=(const BigInt& big);
  BigInt& operator*=(const BigInt& big);
  BigInt& operator/=(const BigInt& big);
  BigInt& operator%=(const BigInt& big);
  BigInt operator-() const;
  BigInt& operator++();
  BigInt operator++(int);
  BigInt& operator--();
  BigInt operator--(int);
  void Swap(BigInt& big);
};

BigInt operator*(const BigInt& lvalue, const BigInt& rvalue);
BigInt operator+(const BigInt& lvalue, const BigInt& rvalue);
BigInt operator/(const BigInt& lvalue, const BigInt& rvalue);
BigInt operator-(const BigInt& lvalue, const BigInt& rvalue);
BigInt operator%(const BigInt& lvalue, const BigInt& rvalue);
BigInt Abs(const BigInt& big);
std::ostream& operator<<(std::ostream& ostream, const BigInt& big);
std::istream& operator>>(std::istream& istream, BigInt& big);