#include <stdio.h>
#include "Datrie.hpp"
#include <string>
#include <string.h>
#include <vector>
#include <algorithm>
#include <functional>
#include <sys/time.h>
#include <time.h>

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
//			d(1);
			t = solve_collision(s, t);
			base[s].son.push_back(t);
			s = t;
		} else if (check[t] == -1){
//			d(2);
			mAreaContainer->get_area(area(t));
			check[t] = s;
			base[t].base = base[s].base;
			base[s].son.push_back(t);
			s = t;	
		} else if (check[t] != s) { // Collision
//			d(3);
			t = solve_collision(s, t);					       
			base[s].son.push_back(t);
			s = t;
		} else {
//			d(4);
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
}

void Datrie::double_size() {
//	printf("In\n");
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
	struct timeval t_begin, t_end, t_bla, t_cost, t_small_a, t_small_b;
	gettimeofday(&t_begin, NULL);
	
	std::vector<int> offsets;
	int old_base = base[base_s].base;
	int coll_off = coll_s - old_base;
	int new_base, range, low_bound;
	for (int i=std::max(old_base, 1); i<=old_base + RANGE; i++){
		if (check[i] == base_s) offsets.push_back(i-old_base);
	}

	// phase 1
	gettimeofday(&t_end, NULL);
	timersub(&t_end, &t_begin, &t_cost);
	solve_1 += t_cost.tv_sec + (1.0 * t_cost.tv_usec) / 1000000;


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
	
	for (std::vector<int>::iterator it = base[base_s].son.begin(); it != base[base_s].son.end(); it++){
		(*it) += (new_base - old_base);
	}

	// phase 2
	gettimeofday(&t_bla, NULL);

	timersub(&t_bla, &t_end, &t_cost);
	solve_2 += t_cost.tv_sec + (1.0 * t_cost.tv_usec) / 1000000;

//	for (auto each : offsets)printf("%d ", each);
//	printf("\n");
	base[base_s].base = new_base;


	offsets.pop_back();	
	for (int off : offsets){
		gettimeofday(&t_small_a, NULL);
		int new_pos = new_base + off;
		int old_pos = old_base + off;
		while(new_pos >= size)double_size();
		check[new_pos] = base_s;
		base[new_pos] = base[old_pos];

		gettimeofday(&t_small_b, NULL);
		timersub(&t_small_b, &t_small_a, &t_cost);
		solve_3_1 += t_cost.tv_sec + (1.0 * t_cost.tv_usec) /1000000;

		check[old_pos] = NULL_VALUE;
		base[old_pos].base = NULL_VALUE;
		base[old_pos].value = NULL_VALUE;
		base[old_pos].son.clear();
//		display_used();

		gettimeofday(&t_small_a, NULL);
		timersub(&t_small_a, &t_small_b, &t_cost);
		solve_3_2 += t_cost.tv_sec + (1.0 * t_cost.tv_usec) /1000000;
		
//		d(50);

		mAreaContainer->get_area(area(new_pos));
		
//		d(51);
		mAreaContainer->ret_area(area(old_pos));

//		d(52);
		gettimeofday(&t_small_b, NULL);
		timersub(&t_small_b, &t_small_a, &t_cost);
		solve_3_3 += t_cost.tv_sec + (1.0 * t_cost.tv_usec) /1000000;



	}
	base[new_base + coll_off].base = new_base;
	check[new_base + coll_off] = base_s;
//	display_used();
//	d(40);
	mAreaContainer->get_area(area(new_base + coll_off));
//	d(41);

	// phase 3
	gettimeofday(&t_end, NULL);
	timersub(&t_end, &t_bla, &t_cost);
	solve_3 += t_cost.tv_sec + (1.0 * t_cost.tv_usec) / 1000000;

	for (int each : offsets){
		for (int every : base[each + new_base].son){
			check[every] += (new_base - old_base);
		}
	}

	gettimeofday(&t_bla, NULL);
	solve_cnt++;
	timersub(&t_bla, &t_end, &t_cost);
	solve_time += t_cost.tv_sec + (1.0 * t_cost.tv_usec) / 1000000;

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
//		d(20);
		return (*it).start;
	} else {
//		d(21);
//		printf("Container Double_Size\n");
		mDatrie->double_size();
		return get_base(range);	
	}
}

void AreaContainer::get_area(area  pos){
	struct timeval t_begin, t_end, t_cost;
	gettimeofday(&t_begin, NULL);
	if (pos.size() <= 0) return;
//	printf("Getting [%d, %d)\n", pos.start, pos.end);
	std::multiset<area>::iterator it = areas.upper_bound(pos), b_it;
	if (it != areas.begin()){
		it--;
		area tmp(*it);
	//	printf("tmp: [%d, %d)\n", tmp.start, tmp.end);
	
//		d(30);
		areas.erase(it);
//		d(31);
		b_it = blocks.lower_bound(tmp);
//		while (*b_it != tmp)--b_it;
		blocks.erase(b_it);
//		d(32);
//		if (tmp.start < pos.start - 5)
		{
			area front(tmp.start, pos.start);
			areas.insert(front);
			blocks.insert(front);
		}
//		d(33);
//		if (tmp.end > pos.end + 5)
		{
			area back(pos.end, tmp.end);
			areas.insert(back);
			blocks.insert(back);
		}
//		d(34);
	}
	get_area_cnt++;
	gettimeofday(&t_end, NULL);
	timersub(&t_end, &t_begin, &t_cost);
	get_area_time += t_cost.tv_sec + (1.0 * t_cost.tv_usec) / 1000000;
//	printf("After Get\n");
//	mDatrie->display_used();
//	printf("Out\n");
}

void AreaContainer::ret_area(area pos){
//	printf("Returning [%d, %d)\n", pos.start, pos.end);
	struct timeval t_begin, t_end, t_cost;
	gettimeofday(&t_begin, NULL);
	std::multiset<area>::const_iterator it, b_it;
	it = areas.upper_bound(pos);
	if (it != areas.begin()){
		area tmp(*it);
		if ((*it).start == pos.end){
			b_it = blocks.lower_bound(tmp);
//			while((*b_it) != tmp)--b_it;
			pos.end = tmp.end;
			it = areas.erase(it);
			blocks.erase(b_it);
		}

		it--;
		
		tmp = *it;

		if ((*it).end == pos.start){
			b_it = blocks.lower_bound(tmp);
//			while((*b_it) != tmp)--b_it;
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
			b_it = blocks.lower_bound(tmp);
//			while((*b_it) != tmp)--b_it; 
			areas.erase(it);
			blocks.erase(b_it);
		//	if (front.size() >= 5)
			{
				areas.insert(front);
				blocks.insert(front);
			}
		} else {
		//	if (pos.size() >= 5)
			{
				areas.insert(pos);
				blocks.insert(pos);
			}
		}
	}
	ret_area_cnt++;
	gettimeofday(&t_end, NULL);
	timersub(&t_end, &t_begin, &t_cost);
	ret_area_time += t_cost.tv_sec + (1.0 * t_cost.tv_usec) / 1000000;
//	printf("After Ret\n");
//	mDatrie->display_used();
}

float AreaContainer::used_rate(){
	float used_cnt = 0.01;
	int low = (*(areas.begin())).start;
	for (area each : areas){
		used_cnt += each.size(); 
	}
	return  1 - used_cnt / (mDatrie->get_size() - low);
}

