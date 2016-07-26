#include <stdio.h>
#include "Datrie.hpp"
#include <string>
#include <string.h>
#include <vector>
#include <algorithm>
#include <functional>

#define MAX_SIZE 100000000
#define RANGE ('z' - 'A' + 1)

inline int has(int n){
	return n;
}

int hash(char ch){
	if (ch < 'A' || ch > 'z') return 0;
	else return ch - 'A' + 1;
}

int hash(std::string str){
	int ret = 0;
	for(int i=0; i<str.size(); i++){
		ret = (ret * RANGE) + hash(str[i]);
	}
	return ret;
}

std::vector<std::string> split(std::string str){
	std::vector<std::string> ret;
	std::string tmp;
	for (auto ch : str){
		if(ch != ' '){
			tmp.push_back(ch);
		} else {
			ret.push_back(tmp);
			tmp.clear();
		}
	}
	return ret;
}

inline void d(int n){
	printf("%d\n", n);
}

inline void d(){
	printf("\n");
}

inline void d(char ch){
	printf("%c\n", ch);
}


void Datrie::display(int n){
	if (n > size)
		n = size;;
	printf("#            base            value            check\n");
	for (int i=0; i<n; i++){
		printf("%-3d:   %12d   %12d   %12d\n", i, base[i].base, base[i].value, check[i]);
	}
}

void Datrie::display_remain(){
	for (auto each : remain_area){
		printf("[%d, %d] ", each.start, each.end);
	}
	printf("\n");
}

void Datrie::display_used(){
	printf("Used:");
	for (auto each : used_area){
		printf("[%d, %d] ", each.start, each.end);
	}
	printf("\n");
}


Datrie::Datrie(){
	size = 128;
	cnt = 0;
	base = new node[size];
	check = new int[size];
	memset(base, NULL_VALUE, sizeof(node)*size);
	memset(check, NULL_VALUE, sizeof(int)*size);
	base[0].base = 1;
	check[0] = ROOT_VALUE;
	remain_area.push_back(area(1, size));
}

int Datrie::query(std::string key){
	int s = 0;
	int t;
	for (auto each : split(key)){
		int ch = hash(each);
		t = base[s].base + ch;
		if (check[t] == s){
			s = t;
		} else {
			return UNEXIST_VALUE;
		}
	}
	return base[s].value;
}

void Datrie::insert(std::string key, int value){
	printf("Insert: %s\n", key.c_str());
	int s = 0, t, ch;
	for (auto  each : split(key)){
		ch = hash(each);
		printf("%d\t", ch);
		t = base[s].base + ch;
		
		while (t >= size)double_size();
		
		if (check[t] == -1){
			if (check_valid(t, s) == 1){
				check[t] = s;
				base[t].base = base[s].base;
				s = t;	
				sort_remain(area(t, t+1));
			} else if (check_valid(t, s) == 2) {
				check[t] = s;
				base[t].base = base[s].base;
				s = t;
			} else {
				s = solve_collision(s, t);
			}
		} else if (check[t] != s) { // Collision
			s = solve_collision(s, t);					       
		} else {
			s = t;
		}
	}
	printf("\n");
	base[s].value = value;
	if (s > cnt) cnt = s;
//	display_remain();
//	display_used();
}

void Datrie::double_size() {
	if (size>=MAX_SIZE)exit(0);	
	node* tmp_base = base;
	int* tmp_check = check;
	base = new node[2*size];
	check = new int[2*size];
	memcpy(base, tmp_base, sizeof(node)*size);
	memcpy(check, tmp_check, sizeof(int)*size);
	memset(&base[size], NULL_VALUE, sizeof(node)*size);
	memset(&check[size], NULL_VALUE, sizeof(int)*size);

	std::vector<area>::iterator it = remain_area.end();
	it--;
	if ((*it).end == size)
		(*it).end = size*2;
	else
		remain_area.push_back(area(size, size * 2));

	size *= 2;

	display_remain();
	delete []tmp_base;
	delete []tmp_check;
	
	
	printf("Size:%-20d\n", size);
}

int Datrie::solve_collision(int base_s, int coll_s){
	std::vector<int> offsets;
	int old_base = base[base_s].base;
	int coll_off = coll_s - old_base;
	int new_base, range, low_bound;
	for (int i=old_base; i<=old_base + RANGE; i++){
		if (check[i] == base_s) offsets.push_back(i-old_base);
	}
	if (offsets.size() == 0){
		range = 1;
		low_bound = coll_off;
	} else {
		range = std::max(*(offsets.end()-1), coll_off) - std::min(coll_off, *(offsets.begin())) + 1;
		low_bound = std::min(coll_off, *(offsets.begin()));
	}
	offsets.push_back(coll_off);
	new_base = find_remain(range, low_bound, base_s);
/*
	int j, cnt=0;
	for (int i=1; i < size; i++){
		for(j=0; j < offsets.size(); j++){
			if ( offsets[j] + i >= size){
				double_size();
				break;
			}	
			if (check[offsets[j] + i] != NULL_VALUE) break;
		}
		if (j == offsets.size()) {
			new_base = i;
			break;
		}
	}
*/	
	base[base_s].base = new_base;
	offsets.pop_back();	
	for (auto off : offsets){
		check[new_base + off] = base_s;
		base[new_base + off] = base[old_base + off];
		check[old_base + off] = NULL_VALUE;
		base[old_base + off].base = NULL_VALUE;
		base[old_base + off].value = NULL_VALUE;
		ret_remain(old_base + off);
	}
	base[new_base + coll_off].base = new_base;
	check[new_base + coll_off] = base_s;
	for (int i=0; i< size; i++){
		std::vector<int>::iterator it = find(offsets.begin(), offsets.end(), check[i] - old_base);	
		if (it != offsets.end()){
			check[i] += (new_base - old_base);
		}		
	}
	return new_base + coll_off;
}

