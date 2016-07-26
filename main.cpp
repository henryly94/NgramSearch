#include "Datrie.hpp"
#include <string>
#include <stdio.h>
#include <fstream>
#include <vector>
#include <stdlib.h>

Datrie* a;

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
	if (argc > 1){
		a = new Datrie();
		
		a->display_remain();

		a->sort_remain(area(5));
		//a->find_remain(5);

		a->display_remain();

		a->sort_remain(area(10, 15));
		//a->find_remain(33);

		a->display_remain();	
	} else {
	
		std::ifstream in;
	
		std::string path("2.txt");	

		in.open(path.c_str(), std::ios_base::in);
		
		char buffer[128];

		a = new Datrie();
		
		int cnt = 0, c_cnt = 0;
//		printf("HHH\n");
		while (in.getline(buffer, 128, '\n')){
			std::string str = std::string(buffer);
			std::vector<std::string> pair = bin_split(str, '\t');
			if (pair.size() == 2){
				cnt++;
				printf("---%d\r", cnt);
				a->insert(pair[0].c_str(), atoi(pair[1].c_str()));
			}
		}
		
		in.close();

		in.open(path.c_str(), std::ios_base::in);	

		while (in.getline(buffer, 128, '\n')){
			std::string str = std::string(buffer);
			std::vector<std::string> pair = bin_split(str, '\t');
			if (pair.size() == 2){
				if (quiet_query(pair[0].c_str()) == atoi(pair[1].c_str())){
					c_cnt++;
				} else {
					printf("%s\n", pair[0].c_str());
				}
			}
		}
		
		in.close();

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
