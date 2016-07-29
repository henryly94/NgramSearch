#include <stdio.h>
#include "Datrie.hpp"
#include <string>
#include <string.h>
#include <vector>
#include <algorithm>
#include <functional>

#define MAX_SIZE 100000000
#define RANGE ('z' - 'A' + 1)
#define BATCH_MAX 2500
#define USED_RATE_LIMIT 0.4


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
/*
bool area_cmp(area a, area b){
	return a.start < b.start;
}

bool block_cmp(area a, area b){
	return a.size() < b.size();
}
*/

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

/*
bool Integrity(std::vector<area> a, std::vector<area> b){
	std::vector<area>::iterator it;
	for (area each : b){
		it = lower_bound(a.begin(), a.end(), each, area_cmp);
		if (*it != each) {
			printf("Missing: [%d, %d)\n", each.start, each.end);
			return false;
		}
	}
	return true;
}
*/
int Datrie::get_size(){
	return size;
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
	printf("Areas:");
	for (area each : mAreaContainer->areas){
		printf("[%d, %d] ", each.start, each.end);
	}
	printf("\n");
	printf("Blocks:");
	for (area each : mAreaContainer->blocks){
		printf("[%d, %d] ", each.start, each.end);
	}
	printf("\n");
}


Datrie::Datrie(){
	size = 128;
	cnt = 0;
	insert_cnt = 0;
	base.push_back(node(1, NULL_VALUE));
	base.resize(size, node(NULL_VALUE, NULL_VALUE));
	check.push_back(ROOT_VALUE);
	check.resize(size, -1);
	//memset(base, NULL_VALUE, sizeof(node)*size);
	//memset(check, NULL_VALUE, sizeof(int)*size);
	//base[0].base = 1;
//	check[0] = ROOT_VALUE;
	mAreaContainer = new AreaContainer(this);
	mAreaContainer->areas.insert(area(1, size));
	mAreaContainer->blocks.insert(area(1, size));
}

