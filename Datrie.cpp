#include <stdio.h>
#include "Datrie.hpp"
#include <string>
#include <string.h>
#include <vector>
#include <algorithm>
#include <functional>
#include <sys/time.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <fstream>
#include <iostream>

using std::cout;
using std::endl;

#define WRITE(x) write((char*)&x, 4)
#define READ(x) read((char*)&x, 4)

const int Datrie::NULL_VALUE = -1;

inline void d(int n){
	printf("%d\n", n);
}

inline void d(){
	printf("\n");
}

inline void d(char ch){
	printf("%c\n", ch);
}

inline vector<string> split_phrase(string phrase, char delim=' '){
	vector<string> result;
	int head = 0, tail;
	for (tail=0; tail<phrase.size(); tail++){
		if (phrase[tail] == delim){
			if (tail > head){
				result.push_back(phrase.substr(head, tail - head));
			}
			head = tail + 1;
		}
	}
	if (head <= tail){
		result.push_back(phrase.substr(head, tail-head));
	}
	return result;
}	

inline int has(int n){
	return n;
}

int Datrie::hash(std::string str, int (*func)(std::string)){
	return func(str);
}

int Datrie::hash(char ch){
	if ((ch < 'A' || ch > 'Z') && (ch < 'a' || ch > 'z') && (ch > '9' || ch < '0') && ch != '\'' && ch != ' ') return 0;
	else if (ch >= 'A' && ch <= 'Z'){
		return ch - 'A' + 1;
	} else if (ch >= 'a' && ch <= 'z'){
		return ch - 'a' + 27;
	} else if (ch >= '0' && ch <= '9'){
		return ch - '0' + 53;
	} else if (ch == '\''){
		return 63;
	} else { // 'space'
		return 64;
	}
}

int Datrie::hash(std::string str){
	int ret = 0;
	for(int i=0; i<str.size(); i++){
		ret = (ret * RANGE) + hash(str[i]);
	}
	return ret;
}

int Datrie::word_hash(string str){
	unordered_map<string,int>::iterator it = alphabet.find(str);
	if (it != alphabet.end()){
		return it->second;
	} else {
		return UNEXIST_VALUE;
	}
}


void Datrie::word_insert(string key, int value){
	int s = 0, t, ch;
	for (auto  each : split_phrase(key)){
		new_word(each);
		ch = word_hash(each);
		t = base[s].base + ch;
		while (t >= size){
			if (!double_size()){
				finish_flag = false;
				return;
			}
		}
		if (t <= 0){
			if ((t = solve_collision(s, t)) == NULL_VALUE){
				finish_flag = false;
				return;
			}
			
			base[s].son.push_back(t);
			s = t;
		} else if (check[t] == -1){
			mAreaContainer->get_area(area(t));
			check[t] = s;
			base[t].base = base[s].base;
			base[s].son.push_back(t);
			s = t;	
		} else if (check[t] != s) { // Collision
			if ((t = solve_collision(s, t)) == NULL_VALUE){
				finish_flag = false;
				return;
			}
			base[s].son.push_back(t);
			s = t;
		} else {
			s = t;
		}
	}

	base[s].value = value;
	dirty = true;
	finish_flag = true;
}

void Datrie::new_word(string word){
	if (alphabet.find(word) != alphabet.end()){
		return;
	} else {
		alphabet.insert(make_pair(word, hash_cnt));
		alpha_save.push_back(word);
		hash_cnt++;
		RANGE = hash_cnt;
	}
}

int Datrie::word_query(string key){
	int s = 0;
	int t;
	for (auto each : split_phrase(key)){
		int ch = word_hash(each);
		t = base[s].base + ch;
		if (check[t] == s){
			s = t;
		} else {
			return UNEXIST_VALUE;
		}
	}
	return base[s].value;

}

std::vector<std::string> split(std::string str, char delim = ' '){
	std::vector<std::string> ret;
	std::string tmp;
	for (auto ch : str){
		if(ch != delim){
			tmp.push_back(ch);
		} else {
			ret.push_back(tmp);
			tmp.clear();
		}
	}
	return ret;
}

