# List

### This is `std::list<T>` implementation

## Here is the interface that the class corresponds to

### Constructors

* `List()`
* `List(size_t count, const T& value = T(), const Allocator& alloc = Allocator())`
* `explicit List(size_t count, const Allocator& alloc = Allocator())`
* `List(const List& other)`
* `List(List&& other)`
* `List(std::initializer_list<T> init, const Allocator& alloc = Allocator())`

### Destructor

### Iterators

* `begin()`
* `end()`
* `rbegin()`
* `rend()`
* `cbegin()`
* `cend()`

### operator=

* `List& operator=(const List& other)`
* `List& operator=(List&& other)`

### element access methods

* `T& front()`
* `const T& front() const`
* `T& back()`
* `const T& back() const`

### Capacity

* `bool empty()`
* `size_t size()`

### Modifiers

* `push_back(const T&)`
* `push_back(T&&)`
* `push_front(const T&)`
* `push_front(T&&)`
* `push_back()`
* `pop_front();`

> Plus all modify methods are exception-safety

## List also supports working with iterators

### Internal type `iterator` can

- Increment, decrement
- Comparisons `==,!=`
- Dereference (`operator*`). Returns `T&`
- `operator->` (Returns `T*`)
- various uses: `value_type`, `pointer`, `iterator_category`, `reference`
- Internal `const_iterator` type. The difference from the usual one is that it does not allow you to change the element lying under it. Conversion (including implicit conversion) from non-constant to constant is acceptable. But reverse conversion is not allowed.
- Internal type `reverse_iterator` (uses `std::reverse_iterator`)
- Methods for accessing iterators:
- `begin`, `cbegin` - return an iterator (constant iterator) to the first element of the list
    - `end`, `cend` - returns an iterator (constant iterator) to the "element following the last one"
- `rbegin`, `rend`, `crbegin`, `crend` - reverses iterators to the corresponding elements