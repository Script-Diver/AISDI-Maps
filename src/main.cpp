#include <cstddef>
#include <cstdlib>
#include <string>
//#include <iostream>

#include "TreeMap.h"
#include "HashMap.h"

namespace
{
using Tree = aisdi::TreeMap<int, int>;
using Hmap = aisdi::HashMap<int, int>;

void addNodeT(int b, int e, Tree& tr)
{
  int i = (b+e)/2;
  tr.operator[](i) = i;
  if(b >= e) return;
  addNodeT(b, i-1, tr);
  addNodeT(i+1, e, tr);
}

void addNodeH(int b, int e, Hmap& h)
{
  int i = (b+e)/2;
  h[i] = i;
  if(b >= e) return;
  addNodeH(b, i-1, h);
  addNodeH(i+1, e, h);
}

void perfomTest()
{
  int n = 10000;

  //tworzenie
  clock_t t = clock(); 

  Tree tree;
  t = clock() - t;
  std::cout<<"Czas tworzenia drzewa:  "<<t<<std::endl;

  t = clock();
  Hmap hmap;
  t = clock() - t;
  std::cout<<"Czas tworzenia hashmapy:  "<<t<<std::endl;

  //dodawanie elementow
  t = clock();
  addNodeT(0, n, tree);
  t = clock() - t;
  std::cout<<"Czas dodawania elementow do drzewa:  "<<t<<std::endl;

  t = clock();
  addNodeH(0, n, hmap);
  t = clock() - t;
  std::cout<<"Czas dodawania elementow do hashmapy:  "<<t<<std::endl;

  //przegladanie
  t = clock();
  for(int i = 0 ; i < n ; i++)  tree[i];
  t = clock() - t;
  std::cout<<"Czas przejrzenia calego drzewa:  "<<t<<std::endl;

  t = clock();
  for(int i = 0 ; i < n ; i++)  hmap[i];
  t = clock() - t;
  std::cout<<"Czas przejrzenia calej hashmapy:  "<<t<<std::endl;


  //usuwanie wybranych elementow
  t = clock();
  for(int i = n-1 ; i > 0; i -= 100) tree.remove(i);
  t = clock() - t;
  std::cout<<"Czas usuwania elementow z drzewa:  "<<t<<std::endl;

  t = clock();
  for(int i = n-1 ; i > 0 ; i -= 100) hmap.remove(i);
  t = clock() - t;
  std::cout<<"Czas usuwania elementow z hashmapy:  "<<t<<std::endl;

 // tree.print();
}

} // namespace

int main(int argc, char** argv)
{
  const std::size_t repeatCount = argc > 1 ? std::atoll(argv[1]) : 1;
  for (std::size_t i = 0; i < repeatCount; ++i)
    perfomTest();
  return 0;
}
