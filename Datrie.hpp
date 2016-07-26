#ifndef DATRIE_HPP
#define DATRIE_HPP

#include <string>
#include <vector>

struct node{
	int base;
	int value;
};


struct area{
	area(int s=0, int e=0, int f=0){
		if (e == 0)
			start = end = s, father=0;
		else
			start = s, end = e, father=f;			
	}
	area(const area &a){
		start = a.start;
		end = a.end;
		father = a.father;
	}
	int size(){
		return end - start;
	}
	int start;
	int end;
	int father;
};

bool area_cmp(area a, area b);

class Datrie{
	public:
		Datrie();
		int query(std::string key);
		void insert(std::string key, int value);
		int NULL_VALUE = -1;
		int LEAF_VALUE = -2;
		int ROOT_VALUE = -3;
		int UNEXIST_VALUE = -1;
		void display(int n);
		void display_remain();
		void display_used();
		std::vector<area> remain_area;
		std::vector<area> used_area;
		//int find_remain(area new_area);
		int find_remain(int range, int low_bound, int father);
		void sort_remain(area new_area);

	private:
		node *base;
		int *check;
		int size;
		int cnt;
		void double_size();
		int solve_collision(int base_s, int coll_s);
		void ret_remain(int ret_area);
		int check_valid(int pos, int father);
};


class AreaContainer{
	private:
		AreaContainer();
		~AreaContainer();
		std::vector<area> areas;
		std::vector<area> blocks;
	public:
		int get_base(int range);
		void get_area(area pos);
		void ret_area(area pos);
};

#endif
