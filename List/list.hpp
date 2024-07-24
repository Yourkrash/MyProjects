#pragma once
#include <initializer_list>
#include <iostream>
#include <iterator>
#include <memory>

template <typename T, typename Alloc = std::allocator<T>>
class List {
 private:
  // node
  struct BasicNode {
    BasicNode* previous;
    BasicNode* next;
  };

  struct Node : public BasicNode {
    T element;
    Node() {}
    Node(const T& elem) : element(elem) {}
    Node(T&& elem) : element(elem) {}
  };

  // allocator's usings
  using alloc_traits = std::allocator_traits<Alloc>;
  using node_alloc = typename alloc_traits::template rebind_alloc<Node>;
  using node_alloc_traits = typename alloc_traits::template rebind_traits<Node>;

  // class fields
  BasicNode init_;
  size_t size_;
  node_alloc alloc_;

  // private methods
  static void bundle(BasicNode* first, BasicNode* last) {
    first->next = last;
    last->previous = first;
  }

  static void insert_node(BasicNode* node, BasicNode* first, BasicNode* last) {
    bundle(first, node);
    bundle(node, last);
  }

  template <typename... Args>
  void insert(BasicNode* first, BasicNode* last, Args&&... args) {
    insert_node(create_node(std::forward<Args>(args)...), first, last);
    ++size_;
  }

  template <typename... Args>
  BasicNode* create_node(Args&&... args) {
    BasicNode* ptr = node_alloc_traits::allocate(alloc_, 1);
    try {
      node_alloc_traits::construct(alloc_, static_cast<Node*>(ptr),
                                   std::forward<Args>(args)...);
    } catch (...) {
      node_alloc_traits::deallocate(alloc_, static_cast<Node*>(ptr), 1);
      throw;
    }
    return ptr;
  }

  void destroy_node(BasicNode* ptr) {
    if (ptr != nullptr) {
      node_alloc_traits::destroy(alloc_, static_cast<Node*>(ptr));
      node_alloc_traits::deallocate(alloc_, static_cast<Node*>(ptr), 1);
    }
  }

  void erase(BasicNode* node) {
    if (empty()) {
      return;
    }
    bundle(node->previous, node->next);
    destroy_node(node);
    --size_;
  }

 public:
  using allocator_type = Alloc;
  using value_type = T;

  // iterator
  template <bool IsConst>
  class CommonIterator {
   private:
    BasicNode* pointer_;

   public:
    using value_type = T;
    using type = std::conditional_t<IsConst, const T, T>;
    using iterator_category = std::bidirectional_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using pointer = type*;
    using reference = type&;

    CommonIterator(BasicNode* node) : pointer_(node){};

    CommonIterator(const CommonIterator& other) : pointer_(other.pointer_) {}

    CommonIterator& operator=(const CommonIterator& other) {
      pointer_ = other.pointer_;
    }

    CommonIterator& operator++() {
      pointer_ = pointer_->next;
      return *this;
    }

    CommonIterator operator++(int) {
      pointer_ = pointer_->next;
      return CommonIterator(pointer_->previous);
    }

    CommonIterator& operator--() {
      pointer_ = pointer_->previous;
      return *this;
    }

    CommonIterator operator--(int) {
      pointer_ = pointer_->previous;
      return CommonIterator(pointer_->next);
    }

    reference operator*() { return static_cast<Node*>(pointer_)->element; }

    pointer operator->() { return &(*(*this)); }

    const type& operator*() const {
      return static_cast<Node*>(pointer_)->element;
    }

    const type* operator->() const { return &(*(*this)); }

    bool operator==(const CommonIterator& other) const {
      return pointer_ == other.pointer_;
    }

    bool operator!=(const CommonIterator& other) const {
      return pointer_ != other.pointer_;
    }
  };

  // iterator's usings
  using iterator = CommonIterator<false>;
  using const_iterator = CommonIterator<true>;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  // list body
  List(const Alloc& alloc = Alloc()) : size_(0), alloc_(alloc) {
    bundle(&init_, &init_);
  }

  List(size_t count, const T& element, const Alloc& alloc = Alloc())
      : List(alloc) {
    size_t ind = 0;
    try {
      for (; ind < count; ++ind) {
        push_back(element);
      }
    } catch (...) {
      for (size_t j = 0; j < ind; ++j) {
        pop_front();
      }
      throw;
    }
  }

