#include <string.h>
#include <stdio.h>
#include <string>
#include <algorithm>
#include <vector>

using namespace std;

struct node{
	int base;
	int value;
};
int main(){
	node *p = new node[16];
	memset(p, -1, sizeof(node)*16);
	for (int i=0; i<16; i++ ){
	//	printf("%d %d\n", p[i].base, p[i].value);
	}
	
	string a = string("abcdef");

	int b = a.find('z');
	int c = a.find('c');

	printf("%d %d\n", b, c);

	printf("%s\n", a.substr(0, c).c_str());

	vector<int> list = {5, 3, 2, 1, 4};

	for(vector<int>::iterator it = list.begin(); it!=list.end();it++)
		printf("%d ", *it);
	printf("\n");

	printf("%d\n", *lower_bound(list.begin(), list.end(), 3));

	sort(list.begin(), list.end());

	for (auto each: list)
		printf("%d ", each);
	printf("\n");

	return 0;
}
