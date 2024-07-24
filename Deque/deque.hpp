#include <cmath>
#include <iostream>
#include <iterator>
#include <memory>
#include <vector>

template <typename T, typename Alloc = std::allocator<T>>
class Deque {
 private:
  static constexpr size_t kChunkSize = 16;
  static constexpr size_t kNewChunks = 69;
  using alloc_traits = std::allocator_traits<Alloc>;
  using vector_alloc = typename alloc_traits::template rebind_alloc<T*>;
  using data = std::vector<T*, vector_alloc>;

  // class fields
  data data_;
  size_t size_;
  size_t first_chunk_;
  size_t first_element_;
  Alloc alloc_;

  // private methods
  void default_construct() {
    if (data_.empty()) {
      first_chunk_ = 1;
      data_.resize(3);
      data_[1] = alloc_traits::allocate(alloc_, kChunkSize);
    }
  }

  template <typename... Args>
  void try_construct(size_t ind, size_t chunk_count, Args&&... args) {
    try {
      alloc_traits::construct(alloc_, &(*this)[ind],
                              std::forward<Args>(args)...);
    } catch (...) {
      for (size_t j = 0; j < ind; ++j) {
        alloc_traits::destroy(alloc_, &((*this)[ind]));
      }
      for (size_t j = 0; j < chunk_count; ++j) {
        alloc_traits::deallocate(alloc_, data_[first_chunk_ + j], kChunkSize);
        data_[first_chunk_ + j] = nullptr;
      }
      throw;
    }
  }

 public:
  template <bool IsConst>
  class CommonIterator {
   private:
    Deque<T, Alloc>* deque_;
    size_t number_;
    size_t first_chunk_;
    size_t first_element_;

   public:
    using value_type = T;
    using type = std::conditional_t<IsConst, const T, T>;
    using iterator_category = std::random_access_iterator_tag;
    using difference_type = size_t;
    using pointer = type*;
    using reference = type&;

    std::pair<size_t, size_t> con_index() const {
      std::pair<size_t, size_t> index;
      index.first = first_chunk_ + (number_ + first_element_) / kChunkSize;
      index.second = (first_element_ + number_) % kChunkSize;
      return index;
    }

    CommonIterator(const size_t& index, Deque<T, Alloc>* deque)
        : number_(index),
          deque_(deque),
          first_chunk_(deque_->first_chunk_),
          first_element_(deque_->first_element_) {}

    CommonIterator(const CommonIterator& other)
        : deque_(other.deque_),
          number_(other.number_),
          first_chunk_(other.first_chunk_),
          first_element_(other.first_element_) {}

    CommonIterator& operator=(const CommonIterator& other) {
      deque_ = other.deque_;
      number_ = other.number_;
      first_chunk_ = other.first_chunk_;
      first_element_ = other.first_element_;
      return *this;
    }

    CommonIterator& operator++() {
      ++number_;
      return *this;
    }

    CommonIterator& operator--() {
      --number_;
      return *this;
    }

    CommonIterator operator--(int) {
      size_t copy = number_;
      --number_;
      return CommonIterator(copy, deque_);
    }

    CommonIterator operator++(int) {
      size_t copy = number_;
      ++number_;
      return CommonIterator(copy, deque_);
    }

    CommonIterator& operator+=(const difference_type& num) {
      number_ += num;
      return *this;
    }

    CommonIterator& operator-=(const difference_type& num) {
      number_ -= num;
      return *this;
    }

    CommonIterator operator-(const difference_type& num) const {
      CommonIterator tmp = *this;
      tmp.number_ -= num;
      return tmp;
    }

    CommonIterator operator+(const difference_type& num) const {
      CommonIterator tmp = *this;
      tmp.number_ += num;
      return tmp;
    }

    reference operator*() {
      std::pair<size_t, size_t> index = con_index();
      return deque_->data_[index.first][index.second];
    }

    pointer operator->() { return &(operator*()); }

    pointer operator->() const { return &(operator*()); }

    const type& operator*() const {
      std::pair<size_t, size_t> index = con_index();
      return deque_->data_[index.first][index.second];
    }

    difference_type operator-(const CommonIterator& iter) const {
      return number_ - iter.number_;
    }

    bool operator<(const CommonIterator& other) const {
      return number_ < other.number_;
    }

    bool operator>(const CommonIterator& other) const {
      return (other < *this);
    }

    bool operator<=(const CommonIterator& other) const {
      return !(other < *this);
    }

    bool operator>=(const CommonIterator& other) const {
      return !(other > *this);
    }

    bool operator==(const CommonIterator& other) const = default;

    bool operator!=(const CommonIterator& other) const {
      return !(other == *this);
    }
  };

  using iterator = CommonIterator<false>;
  using const_iterator = CommonIterator<true>;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  Deque(const Alloc& alloc = Alloc())
      : size_(0), first_chunk_(0), first_element_(0), alloc_(alloc){};

