#include <cstddef>
#include <memory>

struct BaseControlBlock {
  size_t shared_count = 0;
  size_t weak_count = 0;
  virtual void delete_block() = 0;
  virtual ~BaseControlBlock() = default;
  virtual void delete_item() = 0;
};

template <typename T>
class SharedPtr {
 private:
  T* ptr_ = nullptr;
  BaseControlBlock* cptr_ = nullptr;

  template <typename Deleter, typename Alloc>
  struct ControlBlockRegular : public BaseControlBlock {
    T* ptr = nullptr;
    Deleter deleter;
    Alloc alloc;

    using all_tr = std::allocator_traits<Alloc>;
    using cblock_all = typename all_tr::template rebind_alloc<
        ControlBlockRegular<Deleter, Alloc>>;
    using cblock_all_tr = std::allocator_traits<cblock_all>;

    ControlBlockRegular(T* ptrr, Deleter delet = Deleter(),
                        Alloc alloca = Alloc())
        : ptr(ptrr), deleter(delet), alloc(alloca) {}
    void delete_block() override {
      cblock_all copy_alloc(alloc);
      alloc.~Alloc();
      cblock_all_tr::deallocate(copy_alloc, this, 1);
    }
    ~ControlBlockRegular() = default;

    void delete_item() override {
      deleter(ptr);
      ptr = nullptr;
    }
  };

  template <typename Alloc = std::allocator<T>>
  struct ControlBlockMakeShared : public BaseControlBlock {
    T obj;
    Alloc alloc;
    T* ptr = nullptr;

    using all_tr = std::allocator_traits<Alloc>;
    using cblock_all =
        typename all_tr::template rebind_alloc<ControlBlockMakeShared<Alloc>>;
    using cblock_all_tr = std::allocator_traits<cblock_all>;
    template <typename... Args>
    ControlBlockMakeShared(Alloc alloc, Args&&... args)
        : alloc(alloc), obj(std::forward<Args>(args)...), ptr(&obj) {}
    ~ControlBlockMakeShared() = default;
    void delete_block() override {
      cblock_all copy_alloc(alloc);
      alloc.~Alloc();
      cblock_all_tr::deallocate(copy_alloc, this, 1);
    }
    void delete_item() override {
      all_tr::destroy(alloc, ptr);
      ptr = nullptr;
    }
  };

  template <typename Y, typename Alloc, typename... Args>
  friend SharedPtr<Y> AllocateShared(const Alloc& alloc, Args&&... args);
  template <typename Y>
  friend class SharedPtr;
  template <typename Y>
  friend class WeakPtr;

  SharedPtr(BaseControlBlock* cptr, T* ptr) noexcept : ptr_(ptr), cptr_(cptr) {
    if (cptr != nullptr) {
      ++cptr_->shared_count;
    }
  }

 public:
  void swap(SharedPtr& other) {
    std::swap(cptr_, other.cptr_);
    std::swap(ptr_, other.ptr_);
  }

  SharedPtr() noexcept : ptr_(nullptr), cptr_(nullptr){};

  SharedPtr(std::nullptr_t ptr) noexcept : ptr_(ptr), cptr_(nullptr){};

  template <typename Y, typename Deleter = std::default_delete<Y>,
            typename Alloc = std::allocator<Y>>
  SharedPtr(Y* ptr, Deleter del = Deleter(), Alloc alloc = Alloc())
      : ptr_(static_cast<T*>(ptr)) {
    using all_tr = std::allocator_traits<Alloc>;
    using cblock_all = typename all_tr::template rebind_alloc<
        ControlBlockRegular<Deleter, Alloc>>;
    using cblock_all_tr = std::allocator_traits<cblock_all>;
    cblock_all tmp_alloc(alloc);
    auto tmp = cblock_all_tr::allocate(tmp_alloc, 1);
    cblock_all_tr::construct(tmp_alloc, tmp, ptr_, del, alloc);
    cptr_ = static_cast<BaseControlBlock*>(tmp);
    ++(cptr_->shared_count);
  }

  template <typename Y>
  SharedPtr(const SharedPtr<Y>& copy) noexcept
      : cptr_(copy.cptr_), ptr_(static_cast<T*>(copy.ptr_)) {
    if (cptr_ != nullptr) {
      ++(cptr_->shared_count);
    }
  }

  SharedPtr(const SharedPtr& copy) noexcept
      : cptr_(copy.cptr_), ptr_(static_cast<T*>(copy.ptr_)) {
    if (cptr_ != nullptr) {
      ++(cptr_->shared_count);
    }
  }

  template <typename Y>
  SharedPtr(SharedPtr<Y>&& copy) noexcept
      : cptr_(copy.cptr_), ptr_(static_cast<T*>(copy.ptr_)) {
    if (cptr_ != nullptr) {
      ++(cptr_->shared_count);
    }
    SharedPtr().swap(copy);
  }

  SharedPtr(SharedPtr&& copy) noexcept
      : cptr_(copy.cptr_), ptr_(static_cast<T*>(copy.ptr_)) {
    if (cptr_ != nullptr) {
      ++(cptr_->shared_count);
    }
    SharedPtr().swap(copy);
  }

  template <typename Y>
  SharedPtr& operator=(const SharedPtr<Y>& copy) noexcept {
    this->~SharedPtr();
    ptr_ = copy.ptr_;
    cptr_ = copy.cptr_;
    if (copy.cptr_ != nullptr) {
      ++(cptr_->shared_count);
    }
    return *this;
  }

  SharedPtr& operator=(const SharedPtr& copy) noexcept {
    if (this == &copy) {
      return *this;
    }
    this->~SharedPtr();
    ptr_ = copy.ptr_;
    cptr_ = copy.cptr_;
    if (copy.cptr_ != nullptr) {
      ++(cptr_->shared_count);
    }
    return *this;
  }

