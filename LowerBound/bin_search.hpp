#pragma once

#include <iostream>

int* LowerBound(int* first, int* last, int value) {
  if (value > *(last - 1)) {
    return last;
  }
  int n = last - first;
  int middle;
  while (last - first > 1) {
    middle = (last - first) / 2;
    if (value < *(first + middle)) {
      last = last - middle;
    } else {
      first = first + middle;
    }
  }
  if (value > *first) {
    ++first;
  }
  return first;
}