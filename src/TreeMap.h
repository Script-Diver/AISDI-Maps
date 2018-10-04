#ifndef AISDI_MAPS_TREEMAP_H
#define AISDI_MAPS_TREEMAP_H

#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <utility>
#include <iostream>

namespace aisdi
{

template <typename KeyType, typename ValueType>
class TreeMap
{
public:
  using key_type = KeyType;
  using mapped_type = ValueType;
  using value_type = std::pair<const key_type, mapped_type>;
  using size_type = std::size_t;
  using reference = value_type&;
  using const_reference = const value_type&;

  class ConstIterator;
  class Iterator;
  using iterator = Iterator;
  using const_iterator = ConstIterator;

private:
  class Node{
    public:
      value_type val;
      Node* parent;
      Node* left;
      Node* right;
      short balance;

      Node(key_type key, mapped_type mapped)
        : val(std::make_pair( key, mapped )), parent(nullptr), left(nullptr), right(nullptr), balance(0) {}
      Node(value_type it) : Node(it.first, it.second) {}
      ~Node(){}
  };

  Node* root;
  unsigned int n;

  void insert(Node* node)
  {
    Node* curr = root;
    if (root == nullptr)  {  root = node;  }
    else{
      while(true){
        if(node->val.first > curr->val.first){
          if(curr->right == nullptr){
            curr->right = node;
            node->parent = curr;
            break;
          }
          else  curr = curr->right;
        }
        else{
          if(curr->left == nullptr){
            curr->left = node;
            node->parent = curr;
            break;
          }
          else  curr = curr->left;
        }
      }
    }
    n++;
  }

  void delete_tree(Node* node)
  {
    if(node != nullptr){
      delete_tree(node->left);
      delete_tree(node->right);
      delete(node);
    }
    this->root = nullptr;
  }

  Node* findN (const key_type& key) const
  {
    Node *tmp = this->root;
    while(tmp != nullptr){
      if(tmp->val.first == key) break;
      else if(tmp->val.first < key)  tmp = tmp->right;
      else tmp = tmp->left;
    }
    return tmp;
  }

  void removeN(Node* node)
  {
    if(node == nullptr) return;
    
    if(node->left == nullptr){
      if(node->parent != nullptr){
        if(node->parent->left == node) node->parent->left = node->right;
        else node->parent->right = node->right;
      }else root = node->right;
      if(node->right != nullptr) node->right->parent = node->parent;
    }
    else if(node->right == nullptr){
      if(node->parent != nullptr){
        if(node->parent->left == node) node->parent->left = node->left;
        else node->parent->right = node->left;
      }else root = node->left;
      if(node->left != nullptr) node->left->parent = node->parent;
    }
    else{
      Node *tmp = node->right; //szukamy nastepnika node (istnieje, bo node ma dwojke dzieci)

      if(tmp->left == nullptr){
        tmp->left = node->left;
        node->left->parent = tmp;
        if(node == this->root) { root = tmp; tmp->parent = nullptr; }
        else if(node->parent->left == node){ node->parent->left = tmp; tmp->parent = node->parent; }
        else if(node->parent->right == node){ node->parent->right = tmp; tmp->parent = node->parent; }
      }
      else{
        while(tmp->left != nullptr) tmp = tmp->left;
        if(tmp->parent->left == tmp) { tmp->parent->left = tmp->right; }
        if(tmp->parent->right == tmp) { tmp->parent->right = tmp->right; }
        if(tmp->right != nullptr) tmp->right->parent = tmp->parent;
        tmp->left = node->left;
        node->left->parent = tmp;
        tmp->right = node->right;
        node->right->parent = tmp;
        if(node == root) root = tmp;
        else if(node->parent->left == node) {node->parent->left = tmp;}
        else if(node->parent->right == node) {node->parent->right = tmp;}
        tmp->parent = node->parent;
      }
    }
    n--;
    delete(node);
  }

public:
  TreeMap(): root(nullptr), n(0) {}
  ~TreeMap()  {  delete_tree(root);  }

