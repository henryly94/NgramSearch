#include "NgramSearch.hpp"
#include <string>
#include <algorithm>
#include <iostream>
using namespace std;


bool cmp(const datrie_info &a, const datrie_info &b){
	return a.id < b.id;
}

bool str_cmp(const datrie_info &a, const datrie_info &b){
	return a.start < b.start;
}


NgramSearch::NgramSearch(string path, int cache_size):path_(path), cache_(cache_size, path){
	cnt = 0;
}


void NgramSearch::new_db(string key){
	datrie_info new_da_entry(cnt, 0, key);
	datries_.push_back(new_da_entry);
	cursor_ = new Datrie(cnt);
	cache_.push(cursor_);
	cnt += 1;	
}

void NgramSearch::switch_db(string key){
	if (cursor_ != NULL){
		if (find_id(key) == cursor_->get_id()){
			return;
		}
	}
	Datrie* tmp;
	int new_id = find_id(key);
	if ((tmp = cache_.has(new_id)) != NULL){
		cursor_ = tmp;
	} else {
		tmp = new Datrie(new_id);
		tmp->load(path_, new_id);
		cursor_ = tmp;
		cache_.push(cursor_);
	}
}

int NgramSearch::find_id(string key){
	vector<datrie_info>::iterator it;
	it = upper_bound(datries_.begin(), datries_.end(), datrie_info(0, 0, key), str_cmp);
	if (it != datries_.begin()){
		it--;
	}
	return (*it).id;
}

void NgramSearch::insert(string key, int value){
	if (datries_.empty()){
		new_db(key);
	}
	switch_db(key);
	cursor_->word_insert(key, value);
	if (!cursor_->finish_flag){
		new_db(key);
		insert(key, value);
	}
}

int NgramSearch::query(string key){
	switch_db(key);
	int tmp = cursor_->word_query(key);
//	cout << key << "    " << tmp << endl;
	return tmp;
}

