#pragma once

#include <algorithm>
#include <cstring>
#include <iostream>
#include <memory>
#include <optional>
#include <ostream>
#include <set>
#include <string>
#include <utility>
#include <vector>

template <typename _Value> class trie_node {
public:
  using key_type = char;
  using value_type = _Value;

  trie_node(char key, std::optional<_Value> value = std::nullopt,
            trie_node *parent = nullptr) noexcept;
  trie_node(const trie_node &other_node) noexcept;
  ~trie_node() noexcept;

  static std::pair<trie_node<_Value> *, bool>
  make_trie_node(trie_node<_Value> *base, std::string full_key,
                 std::optional<_Value> value = std::nullopt);

  // ###### path ######
  trie_node<_Value> *get_parent() const noexcept;
  trie_node<_Value> *get_child(const char key) const noexcept;

  // ###### print ######
  void print_tree_from_this(const int level = 0) const noexcept;

  // ###### get ######
  std::optional<_Value> &get_value() noexcept;
  const std::optional<_Value> &get_value() const noexcept;
  std::string get_key() const noexcept;
  char get_node_key() const noexcept;
  std::vector<char> get_children_keys() const noexcept;
  bool has_child(char child_key) const noexcept;
  bool has_previous_child(char child_key) const noexcept;
  bool has_children() const noexcept;

  // ###### Modifiers ######
  void set_parent(trie_node<_Value> *new_parent) noexcept;
  void assign_value(const _Value value) noexcept;
  void erase_value() noexcept;
  void erase_child(char key) noexcept;
  void clear_children() noexcept;
  trie_node<_Value> *
  insert_child(const char key,
               std::optional<_Value> value = std::nullopt) noexcept;

  // ###### operators ######
  friend bool operator<(const trie_node &node1, const trie_node &node2) {
    return node1._key < node2._key;
  }
  friend std::ostream &operator<<(std::ostream &os, const trie_node &node) {
    if (node._value.has_value())
      return os << node.get_key() << " :: " << node._value.value();
    return os << node.get_key() << " :: "
              << "none";
  }

  struct compare {
    bool operator()(const trie_node *a, const trie_node *b) const {
      return a->_key < b->_key;
    }
  };

private:
  char _key;
  std::optional<_Value> _value;
  trie_node<_Value> *_parent;
  std::set<trie_node<_Value> *, compare> _children;

  trie_node(trie_node<_Value> *base, std::string full_key,
            std::optional<_Value> value = std::nullopt) noexcept;

  trie_node<_Value> *insert_child(trie_node<_Value> *child) noexcept;
};

template <typename _Value>
trie_node<_Value>::trie_node(char key, std::optional<_Value> value,
                             trie_node<_Value> *parent) noexcept {
  _key = key;
  _value = value;
  _parent = parent;
}

template <typename _Value>
trie_node<_Value>::trie_node(trie_node<_Value> *base, std::string full_key,
                             std::optional<_Value> value) noexcept {

  _key = full_key.back();
  _value = value;
  base->insert_child(this);
}

template <typename _Value>
trie_node<_Value>::trie_node(const trie_node<_Value> &other_node) noexcept {
  _key = other_node._key;
  _value = std::nullopt;
  _parent = nullptr;
  if (other_node.get_value() != std::nullopt)
    _value = std::optional<_Value>(other_node._value.value());
  for (auto it = other_node._children.begin(); it != other_node._children.end();
       ++it) {
    _children.insert(new trie_node<_Value>(*(*it)));
    (*_children.find((*it)))->set_parent(this);
  }
}

template <typename _Value> trie_node<_Value>::~trie_node() noexcept {
  clear_children();
}

// ###### path ######
template <typename _Value>
trie_node<_Value> *trie_node<_Value>::get_parent() const noexcept {
  return _parent;
}