  Deque(const size_t& count, const Alloc& alloc = Alloc())
      : size_(count),
        first_chunk_(count / kChunkSize +
                     ((count / kChunkSize + 1) * kNewChunks) / 2),
        first_element_(0),
        alloc_(alloc) {
    size_t chunk_count = size_ / kChunkSize + 1;
    data_.resize(kNewChunks * chunk_count + first_chunk_);
    for (size_t i = first_chunk_; i < (size_ / kChunkSize) + first_chunk_ + 1;
         ++i) {
      data_[i] = alloc_traits::allocate(alloc_, kChunkSize);
    }
    for (size_t i = 0; i < size_; ++i) {
      try_construct(i, chunk_count);
    }
  }

  Deque(const size_t& count, const T& element, const Alloc& alloc = Alloc())
      : size_(count),
        first_chunk_(count / kChunkSize +
                     ((count / kChunkSize + 1) * kNewChunks) / 2),
        first_element_(0),
        alloc_(alloc) {
    size_t chunk_count = size_ / kChunkSize + 1;
    data_.resize(kNewChunks * chunk_count + first_chunk_);
    for (size_t i = first_chunk_; i < (size_ / kChunkSize) + first_chunk_ + 1;
         ++i) {
      data_[i] = alloc_traits::allocate(alloc_, kChunkSize);
    }
    for (size_t i = 0; i < size_; ++i) {
      try_construct(i, chunk_count, element);
    }
  }

  Deque(const Deque& copy)
      : data_(copy.data_),
        size_(copy.size_),
        first_chunk_(copy.first_chunk_),
        first_element_(copy.first_element_),
        alloc_(
            alloc_traits::select_on_container_copy_construction(copy.alloc_)) {
    if (!data_.empty()) {
      size_t chunk_count = (size_ + first_element_) / kChunkSize + 1;
      for (size_t i = 0; i < chunk_count; ++i) {
        data_[first_chunk_ + i] = alloc_traits::allocate(alloc_, kChunkSize);
      }
      for (size_t i = 0; i < size_; ++i) {
        try_construct(i, chunk_count, copy[i]);
      }
    }
  }

  Alloc get_allocator() const { return alloc_; }

  Deque(std::initializer_list<T> init, const Alloc& alloc = Alloc())
      : Deque(alloc) {
    default_construct();
    auto itr = init.begin();
    try {
      for (auto end = init.end(); itr != end; ++itr) {
        push_back(*itr);
      }
    } catch (...) {
      while (!empty()) {
        pop_back();
      }
      throw;
    }
  }

  Deque(Deque&& other) noexcept
      : Deque(
            alloc_traits::select_on_container_copy_construction(other.alloc_)) {
    std::swap(first_chunk_, other.first_chunk_);
    std::swap(first_element_, other.first_element_);
    std::swap(size_, other.size_);
    data_.swap(other.data_);  // noexept
  }

  Deque& operator=(Deque&& other) noexcept {
    if (this == &other) {
      return *this;
    }
    while (!empty()) {  // noexept
      pop_back();       // noexept
    }
    std::swap(first_chunk_, other.first_chunk_);
    std::swap(first_element_, other.first_element_);
    std::swap(size_, other.size_);
    data_.swap(other.data_);  // noexept
    return *this;
  }

  void swap(Deque& other) {
    std::swap(data_, other.data_);
    std::swap(size_, other.size_);
    std::swap(first_chunk_, other.first_chunk_);
    std::swap(first_element_, other.first_element_);
    if (alloc_traits::propagate_on_container_swap::value) {
      std::swap(alloc_, other.alloc_);
    }
  }

  Deque& operator=(const Deque& copy) {
    if (this == &copy) {
      return *this;
    }
    Deque tmp(copy);
    swap(tmp);
    if (alloc_traits::propagate_on_container_copy_assignment::value) {
      alloc_ = copy.alloc_;
    }
    return *this;
  }

  ~Deque() {
    if (!data_.empty()) {
      size_t chunk_count = (size_ + first_element_) / kChunkSize + 1;
      for (size_t i = 0; i < size_; ++i) {
        alloc_traits::destroy(alloc_, &((*this)[i]));
      }
      for (size_t i = first_chunk_; i < chunk_count + first_chunk_; ++i) {
        alloc_traits::deallocate(alloc_, data_[i], kChunkSize);
        data_[i] = nullptr;
      }
    }
  }

  size_t size() const { return size_; }

  bool empty() const noexcept { return size_ == 0; }

  void reserve(size_t new_size) {
    data_.resize(new_size);
    size_t count_chunk = (size_ + first_element_) / kChunkSize + 1;
    std::vector<T*> tmp;
    for (size_t i = 0; i < count_chunk; ++i) {
      tmp.push_back(data_[i + first_chunk_]);
      data_[i + first_chunk_] = nullptr;
    }
    first_chunk_ = (new_size - count_chunk) / 2;
    for (size_t i = 0; i < count_chunk; ++i) {
      data_[i + first_chunk_] = tmp[i];
      tmp[i] = nullptr;
    }
  }

  void push_back(const T& element) { emplace_back(element); }

  void push_back(T&& element) { emplace_back(std::move(element)); }

