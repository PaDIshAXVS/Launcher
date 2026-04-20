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


void show_commands(std::string preset, json& presets){
	int i=1;
	for(const auto& com:presets[preset]){
		std::cout<<i<<". "<<com<<'\n';
		i++;
	}
}


void edit(json& presets){
	std::cout<<"How do you want to edit presets? VS Code/launcher/exit(1/2/'exit')\n";
	
	std::string answer;
	std::cin>>answer;

	if(answer == "1"){
		std::string command="code --wait ";
		command+=PATH;

		int status=std::system(command.c_str());
		if(status != 0){
			std::cerr<<"Process returned with status "<<status<<'\n';
		}

		std::ifstream reload_file(PATH);
		if(reload_file.is_open()){
			presets.clear();
			reload_file>>presets;
			reload_file.close();
		}
	}

	else if(answer == "2"){
		std::cout<<"Enter preset name\n";
		
		std::string preset;
		std::cin>>preset;

		while(!presets.contains(preset)){
			std::cout<<"Enter preset. Use 'list' or 'exit'\n";
			std::cin>>preset;
			std::cin.ignore(10000,'\n');

			if(preset == "list") 
				list_presets(presets);
			
			else if(preset == "exit"){
				std::cin.ignore(10000,'\n');
				return;
			}
		}

		std::cout<<"Changing preset ["<<preset<<"]\n";

		std::string answer;
		std::string command;
		int preset_number;

		while(true){
			show_commands(preset,presets);
			std::cout<<"What to do? (add <cmd> / del <number> / 'exit'): ";
			std::cin>>answer;
			std::cin.ignore(10000,'\n');

			if(answer == "exit")
				return;

			else if(answer == "add"){
				std::getline(std::cin>>std::ws, command);
				presets[preset].push_back(command);
				
				saving(presets);
			}

			else if(answer == "del"){
				std::cin>>preset_number;
				
				if(preset_number > 0 && preset_number <= presets[preset].size()){
					presets[preset].erase(preset_number-1);
				}

				else 
					std::cerr<<"Error: invalid preset index\n";

				saving(presets);
			}

			else	
				std::cerr<<"Error: unknown command\n";
		}
	}

	else 
		return;
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
			edit(presets);
		}

		else{
			std::cout<<"Error: option is not correct!\n";
		}
	}
	return 0;
}
