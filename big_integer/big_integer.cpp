#include "big_integer.hpp"

BigInt::BigInt() {}

BigInt::BigInt(int64_t big) {
  is_negative_ = false;
  size_ = 0;
  uint64_t bigint = llabs(big);
  if (big == -INT64_MIN) {
    bigint = -INT64_MIN;
  }
  if (big < 0) {
    is_negative_ = true;
  }
  int itr = 0;
  if (bigint != 0) {
    ++size_;
  }
  biginteger_.push_back(0);
  while (bigint / kSystcomp > 0) {
    biginteger_.push_back(0);
    ++size_;
    biginteger_[itr + 1] = bigint / kSystcomp;
    biginteger_[itr] = bigint % kSystcomp;
    bigint /= kSystcomp;
    ++itr;
  }
  biginteger_[itr] = bigint % kSystcomp;
}

BigInt::BigInt(std::string bigstring) {
  is_negative_ = false;
  if (bigstring[0] == '-') {
    is_negative_ = true;
  }
  int minus = static_cast<int>(is_negative_);
  int pointer = bigstring.size();
  int iterator = 0;
  size_ = 0;
  while (pointer - kCellsize - minus > 0) {
    biginteger_.push_back(
        stoi(bigstring.substr(pointer - kCellsize, kCellsize)));
    ++iterator;
    if (stoi(bigstring.substr(pointer - kCellsize, kCellsize)) != 0) {
      size_ = iterator;
    }
    pointer -= kCellsize;
  }
  if (stoi(bigstring.substr(minus, pointer)) != 0) {
    biginteger_.push_back(stoi(bigstring.substr(minus, pointer - minus)));
    size_ = ++iterator;
  }
}

std::vector<int64_t>& BigInt::Data() { return biginteger_; }

std::vector<int64_t> BigInt::Data() const { return biginteger_; }

int& BigInt::Size() { return size_; }

int BigInt::Size() const { return size_; }

bool& BigInt::Negative() { return is_negative_; }

bool BigInt::Negative() const { return is_negative_; }

void BigInt::Swap(BigInt& big) {
  std::vector<int64_t> tmp1 = biginteger_;
  int tmp2 = size_;
  bool tmp3 = is_negative_;
  biginteger_ = big.biginteger_;
  size_ = big.size_;
  is_negative_ = big.is_negative_;
  big.biginteger_ = tmp1;
  big.size_ = tmp2;
  big.is_negative_ = tmp3;
}

std::ostream& operator<<(std::ostream& ostream, const BigInt& big) {
  const int kCellsize = 9;
  if (big.Size() == 0) {
    ostream << 0;
    return ostream;
  }
  if (big.Negative()) {
    ostream << '-';
  }
  ostream << big.Data()[big.Size() - 1];
  for (int i = big.Size() - 2; i >= 0; --i) {
    std::string insstr = std::to_string(big.Data()[i]);
    std::string strout(kCellsize - insstr.size(), '0');
    strout += insstr;
    ostream << strout;
  }
  return ostream;
}

BigInt::BigInt(const BigInt& copy) {
  biginteger_ = copy.biginteger_;
  size_ = copy.size_;
  is_negative_ = copy.is_negative_;
}

BigInt& BigInt::operator=(const BigInt& copy) {
  if (this == &copy) {
    return *this;
  }
  biginteger_ = copy.biginteger_;
  is_negative_ = copy.is_negative_;
  size_ = copy.size_;
  return *this;
}

bool BigInt::operator==(const BigInt& big) const {
  if (size_ == 0 && big.size_ == 0) {
    return true;
  }
  if (size_ != big.size_) {
    return false;
  }
  if (is_negative_ != big.is_negative_) {
    return false;
  }
  for (int i = 0; i < size_; ++i) {
    if (biginteger_[i] != big.biginteger_[i]) {
      return false;
    }
  }
  return true;
}

BigInt Abs(const BigInt& big) {
  BigInt bigabs = big;
  bigabs.Negative() = false;
  return bigabs;
}

