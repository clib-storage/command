#include "command.hpp"


class depth_recursion_error : public std::exception{
	private:
		std::string message;
	public:
		depth_recursion_error(std::string s) : std::exception(){
			message = s;
		}
		virtual const char* what() const throw(){
			return message.c_str();
		}
};


namespace Command{ //Command::Input class implementation
		Input::Input(std::istream& in){
			std::string s;
			std::getline(in, s);
			*this = parse(s);
		}

		Input::Input(const std::string& s){
			*this = parse(s);
		}

		Input::Input(const char* s){
			*this = parse(s);
		}

		Input::Input(const Input& c){
			command = c.command;
			args = c.args;
			kwargs = c.kwargs;
			raw_args = c.raw_args;
		}

		Input::Input(){
			command = "";
			args = std::vector<std::string>();
			kwargs = std::map<std::string, std::string>();
			raw_args = "";
		}

		Input::~Input(){
		}

		Input& Input::operator=(const Input& c){
			command = c.command;
			args = c.args;
			kwargs = c.kwargs;
			return *this;
		}

		Input& Input::operator=(const std::string& s){
			*this = parse(s);
			return *this;
		}

		bool Input::operator==(const Input& c) const{
			return command == c.command && args == c.args && kwargs == c.kwargs;
		}

		std::string Input::operator[](const std::string& key) const{
			return kwargs.at(key);
		}

		std::string Input::operator[](const char* key) const{
			return kwargs.at(key);
		}

		std::string Input::operator[](int i) const{
			return args.at(i);
		}

		std::string& Input::operator[](const std::string& key){
			return kwargs[key];
		}

		std::string& Input::operator[](const char* key){
			return kwargs[key];
		}

		std::string& Input::operator[](int i){
			return args[i];
		}

		std::ostream& operator<<(std::ostream& os, const Input& c){
			os << c.command;
			for(int i = 0; size_t(i) < c.args.size(); i++){
				os << " " << c.args[i];
			}
			for(auto it = c.kwargs.begin(); it != c.kwargs.end(); it++){
				os << " " << it->first << "=" << it->second;
			}
			return os;
		}

		std::istream& operator>>(std::istream& is, Input& c){
			std::string s;
			std::getline(is, s);
			c = Input::parse(s);
			return is;
		}

		Input Input::parse(const std::string& s){
			Input cmd;
			std::vector<std::string> tokens = split(s, ' ');
			cmd.command = tokens[0];
			for(int i = 1; size_t(i) < tokens.size(); i++){
				cmd.raw_args += tokens[i] + " ";
				if(tokens[i].find('=') != std::string::npos){
					std::vector<std::string> kv = split(tokens[i], '=');
					cmd.kwargs[kv[0]] = kv[1];
				}else{
					cmd.args.push_back(tokens[i]);
				}
			}
			return cmd;
		}

		Input Input::parse(const char* s){
			return parse(std::string(s));
		}
}

namespace Command{ //Command::Command class implementation


	Command::Command(const std::string& _name, const std::string& _description, const std::vector<std::string>& long_desc, const std::string& _usage)
		: name(_name), description(_description), long_description(long_desc), usage(_usage){
		parse_usage();
	}

	Command::Command(const std::string& _name)
		: name(_name), description(""), usage(_name){
	}

	Command::Command(const char* _name, const char* _description, const char** long_desc, const size_t& long_desc_size, const char* _usage)
		: name(_name), description(_description), usage(_usage){
		for(size_t i = 0; i < long_desc_size; i++){
			long_description.push_back(long_desc[i]);
		}
		parse_usage();
	}

	Command::Command(const std::string& _name, const std::string& _description, const std::string& long_desc, const std::string& _usage)
		: name(_name), description(_description), long_description(split(long_desc, '\n')), usage(_usage){
		parse_usage();
	}
	Command::Command(const char* _name, const char* _description, const char* long_desc, const char* _usage)
		: name(_name), description(_description), long_description(split(long_desc, '\n')), usage(_usage){
		parse_usage();
	}
	
	Command::Command(const char* _name)
		: name(_name), description(""), usage(_name){
	}

	Command::Command(const Command& c)
		: name(c.name), description(c.description), usage(c.usage){
			master = nullptr;
	}

	Command::~Command(){
	}

	void Command::setName(const std::string& name){
		master->removeCommand(this);
		this->name = name;
		master->addCommand(this);
	}

