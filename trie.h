#pragma once

#include "trie_node.h"
#include <cctype>

template <typename _Value> class trie {
public:
  using key_type = std::string;
  using mapped_type = _Value;
  using value_type = trie_node<_Value>;
  using size_type = unsigned int;

  template <typename _IterValue> struct trie_iterator {
    using iterator_category = std::bidirectional_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = _IterValue;
    using pointer = _IterValue *;
    using reference = _IterValue &;

    enum Direction { forward = true, backward = false };

    trie_iterator(pointer ptr) : _ptr(ptr), _current_key('\0') {}
    reference operator*() const { return *_ptr; }
    pointer operator->() { return _ptr; }

    trie_iterator &operator++() {
      step(forward);
      while (_ptr &&
             (_ptr->get_value() == std::nullopt || _current_key != '\0')) {
        step(forward);
      }
      return *this;
    }
    trie_iterator operator++(int) {
      trie_iterator t = *this;
      ++(*this);
      return t;
    }
    trie_iterator &operator--() {
      step(backward);
      while (_ptr && (_ptr->get_value() == std::nullopt ||
                      _ptr->has_previous_child(_current_key))) {
        step(backward);
      }
      return *this;
    }
    trie_iterator operator--(int) {
      trie_iterator t = *this;
      --(*this);
      return t;
    }

    friend bool operator==(const trie_iterator &it1, const trie_iterator &it2) {
      return it1._ptr == it2._ptr;
    };
    friend bool operator!=(const trie_iterator &it1, const trie_iterator &it2) {
      return it1._ptr != it2._ptr;
    };

  private:
    pointer _ptr;
    char _current_key;

    pointer get_next_child(Direction dir) {
      std::vector<char> keys = _ptr->get_children_keys();

      if (_current_key == '\0') {
        return _ptr->get_child((dir ? keys.front() : keys.back()));
      } else {
        auto ckey = std::find(keys.begin(), keys.end(), _current_key);
        if (dir) {
          ckey++;
          if (ckey != keys.end())
            return _ptr->get_child((*ckey));
        } else {
          if (ckey != keys.begin())
            return _ptr->get_child((*(--ckey)));
        }
      }
      return nullptr;
    }

    void step(Direction dir) {
      if (_ptr->has_children()) {
        auto ptr = get_next_child(dir);
        if (ptr != nullptr) {
          _current_key = '\0';
          _ptr = ptr;
          return;
        }
      }
      _current_key = _ptr->get_node_key();
      _ptr = _ptr->get_parent();
    }
  };

  using iterator = trie_iterator<value_type>;
  using const_iterator = trie_iterator<const value_type>;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  trie() noexcept;
  trie(const trie<_Value> &other_trie) noexcept;
  ~trie() noexcept;

  // ###### Printers ######
  void print_tree() noexcept;

  // ###### Element access ######
  std::optional<_Value> &at(const std::string &key);
  const std::optional<_Value> &at(const std::string &key) const;
  std::optional<_Value> &operator[](const std::string &key);
  std::optional<_Value> &operator[](std::string &&key);

  // ###### Iterators ######
  iterator begin() noexcept;
  iterator end() noexcept;
  const_iterator cbegin() const noexcept;
  const_iterator cend() const noexcept;
  reverse_iterator rbegin() noexcept;
  reverse_iterator rend() noexcept;
  const_reverse_iterator crbegin() const noexcept;
  const_reverse_iterator crend() const noexcept;

  // ###### Capacity ######
  bool empty() const noexcept;
  size_type size() const noexcept;
  size_type max_size() const noexcept;

  // ###### Modifiers ######
  void clear() noexcept;
  std::pair<iterator, bool> insert(const std::string &key, const _Value &value);
  std::pair<iterator, bool> insert(std::string &&key, _Value &&value);
  std::pair<iterator, bool> insert_or_assign(const std::string &key,
                                             _Value &&value);
  std::pair<iterator, bool> insert_or_assign(std::string &&key, _Value &&value);
  template <typename... Args> std::pair<iterator, bool> emplace(Args &&...args);
  iterator erase(iterator pos);
  size_type erase(const std::string &key);

  // ###### Lookup ######
  size_type count(const std::string &key) const;
  const_iterator find(const std::string &key) const;
  iterator find(const std::string &key);
  bool contains(const std::string &key) const;

private:
  trie_node<_Value> *_base_node;

  std::pair<iterator, bool> emplacer(std::string key, std::optional<_Value> value = std::nullopt);

  // ###### Utilities ######
  trie_node<_Value> *move_up(trie_node<_Value> *current_node) const noexcept;
  trie_node<_Value> *move_down(char key,
                               trie_node<_Value> *current_node) const noexcept;
  std::pair<trie_node<_Value> *, bool>
  insert_node(trie_node<_Value> *current_node, char key,
              const std::optional<_Value> value = std::nullopt) noexcept;
  void erase_at(iterator pos) noexcept;
  trie_node<_Value> *release_path(trie_node<_Value> *current_node) noexcept;
};