  TreeMap(std::initializer_list<value_type> list) : TreeMap()
  {
    for (auto it = list.begin(); it != list.end(); ++it)  insert(new Node(*it));
  }

  TreeMap(const TreeMap& other) : TreeMap()
  {
    for(Iterator it(other.begin()) ; it != other.end() ; it++)  insert(new Node(*it));
  }

  TreeMap(TreeMap&& other)
  {
    this->root = other.root;
    this->n = other.n;
    other.n = 0;
    other.root = nullptr;
  }


  TreeMap& operator=(const TreeMap& other)
  {
    if(&other != this){
      this->delete_tree(this->root);
      this->n = 0;
      this->root = nullptr;
      for(Iterator it(other.begin()) ; it != other.end() ; it++){  insert(new Node(*it));  }  //zrobic wywazanie
    }
    return *this;
  }

  TreeMap& operator=(TreeMap&& other)
  {
    if(&other != this){
      delete_tree(this->root);
      this->root = other.root;
      this->n = other.n;
      other.n = 0;
      other.root = nullptr;
    }
    return *this;
  }

  bool isEmpty() const  {  return n == 0;  }

  mapped_type& operator[](const key_type& key)
  {
    Node* curr = findN(key);
    if(curr == nullptr){
      curr = new Node(key, mapped_type());
      insert(curr);
    }
    return curr->val.second;
  }

  const mapped_type& valueOf(const key_type& key) const
  {
    const Node* curr = findN(key);
    if(curr == nullptr)  throw std::out_of_range("Nie ma wezla o zadanym kluczu, nie mozna zwrocic val.second - valueOf");
    return curr->val.second;
  }

  mapped_type& valueOf(const key_type& key)
  {
    Node* curr = findN(key);
    if(curr == nullptr)  throw std::out_of_range("Nie ma wezla o zadanym kluczu, nie mozna zwrocic val.second - valueOf");
    return curr->val.second;
  }

  const_iterator find(const key_type& key) const  {  return ConstIterator(findN(key), this);  }
  iterator find(const key_type& key)  {  return Iterator(findN(key), this);  }


  void remove(const key_type& key)
  {
    Node* node;
    node = findN(key);
    if(node == nullptr) throw std::out_of_range("nie mozna usunac elementu, ktory nie istnieje");
    removeN(node);
  }

  void remove(const const_iterator& it)
  {
    Iterator iter(it);
    key_type key = iter.getKey();
    remove(key);
  }

  size_type getSize() const  {  return n;  }

  bool operator==(const TreeMap& other) const
  {
    Iterator it1(this->cbegin()), it2(other.cbegin());
    bool flag = true;

    if(this->n != other.n) flag = false;
    else{
      while(it1 != end()){
        if(it1->first != it2->first || it1->second != it2->second) {  flag = false;  break;  }
        it1++;
        it2++;
      }
    }
    return flag;
  }

  bool operator!=(const TreeMap& other) const  {  return !(*this == other);  }

  const_iterator cbegin() const
  {
    auto tmp = root;
    if(root != nullptr){  while(tmp->left != nullptr) tmp = tmp->left;  }
    return ConstIterator(tmp, this);
  }

  const_iterator cend() const  {  return ConstIterator(nullptr, this);  }
  const_iterator begin() const  {  return cbegin();  }
  const_iterator end() const  {  return cend();  }
  iterator begin() { return Iterator(this->cbegin()); }
  iterator end() { return Iterator(this->cend()); }

  void print(){ this->print(root); }
  void print(Node *node){
    if(node == nullptr) return;
    print(node->left);
    std::cout<<node->val.second<<std::endl;
    print(node->right);
  }
};

template <typename KeyType, typename ValueType>
class TreeMap<KeyType, ValueType>::ConstIterator
{
protected:
  Node *curr;
  const TreeMap *tree;

public:
  using reference = typename TreeMap::const_reference;
  using iterator_category = std::bidirectional_iterator_tag;
  using value_type = typename TreeMap::value_type;
  using pointer = const typename TreeMap::value_type*;

