#pragma once

#include <cstring>
#include <iostream>
#include <vector>

class String {
 private:
  char* str_;
  size_t size_;
  size_t capacity_;

 public:
  void Swap(String& other);
  explicit String();
  explicit String(int size, char character);
  String(const char* conststr);
  String(const String& string);
  ~String();
  String& operator=(const String& string);
  void Clear();
  void PushBack(char character);
  void PopBack();
  void Resize(size_t new_size);
  void Resize(size_t new_size, char character);
  char* Data();
  const char* Data() const;
  void Reserve(size_t new_cap);
  void ShrinkToFit();
  char& operator[](int ind);
  const char& operator[](int ind) const;
  char& Front();
  char& Back();
  char Front() const;
  char Back() const;
  bool operator<(const String& string) const;
  bool operator>(const String& string) const;
  bool operator<=(const String& string) const;
  bool operator>=(const String& string) const;
  bool operator==(const String& string) const;
  bool operator!=(const String& string) const;
  String& operator+=(const String& string);
  String& operator*=(size_t n);
  std::vector<String> Split(const String& delimiter = " ");
  String Join(const std::vector<String>& strings) const;
  size_t Size() const;
  size_t Capacity() const;
  bool Empty() const;
  void NullTerminate();
};
String operator*(const String& string, int n);
String operator+(const String& string_one, const String& string_two);
std::ostream& operator<<(std::ostream& ostream, const String& string);
std::istream& operator>>(std::istream& istream, String& string);