	//count the number of required arguments (in <>) and optional arguments (in [])
	void Command::parse_usage(){
		std::vector<std::string> tokens = split(usage, ' ');
		tokens.erase(tokens.begin()); //the first token is the command name
		for(auto t : tokens){			
			if(t == "[args...]"){
				//the user can pass any number of arguments, and their keyes will be the position in the list (in optional_args)
				//this is a special case, so we handle it here
				optional_args.push_back("args...");
				set_default_value("args...", "");
				args_ordered.push_back("args...");
				//if they are remaining arguments, they will be ignored
				break; //we don't need to parse the rest of the usage string
			}
			if(t[0] == '[' && t[t.size()-1] == ']'){
				optional_args.push_back(t.substr(1, t.size()-2));
				set_default_value(t.substr(1, t.size()-2), "");
				args_ordered.push_back(t.substr(1, t.size()-2));
			}else if(t[0] == '<' && t[t.size()-1] == '>'){
				required_args.push_back(t.substr(1, t.size()-2));
				args_ordered.push_back(t.substr(1, t.size()-2));
			}else{
				std::cout << "Invalid usage string: " << usage << std::endl;
			}
		}
	}

	void Command::set_default_value(const std::string& arg, const std::string& value){
		default_values[arg] = value;
	}
	void Command::set_default_value(const char* arg, const char* value){
		default_values[arg] = value;
	}

	char Command::is_argument(const std::string& arg) const{
		if(std::find(required_args.begin(), required_args.end(), arg) != required_args.end()){
			return 1;
		}else if(std::find(optional_args.begin(), optional_args.end(), arg) != optional_args.end()){
			return 2;
		}else{
			return 0;
		}
	}

}

namespace Command{ //Command::CommandManager class implementation

	CommandManager::CommandManager(std::string _name, std::istream& _in, std::ostream& _out, std::ostream& _err)
		: in(_in), out(_out), err(_err), name(_name){
	}
	CommandManager::~CommandManager(){
		for(auto it = commands.begin(); it != commands.end(); it++){
			delete it->second;
		}
	}


	std::string CommandManager::parse_question(){
		std::string question = this->question;
		size_t pos = question.find("%name");
		while(pos != std::string::npos){
			question.replace(pos, 5, name);
			pos = question.find("%name");
		}
		return question;
	}


	void CommandManager::addCommand(Command* c){
		if(this == c->master) return;
		commands[c->name] = c;
		c->master = this;
	}
	void CommandManager::removeCommand(const std::string& name){
		commands[name]->master = nullptr;
		commands.erase(name);
	}
	void CommandManager::removeCommand(const char* name){
		commands[name]->master = nullptr;
		commands.erase(name);
	}
	void CommandManager::removeCommand(Command* c){
		c->master = nullptr;
		commands.erase(c->name);
	}


	std::vector<std::string> CommandManager::similar(const std::string& name,unsigned int max) const{
		std::vector<std::string> similar;
		for(auto it = commands.begin(); it != commands.end(); it++){
			if(difference(it->first, name) <= max){
				similar.push_back(it->first);
			}
		}
		return similar;
	}


	void CommandManager::execute(Input i){
		if(i.name() == ""){
			return;
		}
		if(commands.find(i.name()) != commands.end()){ //the command exists
			::Command::Command* cmd = commands[i.name()]; //get the command
			std::vector<std::string> missing = cmd->required_args; //get the required arguments
			std::vector<std::string> missings_optional = cmd->optional_args; //get the optional arguments
			Command::Command::Kwargs kwargs;

			for(auto kwarg : i.getKwargs()){ //set the default values
			std::string key = kwarg.first, value = kwarg.second;
				if(cmd->is_argument(key) > 0){ //the kwargs is ok
					kwargs[key] = value;
					missing.erase(std::remove(missing.begin(), missing.end(), key), missing.end());
					missings_optional.erase(std::remove(missings_optional.begin(), missings_optional.end(), key), missings_optional.end());

				}else{ //the kwargs is not ok
					print("Command '" + i.name() + "' does not have an argument '" + key + "'. Ingoring it.");
				}
			}
			//here, we've parsed only the kwargs, now we parse the args

			for(size_t j = 0; j < missing.size(); ++j){// get missings arguments from the input
				if(j < i.getArgs().size()){ //if there is enough arguments
					kwargs[cmd->args_ordered[j]] = i.getArgs()[j];
				}else{
					throw CommandException("Command '" + i.name() + "' required argument '" + cmd->args_ordered[j] + "' is missing.");
				}
			}
			for(size_t j = 0; j < missings_optional.size(); ++j){// get missings optional arguments from the input
				if(j < i.getArgs().size()){
					kwargs[cmd->args_ordered[j]] = i.getArgs()[j];
				}else{
					break; //if there is no more arguments, we stop
				}
			}
			//here, we parsed the gived arguments, so now, if all are good, we only have optional arguments to parse

			for(auto arg : cmd->optional_args){ // set the default values for the optional arguments
				if(kwargs.find(arg) == kwargs.end()){
					if(cmd->default_values.find(arg) != cmd->default_values.end()){
						kwargs[arg] = cmd->default_values[arg];
					}else{
						throw CommandException("Command '" + i.name() + "' required argument '" + arg + "' does not have a default value.");
					}
				}
			}
			
			//if there is more arguments than the command can handle, we print an error
			if(i.getArgCount() + i.getKwargCount() > cmd->args_ordered.size()){
				std::string msg = "Command '" + i.name() + "' has too many arguments. ";
				msg += "The command can handle " + std::to_string(cmd->args_ordered.size()) + " arguments, but " + std::to_string(i.getArgCount()+i.getKwargCount()) + " were given.";
				throw CommandException(msg);
			}

			cmd->execute(kwargs);


		}
		else if(allow_execution){
			try{
				execute_file(i.name(), i.getArgs());
			}catch(CommandException& e){
				err << e.what() << std::endl;
			}
		}
		else{ // if the command does not exist
			std::vector<std::string> similar = this->similar(i.name(), 2);
			std::string msg = "Command '" + i.name() + "' not found.";
			if(similar.size() > 0){
				msg += " Did you mean " + similar[0] + " ?";
			}
			print(msg);
		}
	}
	void CommandManager::execute(const std::string& s){
		execute(Input::parse(s));
	}
	void CommandManager::execute(const char* s){
		execute(Input::parse(s));
	}

