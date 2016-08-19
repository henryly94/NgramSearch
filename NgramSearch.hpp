#ifndef NGRAM_SEARCH_HPP
#define NGRAM_SEARCH_HPP
#include "Datrie.hpp"
#include <string>
#include <vector>
#include <array>
#include <iostream>


using namespace std;

struct datrie_info{
	int id;
	int size;
	string start;
	datrie_info(int i, int s, string str):id(i), size(s), start(str){};
};

class mQueue{
	public:
		mQueue(int s, string path):size(s), mPath(path){
			data = new Datrie*[s];
			for (int i=0; i<s; i++)data[i] = NULL;
			cnt = 0;
			head = 0;
			tail = -1;
		}
		void display(){
			cout << "head: " << head << " tail: " << tail << endl;
			for (int i=0; i<size; i++){
				if (data[i] == NULL){
					cout << " -1 ";
				} else {
					cout << " " << data[i]->get_id() << " " << data[i] << " "; 
				}
				cout << "|";
			}
			cout << '\n';
		}

		void push(Datrie* new_entry){
			//cout << "Push " << new_entry->get_id() << '\n';
			if (has(new_entry->get_id()) != NULL) {
			//	cout << "Already has " << '\n';
				return;
			}
			if (cnt == size){
				pop();
			} 
			tail = (tail == size-1) ? 0 : tail+1;
			data[tail] = new_entry;	
			cnt++;
			//display();	
		}

		void pop(){
			//cout << "Pop ";
			if (cnt == 0) return;
			//cout << data[head]->get_id() << '\n';
			if (data[head]->dirty){
				data[head]->save(mPath);
			}
			delete data[head];
			data[head] = NULL;
			head = (head == size-1) ? 0 : head+1;
			cnt--;			
			//display();
		}

		Datrie* has(int id){
			if (tail == -1)return NULL;
			for (int i=0; i<cnt; i++){
				if (data[(head+i)%size]->get_id() == id) return data[i];
			}
			return NULL;
		}

		int size;
		int cnt;
		Datrie** data;
		int head;
		int tail;
		string mPath;
};

class NgramSearch{
	private:
		Datrie* cursor_;	
		mQueue cache_;	
		vector<datrie_info> datries_;
		void new_db(string key);	
		void switch_db(string key);
		int find_id(string key);
	
		string path_;

		int cnt;	

	public:
		NgramSearch(string path, int cache_size);

		void insert(string key, int value);
		int query(string key);
};


#endif 