// ###### trie ######

template <typename _Value> trie<_Value>::trie() noexcept {
  _base_node = new trie_node<_Value>('\0');
}

template <typename _Value>
trie<_Value>::trie(const trie<_Value> &other_trie) noexcept {
  _base_node = new trie_node<_Value>(*other_trie._base_node);
}

template <typename _Value> trie<_Value>::~trie() noexcept { delete _base_node; }

// ###### Printers ######

template <typename _Value> void trie<_Value>::print_tree() noexcept {
  _base_node->print_tree_from_this();
}

// ###### Element access ######

template <typename _Value>
std::optional<_Value> &trie<_Value>::at(const std::string &key) {
  auto it = trie<_Value>::find(key);
  if (it == nullptr)
    throw std::out_of_range("");
  return (*it).get_value();
}

template <typename _Value>
const std::optional<_Value> &trie<_Value>::at(const std::string &key) const {
  auto it = trie<_Value>::find(key);
  if (it == nullptr)
    throw std::out_of_range("");
  return (*it).get_value();
}

template <typename _Value>
std::optional<_Value> &trie<_Value>::operator[](const std::string &key) {
  auto it = trie<_Value>::find(key);
  if (it == nullptr)
    it = trie<_Value>::insert(key, _Value());
  return (*it).get_value();
}

template <typename _Value>
std::optional<_Value> &trie<_Value>::operator[](std::string &&key) {
  auto it = trie<_Value>::find(key);
  if (it == nullptr)
    it = trie<_Value>::insert(std::move(key), std::move(_Value())).first;
  return (*it).get_value();
}

// ###### Iterators ######

template <typename _Value>
typename trie<_Value>::iterator trie<_Value>::begin() noexcept {
  trie<_Value>::iterator it = trie<_Value>::iterator(_base_node);
  ++it;
  return it;
}

template <typename _Value>
typename trie<_Value>::iterator trie<_Value>::end() noexcept {
  return trie<_Value>::iterator(nullptr);
}

template <typename _Value>
typename trie<_Value>::const_iterator trie<_Value>::cbegin() const noexcept {
  trie<_Value>::const_iterator cit = trie<_Value>::const_iterator(_base_node);
  ++cit;
  return cit;
}

template <typename _Value>
typename trie<_Value>::const_iterator trie<_Value>::cend() const noexcept {
  return trie<_Value>::const_iterator(nullptr);
}

template <typename _Value>
typename trie<_Value>::reverse_iterator trie<_Value>::rbegin() noexcept {
  trie<_Value>::iterator it = trie<_Value>::iterator(_base_node);
  while ((*it).has_children()) {
    --it;
  }
  return trie<_Value>::reverse_iterator(it);
}

template <typename _Value>
typename trie<_Value>::reverse_iterator trie<_Value>::rend() noexcept {
  return trie<_Value>::reverse_iterator(nullptr);
}

template <typename _Value>
typename trie<_Value>::const_reverse_iterator
trie<_Value>::crbegin() const noexcept {
  trie<_Value>::const_iterator cit = trie<_Value>::const_iterator(_base_node);
  while ((*cit).has_children()) {
    --cit;
  }
  return trie<_Value>::const_reverse_iterator(cit);
}

template <typename _Value>
typename trie<_Value>::const_reverse_iterator
trie<_Value>::crend() const noexcept {
  return trie<_Value>::const_reverse_iterator(nullptr);
}

// ###### Capacity ######

template <typename _Value> bool trie<_Value>::empty() const noexcept {
  return !_base_node->has_children();
}

template <typename _Value>
typename trie<_Value>::size_type trie<_Value>::size() const noexcept {
  trie<_Value>::size_type size = 0;
  for (auto cit = this->cbegin(); cit != this->cend(); cit++)
    size++;
  return size;
}

template <typename _Value>
typename trie<_Value>::size_type trie<_Value>::max_size() const noexcept {
  unsigned long max_uint = std::numeric_limits<unsigned int>().max();
  return max_uint / sizeof(trie_node<_Value>);
}

// ###### Modifiers ######

template <typename _Value> void trie<_Value>::clear() noexcept {
  _base_node->clear_children();
}

template <typename _Value>
std::pair<typename trie<_Value>::iterator, bool>
trie<_Value>::insert(const std::string &key, const _Value &value) {
  return emplacer(key, std::optional<_Value>(value));
}

template <typename _Value>
std::pair<typename trie<_Value>::iterator, bool>
trie<_Value>::insert(std::string &&key, _Value &&value) {
  return emplacer(std::move(key), std::optional<_Value>(std::move(value)));
}

template <typename _Value>
std::pair<typename trie<_Value>::iterator, bool>
trie<_Value>::insert_or_assign(const std::string &key, _Value &&value) {
  return insert_or_assign(std::move(key), std::move(value));
}

