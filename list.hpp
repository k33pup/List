#pragma once

#include <iostream>

template <typename T, typename Allocator = std::allocator<T>>
class List {
 public:
  List();

  List(size_t count, const T& value, const Allocator& alloc = Allocator());

  explicit List(size_t count, const Allocator& alloc = Allocator());

  List(const List& other);

  List(List&& other) noexcept;

  List(std::initializer_list<T> init, const Allocator& alloc = Allocator());

  ~List();

  List& operator=(const List& other);

  List& operator=(List&& other) noexcept;

  T& front() { return static_cast<Node*>(fake_node_.next)->value; }

  const T& front() const { return static_cast<Node*>(fake_node_.next)->value; }

  T& back() { return static_cast<Node*>(fake_node_.prev)->value; }

  const T& back() const { return static_cast<Node*>(fake_node_.prev)->value; };

  [[nodiscard]] bool empty() const { return count_ == 0; };

  [[nodiscard]] size_t size() const { return count_; };

  void push_back(const T& value);

  void push_back(T&& value);

  void push_front(const T& value);

  void push_front(T&& value);

  void pop_back();

  void pop_front();

  Allocator get_allocator() const { return alloc_; }

  template <bool IsConst = false>
  class BaseIterator;

  using value_type = T;
  using allocator_type = Allocator;
  using iterator = BaseIterator<false>;
  using const_iterator = BaseIterator<true>;
  using reverse_iterator = std::reverse_iterator<BaseIterator<false>>;
  using const_reverse_iterator = std::reverse_iterator<BaseIterator<true>>;

  iterator begin() { return iterator(fake_node_.next); }

  const_iterator begin() const { return const_iterator(fake_node_.next); }

  iterator end() { return iterator(&fake_node_); }

  const_iterator end() const { return const_iterator(&fake_node_); }

  const_iterator cbegin() const { return const_iterator(fake_node_.next); }

  const_iterator cend() const { return const_iterator(&fake_node_); }

  reverse_iterator rbegin() { return reverse_iterator(&fake_node_); }

  reverse_iterator rend() { return reverse_iterator(&fake_node_); }

  const_reverse_iterator crbegin() const {
    return const_reverse_iterator(&fake_node_);
  }

  const_reverse_iterator crend() const {
    return const_reverse_iterator(&fake_node_);
  }

 private:
  struct BaseNode {
    BaseNode() : prev(this), next(this) {}

    BaseNode* prev;
    BaseNode* next;
  };

  struct Node : public BaseNode {
    Node() = default;

    explicit Node(const T& value) : value(value) {}

    explicit Node(T&& value) : value(std::move(value)) {}

    T value;
  };

  template <typename... Args>
  void build_list(BaseNode* cur_node, size_t& ind, Args&&... args) {
    for (; ind < count_; ++ind) {
      Node* next_node = node_alloc_traits::allocate(alloc_, 1);
      cur_node->next = next_node;
      node_alloc_traits::construct(alloc_, next_node, std::forward<Args>(args)...);
      next_node->prev = cur_node;
      cur_node = next_node;
    }
    cur_node->next = &fake_node_;
    fake_node_.prev = cur_node;
  }

  void clear_memory(BaseNode* cur_node, size_t ind) {
    for (size_t j = 0; j < ind; ++j) {
      BaseNode* next_node = cur_node->next;
      node_alloc_traits::destroy(alloc_, static_cast<Node*>(cur_node));
      node_alloc_traits::deallocate(alloc_, static_cast<Node*>(cur_node), 1);
      cur_node = next_node;
    }
    node_alloc_traits::deallocate(alloc_, static_cast<Node*>(cur_node), 1);
  }

  using node_alloc =
      typename std::allocator_traits<Allocator>::template rebind_alloc<Node>;
  using alloc_traits = std::allocator_traits<Allocator>;
  using node_alloc_traits = std::allocator_traits<node_alloc>;

  BaseNode fake_node_;
  size_t count_ = 0;
  node_alloc alloc_;
};

template <typename T, typename Allocator>
List<T, Allocator>::List() = default;

template <typename T, typename Allocator>
List<T, Allocator>::List(size_t count, const T& value, const Allocator& alloc)
    : count_(count), alloc_(alloc) {
  BaseNode* cur_node = &fake_node_;
  size_t ind = 0;
  try {
    build_list(cur_node, ind, value);
  } catch (...) {
    cur_node = fake_node_.next;
    clear_memory(cur_node, ind);
    throw;
  }
}

template <typename T, typename Allocator>
List<T, Allocator>::List(size_t count, const Allocator& alloc)
    : count_(count), alloc_(alloc) {
  BaseNode* cur_node = &fake_node_;
  size_t ind = 0;
  try {
    build_list(cur_node, ind);
  } catch (...) {
    cur_node = fake_node_.next;
    clear_memory(cur_node, ind);
    throw;
  }
}