template <typename _Value>
trie_node<_Value> *trie_node<_Value>::get_child(const char key) const noexcept {
  auto item =
      std::find_if(_children.begin(), _children.end(),
                   [&key](const trie_node *node) { return node->_key == key; });

  if (item != _children.end())
    return (*item);

  return nullptr;
}

// ###### print ######
template <typename _Value>
void trie_node<_Value>::print_tree_from_this(const int level) const noexcept {
  if (level == 0)
    std::cout << " " << this->_key << std::endl;
  std::string level_marker = "";
  for (int l = 0; l < level; ++l)
    level_marker += " │ ";

  for (auto iterator = _children.begin(); iterator != _children.end();
       ++iterator) {
    std::cout << level_marker;
    if ((*(*iterator)).get_value().has_value())
      std::cout << " ├─ " << (*(*iterator)).get_node_key()
                << " :: " << (*(*iterator)).get_value().value();
    else
      std::cout << " ├─ " << (*(*iterator)).get_node_key()
                << " :: none";
    if ((*iterator)->has_children()) {
      std::cout << std::endl;
      (*iterator)->print_tree_from_this(level + 1);
    }
    std::cout << std::endl;
  }
  std::cout << level_marker;
}

// ###### get ######

template <typename _Value>
const std::optional<_Value> &trie_node<_Value>::get_value() const noexcept {
  return _value;
}

template <typename _Value>
std::optional<_Value> &trie_node<_Value>::get_value() noexcept {
  return _value;
}

template <typename _Value>
std::string trie_node<_Value>::get_key() const noexcept {
  std::string key = "";
  auto current_node = this;
  while (current_node != nullptr) {
    key = current_node->get_node_key() + key;
    current_node = current_node->get_parent();
  }
  return key;
}

template <typename _Value>
char trie_node<_Value>::get_node_key() const noexcept {
  return _key;
}

template <typename _Value>
std::vector<char> trie_node<_Value>::get_children_keys() const noexcept {
  std::vector<char> keys;
  for (auto iterator = _children.begin(); iterator != _children.end();
       ++iterator) {
    keys.push_back((*iterator)->get_node_key());
  }
  return keys;
}

template <typename _Value>
bool trie_node<_Value>::has_child(char child_key) const noexcept {
  return get_child(child_key) != nullptr;
}

template <typename _Value>
bool trie_node<_Value>::has_previous_child(char child_key) const noexcept {
  if (!has_children())
    return false;
  std::vector<char> keys = get_children_keys();
  auto ckey = std::find(keys.rbegin(), keys.rend(), child_key);
  ckey++;
  return ckey != keys.rend();
}

template <typename _Value>
bool trie_node<_Value>::has_children() const noexcept {
  return _children.size() != 0;
}

// ###### set ######
template <typename _Value>
void trie_node<_Value>::set_parent(trie_node<_Value> *new_parent) noexcept {
  _parent = new_parent;
}

template <typename _Value>
void trie_node<_Value>::assign_value(const _Value value) noexcept {
  _value = std::optional<_Value>(value);
}

template <typename _Value> void trie_node<_Value>::erase_value() noexcept {
  _value = std::nullopt;
}

template <typename _Value>
void trie_node<_Value>::erase_child(char key) noexcept {
  if (has_child(key)) {
    auto it = _children.find(get_child(key));
    delete get_child(key);
    _children.erase(it);
  }
}

template <typename _Value> void trie_node<_Value>::clear_children() noexcept {
  for (auto iterator = _children.begin(); iterator != _children.end();
       ++iterator) {
    delete (*iterator);
  }
  _children.clear();
}

template <typename _Value>
trie_node<_Value> *
trie_node<_Value>::insert_child(const char key,
                                std::optional<_Value> value) noexcept {
  return insert_child(new trie_node(key, value));
}

template <typename _Value>
trie_node<_Value> *
trie_node<_Value>::insert_child(trie_node<_Value> *child) noexcept {
  child->_parent = this;
  _children.insert(child);
  return (child);
}