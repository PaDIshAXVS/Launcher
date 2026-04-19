#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include "json.hpp"
#include <vector>
#include <sstream>
#include <filesystem>


using json = nlohmann::json;


std::vector<std::string> options={
	"'list' for all presets",
	"'start' for launching presets",
	"'add' for adding presets",
	"'edit' for editing presets",
	"'delete' for deleting presets",
	"'exit' for exit" 
};

std::string home=std::getenv("HOME");
std::string PATH=home+"/.presets.json";


void saving(json& presets){
	std::ofstream file(PATH);
	file<<presets.dump(4);
}


void list_presets(const json& presets){
	std::cout<<"All presets:\n";

	int i=1;
	for(const auto& [name,command]:presets.items()){
		std::cout<<i<<"'"<<name<<"'\n";
		i++;
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

			else if(!presets.contains(preset)){
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


void add(std::string preset, json& presets){
	if(preset == "@#" || presets.contains(preset)){
		while(true){
			std::cout<<"Please, enter unique preset name: ";
			std::cin>>preset;

			if(preset == "list"){
				list_presets(presets);
				continue;
			}

			else if(preset == "exit"){
				std::cin.ignore(10000,'\n');
				return;
			}

			else if(presets.contains(preset)){
				std::cerr<<"Error: preset ["<<preset<<"] already exist! Use 'list' to see all or 'exit' for exit\n";
			}

			else break;
		}
		std::cin.ignore(10000,'\n');
	}

	std::vector<std::string> new_preset;

	while(true){
		std::cout<<"Enter command, for exit type 'exit': ";
		std::string command;
		std::getline(std::cin, command);

		if(command == "exit")
			break;

		else if(command.empty())
			continue;

		else 
			new_preset.push_back(command);
	}

	std::cout<<"Do you want to save preset?(y/n): ";
	char ans;
	std::cin>>ans;

	if(ans == 'y'){
		for(const auto& cmd:new_preset){
			presets[preset].push_back(cmd);
		}

		std::cin.ignore(10000,'\n');
		saving(presets);
	}

	else{
		std::cin.ignore(10000,'\n');
		return;
	}
}


void edit(std::string preset, json& presets){

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

			else if(!presets.contains(preset)){
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
	std::ifstream file(PATH);
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

		else if(option == "list") 
			list_presets(presets);

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

		else if(option == "add"){
			std::string parametr;
			if(argc.size() > 1) parametr=argc[1];
			else parametr="@#";

			add(parametr,presets);
		}

		else if(option == "edit"){
			std::string parametr;
			if(argc.size() > 1) parametr=argc[1];
			else parametr="@#";

			edit(parametr,presets);
		}

		else{
			std::cout<<"Error: option is not correct!\n";
		}
	}
	return 0;
}