  template <typename Y>
  SharedPtr& operator=(SharedPtr<Y>&& copy) noexcept {
    SharedPtr(std::move(copy)).swap(*this);
    return *this;
  }

  SharedPtr& operator=(SharedPtr&& copy) noexcept {
    if (this == &copy) {
      return *this;
    }
    SharedPtr(std::move(copy)).swap(*this);
    return *this;
  }

  size_t use_count() const noexcept {
    if (cptr_ != nullptr) {
      return cptr_->shared_count;
    }
    return 0;
  }

  T* get() noexcept { return ptr_; }

  T& operator*() noexcept { return *ptr_; }

  T* operator->() noexcept { return ptr_; }

  const T* get() const noexcept { return ptr_; }

  const T& operator*() const noexcept { return *ptr_; }

  const T* operator->() const noexcept { return ptr_; }

  void reset() noexcept { SharedPtr().swap(*this); }

  template <typename Y, typename Deleter = std::default_delete<Y>,
            typename Alloc = std::allocator<Y>>
  void reset(Y* pointer, Deleter deleter = Deleter(), Alloc alloc = Alloc()) {
    SharedPtr(pointer, deleter, alloc).swap(*this);
  }

  ~SharedPtr() {
    if (cptr_ != nullptr) {
      if (cptr_->shared_count == 1) {
        cptr_->delete_item();
        if (cptr_->weak_count == 0) {
          cptr_->delete_block();
          cptr_ = nullptr;
          ptr_ = nullptr;
          return;
        }
      }
      --(cptr_->shared_count);
      cptr_ = nullptr;
      ptr_ = nullptr;
    }
  };
};

template <typename T>
class WeakPtr {
 private:
  T* ptr_weak_ = nullptr;
  BaseControlBlock* weak_cptr_ = nullptr;

 public:
  template <typename Y>
  friend class SharedPtr;

  template <typename Y>
  friend class WeakPtr;

  void swap(WeakPtr& copy) {
    std::swap(ptr_weak_, copy.ptr_weak_);
    std::swap(weak_cptr_, copy.weak_cptr_);
  }
  WeakPtr() noexcept {};

  template <typename Y>
  WeakPtr(const WeakPtr<Y>& copy) noexcept
      : ptr_weak_(static_cast<T*>(copy.ptr_)), weak_cptr_(copy.cptr_) {
    if (weak_cptr_ != nullptr) {
      ++(weak_cptr_->weak_count);
    }
  }
  template <typename Y>
  WeakPtr(const SharedPtr<Y>& copy) noexcept {
    weak_cptr_ = copy.cptr_;
    ptr_weak_ = copy.ptr_;
    ++(weak_cptr_->weak_count);
  }

  template <typename Y>
  WeakPtr& operator=(const SharedPtr<Y>& copy) noexcept {
    ~WeakPtr();
    weak_cptr_ = copy.cptr_;
    ptr_weak_ = copy.ptr_;
    ++(weak_cptr_->weak_count);
    return *this;
  }

  template <typename Y>
  WeakPtr(WeakPtr<Y>&& copy) noexcept
      : ptr_weak_(static_cast<T*>(copy.ptr_weak_)),
        weak_cptr_(copy.weak_cptr_) {
    WeakPtr().swap(copy);
  }

  template <typename Y>
  WeakPtr& operator=(const WeakPtr<Y>& copy) noexcept {
    ~WeakPtr();
    weak_cptr_ = copy.cptr_;
    ptr_weak_ = copy.cptr_;
    if (weak_cptr_ != nullptr) {
      ++(weak_cptr_->shared_count);
    }
    return *this;
  }

  template <typename Y>
  WeakPtr& operator=(WeakPtr<Y>&& copy) noexcept {
    ~WeakPtr();
    weak_cptr_ = copy.cptr_;
    ptr_weak_ = copy.cptr_;
    WeakPtr().swap(copy);
    return *this;
  }

  bool expired() const noexcept { return weak_cptr_->shared_count == 0; }

  SharedPtr<T> lock() const {
    return expired() ? SharedPtr<T>() : SharedPtr<T>(ptr_weak_, weak_cptr_);
  }

  ~WeakPtr() {
    if (weak_cptr_ != nullptr) {
      --(weak_cptr_->weak_count);
      if (weak_cptr_->shared_count == 0 && weak_cptr_->weak_count == 0) {
        weak_cptr_->delete_block();
        weak_cptr_ = nullptr;
        ptr_weak_ = nullptr;
      }
    }
  };
};

template <typename T, typename Alloc, typename... Args>
SharedPtr<T> AllocateShared(const Alloc& alloc, Args&&... args) {
  using all_tr = std::allocator_traits<Alloc>;
  using control_block =
      typename SharedPtr<T>::template ControlBlockMakeShared<Alloc>;
  using cblock_all = typename all_tr::template rebind_alloc<control_block>;
  using cblock_all_tr = std::allocator_traits<cblock_all>;
  cblock_all tmp_alloc(alloc);
  control_block* tmp = cblock_all_tr::allocate(tmp_alloc, 1);
  cblock_all_tr::construct(tmp_alloc, tmp, alloc, std::forward<Args>(args)...);
  return SharedPtr<T>(static_cast<BaseControlBlock*>(tmp), tmp->ptr);
}

template <typename T, typename... Args>
SharedPtr<T> MakeShared(Args&&... args) {
  return AllocateShared<T, std::allocator<T>, Args...>(
      std::allocator<T>(), std::forward<Args>(args)...);
}