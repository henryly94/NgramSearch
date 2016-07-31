#include "Datrie.hpp"
#include <string>
#include <stdio.h>
#include <fstream>
#include <vector>
#include <stdlib.h>
#include <time.h>

Datrie* a;
float MAX;

void query(std::string key){
	printf("%-10s:%8d\n", key.c_str(), a->query(key));
}

int quiet_query(Datrie* da, std::string key){
	return da->query(key);
}


std::vector<std::string> bin_split(std::string ori_string, char delim=' '){
	std::vector<std::string> ret;
	int space = ori_string.find(delim);
	if (space != -1){
		ret.push_back(ori_string.substr(0, space));
		ret.push_back(ori_string.substr(space + 1));
	} 
	return ret;
}

int main(int argc, char* argv[]){
	if (argc > 2){
	 
		a = new Datrie(0);
		


	} else {
		if (argc == 2){
			MAX = atoi(argv[1]);
		} else {
			MAX = 750371;
		}
	
		std::ifstream in;
	
		std::string path("testfile");	

		in.open(path.c_str(), std::ios_base::in);
		
		char buffer[128];
		std::vector<std::string> pairs;

		a = new Datrie(173);

		AreaContainer* container = a->get_areaContainer();
		int cnt = 0, c_cnt = 0;

		time_t time_start, time_end;

		time_start = time(NULL);

		while (!in.eof()){
			in.getline(buffer, 128, '\n');
			std::string str = std::string(buffer);
			std::vector<std::string> pair = bin_split(str);
			if (pair.size() == 2){
				cnt++;
				a->insert(pair[0].c_str(), atoi(pair[1].c_str()));
				pairs.push_back(str);
			}
			printf("----%d%%----\r", (int)(cnt*100 / MAX));
			fflush(stdout);
		}
		printf("\n");
		printf("Using rate: %.3f %%\n", container->used_rate() * 100 );
		time_end = time(NULL);
		printf("Time: %.0f s\n", difftime(time_end, time_start));
			
//		a->display(10000);	

		printf("Now saving...\n");

		a->save(std::string("test_save"));

		Datrie* b = new Datrie(0);

		printf("Now loading\n");

		b->load(std::string("test_save"), 173);
		
//		b->display(1000);

		for(std::string  _str : pairs){
			std::string str = std::string(_str);
			std::vector<std::string> pair = bin_split(str);
			if (pair.size() == 2){
				if (quiet_query(b, pair[0].c_str()) == atoi(pair[1].c_str())){
					c_cnt++;
				} else {
					printf("%s\n", pair[0].c_str());
				}
			}
		}
		

		printf("Correctness: %d/%d\n", c_cnt, cnt);

		printf("===================\n");

		printf("Get Area: %d, %lf \n", container->get_area_cnt, container->get_area_time);
		printf("Ret Area: %d, %lf \n", container->ret_area_cnt, container->ret_area_time);
		printf("Sol Coll: %d, %lf: %lf| %lf| %lf  \n", a->solve_cnt, a->solve_time, a->solve_1, a->solve_2, a->solve_3);
		printf("Sol 3 detail: %lf, %lf, %lf \n", a->solve_3_1, a->solve_3_2, a->solve_3_3);
		
//		a->save(std::string("test_save"));		

	}
	return 0;
}