  explicit ConstIterator() = delete;
  ConstIterator(Node* node, const TreeMap *t)  : curr(node), tree(t) {}
  ConstIterator(const ConstIterator& other) = default;

  ConstIterator& operator++()
  {
    if(this->curr != nullptr){
      if(this->curr->right != nullptr){
        this->curr = this->curr->right;
        while(this->curr->left != nullptr)  this->curr = this->curr->left;
      }
      else{
        while(this->curr->parent != nullptr && this->curr->parent->right == this->curr)  this->curr = this->curr->parent;
        this->curr = this->curr->parent;
      }
    }
    else  throw std::out_of_range("iterator na nullptr - nie mozna wykonac operacji ++");
    return *this;
  }

  ConstIterator operator++(int)
  {
    auto tmp = *this;
    operator++();
    return tmp;
  }

  ConstIterator& operator--()
  {
    Node *tmp = tree->root;

    if(this->curr != nullptr){
      if(this->curr->left != nullptr){
        this->curr = this->curr->left;
        while(this->curr->right != nullptr)  this->curr = this->curr->right;
      }
      else{
        while(tmp != nullptr && tmp->left != nullptr) tmp = tmp->left;
        if(this->curr == tmp)  throw std::out_of_range("iterator na najmniejszym wezle - nie mozna wykonac operacji --");
        while(this->curr->parent != nullptr && this->curr->parent->left == this->curr)  this->curr = this->curr->parent;
        this->curr = this->curr->parent;
      }
    }
    else if(tree->root != nullptr){
      this->curr = tree->root;
      while(this->curr->right != nullptr) this->curr = this->curr->right;
    }
    else  throw std::out_of_range("brak korzenia - nie mozna wykonac operacji --");
    return *this;
  }

  ConstIterator operator--(int)
  {
    auto tmp = *this;
    operator--();
    return tmp;
  }

  reference operator*() const
  {
    if(this->curr == nullptr)  throw std::out_of_range("iterator wskazuje na nullptr - nie mozna wykonac operacji *");
    return this->curr->val;
  }

  pointer operator->() const  {  return &this->operator*();  }
  bool operator==(const ConstIterator& other) const  {  return (this->tree == other.tree && this->curr == other.curr);  }
  bool operator!=(const ConstIterator& other) const  {  return !(*this == other);  }
  key_type getKey()  {  if(this->curr == nullptr) throw std::out_of_range("brak klucza"); return this->curr->val.first;  }
};

template <typename KeyType, typename ValueType>
class TreeMap<KeyType, ValueType>::Iterator : public TreeMap<KeyType, ValueType>::ConstIterator
{
public:
  using reference = typename TreeMap::reference;
  using pointer = typename TreeMap::value_type*;

  explicit Iterator() = delete;
  explicit Iterator(Node *node, TreeMap *t) : ConstIterator(node, t) {}
  Iterator(const ConstIterator& other) : ConstIterator(other) {}

  Iterator& operator++()
  {
    ConstIterator::operator++();
    return *this;
  }

  Iterator operator++(int)
  {
    auto result = *this;
    ConstIterator::operator++();
    return result;
  }

  Iterator& operator--()
  {
    ConstIterator::operator--();
    return *this;
  }

  Iterator operator--(int)
  {
    auto result = *this;
    ConstIterator::operator--();
    return result;
  }

  pointer operator->() const
  {
    return &this->operator*();
  }

  reference operator*() const
  {
    // ugly cast, yet reduces code duplication.
    return const_cast<reference>(ConstIterator::operator*());
  }

  key_type getKey()  {  if(this->curr == nullptr) throw std::out_of_range("brak klucza"); return this->curr->val.first;  }
};

}

#endif /* AISDI_MAPS_MAP_H */