  explicit List(size_t count, const Alloc& alloc = Alloc()) : List(alloc) {
    size_t ind = 0;
    for (; ind < count; ++ind) {
      try {
        insert(init_.previous, &init_);
      } catch (...) {
        for (size_t j = 0; j < ind; ++j) {
          pop_front();
        }
        throw;
      }
    }
  }

  List(std::initializer_list<T> init, const Alloc& alloc = Alloc())
      : List(alloc) {
    try {
      for (auto it = init.begin(), end = init.end(); it != end; ++it) {
        push_back(*it);
      }
    } catch (...) {
      while (!empty()) {
        pop_front();
      }
      throw;
    }
  }

  List(const List& other)
      : List(
            alloc_traits::select_on_container_copy_construction(other.alloc_)) {
    try {
      for (auto it = other.cbegin(), end = other.cend(); it != end; ++it) {
        push_back(*it);
      }
    } catch (...) {
      while (!empty()) {
        pop_front();
      }
      throw;
    }
  }

  static void swap(List& first, List& second) {
    if (node_alloc_traits::propagate_on_container_swap::value) {
      std::swap(first.alloc_, second.alloc_);
    }

    BasicNode* tmpl = second.init_.previous;
    BasicNode* tmpr = second.init_.next;
    if (first.size_ != 0) {
      insert_node(&second.init_, first.init_.previous, first.init_.next);
    } else {
      bundle(&second.init_, &second.init_);
    }
    if (second.size_ != 0) {
      insert_node(&first.init_, tmpl, tmpr);
    } else {
      bundle(&first.init_, &first.init_);
    }
    std::swap(first.size_, second.size_);
  }

  List(List&& other)
      : List(
            alloc_traits::select_on_container_copy_construction(other.alloc_)) {
    if (this == &other) {
      return;
    }
    swap(*this, List());
    swap(other, *this);
  }

  List& operator=(List&& other) {
    List copy(std::move(other));
    swap(*this, copy);
    if (node_alloc_traits::propagate_on_container_move_assignment::value) {
      alloc_ = other.alloc_;
    }
    return *this;
  }

  List& operator=(const List& other) {
    List copy(other);
    swap(*this, copy);

    if (node_alloc_traits::propagate_on_container_copy_assignment::value) {
      alloc_ = other.alloc_;
    }
    return *this;
  }

  void push_back(const T& element) { insert(init_.previous, &init_, element); }

  void push_front(const T& element) { insert(&init_, init_.next, element); }

  void push_back(T&& element) { insert(init_.previous, &init_, element); }

  void push_front(T&& element) { insert(&init_, init_.next, element); }

  void pop_back() { erase(init_.previous); }

  void pop_front() { erase(init_.next); }

  T& front() { return static_cast<Node*>(init_.next)->element; }

  T& back() { return static_cast<Node*>(init_.previous)->element; }

  const T& front() const { return static_cast<Node*>(init_.next)->element; }

  const T& back() const { return static_cast<Node*>(init_.previous)->element; }

  bool empty() const { return size_ == 0; }

  size_t size() const { return size_; }

  iterator begin() const {
    return iterator(const_cast<BasicNode*>(init_.next));
  }

  iterator end() const { return iterator(const_cast<BasicNode*>(&init_)); }

  const_iterator cbegin() const {
    return const_iterator(const_cast<BasicNode*>(init_.next));
  }

  const_iterator cend() const {
    return const_iterator(const_cast<BasicNode*>(&init_));
  }

  reverse_iterator rbegin() { return std::make_reverse_iterator(end()); }

  reverse_iterator rend() { return std::make_reverse_iterator(begin()); }

  const_reverse_iterator rcbegin() const {
    return std::make_reverse_iterator(cend());
  }

  const_reverse_iterator rcend() const {
    return std::make_reverse_iterator(cbegin());
  }

  Alloc get_allocator() const { return alloc_; }

  ~List() {
    for (BasicNode *it = init_.next, *end = &init_; it != end;) {
      BasicNode* tmp = it->next;
      destroy_node(it);
      it = tmp;
    }
  }
};