  template <class... Args>
  void emplace_back(Args&&... args) {
    default_construct();
    if (size_ == 0) {
      alloc_traits::construct(alloc_, &data_[first_chunk_][first_element_],
                              std::forward<Args>(args)...);
      ++size_;
      return;
    }
    size_t end_chunk = first_chunk_ + (size_ + first_element_) / kChunkSize;
    if (data_.end() <= data_.begin() + end_chunk + 1) {
      reserve(kNewChunks * (end_chunk - first_chunk_ + 1));
    }
    end_chunk = first_chunk_ + (size_ + first_element_) / kChunkSize;
    size_t end = (first_element_ + size_) % kChunkSize;
    if (end != 0) {
      alloc_traits::construct(alloc_, &data_[end_chunk][end],
                              std::forward<Args>(args)...);
    } else {
      if (data_[end_chunk] == nullptr) {
        data_[end_chunk] = alloc_traits::allocate(alloc_, kChunkSize);
      }
      try {
        alloc_traits::construct(alloc_, data_[end_chunk],
                                std::forward<Args>(args)...);
      } catch (...) {
        alloc_traits::deallocate(alloc_, data_[end_chunk], kChunkSize);
        data_[end_chunk] = nullptr;
        throw;
      }
    }
    ++size_;
  }

  void push_front(const T& element) { emplace_front(element); }

  void push_front(T&& element) { emplace_front(std::move(element)); }

  template <class... Args>
  void emplace_front(Args&&... args) {
    default_construct();
    size_t count_chunk = (size_ + first_element_) / kChunkSize + 1;
    if (size_ == 0) {
      alloc_traits::construct(alloc_, &data_[first_chunk_][first_element_],
                              std::forward<Args>(args)...);
      ++size_;
      return;
    }
    if (first_element_ != 0) {
      --first_element_;
    } else {
      if (first_chunk_ == 0) {
        reserve(kNewChunks * count_chunk);
      }
      --first_chunk_;
      first_element_ = kChunkSize - 1;
      data_[first_chunk_] = alloc_traits::allocate(alloc_, kChunkSize);
    }
    try {
      alloc_traits::construct(alloc_, &data_[first_chunk_][first_element_],
                              std::forward<Args>(args)...);
    } catch (...) {
      if (first_element_ == kChunkSize - 1) {
        alloc_traits::deallocate(alloc_, data_[first_chunk_], kChunkSize);
        first_element_ = 0;
        ++first_chunk_;
        throw;
      }
      ++first_element_;
      throw;
    }
    ++size_;
  }

  void pop_back() noexcept {
    if (size_ == 0) {
      return;
    }
    alloc_traits::destroy(alloc_, &((*this)[size_ - 1]));
    if (size_ == 1) {
      --size_;
      return;
    }
    if ((first_element_ + size_) % kChunkSize == 0) {
      alloc_traits::deallocate(
          alloc_, data_[first_chunk_ + (first_element_ + size_) / kChunkSize],
          kChunkSize);
      data_[first_chunk_ + (first_element_ + size_) / kChunkSize] = nullptr;
    }
    --size_;
  }

  void pop_front() noexcept {
    alloc_traits::destroy(alloc_, &((*this)[0]));
    --size_;
    if (first_element_ != kChunkSize - 1) {
      ++first_element_;
    } else {
      if (size_ != 0) {
        alloc_traits::deallocate(alloc_, data_[first_chunk_], kChunkSize);
        data_[first_chunk_] = nullptr;
      }
      ++first_chunk_;
      first_element_ = 0;
    }
  }

  void insert(iterator iter, const T& element) {
    push_back(element);
    size_t count = (iter - begin());
    for (size_t i = size_ - 1; i > count; --i) {
      std::swap((*this)[i], (*this)[i - 1]);
    }
  }

  void erase(iterator iter) {
    for (iterator it = iter; it < (end() - 1); ++it) {
      std::swap(*it, *(it + 1));
    }
    pop_back();
  }

  iterator begin() { return iterator(0, this); }

  iterator end() { return iterator(size(), this); }

  const_iterator cbegin() const {
    return const_iterator(0, const_cast<Deque*>(this));
  }

  const_iterator cend() const {
    return const_iterator(size(), const_cast<Deque*>(this));
  }

  reverse_iterator rbegin() { return std::make_reverse_iterator(end()); }

  reverse_iterator rend() { return std::make_reverse_iterator(begin()); }

  const_reverse_iterator crbegin() const {
    return std::make_reverse_iterator(cend());
  }

  const_reverse_iterator crend() const {
    return std::make_reverse_iterator(cbegin());
  }

  T& operator[](size_t ind) { return *iterator(ind, this); }

  const T& operator[](size_t ind) const {
    return *const_iterator(ind, const_cast<Deque*>(this));
  }

  T& at(size_t ind) {
    if (ind >= size_) {
      throw std::out_of_range("out of range");
    }
    return *iterator(ind, this);
  }

  const T& at(size_t ind) const {
    if (ind >= size_) {
      throw std::out_of_range("out of range");
    }
    return *iterator(ind, const_cast<Deque*>(this));
  }
};