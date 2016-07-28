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

inline bool area_cmp(area a, area b){
	return a.start < b.start;
}

inline bool block_cmp(area a, area b){
	return a.size() < b.size();
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
	for (auto each : mAreaContainer->areas){
		printf("[%d, %d] ", each.start, each.end);
	}
	printf("\n");
	printf("Blocks:");
	for (auto each : mAreaContainer->blocks){
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
	mAreaContainer->areas.push_back(area(1, size));
	mAreaContainer->blocks.push_back(area(1, size));
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
	insert_cnt++;
	if (insert_cnt >= BATCH_MAX){
		printf("Try clean areas..\n");
		insert_cnt = 0;
		try_clean();
	}
}

void Datrie::double_size() {
	if (size>=MAX_SIZE)exit(0);	
	//node* tmp_base = base;
	//int* tmp_check = check;
	//base = new node[2*size];
	

	check.resize(2*size, NULL_VALUE);
	base.resize(2*size, node(NULL_VALUE, NULL_VALUE));
	//check = new int[2*size];
	//memcpy(base, tmp_base, sizeof(node)*size);
	//memcpy(check, tmp_check, sizeof(int)*size);
	//memset(&base[size], NULL_VALUE, sizeof(node)*size);
	//memset(&check[size], NULL_VALUE, sizeof(int)*size);
	
	if (mAreaContainer->areas.empty()){
		mAreaContainer->areas.insert(mAreaContainer->areas.end(), area(size, size*2));
		mAreaContainer->blocks.insert(mAreaContainer->blocks.end(), area(size, size*2));
	} else {

		std::vector<area>::iterator it = mAreaContainer->areas.end(), b_it;
		if ((*(it-1)).end == size) {
	//		d(66);
			b_it = upper_bound(mAreaContainer->blocks.begin(), mAreaContainer->blocks.end(), *(it-1), block_cmp);
			while (*b_it != *(it-1) || b_it == mAreaContainer->blocks.end()){
	//			d(777);
				--b_it;
			}
			(*(it-1)).end = size*2;
	//		printf("[%d, %d], %d\n", (*b_it).start, (*b_it).end, b_it==mAreaContainer->blocks.end());
			mAreaContainer->blocks.erase(b_it);
			mAreaContainer->blocks.insert(mAreaContainer->blocks.end(), *(it-1));		
		} else { 
			mAreaContainer->areas.insert(it, area(size, size*2));
			mAreaContainer->blocks.insert(mAreaContainer->blocks.end(), area(size, size*2));
		}

	}
	size *= 2;

	//delete []tmp_base;
	//delete []tmp_check;
	
	//display_used();	
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
		mAreaContainer->get_area(area(new_pos));
		mAreaContainer->ret_area(area(old_pos));
	}
	base[new_base + coll_off].base = new_base;
	check[new_base + coll_off] = base_s;
	mAreaContainer->get_area(area(new_base + coll_off));
	for (int i=0; i< size; i++){
		std::vector<int>::iterator it = find(offsets.begin(), offsets.end(), check[i] - old_base);	
		if (it != offsets.end()){
			check[i] += (new_base - old_base);
		}		
	}
	return new_base + coll_off;
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
	std::vector<area>::iterator it = lower_bound(blocks.begin(), blocks.end(), area(0, range), block_cmp);
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
	std::vector<area>::iterator it = upper_bound(areas.begin(), areas.end(), pos, area_cmp);
	if (it != areas.begin()) {
		std::vector<area>::iterator b_it = upper_bound(blocks.begin(), blocks.end(), *(it-1), block_cmp);
		while((*b_it).start != (*(it-1)).start || (*b_it).end != (*(it-1)).end) {
			--b_it;
//			if ((*b_it).size() <  (*it).size())break;
		}
		if ((*(it-1)).end >= pos.end) {
			blocks.erase(b_it);
			int end = (*(it-1)).end;
			(*(it-1)).end = pos.start;
			area front(*(it-1));
			if (end > pos.end){
				area back(pos.end, end);
//				if (back.size() >= 5)
				{
					it = areas.insert(it, back);
					b_it = upper_bound(blocks.begin(), blocks.end(), back, block_cmp);
					blocks.insert(b_it, back);
				}
			}
//			if ((*(it-1)).size() >= 5)
			if ((*(it-1)).size())
			{
				b_it = upper_bound(blocks.begin(), blocks.end(), front, block_cmp);
				blocks.insert(b_it, front);
			} else {
				areas.erase(it-1);
			}
		}
	}


}

void AreaContainer::ret_area(area pos){
	std::vector<area>::iterator it = upper_bound(areas.begin(), areas.end(), pos, area_cmp), b_it;	
	if (it != areas.begin()){
		if ((*(it-1)).end == pos.start){
			pos.start = (*(it-1)).start;
			b_it = upper_bound(blocks.begin(), blocks.end(), *(it-1), block_cmp);
			while (*b_it != *(it-1) || b_it == blocks.end())--b_it;
			blocks.erase(b_it);
			it = areas.erase(it-1);
		} 
		if ((*it).start == pos.end){
			pos.end = (*it).end;
			b_it = upper_bound(blocks.begin(), blocks.end(), *it, block_cmp);
			while (*b_it != *it || b_it == blocks.end())--b_it;
			blocks.erase(b_it);
			it = areas.erase(it);
		}
//		if (pos.size() >= 5)
		{
			it = areas.insert(it, pos);
			b_it = upper_bound(blocks.begin(), blocks.end(), *it, block_cmp);
			blocks.insert(b_it, pos);	
		}
	} else {

		if ((*it).start == pos.end){
			b_it = upper_bound(blocks.begin(), blocks.end(), *it, block_cmp);
			while(b_it == blocks.end() || *b_it != *it)--b_it;
			blocks.erase(b_it);
			pos.end = (*it).end;
			it = areas.erase(it);
		}
//		if (pos.size() >= 5)
		{
			areas.insert(it, pos);
			b_it = upper_bound(blocks.begin(), blocks.end(), pos, block_cmp);
			blocks.insert(b_it, pos);
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