	void CommandManager::execute_file(const fs::path& executable, const std::vector<std::string>& args){
		if(fs::exists(executable)){
			if(fs::is_regular_file(executable)){
				std::string cmd = executable.string();
				for(auto arg : args){
					cmd += " " + arg;
				}
				int res = system(cmd.c_str());
				if(res != 0){
					throw CommandException("The file '" + executable.string() + "' returned an error code.");
				}
			}
			else{
				throw CommandException("The file '" + executable.string() + "' is not a regular file.");
			}
		}
		else{
			throw CommandException("The file '" + executable.string() + "' does not exist.");
		}
	}

	void CommandManager::operator()(Input& i){
		execute(i);
	}
	void CommandManager::operator()(const std::string& s){
		execute(s);
	}
	void CommandManager::operator()(const char* s){
		execute(s);
	}

	void CommandManager::printHelp() const{
		unsigned int max_usage_length = 0;
		for(auto it = commands.begin(); it != commands.end(); it++){
			if(it->second->usage.size() > max_usage_length){
				max_usage_length = it->second->usage.size();
			}
		}
		for(auto it = commands.begin(); it != commands.end(); it++){
			out << extend(it->second->usage, max_usage_length+4) << it->second->description << std::endl;
		}
	}

	void CommandManager::printHelp(const std::string& name) const{
		if(commands.find(name) != commands.end()){
			out << "Usage :" << std::endl;
			out << '\t' << commands.at(name)->usage << std::endl;
			out << "Description :" << std::endl;
			if(commands.at(name)->getLongDescription().size() > 0){
				for(std::string line : commands.at(name)->getLongDescription()){
					out << '\t' << line << std::endl;
				}
			}
			else{
				out << '\t' << commands.at(name)->description << std::endl;
			}
		}else{
			out << "Command '" << name << "' not found." << std::endl;
		}
	}

	int CommandManager::mainloop(){
		std::string line;
		mainloop_running = true;
		while(mainloop_running){
			set_exit_code(EXIT_SUCCESS); //we reset the exit code
			out << parse_question();
			std::getline(in, line);
			if(in.eof()){ //if the input is closed, we stop the mainloop
				mainloop_running = false;
				return EXIT_SUCCESS;
			}
			if(line.size() == 0){ //if the line is empty, we continue
				continue;
			}
			try{
				execute(line);
			}catch(CommandException& e){
				err << e.what() << std::endl;
			}
		}
		return get_exit_code();
	}

	PreDefinedCmd::HelpCommand::HelpCommand(std::ostream& _out)
		: Command("help", "Prints this help message.","", "help [command]"), out(_out){
			set_default_value("command", "");
	}
	PreDefinedCmd::HelpCommand::~HelpCommand(){
	}
	void PreDefinedCmd::HelpCommand::execute(const Kwargs& kwargs){
		if(kwargs.at("command") == ""){
			master->printHelp();
		}else{
			master->printHelp(kwargs.at("command"));
		}
	}

	void PreDefinedCmd::ExitCommand::execute(const Kwargs&){
		master->stopMainloop();
	}

}

namespace Command{ //Command::CommandException implementation

	CommandException::CommandException(const std::string& msg)
		: message(msg){
	}
	CommandException::CommandException(const char* msg)
		: message(msg){
	}
	CommandException::CommandException(const CommandException& e){
		message = e.message;
	}
	CommandException::CommandException(){
		message = "";
	}

	CommandException::~CommandException(){
	}

	const char* CommandException::what() const throw(){
		return message.c_str();
	}

} // namespace Command