#ifndef AISDI_MAPS_HASHMAP_H
#define AISDI_MAPS_HASHMAP_H

#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <utility>

namespace aisdi
{

template <typename KeyType, typename ValueType>
class HashMap
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
    Node *prev;
    Node *next;

    Node(key_type key, mapped_type mapped)
      : val(std::make_pair(key , mapped)), prev(nullptr), next(nullptr) {}
    Node(value_type it) : Node(it.first, it.second) {}
    ~Node(){}
  };

  Node **tab;
  static const size_type size = 870173;
  size_type n; //liczba elementow

  void delete_nodes()
  {
    Node *tmp1, *tmp2;
    if(this->tab != nullptr){
            this->n = 0;
      for(size_type i = 0 ; i < size ; i++){
        if((tmp1 = tab[i]) != nullptr){
          while(tmp1 != nullptr){
            tmp2 = tmp1;
            tmp1 = tmp1->next;
            delete tmp2;
          }
          tmp1 = nullptr;
        }
        tab[i] = nullptr;
      }
    }
  }

  void delete_hashMap()
  {
    delete_nodes();
    delete []tab;
  }

  size_type hashIdx(key_type key) const
  {
    size_type i = (std::hash<key_type>{}(key)) % size;
    return i;
  }

  const_iterator findC(const key_type& key) const
  {
      size_type i = this->hashIdx(key);
      Node *tmp = this->tab[i];

      while(tmp != nullptr){
        if(tmp->val.first == key) break;
        tmp = tmp->next;
      }
      if(tmp == nullptr) i = size;
      return ConstIterator(tmp, this, i);
  }

public:
  HashMap(): n(0)
  {
    tab = new Node*[size+1];  //+1 element na straznika
    for(size_type i = 0 ; i < size + 1 ; ++i) tab[i] = nullptr;
  }

  ~HashMap() {  delete_hashMap();  }

  HashMap(std::initializer_list<value_type> list): HashMap()
  {
    for (auto& tmp : list) operator[](tmp.first) = tmp.second;
  }

  HashMap(const HashMap& other): HashMap()
  {
    for (auto it = other.begin(); it != other.end(); ++it)
      operator[](it->first) = it->second;
  }

  HashMap(HashMap&& other)
  {
    this->tab = other.tab;
    this->n = other.n;
    other.tab = nullptr;
    other.n = 0;
  }

  HashMap& operator=(const HashMap& other)
  {
    if(this != &other){
      this->delete_nodes();
      for(auto it = other.begin(); it != other.end(); ++it)
        operator[](it->first) = it->second;
    }
    return *this;
  }

  HashMap& operator=(HashMap&& other)
  {
    if(this != &other){
      this->delete_hashMap();
      this->tab = other.tab;
      this->n = other.n;
      other.tab = nullptr;
      other.n = 0;
    }
    return *this;
  }

  bool isEmpty() const
  {
    return (this->n == 0);
  }

  mapped_type& operator[](const key_type& key)
  {
    size_type i = hashIdx(key);
    Node *tmp = this->tab[i];
    Node *tmp2;

    while(tmp != nullptr){
      if(tmp->val.first == key) break;
      tmp = tmp->next;
    }

    if(tmp == nullptr){
      tmp = new Node(key, mapped_type());
      if(this->tab[i] == nullptr) this->tab[i] = tmp;
      else{
        tmp2 = this->tab[i];
        while(tmp2->next != nullptr) tmp2 = tmp2->next;
        tmp2->next = tmp;
        tmp->prev = tmp2;
      }
      (this->n)++;
    }
    return tmp->val.second;
  }

  const mapped_type& valueOf(const key_type& key) const
  {
    const_iterator it = this->findC(key);
    if(it == cend())  throw std::out_of_range("nie znaleziono wezla o zadanym kluczu - valueOf const");
    return it->second;
  }

  mapped_type& valueOf(const key_type& key)
  {
    iterator it(this->findC(key));
    if(it == end())  throw std::out_of_range("nie znaleziono wezla o zadanym kluczu - valueOf");
    return it->second;
  }

  const_iterator find(const key_type& key) const
  {
    return this->findC(key);
  }

  iterator find(const key_type& key)
  {
    const_iterator it = this->findC(key);
    return Iterator(it);
  }

  void remove(const key_type& key) {  remove(this->find(key));  }

  void remove(const const_iterator& it)
  {
    if(it == this->cend()) throw std::out_of_range("Nie mozna usunac straznika");

    size_type i = hashIdx(it->first)%size;
    Node *tmp = this->tab[i];

    while(it->first != tmp->val.first) tmp = tmp->next;

    if(tmp->prev != nullptr) tmp->prev->next = tmp->next;
    else this->tab[i] = tmp->next;

    if(tmp->next != nullptr) tmp->next->prev = tmp->prev;
    delete tmp;
    (this->n)--;
  }

  bool operator==(const HashMap& other) const
  {
    if (this->n != other.n) return false;
    else{
      iterator a = this->begin();
      while(a != this->end()){
        iterator b = other.find(a->first);
        if(*a != *b) return false;
        a++;
      }
    }
    return true;
  }

  bool operator!=(const HashMap& other) const
  {
    return !(*this == other);
  }

  const_iterator cbegin() const
  {
    size_type idx = 0;
    while(idx < size && this->tab[idx] == nullptr)  idx++;
    return ConstIterator(this->tab[idx], this, idx);
  }

  iterator begin() { return Iterator(this->cbegin()); }
  iterator end() { return Iterator(this->cend()); }
  const_iterator cend() const {  return ConstIterator(nullptr, this, size);  }
  const_iterator begin() const {  return cbegin();  }
  const_iterator end() const {  return cend();  }
  size_type getSize() const {  return n;  }
};

