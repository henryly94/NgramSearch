# NgramSearchEngine
A N-grams Search Engine based on Trie

# Usage
Include "NgramSearch.hpp"

Init:
	NgramSearch(string path, int cache_size)
	:param path		: the directory to save whole DB
	:param cache_size	: Maximun amount of datrie that kept in memory

User Interface
	void insert(string key, int value)
	int query(string key)
	void save()
	void load()
# Example

```
#include "NgramSearch.hpp"
#include <stdio.h>
#include <string.h>
using namspace std;
int main(){

	NgramSearch a(string("test_save"), 3);
	a.insert(string("abc"), 123);
	a.insert(string("hello world"), 456);
  
	printf("%d\n", a.query(string("abc")));
	printf("%d\n", a.query(string("hello world")));
	

	a.save();

	//Should has same ID
	Datrie b(string("test_save"), 5);
	
	b.load();
	printf("%d\n", b.query(string("abc")));
	printf("%d\n", b.query(string("hello world")));
	
	return 0;
}
```
