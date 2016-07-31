#ifndef DATRIE_HPP
#define DATRIE_HPP

#include <string>
#include <vector>
#include <set>

#define MAX_SIZE 1000000000
#define RANGE ('z' - 'A' + 2)
#define BATCH_MAX 2500
#define USED_RATE_LIMIT 0.4


struct node{
	node(int b=0, int v=0){
		base = b;
		value = v;
	}
	int base;
	int value;
	std::vector<int> son;
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

	int size() const{
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
		return a.size() == b.size() ? a.start < b.start : a.size() < b.size();
	}
};


class AreaContainer;

class Datrie{

	public:
		Datrie(int id);

		enum loadmode {Cover, Add};

		//=====================================
		//	Basic User Interface
		//=====================================

		int query(std::string key);
		
		void insert(std::string key, int value);

		void save(std::string path);

		void load(std::string path, int id = 0, loadmode lm = Cover);


		//======================================
		//	Support class members
		//======================================

		friend class AreaContainer;

		int get_size();

		AreaContainer* get_areaContainer();

		//Support function to show inner condition
		void display(int n);
		void display_used();

		//Temp variable to check time consumption
		int solve_cnt;
		double solve_time, solve_1, solve_2, solve_3, solve_3_1, solve_3_2, solve_3_3;

	private:	
		int ID;

		// Predetermined Values
		int NULL_VALUE = -1;
		int LEAF_VALUE = -2;
		int ROOT_VALUE = -3;
		int UNEXIST_VALUE = -1;
		int BEGIN_VALUE = 1;
		
		// Remain area manager
		AreaContainer* mAreaContainer;	

		std::vector<node> base;
		std::vector<int> check;

		int size;
		int cnt;
		int insert_cnt;

		int solve_collision(int base_s, int coll_s);
		void save_arrays(std::string path);
		void save_info(std::string path);
		void double_size();		
		void try_clean();
};


class AreaContainer{
	private:
		std::multiset<area, areaFunc> areas;
		std::multiset<area, blockFunc> blocks;
		Datrie* mDatrie;
		AreaContainer(Datrie* da);
		~AreaContainer();
		int get_base(int range);
		void get_area(area pos);
		void ret_area(area pos);
	
		void save(std::string path);
		void load(std::string path);
	public:
		float used_rate();
		int get_area_cnt;
		int ret_area_cnt;
		double  get_area_time;
		double ret_area_time;
	
		friend class Datrie;

};

#endif