template <typename T, typename Allocator>
List<T, Allocator>::List(const List& other) : count_(other.count_) {
  alloc_ =
      node_alloc_traits::select_on_container_copy_construction(other.alloc_);
  BaseNode* cur_node = &fake_node_;
  size_t ind = 0;
  try {
    for (const auto& value : other) {
      Node* next_node = node_alloc_traits::allocate(alloc_, 1);
      cur_node->next = next_node;
      node_alloc_traits::construct(alloc_, next_node, value);
      next_node->prev = cur_node;
      cur_node = next_node;
      ++ind;
    }
    cur_node->next = &fake_node_;
    fake_node_.prev = cur_node;
  } catch (...) {
    cur_node = fake_node_.next;
    clear_memory(cur_node, ind);
    throw;
  }
}

template <typename T, typename Allocator>
List<T, Allocator>::List(List&& other) noexcept
    : fake_node_(other.fake_node_), count_(other.count_) {
  other.fake_node_ = BaseNode();
  other.count_ = 0;
}

template <typename T, typename Allocator>
List<T, Allocator>::List(std::initializer_list<T> init, const Allocator& alloc)
    : count_(init.size()), alloc_(alloc) {
  BaseNode* cur_node = &fake_node_;
  size_t ind = 0;
  try {
    for (const auto& value : init) {
      Node* next_node = node_alloc_traits::allocate(alloc_, 1);
      cur_node->next = next_node;
      node_alloc_traits::construct(alloc_, next_node, value);
      next_node->prev = cur_node;
      cur_node = cur_node->next;
      ++ind;
    }
    cur_node->next = &fake_node_;
    fake_node_.prev = cur_node;
  } catch (...) {
    cur_node = fake_node_.next;
    clear_memory(cur_node, ind);
    throw;
  }
}

template <typename T, typename Allocator>
List<T, Allocator>::~List() {
  BaseNode* cur_node = fake_node_.next;
  for (size_t i = 0; i < count_; ++i) {
    BaseNode* next_node = cur_node->next;
    node_alloc_traits::destroy(alloc_, static_cast<Node*>(cur_node));
    node_alloc_traits::deallocate(alloc_, static_cast<Node*>(cur_node), 1);
    cur_node = next_node;
  }
}

template <typename T, typename Allocator>
List<T, Allocator>& List<T, Allocator>::operator=(
    const List<T, Allocator>& other) {
  if (&other == this) {
    return *this;
  }

  node_alloc next_alloc = alloc_;
  if constexpr (node_alloc_traits::propagate_on_container_copy_assignment::
                    value) {
    next_alloc = other.alloc_;
  }
  BaseNode next_fake;
  BaseNode* cur_node = &next_fake;
  size_t ind = 0;
  try {
    for (const auto& value : other) {
      Node* next_node = node_alloc_traits::allocate(next_alloc, 1);
      cur_node->next = next_node;
      node_alloc_traits::construct(next_alloc, next_node, value);
      next_node->prev = cur_node;
      cur_node = cur_node->next;
      ++ind;
    }
    cur_node->next = &next_fake;
    next_fake.prev = cur_node;
  } catch (...) {
    cur_node = next_fake.next;
    clear_memory(cur_node, ind);
    throw;
  }
  cur_node = fake_node_.next;
  for (size_t j = 0; j < count_; ++j) {
    BaseNode* next_node = cur_node->next;
    node_alloc_traits::destroy(alloc_, static_cast<Node*>(cur_node));
    node_alloc_traits::deallocate(alloc_, static_cast<Node*>(cur_node), 1);
    cur_node = next_node;
  }
  count_ = other.count_;
  fake_node_ = next_fake;
  alloc_ = next_alloc;
  return *this;
}

template <typename T, typename Allocator>
List<T, Allocator>& List<T, Allocator>::operator=(List&& other) noexcept {
  if (&other == this) {
    return *this;
  }
  Allocator next_alloc = alloc_;
  if constexpr (alloc_traits::propagate_on_container_move_assignment::value) {
    next_alloc = other.alloc_;
  }
  BaseNode* cur_node = &fake_node_;
  for (size_t j = 0; j < count_; ++j) {
    BaseNode* next_node = cur_node->next;
    node_alloc_traits::destroy(alloc_, static_cast<Node*>(cur_node));
    node_alloc_traits::deallocate(alloc_, static_cast<Node*>(cur_node), 1);
    cur_node = next_node;
  }
  fake_node_ = other.fake_node_;
  other.fake_node_ = BaseNode();
  count_ = other.count_;
  other.count_ = 0;
  alloc_ = next_alloc;
  return *this;
}


template <typename T, typename Allocator>
void List<T, Allocator>::push_back(const T& value) {
  Node* next_node;
  next_node = node_alloc_traits::allocate(alloc_, 1);
  try {
    node_alloc_traits::construct(alloc_, next_node, value);
    BaseNode* last_node = fake_node_.prev;
    next_node->prev = last_node;
    next_node->next = &fake_node_;
    last_node->next = next_node;
    fake_node_.prev = next_node;
    ++count_;
  } catch (...) {
    node_alloc_traits::deallocate(alloc_, next_node, 1);
    throw;
  }
}