template <typename _Value>
std::pair<typename trie<_Value>::iterator, bool>
trie<_Value>::insert_or_assign(std::string &&key, _Value &&value) {
  auto pair = insert(std::move(key), std::move(value));
  (*(pair.first)).assign_value(value);
  return pair;
}

template <typename _Value>
template <typename... Args>
std::pair<typename trie<_Value>::iterator, bool>
trie<_Value>::emplace(Args &&...args) {
  return emplacer(args...);
}

template <typename _Value>
std::pair<typename trie<_Value>::iterator, bool>
trie<_Value>::emplacer(std::string key, std::optional<_Value> value) {
  bool success = false;
  if (key == "")
    return std::pair<trie<_Value>::iterator, bool>(nullptr, success);
  if (value == std::nullopt)
    value = std::optional<_Value>(_Value());
  char k;
  trie_node<_Value> *current_node = _base_node;

  std::string mutable_key(key);
  char end_char = mutable_key.back();
  mutable_key.erase(mutable_key.length() - 1, 1);

  for (auto str_cit = mutable_key.cbegin(); str_cit != mutable_key.cend();
       ++str_cit) {
    current_node = insert_node(current_node, (*str_cit)).first;
  }

  if (current_node->has_child(end_char)) {
    current_node = current_node->get_child(end_char);
    if(current_node->get_value() == std::nullopt) {
      current_node->assign_value(value.value());
      success = true;
    } else
      success = false;
  } else {
    auto pair =
        insert_node(current_node, end_char, std::optional<_Value>{value.value()});
    current_node = pair.first;
    success = pair.second;
  }
  return std::pair<trie<_Value>::iterator, bool>(
      trie<_Value>::iterator(current_node), success);
}

template <typename _Value>
typename trie<_Value>::iterator
trie<_Value>::erase(trie<_Value>::iterator pos) {
  auto it = pos;
  ++it;
  erase_at(pos);
  return it;
}

template <typename _Value>
typename trie<_Value>::size_type trie<_Value>::erase(const std::string &key) {
  auto it = trie<_Value>::find(key);
  if (it == trie<_Value>::end())
    return 0;
  erase_at(it);
  return 1;
}

// ###### Lookup ######
template <typename _Value>
typename trie<_Value>::size_type
trie<_Value>::count(const std::string &key) const {
  auto it = trie<_Value>::find(key);
  if (it == trie<_Value>::cend())
    return 0;
  return 1;
}

template <typename _Value>
typename trie<_Value>::const_iterator
trie<_Value>::find(const std::string &key) const {
  auto current_node = _base_node;
  for (auto str_cit = key.cbegin(); str_cit != key.cend(); ++str_cit) {
    current_node = move_down(*str_cit, current_node);
    if (current_node == nullptr)
      return trie<_Value>::cend();
  }
  return trie<_Value>::const_iterator(current_node);
}

template <typename _Value>
typename trie<_Value>::iterator trie<_Value>::find(const std::string &key) {
  auto current_node = _base_node;
  for (auto str_cit = key.cbegin(); str_cit != key.cend(); ++str_cit) {
    current_node = move_down(*str_cit, current_node);
    if (current_node == nullptr)
      return trie<_Value>::end();
  }
  return trie<_Value>::iterator(current_node);
}

template <typename _Value>
bool trie<_Value>::contains(const std::string &key) const {
  return trie<_Value>::count(key);
}

// ###### Utilities ######

template <typename _Value>
trie_node<_Value> *
trie<_Value>::move_up(trie_node<_Value> *current_node) const noexcept {
  if (current_node == nullptr)
    return nullptr;
  return current_node->get_parent();
}

template <typename _Value>
trie_node<_Value> *
trie<_Value>::move_down(const char key,
                        trie_node<_Value> *current_node) const noexcept {
  if (current_node == nullptr)
    return nullptr;
  return current_node->get_child(key);
}

template <typename _Value>
std::pair<trie_node<_Value> *, bool>
trie<_Value>::insert_node(trie_node<_Value> *current_node, const char key,
                          std::optional<_Value> value) noexcept {
  bool success = false;
  if (!current_node->has_child(key)) {
    current_node->insert_child(key, value);
    success = true;
  }
  auto node_ptr = move_down(key, current_node);
  return std::pair<trie_node<_Value> *, bool>(node_ptr, success);
}

template <typename _Value> void trie<_Value>::erase_at(iterator pos) noexcept {
  auto current_node = &(*pos);
  char k = current_node->get_node_key();
  current_node = move_up(current_node);
  current_node->erase_child(k);
  release_path(current_node);
}

template <typename _Value>
trie_node<_Value> *
trie<_Value>::release_path(trie_node<_Value> *current_node) noexcept {
  while (!current_node->has_children() &&
         current_node->get_value() == std::nullopt) {
    char key = current_node->get_node_key();
    current_node = move_up(current_node);
    current_node->erase_child(key);
  }
  return current_node;
}