#include "trie.h"

#include <cassert>
#include <stdlib.h>
#include <time.h>

void test_trie_iterators(trie<int> new_trie) {
  std::cout << "### start of test_trie_iterators ###" << std::endl << std::endl;

  std::string normal = "";
  for (auto it = new_trie.begin(); it != new_trie.end(); it++)
    normal = normal + (*it).get_node_key() +
             std::to_string((*it).get_value().value());

  std::string reversed = "";
  for (auto rit = new_trie.rbegin(); rit != new_trie.rend(); rit++)
    reversed = (*rit.base()).get_node_key() +
               std::to_string((*rit.base()).get_value().value()) + reversed;

  std::string const_normal = "";
  for (auto cit = new_trie.cbegin(); cit != new_trie.cend(); cit++)
    const_normal = const_normal + (*cit).get_node_key() +
                   std::to_string((*cit).get_value().value());

  std::string const_reversed = "";
  for (auto crit = new_trie.crbegin(); crit != new_trie.crend(); crit++)
    const_reversed = (*crit.base()).get_node_key() +
                     std::to_string((*crit.base()).get_value().value()) +
                     const_reversed;

  assert(normal == "a1b2c3d4e5f6g7");
  std::cout << "iterator: check" << std::endl;

  assert(reversed == normal);
  std::cout << "reversed_iterator: check" << std::endl;

  assert(const_normal == normal);
  std::cout << "const_iterator: check" << std::endl;

  assert(const_reversed == normal);
  std::cout << "const_reversed_iterator: check" << std::endl;

  std::cout << std::endl << "### end of test_trie_iterators ###" << std::endl;
}

void test_trie_capacity(trie<int> new_trie) {
  std::cout << "### start of test_trie_capacity ###" << std::endl << std::endl;

  assert(new_trie.size() == 7);
  std::cout << "size: check" << std::endl;

  assert(new_trie.max_size() == 59652323);
  std::cout << "max_size: check" << std::endl;

  assert(new_trie.empty() == 0);
  new_trie.clear();
  assert(new_trie.empty() == 1);
  std::cout << "empty: check" << std::endl;

  std::cout << std::endl << "### end of test_trie_capacity ###" << std::endl;
}

void test_trie_lookup(trie<int> new_trie) {
  std::cout << "### start of test_trie_lookup ###" << std::endl << std::endl;

  assert(new_trie.count("ab") == 1);
  assert(new_trie.count("xyz") == 0);
  std::cout << "count: check" << std::endl;

  assert(new_trie.find("ab") != new_trie.end());
  assert((*(new_trie.find("ab"))).get_value().value() == 2);
  assert(new_trie.find("xyz") == new_trie.end());
  std::cout << "find: check" << std::endl;

  assert(new_trie.contains("ab") == 1);
  assert(new_trie.contains("xyz") == 0);
  std::cout << "contains: check" << std::endl;

  std::cout << std::endl << "### end of test_trie_lookup ###" << std::endl;
}

void test_trie_element_access(trie<int> new_trie) {
  std::cout << "### start of test_trie_lookup ###" << std::endl << std::endl;

  assert(new_trie.at("ab").value_or(-1) == 2);
  bool exception_thrown = false;
  try {
    new_trie.at("xyz").value_or(-1);
  } catch (std::out_of_range e) {
    exception_thrown = true;
  }
  assert(exception_thrown);
  std::cout << "at: check" << std::endl;

  assert(new_trie["ab"].value_or(-1) == 2);
  assert(new_trie["xyz"].value_or(-1) == 0);
  assert((new_trie["ab"] = 20) == 20);
  assert(new_trie["ab"].value_or(-1) == 20);

  assert((new_trie["xyz"] = 30) == 30);
  assert(new_trie["xyz"].value_or(-1) == 30);
  std::cout << "[]: check" << std::endl;

  std::cout << std::endl << "### end of test_trie_lookup ###" << std::endl;
}

void test_trie_inserts(trie<int> new_trie) {
  std::cout << "### start of test_trie_inserts ###" << std::endl << std::endl;

  assert(new_trie.insert("e", 5).second == false);
  assert(new_trie.insert("h", 5).second == true);
  assert(new_trie.insert("hello", 5).second == true);
  assert(new_trie.insert("hell", 5).second == true);
  std::cout << "insert: check" << std::endl;
  assert(new_trie.insert_or_assign("e", 5).second == false);
  assert(new_trie.insert_or_assign("h", 5).second == false);
  assert(new_trie.insert_or_assign("i", 5).second == true);
  std::cout << "insert_or_assign: check" << std::endl;

  std::cout << std::endl << "### end of test_trie_inserts ###" << std::endl;
}

void test_trie_clear_erase(trie<int> new_trie) {
  std::cout << "### start of test_trie_clear_erase ###" << std::endl;

  assert(
      strcmp((*new_trie.erase(new_trie.find("ab"))).get_key().c_str(), "ac"));
  std::cout << new_trie.find("ab")->get_key() << std::endl;
  assert(new_trie.find("ab") == new_trie.end());
  assert(new_trie.erase("xyz") == 0);
  assert(new_trie.erase("ab") == 0);
  assert(new_trie.erase("ac") == 1);
  std::cout << "erase: check" << std::endl;
  assert(new_trie.find("ad") != new_trie.end());
  new_trie.clear();
  assert(new_trie.find("ad") == new_trie.end());
  assert(new_trie.empty() == true);
  std::cout << "clear: check" << std::endl;

  std::cout << std::endl << "### end of test_trie_clear_erase ###" << std::endl;
}

void test_trie_emplace(trie<int> new_trie) {
  std::cout << "### start of test_trie_emplace ###" << std::endl << std::endl;

  auto pair = new_trie.emplace("ab", 3);
  assert(!pair.second);
  assert((*pair.first).get_value().value_or(-1) == 2);

  pair = new_trie.emplace("abcdef", 10);
  assert(pair.second);
  assert((*pair.first).get_value().value_or(-1) == 10);

  pair = new_trie.emplace("abcde", 20);
  assert(pair.second);
  assert((*pair.first).get_value().value_or(-1) == 20);
  std::cout << "emplace: check" << std::endl;

  std::cout << std::endl << "### end of test_trie_emplace ###" << std::endl;
}

int main() {
  std::cout << "### start of main ###" << std::endl;

  trie<int> new_trie;
  new_trie.insert_or_assign("a", 1);
  new_trie.insert_or_assign("e", 5);
  new_trie.insert_or_assign("f", 6);
  new_trie.insert_or_assign("ab", 2);
  new_trie.insert_or_assign("ac", 3);
  new_trie.insert_or_assign("ad", 4);
  new_trie.insert_or_assign("fg", 7);
  new_trie.print_tree();

  test_trie_iterators(new_trie);
  test_trie_capacity(new_trie);
  test_trie_lookup(new_trie);
  test_trie_element_access(new_trie);
  test_trie_inserts(new_trie);
  test_trie_clear_erase(new_trie);
  test_trie_emplace(new_trie);

  std::cout << std::endl << "### end of main ###" << std::endl;
  return 0;
}