#include <iostream>
#include <memory>
#include <queue>
#include <set>

template <typename T>
class Tree {
 public:
  virtual void insert(const T&) = 0;
  virtual void insert(T&&) = 0;
  virtual void remove() = 0;
  virtual void remove(T) = 0;
  virtual T& min() = 0;
  virtual T& max() = 0;
  virtual bool hasElem(const T& data) const = 0;
  virtual ~Tree() = default;
};

template <typename T, typename Alloc = std::allocator<T>>
class BinaryTree : public Tree<T> {
 private:
  // node
  struct Node {
    T element;

    std::shared_ptr<Node> left;
    std::shared_ptr<Node> right;
    std::weak_ptr<Node> parent;

    Node() {}
    Node(const T& elem) : element(elem) {}
    Node(T&& elem) : element(std::move(elem)) {}
    Node(const Node& node) = default;
    Node(Node&& node) = default;
    void swap(Node& node) {
      std::swap(element, node.element);
      left.swap(node.left);
      right.swap(node.right);
      parent.swap(node.parent);
    }
  };

  // allocator's usings
  using alloc_traits = std::allocator_traits<Alloc>;
  using node_alloc = typename alloc_traits::template rebind_alloc<Node>;
  using node_alloc_traits = typename alloc_traits::template rebind_traits<Node>;

  node_alloc alloc_;
  std::shared_ptr<Node> top_;

  auto find_leaf(const T& data) {
    if (!top_) {
      return top_;
    }
    auto curr_node = top_;
    while (data < curr_node->element ? curr_node->left : curr_node->right) {
      if (data < curr_node->element) {
        curr_node = curr_node->left;
      } else {
        curr_node = curr_node->right;
      }
    }
    return curr_node;
  }

  auto find(const T& data) const {
    if (!top_) {
      return top_;
    }
    auto curr_node = top_;
    while (data < curr_node->element ? curr_node->left : curr_node->right) {
      if (data == curr_node->element) {
        return curr_node;
      }
      if (data < curr_node->element) {
        curr_node = curr_node->left;
      } else {
        curr_node = curr_node->right;
      }
    }
    return curr_node;
  }

  auto min(std::shared_ptr<Node> ptr) {
    while (ptr->left) {
      ptr = ptr->left;
    }
    return ptr;
  }

  auto max(std::shared_ptr<Node> ptr) {
    while (ptr->right) {
      ptr = ptr->right;
    }
    return ptr;
  }

  void remove_node(std::shared_ptr<Node> ptr) {
    if (!ptr->left && !ptr->right) {
      if (ptr == top_) {
        top_.reset();
        return;
      }
      auto parent = ptr->parent.lock();
      if (parent->left == ptr) {
        parent->left.reset();
      } else {
        parent->right.reset();
      }
    } else if (bool(ptr->left) ^ bool(ptr->right)) {
      auto child = (ptr->left ? ptr->left : ptr->right);
      if (ptr != top_) {
        auto ptr_to_parent = ptr->parent;
        auto parent_to_ptr =
            ((ptr_to_parent.lock()->left == ptr) ? ptr_to_parent.lock()->left
                                                 : ptr_to_parent.lock()->right);
        parent_to_ptr = child;
        child->parent = ptr_to_parent;
      } else {
        top_ = child;
      }
    } else {
      auto minimum = min(ptr->right);
      std::swap(minimum->element, ptr->element);
      remove_node(minimum);
    }
  }

  BinaryTree(std::shared_ptr<Node> ptr) : top_(std::allocate_shared<Node, Alloc>(alloc_, ptr->element)) {
    auto original = ptr;
    auto copy = top_;
    auto original_parent = ptr->parent.lock();

    while (original && (original != original_parent)) {
      if (original->left && !copy->left) {
        auto copy_tmp = std::allocate_shared<Node, Alloc>(alloc_, *original);
        original = original->left;
        copy->left = copy_tmp;
        copy_tmp->parent = copy;
        copy = copy_tmp;
      } else if (original->right && !copy->right) {
        auto copy_tmp = std::allocate_shared<Node, Alloc>(alloc_, *original);
        original = original->right;
        copy->right = copy_tmp;
        copy_tmp->parent = copy;
        copy = copy_tmp;
      } else {
        original = original->parent.lock();
        copy = copy->parent.lock();
      }
    }
  }

 public:
  void remove(T elem) {
    if (!top_) {
      return;
    }
    auto curr_node = find(elem);
    if (curr_node->element == elem) {
      remove_node(curr_node);
    }
  }

  void remove() {
    if (!top_) {
      return;
    }
    remove_node(top_);
  }

  T& max() { return max(top_)->element; }

  T& min() { return min(top_)->element; }

  BinaryTree() = default;

  template <typename Arg>
  void insert_construct(Arg&& elem) {
    if (!top_) {
      top_ = std::allocate_shared<Node, Alloc>(alloc_, elem);
      return;
    }
    auto curr_node = find_leaf(elem);
    if (elem < curr_node->element) {
      curr_node->left =
          std::allocate_shared<Node, Alloc>(alloc_, std::forward<Arg>(elem));
      curr_node->left->parent = curr_node;
    } else {
      curr_node->right =
          std::allocate_shared<Node, Alloc>(alloc_, std::forward<Arg>(elem));
      curr_node->right->parent = curr_node;
    }
  }
  void insert(const T& elem) { insert_construct(elem); }

  void insert(T&& elem) { insert_construct(std::move(elem)); }

  BinaryTree(const BinaryTree& tree)
      : BinaryTree(tree.top_) {
    alloc_ = tree.alloc_;
  }

  BinaryTree(BinaryTree&& tree)
      : alloc_(tree.alloc_), top_(std::move(tree.top_)) {}

  BinaryTree getLeftSubtree() const { return BinaryTree(top_->left); }

  BinaryTree getRightSubtree() { return BinaryTree(top_->right); }

  T top() { return top_->element; }

  bool hasElem(const T& data) const {
    auto ptr = find(data);
    if (!ptr) {
      return 0;
    }
    return ptr->element == data;
  }
};