std::vector<int> split_son(std::string str, char delim = '|'){
	std::vector<int> ret;
	std:: string tmp;
	for (auto ch : str){
		if (ch != delim){
			tmp.push_back(ch);
		} else {
			if (!tmp.empty())
				ret.push_back(atoi(tmp.c_str()));
			tmp.clear();
		}
	}
	return ret;
}


int Datrie::get_size(){
	return size;
}

int Datrie::get_id(){
	return ID;
}

void Datrie::display(int n){
	if (n > size)
		n = size;;
	printf("#            base            value            check\n");
	for (int i=0; i<n; i++){
		printf("%-3d:   %12d   %12d   %12d\n", i, base[i].base, base[i].value, check[i]);
	}
}

void Datrie::build_alphabet(string path){
	ifstream in(path);
	string word;
	while (!in.eof()){
		in >> word;
		alphabet.insert(std::make_pair(word, hash_cnt));
		alpha_save.push_back(word);
		hash_cnt++;
	}
	in.close();
	RANGE = hash_cnt;
	BUILD_FLAG = true;
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


Datrie::Datrie(int id):ID(id){
	size = 128;
	cnt = 0;
	insert_cnt = 0;
	dirty = false;
	base.push_back(node(BEGIN_VALUE, NULL_VALUE));
	base.resize(size, node(NULL_VALUE, NULL_VALUE));
	check.push_back(ROOT_VALUE);
	check.resize(size, NULL_VALUE);
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

int Datrie::query(char* key){
	return query(std::string(key));
}

void Datrie::insert(char* key, int value){
	insert(std::string(key), value);
}

void Datrie::insert(std::string key, int value){
//	printf("Insert: %s\n", key.c_str());
	int s = 0, t, ch;
	for (auto  each : key){
		ch = hash(each);
		t = base[s].base + ch;
		
		while (t >= size)double_size();
		if (t <= 0){
			t = solve_collision(s, t);
			base[s].son.push_back(t);
			s = t;
		} else if (check[t] == -1){
			mAreaContainer->get_area(area(t));
			check[t] = s;
			base[t].base = base[s].base;
			base[s].son.push_back(t);
			s = t;	
		} else if (check[t] != s) { // Collision
			t = solve_collision(s, t); 
			base[s].son.push_back(t);
			s = t;
		} else {
			s = t;
		}
	}
	base[s].value = value;
	dirty = true;
}


bool Datrie::double_size() {
	if (size>=MAX_SIZE){
		finish_flag = false;
		return false;	
	}
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
	//printf("Size:%-20d\n", size);
	finish_flag = true;
	return true;
}

int Datrie::solve_collision(int base_s, int coll_s){
		
	std::vector<int> offsets;
	int old_base = base[base_s].base;
	int coll_off = coll_s - old_base;
	int new_base, range, low_bound;
	
	std::sort(base[base_s].son.begin(), base[base_s].son.end());

	for (auto each : base[base_s].son){
		offsets.push_back(each - old_base);
	}
	/*
	for (int i=std::max(old_base, 1); i<=old_base + RANGE; i++){
		if (check[i] == base_s) offsets.push_back(i-old_base);
	}	
	*/

	if (offsets.size() == 0){
		range = 1;
		low_bound = coll_off;
	} else {
		range = std::max(*(offsets.end()-1), coll_off) - std::min(coll_off, *(offsets.begin())) + 1;
		low_bound = std::min(coll_off, *(offsets.begin()));
	}
	offsets.push_back(coll_off);
	int tmp = mAreaContainer->get_base(range);
	if (tmp != NULL_VALUE){
		new_base = tmp - low_bound;
	} else {
		return NULL_VALUE;
	}
	
	for (std::vector<int>::iterator it = base[base_s].son.begin(); it != base[base_s].son.end(); it++){
		(*it) += (new_base - old_base);
	}

	base[base_s].base = new_base;

	offsets.pop_back();	
	for (int off : offsets){
		int new_pos = new_base + off;
		int old_pos = old_base + off;
		while(new_pos >= size){
			if(!double_size()){
				return NULL_VALUE;
			}
		}
		check[new_pos] = base_s;
		base[new_pos] = base[old_pos];

		check[old_pos] = NULL_VALUE;
		base[old_pos].base = NULL_VALUE;
		base[old_pos].value = NULL_VALUE;
		base[old_pos].son.clear();
	
		mAreaContainer->get_area(area(new_pos));
		mAreaContainer->ret_area(area(old_pos));

	}
	base[new_base + coll_off].base = new_base;
	check[new_base + coll_off] = base_s;
	mAreaContainer->get_area(area(new_base + coll_off));

	for (int each : offsets){
		for (int every : base[each + new_base].son){
			check[every] += (new_base - old_base);
		}
	}

	return new_base + coll_off;
}

void Datrie::save(std::string path){
	DIR* dir_p = opendir(path.c_str());
	if (dir_p == NULL) {
		mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IRWXO);
	}
	save_info(path);
	save_arrays(path);
	save_alphabet(path);
	mAreaContainer->save(path);
	dirty = false;
}

void Datrie::save_alphabet(std::string path){
	char buf[128];
	sprintf(buf, "/%08d.alp", ID);
	path.append(buf);
	std::ofstream out(path);

	// Infos
	
	for (auto each : alpha_save){
		out << each << endl;
	}

	out.close();
}

void Datrie::save_info(std::string path){
	char buf[128];
	sprintf(buf, "/%08d.nsi", ID);
	path.append(buf);
	std::ofstream out(path, std::ios::binary);

	// Infos
	out.WRITE(size);
	out.WRITE(cnt);
	out.close();
}

void Datrie::save_arrays(std::string path){
	char buf[128];
	sprintf(buf, "/%08d.nsd", ID);
	path.append(buf);
	std::ofstream out(path, std::ios::binary);
	for (int i=0; i<cnt; i++){
		if (check[i] != NULL_VALUE){
			out.WRITE(i);
			out.WRITE(base[i].base);
			out.WRITE(base[i].value);
			out.WRITE(check[i]);
			int tmp_size = base[i].son.size();
			out.WRITE(tmp_size);
			for (int each : base[i].son){
				out.WRITE(each);
			}
		}
	}
	out.close();
}

void Datrie::load(std::string path, int id, Datrie::loadmode lm){
	//printf("Load %s id %d\n", path.c_str(), id);	
	DIR* dir_p = opendir(path.c_str());
	if (dir_p == NULL)
		printf("Wrong Directory when loading:%s, id = %d\n", path.c_str(), id);
	else {
		std::ifstream in;
		std::vector<int> area_se;
		std::string base_path(path);
		switch (lm) {
		case Cover:
			char buf[128];
			sprintf(buf, "/%08d.nsi", id);
			path.append(buf);
			in.open(path, std::ios_base::in | std::ios_base::binary);
			if (!in.is_open()){
				printf("No such File when open:%s\n", path.c_str());
				return;
			}
			in.READ(size);
			in.READ(cnt);
			base.clear();
			base.shrink_to_fit();
			check.clear();
			check.shrink_to_fit();
			base.resize(size, node(NULL_VALUE, NULL_VALUE));
			check.resize(size, NULL_VALUE);
			in.close();

			path = base_path;
			sprintf(buf, "/%08d.nsd", id);
			path.append(buf);
			in.open(path.c_str(), std::ios_base::in | std::ios_base::binary);
			if (!in.is_open()){
				printf("No such file when open:%s\n", path.c_str());
				return;
			}
			base[0].base = BEGIN_VALUE;
			check[0] = ROOT_VALUE;
			char buffer[1024];
			int index;
			while (!in.eof()){
				in.READ(index);
				in.READ(base[index].base);
			        in.READ(base[index].value);
			       	in.READ(check[index]);
				int N, tmp;
				in.READ(N);
				for (int i=0; i<N; i++){
					in.READ(tmp);
					base[index].son.push_back(tmp);	
				}
			}
			in.close();
			path = base_path;
			sprintf(buf, "/%08d.nsa", id);
			path.append(buf);
			in.open(path.c_str(), std::ios_base::in | std::ios_base::binary);
			if (!in.is_open()){
				printf("No such file when open:%s\n", path.c_str());
				return;
			}
			mAreaContainer->areas.clear();
			mAreaContainer->blocks.clear();
			while (!in.eof()){
				int tmp_start, tmp_end;
				in.READ(tmp_start);
				in.READ(tmp_end);
				area new_area(tmp_start, tmp_end);
				mAreaContainer->areas.insert(new_area);
				mAreaContainer->blocks.insert(new_area);
			}
			in.close();

			path = base_path;
			sprintf(buf, "/%08d.alp", id);
			path.append(buf);
			in.open(path.c_str(), std::ios_base::in);
			if (!in.is_open()){
				printf("No such file when open:%s\n", path.c_str());
				return;
			}
			while (!in.eof()){
				std::string tmp_word;
				in >> tmp_word;
				new_word(tmp_word);
			}	
			
			break;
		case Add:
		default:
			break;
		} 

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

AreaContainer* Datrie::get_areaContainer(){
	if (mAreaContainer != NULL)
		return mAreaContainer;
}

AreaContainer::AreaContainer(Datrie* da){
	mDatrie = da;
}

int AreaContainer::get_base(int range){
	std::multiset<area>::iterator it = blocks.lower_bound(area(0, range));
	if (it != blocks.end()){
		return (*it).start;
	} else {
		if (mDatrie->double_size()){
			return get_base(range);	
		} else {
			return Datrie::NULL_VALUE;
		}
	}
}

void AreaContainer::get_area(area  pos){
	if (pos.size() <= 0) return;
	if (pos.end > mDatrie->cnt) mDatrie->cnt = pos.end;

	std::multiset<area>::iterator it = areas.upper_bound(pos), b_it;
	if (it != areas.begin()){
		it--;
		area tmp(*it);
		areas.erase(it);
		b_it = blocks.lower_bound(tmp);
		blocks.erase(b_it);
		area front(tmp.start, pos.start);
		if (front.size()){
			areas.insert(front);
			blocks.insert(front);
		}
		area back(pos.end, tmp.end);
		if (back.size()){
			areas.insert(back);
			blocks.insert(back);
		}
	}
}

void AreaContainer::ret_area(area pos){
	std::multiset<area>::const_iterator it, b_it;
	it = areas.upper_bound(pos);
	if (it != areas.begin()){
		area tmp(*it);
		if ((*it).start == pos.end){
			b_it = blocks.lower_bound(tmp);
			pos.end = tmp.end;
			it = areas.erase(it);
			blocks.erase(b_it);
		}
		it--;		
		tmp = *it;
		if ((*it).end == pos.start){
			b_it = blocks.lower_bound(tmp);
			pos.start = tmp.start;
			areas.erase(it);
			blocks.erase(b_it);
		}
		if (pos.size())	{
			areas.insert(pos);
			blocks.insert(pos);
		}


	} else {
		if ((*it).start == pos.end){
			area front(pos.start, (*it).end);
			area tmp(*it);
			b_it = blocks.lower_bound(tmp);
			areas.erase(it);
			blocks.erase(b_it);
			if (front.size()) {
				areas.insert(front);
				blocks.insert(front);
			}
		} else {
			if (pos.size()){
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

void AreaContainer::save(std::string path){
	char buf[128];
	sprintf(buf, "/%08d.nsa", mDatrie->ID);
	path.append(buf);
	std::ofstream out(path.c_str(), std::ios_base::binary);
	
	for (auto each : areas){
		out.WRITE(each.start);
	        out.WRITE(each.end);
	}	
	out.close();
}