int Datrie::query(std::string key){
	int s = 0;
	int t;
	for (auto each : key){
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
//	printf("Insert: %s\n", key.c_str());
	int s = 0, t, ch;
	for (auto  each : key){
		ch = hash(each);
		t = base[s].base + ch;
		
		while (t >= size)double_size();
		if (t <= 0){
			s = solve_collision(s, t);
		} else if (check[t] == -1){
		//	display_used();
			mAreaContainer->get_area(area(t));
			check[t] = s;
			base[t].base = base[s].base;
			s = t;	
		} else if (check[t] != s) { // Collision
			s = solve_collision(s, t);					       
		} else {
			s = t;
		}
	}
	base[s].value = value;
	if (s > cnt) cnt = s;
/*	insert_cnt++;
	if (insert_cnt >= BATCH_MAX){
		printf("Try clean areas..\n");
		insert_cnt = 0;
		try_clean();
	}
*/	
//	display_used();
}

void Datrie::double_size() {
	if (size>=MAX_SIZE)exit(0);	
	check.resize(2*size, NULL_VALUE);
	base.resize(2*size, node(NULL_VALUE, NULL_VALUE));
	
	if (mAreaContainer->areas.empty()){
		mAreaContainer->areas.insert(area(size, size*2));
		mAreaContainer->blocks.insert(area(size, size*2));
	} else {

		std::multiset<area>::const_iterator it = mAreaContainer->areas.end(), b_it;
		it--;
		if ((*it).end == size) {
			b_it = mAreaContainer->blocks.upper_bound(*it);
			while ( !(*b_it == *it) || b_it == mAreaContainer->blocks.end()){
				--b_it;
			}
			area tmp((*it).start, size * 2);
			mAreaContainer->areas.erase(it);
			mAreaContainer->areas.insert(tmp);
			mAreaContainer->blocks.erase(b_it);
			mAreaContainer->blocks.insert(tmp);		
		} else { 
			mAreaContainer->areas.insert(area(size, size*2));
			mAreaContainer->blocks.insert(area(size, size*2));
		}

	}
	size *= 2;
	printf("Size:%-20d\n", size);
}

int Datrie::solve_collision(int base_s, int coll_s){
	std::vector<int> offsets;
	int old_base = base[base_s].base;
	int coll_off = coll_s - old_base;
	int new_base, range, low_bound;
	for (int i=std::max(old_base, 1); i<=old_base + RANGE; i++){
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
	//new_base = find_remain(range, low_bound, base_s);
	new_base = mAreaContainer->get_base(range) - low_bound;

//	for (auto each : offsets)printf("%d ", each);
//	printf("\n");
	base[base_s].base = new_base;

	offsets.pop_back();	
	for (int off : offsets){
		int new_pos = new_base + off;
		int old_pos = old_base + off;
		while(new_pos >= size)double_size();
		check[new_pos] = base_s;
		base[new_pos] = base[old_pos];
		check[old_pos] = NULL_VALUE;
		base[old_pos].base = NULL_VALUE;
		base[old_pos].value = NULL_VALUE;
//		display_used();
		mAreaContainer->get_area(area(new_pos));
		mAreaContainer->ret_area(area(old_pos));
	}
	base[new_base + coll_off].base = new_base;
	check[new_base + coll_off] = base_s;
//	display_used();
	mAreaContainer->get_area(area(new_base + coll_off));
	for (int i=0; i< size; i++){
		std::vector<int>::iterator it = find(offsets.begin(), offsets.end(), check[i] - old_base);	
		if (it != offsets.end()){
			check[i] += (new_base - old_base);
		}		
	}
	return new_base + coll_off;
}

void Datrie::try_clean(){
	if (mAreaContainer->used_rate() > USED_RATE_LIMIT){
		printf("Over rate, Cleaning..\n");
		mAreaContainer->blocks.clear();
		mAreaContainer->areas.clear();
		double_size();
	}
}

AreaContainer::AreaContainer(Datrie* da){
	mDatrie = da;
}

int AreaContainer::get_base(int range){
//	printf("Want range: %d\n", range);
	std::multiset<area>::iterator it = blocks.lower_bound(area(0, range));
	if (it != blocks.end()){
		return (*it).start;
	} else {
//		printf("Container Double_Size\n");
		mDatrie->double_size();
		return get_base(range);	
	}
}

void AreaContainer::get_area(area  pos){
	if (pos.size() <= 0) return;
//	printf("Getting [%d, %d)\n", pos.start, pos.end);
	std::multiset<area>::iterator it = areas.upper_bound(pos), b_it;
	if (it != areas.begin()){
		it--;
		area tmp(*it);
	//	printf("tmp: [%d, %d)\n", tmp.start, tmp.end);
		areas.erase(it);
		b_it = blocks.upper_bound(tmp);
		while (*b_it != tmp)--b_it;
		blocks.erase(b_it);
//		if (tmp.start < pos.start - 5)
		{
			area front(tmp.start, pos.start);
			areas.insert(front);
			blocks.insert(front);
		}
//		if (tmp.end > pos.end + 5)
		{
			area back(pos.end, tmp.end);
			areas.insert(back);
			blocks.insert(back);
		}
	}
//	printf("Out\n");
}

void AreaContainer::ret_area(area pos){
	std::multiset<area>::const_iterator it, b_it;
	it = areas.upper_bound(pos);
	if (it != areas.begin()){
		area tmp(*it);
		if ((*it).start == pos.end){
			b_it = blocks.upper_bound(tmp);
			while((*b_it) != tmp)--b_it;
			pos.end = tmp.end;
			it = areas.erase(it);
			blocks.erase(b_it);
		}

		it--;
		
		tmp = *it;

		if ((*it).end == pos.start){
			b_it = blocks.upper_bound(tmp);
			while((*b_it) != tmp)--b_it;
			pos.start = tmp.start;
			areas.erase(it);
			blocks.erase(b_it);
		}
//		if (pos.size() >= 5)
		{
			areas.insert(pos);
			blocks.insert(pos);
		}


	} else {
		if ((*it).start == pos.end){
			area front(pos.start, (*it).end);
			area tmp(*it);
			b_it = blocks.upper_bound(tmp);
			while((*b_it) != tmp)--b_it; 
			areas.erase(it);
			blocks.erase(b_it);
//			if (front.size() >= 5)
			{
				areas.insert(front);
				blocks.insert(front);
			}
		} else {
//			if (pos.size() >= 5)
			{
				areas.insert(pos);
				blocks.insert(pos);
			}
		}
	}

}

float AreaContainer::used_rate(){
	float used_cnt = 0.01;
	int low = (*(areas.begin())).start;
	for (area each : areas){
		used_cnt += each.size(); 
	}
	return  1 - used_cnt / (mDatrie->get_size() - low);
}