bool BigInt::operator<(const BigInt& big) const {
  if (size_ == 0 || big.size_ == 0) {
    return BigInt::Zerobool(*this, big);
  }
  if (*this == big) {
    return false;
  }
  if (is_negative_ != big.is_negative_) {
    return is_negative_;
  }
  if (size_ < big.size_) {
    return true;
  }
  if (size_ > big.size_) {
    return false;
  }
  if (!is_negative_) {
    for (int i = big.Size() - 1; i >= 0; --i) {
      if (biginteger_[i] != big.biginteger_[i]) {
        return (biginteger_[i] < big.biginteger_[i]);
      }
    }
    return true;
  }
  for (int i = Size() - 1; i >= 0; --i) {
    if (biginteger_[i] != big.biginteger_[i]) {
      return (biginteger_[i] > big.biginteger_[i]);
    }
  }
  return false;
}

bool BigInt::Zerobool(const BigInt& left, const BigInt& right) {
  if ((left.Size() == 0) && (right.Size() == 0)) {
    return false;
  }
  if ((left.Size() == 0) && (right.Negative()) && (right.Size() != 0)) {
    return false;
  }
  if ((left.Size() == 0) && !(right.Negative()) && (right.Size() != 0)) {
    return true;
  }
  if ((right.Size() == 0) && (left.Negative()) && (left.Size() != 0)) {
    return true;
  }
  if ((right.Size() == 0) && !(left.Negative()) && (left.Size() != 0)) {
    return false;
  }
  return false;
}

bool BigInt::operator<=(const BigInt& big) const { return !(*this > big); }

bool BigInt::operator>(const BigInt& big) const {
  return !(*this < big || *this == big);
}

bool BigInt::operator>=(const BigInt& big) const {
  return !(*this > big || *this == big);
}

bool BigInt::operator!=(const BigInt& big) const { return !(*this == big); }

std::istream& operator>>(std::istream& istream, BigInt& big) {
  std::string strint;
  istream >> strint;
  big = strint;
  return istream;
}

BigInt& BigInt::operator+=(const BigInt& big) {
  BigInt bigcopy = big;
  if (is_negative_ == big.is_negative_) {
    if (this->Size() < big.Size()) {
      this->Swap(bigcopy);
    }
    biginteger_.push_back(0);
    for (int i = 0; i < (this->Size() - bigcopy.Size() + 1); ++i) {
      bigcopy.Data().push_back(0);
    }
    for (int i = 0; i < this->Size(); ++i) {
      biginteger_[i + 1] +=
          (biginteger_[i] + bigcopy.biginteger_[i]) / kSystcomp;
      biginteger_[i] = (biginteger_[i] + bigcopy.biginteger_[i]) % kSystcomp;
    }
    if (biginteger_[Size()] == 0) {
      biginteger_.pop_back();
    } else {
      ++size_;
    }
    return *this;
  }
  *this = MinusPart(*this, bigcopy);
  return *this;
}

BigInt BigInt::MinusPart(BigInt left, BigInt bigcopy) const {
  if (Abs(left) < Abs(bigcopy)) {
    left.Swap(bigcopy);
  }
  for (int i = 0; i < (left.Size() - bigcopy.Size()); ++i) {
    bigcopy.Data().push_back(0);
  }
  for (int i = 0; i < left.Size(); ++i) {
    if (left.biginteger_[i] < bigcopy.biginteger_[i]) {
      --left.biginteger_[i + 1];
      left.biginteger_[i] += kSystcomp;
    }
    left.biginteger_[i] -= bigcopy.biginteger_[i];
  }
  for (int i = left.Size() - 1; i >= 0; --i) {
    if (left.biginteger_[i] == 0) {
      --left.size_;
    } else {
      break;
    }
  }
  return left;
}

BigInt& BigInt::operator-=(const BigInt& big) {
  *this += -big;
  return *this;
}

