#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include "json.hpp"
#include <vector>
#include <sstream>


using json = nlohmann::json;


std::vector<std::string> options={
	"'list' for all presets",
	"'start' for launching presets",
	"'delete' for deleting presets",
	"'exit' for exit" 
};


void saving(json& presets){
	std::ofstream file("presets.json");
	file<<presets.dump(4);
}


void list_presets(const json& presets){
	std::cout<<"All presets:\n";
	for(const auto& [name,command]:presets.items()){
		std::cout<<"Preset ["<<name<<"] contains "<<command.size()<<" commands:\n";
		for(const auto& cmd:command){
			std::cout<<"	- "<<cmd<<'\n';
		}
	}
}


void start(std::string preset, const json& presets){
	if(preset == "@#" || !presets.contains(preset)){
		while(true){
			std::cout<<"Please, enter preset: ";
			std::cin>>preset;

			if(preset == "list"){
				list_presets(presets);
				continue;
			}

			else if(preset == "exit"){
				std::cin.ignore(10000,'\n');
				return;
			}

			if(!presets.contains(preset)){
				std::cerr<<"Error: preset is not found! Use 'list' for all presets or 'exit' for exit\n";
			}
			else break;
		}
		std::cin.ignore(10000,'\n');
	}

	std::cout<<"Starting preset ["<<preset<<"]:\n";
	for(const auto& app:presets[preset]){
		std::string command=app;
		std::string full_command=command+" > /dev/null 2>&1 &";

		std::cout<<"	-> "<<command<<'\n';

		int status=std::system(full_command.c_str());
		if(status != 0){
			std::cout<<"Process ["<<command<<"] ended with status "<<status;
		}
	}
	std::cout<<"Done!\n";
}


void Delete(std::string preset, json& presets){
	if(preset == "@#" || !presets.contains(preset)){
		while(true){
			std::cout<<"Please, enter preset: ";
			std::cin>>preset;

			if(preset == "list"){ 
				list_presets(presets);
				continue;
			}

			else if(preset == "exit"){
				std::cin.ignore(10000,'\n');
				return;
			}

			if(!presets.contains(preset)){
				std::cerr<<"Error: preset is not found! Use 'list' for all presets or 'exit' for exit\n";
			}
			else break;
		}
		std::cin.ignore(10000,'\n');
	}
	presets.erase(preset);
	saving(presets);
}


int main(){
	std::ifstream file("presets.json");
	json presets;
	if(file.is_open()){
		file>>presets;
		file.close();
	}

	std::string line;
	std::cout<<"Welcome! Type 'help' for commands\n";

	while(true){
		std::cout<<"~ ";

		if(!std::getline(std::cin, line) || line == "exit") break;
		if(line.empty()) continue;

		std::stringstream ss(line);
		std::string cmd;
		std::vector<std::string> argc;
		while(ss >> cmd) 
			argc.push_back(cmd);

		std::string option=argc[0];

		if(option == "help"){
			std::cout<<"Avaliable options:\n";
			for(const auto& method:options){
				std::cout<<method<<'\n';
			}
		}

		else if(option == "list") list_presets(presets);
		else if(option == "start"){
			std::string parametr;
			if(argc.size() > 1) parametr=argc[1];
			else parametr="@#";

			start(parametr, presets);
		}
		else if(option == "delete"){
			std::string parametr;
			if(argc.size() > 1) parametr=argc[1];
			else parametr="@#";

			Delete(parametr, presets);
		}
		else{
			std::cout<<"Error: option is not correct!\n";
		}
	}
	return 0;
}
