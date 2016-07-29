#ifndef DATRIE_HPP
#define DATRIE_HPP

#include <string>
#include <vector>
#include <set>

struct node{
	node(int b=0, int v=0){
		base = b;
		value = v;
	}
	int base;
	int value;
};


struct area{
	area(int s=0, int e=0){
		if (e == 0){
			start = s;
			end = s + 1;
		} 
		else
			start = s, end = e;			
	}
	area(const area &a){
		start = a.start;
		end = a.end;
	}

	area &operator=(const  area &a){
		start = a.start;
		end = a.end;
	}
	
	bool operator==(const area &a) const{ 
		return (start == a.start) && (end == a.end);
	}
	
	bool operator!=(const area &a)const{
		return (start != a.start) || (end != a.end);
	}

	int size(){
		return end - start;
	}
	int start;
	int end;
};

struct areaFunc{
	bool operator() (const area &a, const area &b){
		return a.start < b.start;
	}
};

struct blockFunc{
	bool operator() (const area &a, const area &b){
		return a.end - a.start  < b.end - b.start;
	}
};


class AreaContainer;

class Datrie{
	public:
		Datrie();
		int query(std::string key);
		void insert(std::string key, int value);
		int NULL_VALUE = -1;
		int LEAF_VALUE = -2;
		int ROOT_VALUE = -3;
		int UNEXIST_VALUE = -1;
		AreaContainer* mAreaContainer;
		void display(int n);
		void display_remain();
		void display_used();
		std::vector<area> remain_area;
		std::vector<area> used_area;
		int find_remain(int range, int low_bound, int father);
		void sort_remain(area new_area);
		int get_size();
		void double_size();
	private:
		std::vector<node> base;
		std::vector<int> check;
		int size;
		int cnt;
		int insert_cnt;
		int solve_collision(int base_s, int coll_s);
		void ret_remain(int ret_area);
		int check_valid(int pos, int father);
		void try_clean();
};


class AreaContainer{
	public:
		std::multiset<area, areaFunc> areas;
		std::multiset<area, blockFunc> blocks;
		Datrie* mDatrie;
		AreaContainer(Datrie* da);
		~AreaContainer();
		int get_base(int range);
		void get_area(area pos);
		void ret_area(area pos);
		float used_rate();
};

#endif
