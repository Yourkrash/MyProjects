#include "string.hpp"
void String::Swap(String& other) {
  int tmp1 = size_;
  int tmp2 = capacity_;
  char* tmp3 = str_;
  size_ = other.size_;
  capacity_ = other.capacity_;
  str_ = other.str_;
  other.size_ = tmp1;
  other.capacity_ = tmp2;
  other.str_ = tmp3;
}

void String::NullTerminate() { str_[size_] = '\0'; }

String::String() : size_(0), capacity_(0), str_(nullptr) {}

String::String(int size, char character)
    : size_(size), capacity_(2 * size), str_(new char[2 * size + 1]) {
  memset(str_, character, size_);
  NullTerminate();
}

String::String(const char* conststr) : size_(strlen(conststr)) {
  capacity_ = 2 * size_;
  str_ = new char[capacity_ + 1];
  memcpy(str_, conststr, size_);
  NullTerminate();
}
String::~String() { delete[] str_; }
String::String(const String& string) : size_(string.size_) {
  capacity_ = 2 * string.size_;
  str_ = new char[capacity_ + 1];
  if ((string.str_ != nullptr) && (str_ != nullptr)) {
    memcpy(str_, string.str_, size_ + 1);
  }
}

String& String::operator=(const String& string) {
  if (this == &string) {
    return *this;
  }
  String str(string);
  this->Swap(str);
  return *this;
}

void String::Clear() { size_ = 0; }

void String::PushBack(char character) {
  ++size_;
  if (size_ > capacity_) {
    Reserve(2 * size_);
  }
  str_[size_ - 1] = character;
  NullTerminate();
}

void String::PopBack() {
  if (Empty()) {
    return;
  }
  --size_;
  NullTerminate();
}

void String::Resize(size_t new_size) {
  if (capacity_ < new_size) {
    Reserve(new_size);
  }
  size_ = new_size;
  NullTerminate();
}

void String::Resize(size_t new_size, char character) {
  if (capacity_ < new_size) {
    Reserve(new_size);
  }
  memset(str_ + size_, character, capacity_ - size_);
  Resize(new_size);
}

char* String::Data() { return str_; }

const char* String::Data() const { return str_; }

void String::Reserve(size_t new_cap) {
  if (new_cap > capacity_) {
    capacity_ = new_cap;
    char* strcpy = str_;
    str_ = new char[capacity_ + 1];
    if (strcpy != nullptr) {
      memcpy(str_, strcpy, size_);
    }
    delete[] strcpy;
    NullTerminate();
  }
}

void String::ShrinkToFit() {
  if (capacity_ > size_) {
    capacity_ = size_;
    Reserve(capacity_);
  }
}

char& String::operator[](int ind) { return str_[ind]; }

const char& String::operator[](int ind) const { return str_[ind]; }

char& String::Front() { return str_[0]; }

char& String::Back() { return str_[size_ - 1]; }

char String::Front() const { return str_[0]; }

char String::Back() const { return str_[size_ - 1]; }

size_t String::Size() const { return size_; }

size_t String::Capacity() const { return capacity_; }

bool String::Empty() const { return size_ == 0; }

bool String::operator<(const String& string) const {
  return strcmp(this->str_, string.str_) < 0;
}

bool String::operator>(const String& string) const {
  return !(*this < string) && (*this != string);
}

bool String::operator<=(const String& string) const {
  return (*this < string) || (*this == string);
}
bool String::operator>=(const String& string) const {
  return !(*this < string);
}

bool String::operator==(const String& string) const {
  return strcmp(this->str_, string.str_) == 0;
}

bool String::operator!=(const String& string) const {
  return !(*this == string);
}

String& String::operator+=(const String& string) {
  if (capacity_ < size_ + string.size_) {
    Reserve(2 * (size_ + string.size_));
  }
  memcpy(str_ + size_, string.str_, string.size_);
  size_ += string.size_;
  NullTerminate();
  return *this;
}

String& String::operator*=(size_t n) {
  if (capacity_ <= n * size_) {
    Reserve(2 * n * size_ + 1);
  }
  for (size_t i = 0; i < n * size_; ++i) {
    str_[i] = str_[i % size_];
  }
  size_ *= n;
  if (str_ != nullptr) {
    NullTerminate();
  }
  return *this;
}

std::ostream& operator<<(std::ostream& ostream, const String& string) {
  ostream << string.Data();
  return ostream;
}

std::istream& operator>>(std::istream& istream, String& string) {
  string.Clear();
  char character;
  while ((istream.get(character)) && !(istream.eof())) {
    string.PushBack(character);
  }
  return istream;
}

std::vector<String> String::Split(const String& delimiter) {
  std::vector<String> result;
  int begin = 0;
  char* ptr = strstr(str_ + begin, delimiter.str_);
  while (ptr != nullptr) {
    String string = "";
    int s_size = ptr - str_ - begin;
    string.Reserve(s_size);
    string.size_ = s_size;
    memcpy(string.str_, (str_ + begin), s_size);
    begin += delimiter.size_ + s_size;
    ptr = strstr(str_ + begin, delimiter.str_);
    if (string.size_ != 0) {
      string.NullTerminate();
    }
    result.push_back(string.str_);
  }
  result.push_back(str_ + begin);
  return result;
}

String String::Join(const std::vector<String>& strings) const {
  int count = strings.size();
  int sizesum = 0;
  for (int i = 0; i < count; ++i) {
    sizesum += strings[i].Size() + size_;
  }
  sizesum -= size_;
  String joinstr = "";
  if (!strings.empty()) {
    joinstr.Reserve(sizesum);
    joinstr = strings[0];
    for (int i = 1; i < count; ++i) {
      joinstr += *this;
      joinstr += strings[i];
    }
  }
  return joinstr;
}

String operator+(const String& string_one, const String& string_two) {
  String sum = string_one;
  sum += string_two;
  return sum;
}
String operator*(const String& string, int n) {
  String multi = string;
  multi *= n;
  return multi;
}