template <typename T, typename Allocator>
void List<T, Allocator>::push_back(T&& value) {
  Node* next_node;
  next_node = node_alloc_traits::allocate(alloc_, 1);
  try {
    node_alloc_traits::construct(alloc_, next_node, std::move(value));
    BaseNode* last_node = fake_node_.prev;
    next_node->prev = last_node;
    next_node->next = &fake_node_;
    last_node->next = next_node;
    fake_node_.prev = next_node;
    ++count_;
  } catch (...) {
    node_alloc_traits::deallocate(alloc_, next_node, 1);
    throw;
  }
}

template <typename T, typename Allocator>
void List<T, Allocator>::push_front(const T& value) {
  Node* next_node;
  next_node = node_alloc_traits::allocate(alloc_, 1);
  try {
    node_alloc_traits::construct(alloc_, next_node, value);
    BaseNode* first_node = fake_node_.next;
    next_node->next = first_node;
    first_node->prev = next_node;
    fake_node_.next = next_node;
    next_node->prev = &fake_node_;
    ++count_;
  } catch (...) {
    node_alloc_traits::deallocate(alloc_, next_node, 1);
    throw;
  }
}

template <typename T, typename Allocator>
void List<T, Allocator>::push_front(T&& value) {
  Node* next_node;
  next_node = node_alloc_traits::allocate(alloc_, 1);
  try {
    node_alloc_traits::construct(alloc_, next_node, std::move(value));
    BaseNode* first_node = fake_node_.next;
    next_node->next = first_node;
    first_node->prev = next_node;
    fake_node_.next = next_node;
    next_node->prev = &fake_node_;
    ++count_;
  } catch (...) {
    node_alloc_traits::deallocate(alloc_, next_node, 1);
    throw;
  }
}

template <typename T, typename Allocator>
void List<T, Allocator>::pop_back() {
  --count_;
  BaseNode* to_erase = fake_node_.prev;
  BaseNode* before_erase = to_erase->prev;
  fake_node_.prev = before_erase;
  before_erase->next = &fake_node_;
  node_alloc_traits::destroy(alloc_, static_cast<Node*>(to_erase));
  node_alloc_traits::deallocate(alloc_, static_cast<Node*>(to_erase), 1);
}

template <typename T, typename Allocator>
void List<T, Allocator>::pop_front() {
  --count_;
  BaseNode* to_erase = fake_node_.next;
  BaseNode* after_erase = to_erase->next;
  fake_node_.next = after_erase;
  after_erase->prev = &fake_node_;
  node_alloc_traits::destroy(alloc_, static_cast<Node*>(to_erase));
  node_alloc_traits::deallocate(alloc_, static_cast<Node*>(to_erase), 1);
}

template <typename T, typename Allocator>
template <bool IsConst>
class List<T, Allocator>::BaseIterator {
 public:
  using iterator_category = std::bidirectional_iterator_tag;
  using value_type = std::conditional_t<IsConst, const T, T>;
  using pointer = value_type*;
  using reference = value_type&;
  using node_type = std::conditional_t<IsConst, const Node*, Node*>;
  using basenode_type = std::conditional_t<IsConst, const BaseNode*, BaseNode*>;
  using difference_type = std::ptrdiff_t;

  BaseIterator(basenode_type ptr) : ptr_(ptr) {}

  BaseIterator(BaseIterator& other) = default;

  BaseIterator& operator=(const BaseIterator& other) = default;

  reference operator*() const { return static_cast<node_type>(ptr_)->value; }

  pointer operator->() const { return &(static_cast<node_type>(ptr_)->value); }

  BaseIterator operator++(int);

  BaseIterator& operator++();

  BaseIterator operator--(int);

  BaseIterator& operator--();

  bool operator==(const BaseIterator& other) const {
    return ptr_ == other.ptr_;
  }

  bool operator!=(const BaseIterator& other) const {
    return !(ptr_ == other.ptr_);
  }

 private:
  basenode_type ptr_;
};

template <typename T, typename Allocator>
template <bool IsConst>
typename List<T, Allocator>::template BaseIterator<IsConst>&
List<T, Allocator>::BaseIterator<IsConst>::operator++() {
  ptr_ = ptr_->next;
  return *this;
}

template <typename T, typename Allocator>
template <bool IsConst>
typename List<T, Allocator>::template BaseIterator<IsConst>
List<T, Allocator>::BaseIterator<IsConst>::operator++(int) {
  auto copy = *this;
  ptr_ = ptr_->next;
  return copy;
}

template <typename T, typename Allocator>
template <bool IsConst>
typename List<T, Allocator>::template BaseIterator<IsConst>&
List<T, Allocator>::BaseIterator<IsConst>::operator--() {
  ptr_ = ptr_->prev;
  return *this;
}

template <typename T, typename Allocator>
template <bool IsConst>
typename List<T, Allocator>::template BaseIterator<IsConst>
List<T, Allocator>::BaseIterator<IsConst>::operator--(int) {
  auto copy = *this;
  ptr_ = ptr_->prev;
  return copy;
}
