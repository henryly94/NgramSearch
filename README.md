# NgramSearchEngine
A N-grams Search Engine based on trie

# Usage
Include "Datrie.hpp"
User Interface
	void insert(char* key, int value) Or void insert(string key, int value)
	int query(char* key) Or int query(string key)
	void save(string path)
	void load(string path)
# Example

```
#include "Datrie.hpp"
#include <stdio.h>
#include <string.h>
using namspace std;
int main(){

	//Arg here is an ID
	Datrie a(173);
	a.insert("abc", 123);
	a.insert("hello world", 456);
  
	printf("%d\n", a.query("abc"));
	printf("%d\n", a.query("hello world"));
	
	string path("test_save");

	a.save(path);

	//Should has same ID
	Datrie b(173);
	
	b.load(path);

	return 0;
}
```