BigInt& BigInt::operator*=(const BigInt& big) {
  BigInt multiply = 0;
  multiply.is_negative_ = false;
  multiply.size_ = size_ + big.size_;
  for (int i = 0; i < multiply.size_ + 2; ++i) {
    multiply.biginteger_.push_back(0);
  }
  for (int i = 0; i < size_; ++i) {
    for (int j = 0; j < big.size_; ++j) {
      multiply.biginteger_[i + j] +=
          (biginteger_[i] * big.biginteger_[j]) % kSystcomp;
      multiply.biginteger_[i + j + 1] +=
          (biginteger_[i] * big.biginteger_[j]) / kSystcomp;
      if (multiply.biginteger_[i + j] >= kSystcomp) {
        multiply.biginteger_[i + j + 1] +=
            multiply.biginteger_[i + j] / kSystcomp;
        multiply.biginteger_[i + j] = multiply.biginteger_[i + j] % kSystcomp;
      }
      if (multiply.biginteger_[i + j + 1] >= kSystcomp) {
        multiply.biginteger_[i + j + 2] +=
            multiply.biginteger_[i + j + 1] / kSystcomp;
        multiply.biginteger_[i + j + 1] =
            multiply.biginteger_[i + j + 1] % kSystcomp;
      }
    }
  }
  *this = Multiply(multiply, big);
  return *this;
}

BigInt BigInt::Multiply(BigInt multiply, const BigInt& big) const {
  for (int i = multiply.size_ - 1; i >= 0; --i) {
    if (multiply.biginteger_[i] == 0) {
      --multiply.size_;
    } else {
      break;
    }
  }
  multiply.is_negative_ = is_negative_ != big.is_negative_;
  return multiply;
}

BigInt operator*(const BigInt& lvalue, const BigInt& rvalue) {
  BigInt multiply = lvalue;
  multiply *= rvalue;
  return multiply;
}

BigInt operator+(const BigInt& lvalue, const BigInt& rvalue) {
  BigInt multiply = lvalue;
  multiply += rvalue;
  return multiply;
}

BigInt operator/(const BigInt& lvalue, const BigInt& rvalue) {
  BigInt multiply = lvalue;
  multiply /= rvalue;
  return multiply;
}

BigInt operator-(const BigInt& lvalue, const BigInt& rvalue) {
  BigInt multiply = lvalue;
  multiply -= rvalue;
  return multiply;
}

BigInt operator%(const BigInt& lvalue, const BigInt& rvalue) {
  BigInt multiply = lvalue;
  multiply %= rvalue;
  return multiply;
}

void BigInt::DivisionByTwo() {
  for (int i = Size() - 1; i > 0; --i) {
    int time = biginteger_[i];
    biginteger_[i] /= 2;
    if ((time % 2) == 1) {
      biginteger_[i - 1] += kSystcomp;
    }
  }
  biginteger_[0] = biginteger_[0] / 2;
}

BigInt& BigInt::operator++() {
  *this += 1;
  return *this;
}

BigInt& BigInt::operator--() {
  *this -= 1;
  return *this;
}

BigInt BigInt::operator--(int) {
  BigInt old = *this;
  operator--();
  return old;
}

BigInt BigInt::operator++(int) {
  BigInt old = *this;
  operator++();
  return old;
}

BigInt BigInt::operator-() const {
  BigInt minus = *this;
  minus.is_negative_ = !minus.is_negative_;
  return minus;
}

BigInt& BigInt::operator/=(const BigInt& big) {
  if (Abs(*this) < Abs(big)) {
    *this = 0;
    return *this;
  }
  BigInt right = 1;
  BigInt left = 1;
  BigInt middle = 1;
  while ((Abs(big) * right) <= Abs(*this)) {
    left = right;
    right *= 2;
  }
  while (((Abs(big) * (left + 1)) < Abs(*this))) {
    middle = (left + right);
    middle.DivisionByTwo();
    if ((Abs(big) * middle) <= Abs(*this)) {
      left = middle;
    } else {
      right = middle;
    }
  }
  *this = BigInt::Check(big, left, *this);
  return *this;
}

BigInt BigInt::Check(BigInt big, BigInt left, BigInt ans) {
  if (((Abs(big) * (left + 1)) <= Abs(ans))) {
    ++left;
  }
  if (ans.Negative() != big.Negative()) {
    left.Negative() = true;
  }
  if (left.Data()[left.Size() - 1] == 0) {
    --left.Size();
  }
  return left;
}

BigInt& BigInt::operator%=(const BigInt& big) {
  *this = (*this - (*this / big) * big);
  for (int i = size_ - 1; i >= 0; --i) {
    if (biginteger_[i] == 0) {
      --size_;
    } else {
      break;
    }
  }
  return *this;
}