template <typename KeyType, typename ValueType>
class HashMap<KeyType, ValueType>::ConstIterator
{
public:
  using reference = typename HashMap::const_reference;
  using iterator_category = std::bidirectional_iterator_tag;
  using value_type = typename HashMap::value_type;
  using pointer = const typename HashMap::value_type*;

protected:
  Node *curr;
  const HashMap* hm;
  size_type idx;

public:
  explicit ConstIterator() = delete;
  ConstIterator(Node* node, const HashMap *hashmap, size_type i)  : curr(node), hm(hashmap), idx(i) {}
  ConstIterator(const ConstIterator& other) = default;

  ConstIterator& operator++()
  {
    if(this->idx == hm->size) throw std::out_of_range("straznik : nie mozna wykonac operacji ++");

    else if(this->curr != nullptr && this->curr->next != nullptr) this->curr = this->curr->next;
    else{
      while(this->idx < this->hm->size){
        (this->idx)++;
        if(this->hm->tab[this->idx] != nullptr)  break;
      }
      this->curr = this->hm->tab[idx];
    }
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
    Node *tmp;
    size_type i;

    if(this->idx < this->hm->size && this->curr != nullptr && this->curr->prev != nullptr)  this->curr = this->curr->prev;
    else{
      i = this->idx;
      tmp = this->curr;

      while(i > 0){
        i--;
        if(this->hm->tab[i] != nullptr) break;
      }
      if(this->hm->tab[i] != nullptr)  tmp = this->hm->tab[i];
      if(this->curr == tmp) throw std::out_of_range("begin - nie mozna wykonac operacji --");  //curr byl na ostatniej pozycji
      if(tmp != nullptr) while(tmp->next != nullptr) tmp = tmp->next;
      this->curr = tmp;
      this->idx = i;
    }
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
    if(this->idx >= this->hm->size) throw std::out_of_range("straznik - nie mozna wykonac operacji *");
    return this->curr->val;
  }

  pointer operator->() const{ return &this->operator*(); }
  bool operator==(const ConstIterator& other) const{ return (this->curr == other.curr && this->hm == other.hm && this->idx == other.idx); }
  bool operator!=(const ConstIterator& other) const{ return !(*this == other); }
  size_type getIdx(){  return this->idx;  }
};

template <typename KeyType, typename ValueType>
class HashMap<KeyType, ValueType>::Iterator : public HashMap<KeyType, ValueType>::ConstIterator
{
public:
  using reference = typename HashMap::reference;
  using pointer = typename HashMap::value_type*;

  explicit Iterator() = delete;
  Iterator(Node* node, const HashMap *hashmap, size_type i): ConstIterator(node, hashmap, i) {}
  Iterator(const ConstIterator& other): ConstIterator(other) {}

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
};

}

#endif /* AISDI_MAPS_HASHMAP_H */
