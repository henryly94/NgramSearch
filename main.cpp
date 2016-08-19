#include "Datrie.hpp"
#include "NgramSearch.hpp"
#include <string>
#include <stdio.h>
#include <fstream>
#include <vector>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <new>

using std::cout;
using std::endl;

Datrie* a;
NgramSearch *n, *b;
float MAX;

void query(std::string key){
	printf("%-10s:%8d\n", key.c_str(), a->query(key));
}

int quiet_query(Datrie* da, std::string key){
	return da->word_query(key);
}

int quiet_query2(NgramSearch* da, std::string key){
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
	char de;
	if (argc > 3){
	 
		a = new Datrie(0);
		


	} else {
		if (argc == 3){
			MAX = atoi(argv[1]);
			de = atoi(argv[2]) == 1 ? ' ' : '\t';	
		} else {
			MAX = 750371;
		}
		printf("%.0f in Total\n", MAX);	

		std::ifstream in, exam;
	
		std::string path("testfile");	
		
		std::string word_path("word");

		in.open(path.c_str(), std::ios_base::in);
		
		char buffer[128];

		a = new Datrie(173);
		
		n = new NgramSearch(std::string("test_save"), 4);
		//a->build_alphabet(word_path);

		//AreaContainer* container = a->get_areaContainer();
		int cnt = 0, c_cnt = 0;

		time_t time_start, time_end;

		time_start = time(NULL);
		

		while (!in.eof()){
			in.getline(buffer, 128, '\n');
			std::string str = std::string(buffer);
			std::vector<std::string> pair = bin_split(str, de);
			if (pair.size() == 2){
				cnt++;
				try{
					//a->word_insert(pair[0].c_str(), atoi(pair[1].c_str()));
					n->insert(pair[0], atoi(pair[1].c_str()));
				} catch (std::bad_alloc &ba){
					std::cerr << "bad_alloc caught:" << ba.what() << '\n';
					break;
				}
				if (cnt >= MAX) break;

				//pairs.push_back(str);
			}
			printf("--------------------------%d%%----%d/%.0f\r", (int)(100 * (cnt / MAX)), cnt, MAX);
			fflush(stdout);
		}
		printf("\n");
		//printf("Using rate: %.3f %%\n", container->used_rate() * 100 );
		time_end = time(NULL);
		printf("Time: %.0f s\n", difftime(time_end, time_start));

		printf("Now saving...\n");

//		a->save(std::string("test_save"));
//		delete n;	
//		n->save();
		delete n;
		time_start = time(NULL);
		printf("Time(saving): %.0f s\n", difftime(time_start, time_end));

//		Datrie* b = new Datrie(0);
		
		b = new NgramSearch(std::string("test_save"), 4);

			
		printf("Now loading\n");

		b->load();

/*
		b->load(std::string("test_save"), 173);

		time_end = time(NULL);
		printf("Time(loading): %.0f s\n", difftime(time_end, time_start));		
*/
		printf("Loading complete\n");
		int e_cnt = 0;
		exam.open(path.c_str(), std::ios_base::in);
		while (!exam.eof()){
			exam.getline(buffer, 128, '\n');
			std::string str(buffer);
			std::vector<std::string> pair = bin_split(str, de);
			if (pair.size() == 2){
				e_cnt++;
				//printf("Query %s\n", pair[0].c_str());
				if (quiet_query2(b, pair[0].c_str()) == atoi(pair[1].c_str())){
					c_cnt++;
				} else {
//					printf("%s\n", pair[0].c_str());
				}
				if (e_cnt >= MAX)break;
			}
		}	
		time_start = time(NULL);
		float dftime = difftime(time_start, time_end);
		printf("Time(querying): %.0f s\n", dftime);
		printf("Each query: %f s\n", dftime / MAX);

		printf("Correctness: %d/%d\n", c_cnt, cnt);

		printf("===================\n");
/*
		printf("Get Area: %d, %lf \n", container->get_area_cnt, container->get_area_time);
		printf("Ret Area: %d, %lf \n", container->ret_area_cnt, container->ret_area_time);
		printf("Sol Coll: %d, %lf: %lf| %lf| %lf  \n", a->solve_cnt, a->solve_time, a->solve_1, a->solve_2, a->solve_3);
		printf("Sol 3 detail: %lf, %lf, %lf \n", a->solve_3_1, a->solve_3_2, a->solve_3_3);
*/

//		a->save(std::string("test_save"));		

	}
	return 0;
}
