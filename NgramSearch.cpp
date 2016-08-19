#include "NgramSearch.hpp"
#include <string>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <cstdlib>

using namespace std;

inline void split(int &a, int &b, string &c, string ori){
	int first = ori.find('\t');
	a = atoi(ori.substr(0, first).c_str());
	string tmp = ori.substr(first+1);
	int second = tmp.find('\t');
	b = atoi(tmp.substr(0, second).c_str());
	c = tmp.substr(second+1);
}

bool cmp(const datrie_info &a, const datrie_info &b){
	return a.id < b.id;
}

bool str_cmp(const datrie_info &a, const datrie_info &b){
	return a.start < b.start;
}


NgramSearch::NgramSearch(string path, int cache_size):path_(path), cache_(cache_size, path){
	cnt = 0;
}

NgramSearch::~NgramSearch(){
	save();
	cache_.clean();
}

void NgramSearch::new_db(string key){
	datrie_info new_da_entry(cnt, 0, key);
	//cout << "New db: " << key << '\t' << cnt << '\n';
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
	return cursor_->word_query(key);
}

void NgramSearch::save(){
	char buf[16] = "/db.info";
	string path(path_);
	path.append(buf);
	ofstream out(path);
	
	for (auto each : datries_){
		out << each.id << '\t' << each.size << '\t' << each.start <<'\n';	
		//cout << each.start << '\t' << each.id << endl;
	}
	out.close();
	cache_.save();
}

void NgramSearch::load(){
	char buf[16] = "/db.info";
	string path(path_);
	path.append(buf);
	ifstream in(path);

	if (!in.is_open()){
		cout << "No such file when open: " << path << endl;
		return;
	}

	int tmp_id, tmp_size;
	string tmp_start;
	char buffer[1024];
	while (!in.eof()){
		in.getline(buffer, 1024, '\n');
		split(tmp_id, tmp_size, tmp_start, string(buffer));
		datries_.push_back(datrie_info(tmp_id, tmp_size, tmp_start));
	}
	datries_.pop_back();
	in.close();
}


void NgramSearch::display(){
	for (auto each : datries_){
		cout << each.id << '\t' << each.start << endl;
	}
}