bool area_cmp(area a, area b){
	return a.start < b.start;
}

bool block_cmp(area a, area b){
	return a.size() < b.size();
}

void Datrie::ret_remain(int ret_area){
	std::vector<area>::iterator it = upper_bound(remain_area.begin(), remain_area.end(), area(ret_area), area_cmp);
	if (it != remain_area.begin()){
		if (it != remain_area.end()){
			if ( (*(it-1)).end == (*it).start-1){
				(*it).start = (*(it-1)).start;
				it--;
				remain_area.erase(it);
			} else {
				if ((*it).start == ret_area+1){
					(*it).start--;
				} else if ((*(it-1)).end == ret_area ){
					(*(it-1)).end++;
				} else {
					remain_area.insert(it, area(ret_area, ret_area+1));
				}
			}
		} else {
			if ((*(it-1)).end == ret_area){
				(*(it-1)).end++;
			} else {
				remain_area.insert(it, area(ret_area, ret_area+1));
			}
		}
	} else {
		remain_area.insert(it, area(ret_area, ret_area+1));
	}

	it = upper_bound(used_area.begin(), used_area.end(), area(ret_area), area_cmp);
	it = used_area.insert(it, area(ret_area+1,  (*(it-1)).end, (*(it-1)).father));
	if ((*it).size() == 0)
		it = used_area.erase(it);
	--it;
	(*it).end =  ret_area;
	if ((*it).size() == 0)
		used_area.erase(it);

}

void Datrie::sort_remain(area new_area){
//	printf("New area:[%d, %d]\n", new_area.start, new_area.end);
	std::vector<area>::iterator it = upper_bound(remain_area.begin(), remain_area.end(), new_area, area_cmp);
	it = remain_area.insert(it, area(new_area.end,  (*(it-1)).end));
	if ((*it).size() == 0)
		it = remain_area.erase(it);
	--it;
	(*it).end =  new_area.start;
	if ((*it).size() == 0)
		remain_area.erase(it);

	it = upper_bound(used_area.begin(), used_area.end(), new_area, area_cmp);
	if (used_area.size() == 0){
		used_area.insert(it, new_area);
		return;
	}
	if (it != used_area.begin()){
		if (it != used_area.end()){
			if ( (*(it-1)).end ==  new_area.start  && (*it).start == new_area.end){
				(*it).start = (*(it-1)).start;
				it--;
				used_area.erase(it);
			} else {
				if ((*it).start == new_area.end){
					(*it).start = new_area.start;
				} else if ((*(it-1)).end == new_area.start ){
					(*(it-1)).end = new_area.end;
				} else {
					used_area.insert(it, new_area);
				}
			}
		} else {
			if ( (*(it-1)).end == new_area.start && (*(it-1)).father== new_area.father){
				(*(it-1)).end = new_area.end;
			} else {
				used_area.insert(it, new_area);
			}
		}
	} else {
		if ((*it).start == new_area.end && (*it).father == new_area.father){
			(*it).start = new_area.start;
		} else {
			used_area.insert(it, new_area);
		}
	}	
}


int  Datrie::check_valid(int pos, int father){
//	printf("pos: %d\n", pos);
	if (used_area.size() == 0) return 1;
	std::vector<area>::iterator it = upper_bound(used_area.begin(), used_area.end(), area(pos, pos+1, father), area_cmp);
	if (it != used_area.begin()){
		if (pos >= (*(it-1)).end){
			return 1;
		} else {
			return father == (*(it-1)).father? 2 :0;
		}
	} else {
		return 1;
	}
}

int Datrie::find_remain(int range, int low_bound, int father){
//	printf("range: %d, low_bound: %d\n", range, low_bound);
	for (auto each : remain_area){
		if (each.size()>=range && each.end >low_bound+range){
			int low = std::max(each.start, low_bound);
		//	printf("each.start: %d, low_bound: %d\n", each.start, low_bound);
			sort_remain(area(low, low + range, father));
			return low - low_bound;
		}
	}
	double_size();
	return find_remain(range, low_bound, father);
}

int AreaContainer::get_base(int range){
	std::vector<area>::iterator it = low_bound();
}

void AreaContainer::get_area(area  pos){

}

void AreaContainer::ret_area(area pos){

}

