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

int quiet_query(std::string key){
	return a->query(key);
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
	 
		a = new Datrie();
		
		a->display_remain();


		a->display_remain();

		//a->find_remain(33);

		a->display_remain();	
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
		a = new Datrie();
		int cnt = 0, c_cnt = 0;
//		printf("HHH\n");
//
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
		printf("Using rate: %.3f %%\n", a->mAreaContainer->used_rate() * 100 );
		time_end = time(NULL);
		printf("Time: %.0f s\n", difftime(time_end, time_start));

		for(std::string  _str : pairs){
			std::string str = std::string(_str);
			std::vector<std::string> pair = bin_split(str);
			if (pair.size() == 2){
				if (quiet_query(pair[0].c_str()) == atoi(pair[1].c_str())){
					c_cnt++;
				} else {
					printf("%s\n", pair[0].c_str());
				}
			}
		}
		

		printf("Correctness: %d/%d\n", c_cnt, cnt);

//		a->display(100000);	
	}
	//a->insert("cd", 33);
	//a->insert("aaaaa", 19);
	//a->display(20);
	
//	query("abc");
	
//	query("ac");
//	printf("==========\n");
//	query("helloworld");	
	return 0;
}
