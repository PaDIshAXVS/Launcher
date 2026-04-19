#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include "json.hpp"
#include <vector>
#include <sstream>


using json = nlohmann::json;


std::vector<std::string> options={"list","start <preset name>", "delete <preset name>", "add <preset name>"};


int main(int argc, char* argv[]){
	if(argc < 2){
		std::cerr<<"Using: "<<argv[0]<<" <option>"<<" [preset name]\n";
		std::cerr<<"Use: help\n";
		return 1;
	}

	std::ifstream file("presets.json");
	if(!file.is_open()){
		std::cerr<<"Error: file is not opened\n";
		return 1;
	}
	json presets;
	file>>presets;

	std::string option=argv[1];

	if(option  == "start"){
		if(argc < 3){
			std::cerr<<"Using: "<<argv[0]<<" start [preset name]\n";
			return 1;
		}
		std::string preset_name=argv[2];

		if(presets.contains(preset_name)){
			std::cout<<"Launching preset "<<preset_name<<'\n';
			int cur_return;
			for(const auto& app: presets[preset_name]){
				std::string command=app;
				std::string full_command=command+" > /dev/null 2>&1 &";
				std::cout<<"Procces: "<<command<<'\n';
	
				cur_return=std::system(full_command.c_str());
				if(cur_return != 0){
					std::cerr<<"Command "<<command<<" returned with status "<<cur_return<<"!\n";
				}
			}
			std::cout<<"Ready!\n";
		}
		else{
			std::cerr<<"Error: preset "<<preset_name<<" not found\n";
			return 1;
		}
	}

	else if(option == "list"){
		std::cout<<"Avaliable presets:\n";
		for(const auto& [name,command]:presets.items()){
			std::cout<<"* "<<name<<" ("<<command.size()<<") apps\n";
			for(const auto& cmd:command){
				std::cout<<"	-"<<cmd<<'\n';
			}
		}
	}

	else if(option == "delete"){
		if(argc < 3){
			std::cerr<<"Use: delete [preset name]\n";
			return 1;
		}

		std::string preset=argv[2];
		if(!presets.contains(preset)){
			std::cerr<<"Error: preset "<<preset<<" not found\n";
		}
		else{
			presets.erase(preset);
			std::ofstream file("presets.json");
			file<<presets.dump(4);
			std::cout<<"Preset "<<preset<<" deleted\n";
		}

	}

	else if(option == "help"){
		std::cout<<"Avaliable options:\n";
		for(const auto& method: options){
			std::cout<<method<<'\n';
		}
	}

	else{
		std::cerr<<"Unknown option\n";
		return 1;
	}
	return 0;
}
