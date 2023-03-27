#ifndef __command_hpp__
#define __command_hpp__

#include <istream>
#include <ostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>

#include <output.hpp>

#include <cutility.hpp>
using namespace Utility;

#include <filesystem>
namespace fs = std::filesystem;


#define EXIT_RESTART 82 //the ascii code for R

namespace Command{
	class Input;
	class Command;
	class CommandManager;
	class CommandException;



	/**
	 * @brief A command line input, consisting of a command name, a list of arguments and a list of keyword arguments.
	 */
	class Input{
		protected:
			/**
			 * @brief The command name
			 */
			std::string command;
			/**
			 * @brief The list of arguments
			 */
			std::vector<std::string> args;
			/**
			 * @brief The list of keyword arguments
			 */
			std::map<std::string, std::string> kwargs;

			/**
			 * @brief store the raw input string, in case we need to print it of for giving them tho another command
			 */
			std::string raw_args;

		public:
			/**
			 * @brief Construct a new Input object from an input stream
			 * @param in The input stream to read from
			 */
			Input(std::istream& in);
			/**
			 * @brief Construct a new Input object from a string
			 * @param s The string to parse, probably from a file containing all commands
			 */
			Input(const std::string& s);
			/**
			 * @brief Construct a new Input object from a string
			 * @param s The string (char*) to parse, probably from a file containing all commands
			 */
			Input(const char* s);
			/**
			 * @brief Construct a new Input object from another
			 * @param c The Input object to copy
			 */
			Input(const Input& c);
			/**
			 * @brief Construct a new empty Input object
			 */
			Input();
			/**
			 * @brief Destroy the Input object
			 */
			~Input();

			/**
			 * @brief Copy an Input object
			 * @param c The Input object to copy
			 */
			Input& operator=(const Input& c);
			/**
			 * @brief Copy an Input object from a string
			 * @param s The string to parse, probably from a file containing all commands
			 */
			Input& operator=(const std::string& s);

			/**
			 * @brief Compare two Input objects
			 * @param c The other Input object to compare to
			 * @return a boolean indicating whether the two Input objects are equal or not
			 */
			bool operator==(const Input& c) const;

			/**
			 * @brief Get the command name
			 * @return a string containing the command name
			 */
			inline const std::string& name() const { return command; }
			
			/**
			 * @brief Get the command name
			 * @return a string containing the command name
			 */
			inline const std::string& getCommand() const { return command; }
			/**
			 * @brief Get the list of arguments
			 * @return a vector of strings containing the arguments
			 */
			inline const std::vector<std::string>& getArgs() const { return args; }
			/**
			 * @brief Get the list of keyword arguments
			 * @return a map of strings containing the keyword arguments
			 */
			inline const std::map<std::string, std::string>& getKwargs() const { return kwargs; }

			/**
			 * @brief Get a keyword argument
			 * @param key The key of the keyword argument
			 * @return a string containing the value of the keyword argument
			 */
			std::string operator[](const std::string& key) const; // get a kwarg
			/**
			 * @brief Get a keyword argument
			 * @param key The key of the keyword argument
			 * @return a string containing the value of the keyword argument
			 */
			std::string operator[](const char* key) const; // get a kwarg
			/**
			 * @brief Get an argument
			 * @param i The index of the argument
			 * @return a string containing the value of the argument
			 */
			std::string operator[](int i) const; // get an arg

			/**
			 * @brief Get a keyword argument
			 * @param key The key of the keyword argument
			 * @return a refrerence to a string containing the value of the keyword argument
			 */
			std::string& operator[](const std::string& key); // get a kwarg
			/**
			 * @brief Get a keyword argument
			 * @param key The key of the keyword argument
			 * @return a reference to a string containing the value of the keyword argument
			 */
			std::string& operator[](const char* key); // get a kwarg
			/**
			 * @brief Get an argument
			 * @param i The index of the argument
			 * @return a reference to a string containing the value of the argument
			 */
			std::string& operator[](int i); // get an arg

			/**
			 * @brief Set a argument
			 * @param i The key of the keyword argument
			 * @param s The value of the keyword argument
			 */
			inline void setArg(int i, const std::string& s){ args[i] = s;}
			/**
			 * @brief Set a argument
			 * @param i The key of the keyword argument
			 * @param s The value of the keyword argument
			 */
			inline void setArg(int i, const char* s){ args[i] = s;}

			/**
			 * @brief Add a argument
			 * @param s The value of the keyword argument
			 */
			inline void addArg(const std::string& s){ args.push_back(s);}
			/**
			 * @brief Add a argument
			 * @param s The value of the keyword argument
			 */
			inline void addArg(const char* s){ args.push_back(s);}

			/**
			 * @brief tell if a keyword argument exists
			 * @param key The key of the keyword argument
			 * @return a boolean indicating whether the keyword argument exists or not
			 */
			inline bool hasKwarg(const std::string& key) const { return kwargs.find(key) != kwargs.end(); }
			/**
			 * @brief tell if a keyword argument exists
			 * @param key The key of the keyword argument
			 * @return a boolean indicating whether the keyword argument exists or not
			 */
			inline bool hasKwarg(const char* key) const { return kwargs.find(key) != kwargs.end(); }
			/**
			 * @brief tell if an argument exists
			 * @param i The index of the argument
			 * @return a boolean indicating whether the argument exists or not
			 */
			inline bool hasArg(int i) const { return i >= 0 && size_t(i) < args.size(); }

			/**
			 * @brief Get number of arguments
			 * @return a size_t containing the number of arguments
			 */
			inline size_t getArgCount() const { return args.size(); }
			/**
			 * @brief Get number of keyword arguments
			 * @return a size_t containing the number of keyword arguments
			 */
			inline size_t getKwargCount() const { return kwargs.size(); }

			/**
			 * @brief Get the string representation of the Input object
			 * 
			 * @param os The output stream
			 * @param c The Input object
			 * @return a reference to the output stream
			 */
			friend std::ostream& operator<<(std::ostream& os, const Input& c);
			/**
			 * @brief Parse a string and fill an Input object
			 * 
			 * @param is The input stream
			 * @param c The Input object
			 * @return a reference to the input stream
			 */
			friend std::istream& operator>>(std::istream& is, Input& c);

			/**
			 * @brief Parse a string and fill an Input object
			 * 
			 * @param s The string to parse
			 * @return an Input object
			 */
			static Input parse(const std::string& s);
			/**
			 * @brief Parse a string and fill an Input object
			 * 
			 * @param s The string to parse
			 * @return an Input object
			 */
			static Input parse(const char* s);
	};

	/**
	 * @brief A programmer defined command
	 */
	class Command{

		friend class CommandManager;

		protected:
			/**
			 * @brief The name of the command
			 */
			std::string name;
			/**
			 * @brief A short description of the command
			 */
			std::string description;
			/**
			 * @brief A long description (on some rows) of the command
			 */
			std::vector<std::string> long_description;
			/**
			 * @brief The usage of the command (how the user should use it; ex: "command <arg1> [arg2] -k1 <kwarg1> -k2 <kwarg2>")
			 */
			std::string usage;
			/**
			 * @brief A vector containing the names of the required arguments
			 */
			std::vector<std::string> required_args;
			/**
			 * @brief A vector containing the names of the optional arguments
			 */
			std::vector<std::string> optional_args;
			/**
			 * @brief A vector containing all the arguments in the order they were given
			 */
			std::vector<std::string> args_ordered;
			/**
			 * @brief A vector containing the names of the required keyword arguments
			 */
			std::map<std::string, std::string> default_values;

			/**
			 * @brief Construct a instance of command, but with all settings gived in the constructor
			 * @param name: A string containing the name of the command
			 * @param description: A string containing a short description of the command
			 * @param long_desc: A vector of strings containing a long description of the command (one line per string)
			 * @param usage: A string containing the usage of the command
			 */
			Command(const std::string& name, const std::string& description, const std::vector<std::string>& long_desc, const std::string& usage);
			/**
			 * @brief Construct a instance of command, but with all settings gived in the constructor
			 * @param name: A string containing the name of the command
			 * @param description: A string containing a short description of the command
			 * @param long_desc: A table of strings containing a long description of the command (one line per string)
			 * @param long_desc_size: The size of the table
			 * @param usage: A string containing the usage of the command
			 */
			Command(const char* name, const char* description, const char** long_desc, const size_t& long_desc_size, const char* usage);
			/**
			 * @brief Construct a instance of command, but with all settings gived in the constructor
			 * @param name: A string containing the name of the command
			 * @param description: A string containing a short description of the command
			 * @param long_desc: A strings containing a long description of the command, it will be split on '\\n' characters
			 * @param usage: A string containing the usage of the command
			 */
			Command(const std::string& name, const std::string& description, const std::string& long_desc, const std::string& usage);
			/**
			 * @brief Construct a instance of command, but with all settings gived in the constructor
			 * @param name: A string containing the name of the command
			 * @param description: A string containing a short description of the command
			 * @param long_desc: A string containing a long description of the command, it will be split on '\\n' characters
			 * @param usage: A string containing the usage of the command
			 */
			Command(const char* name, const char* description, const char* long_desc, const char* usage);

		private:
			/**
			 * @brief parse the usage string to extract the arguments and keyword arguments
			 */
			void parse_usage();

		public:
			/**
			 * @brief A pointer to the CommandManager that owns this command
			 */
			CommandManager* master = nullptr;

			using Kwargs = std::map<std::string, std::string>;

			/**
			 * @brief Construct a new Command object
			 * 
			 * @param name: A string containing the name of the command
			 * @note The description, long description and usage will be empty, and have to be set using the corresponding setters
			 */
			Command(const std::string& name);
			Command(const char* name);

			/**
			 * @brief Construct a new Command object by copying another one
			 * 
			 * @param c: A reference to the Command object to copy
			 * @note this will note copy the master pointer, il will create a independent command, with the same settings
			 */
			Command(const Command& c);
			Command() = delete;
			/**
			 * @brief Destroy the Command object
			 * @note When creating the herited class, you should write a destructor that will delete all the allocated memory
			 */
			virtual ~Command();

			/**
			 * @brief execute the command with the given input
			 * @param input: An input object containing the arguments and keyword arguments
			 * @note I don't know why you want to call this function, but you can do it if you want
			 */
			virtual void execute(const Kwargs& kwargs) = 0;
			/**
			 * @brief execute the command with the given input
			 * @param input: An input object containing the arguments and keyword arguments
			 * @note I don't know why you want to call this function, but you can do it if you want
			 */
			virtual inline void operator()(const Kwargs& kwargs) final{ execute(kwargs); }

			/**
			 * @brief Get a constant reference to the name of the command
			 * 
			 * @return a constant reference to the name of the command
			 */
			virtual inline const std::string& getName() const final { return name; }
			/**
			 * @brief Get a constant reference to the description of the command
			 * 
			 * @return a constant reference to the description of the command
			 */
			virtual inline const std::string& getDescription() const final { return description; }
			/**
			 * @brief Get a constant reference to the long description of the command
			 * 
			 * @return a constant reference to the long description of the command
			 */
			virtual inline const std::vector<std::string>& getLongDescription() const final { return long_description; }
			/**
			 * @brief Get a constant reference to the usage of the command
			 * 
			 * @return a constant reference to the usage of the command
			 */
			virtual inline const std::string& getUsage() const final { return usage; }

			/**
			 * @brief Set the Name object
			 * 
			 * @param name: A string containing the new name of the command
			 */
			virtual void setName(const std::string& name) final;
			/**
			 * @brief Set the short description of the command
			 * 
			 * @param description: a string containing the new short description of the command
			 */
			virtual inline void setDescription(const std::string& description) final { this->description = description; }
			/**
			 * @brief Set the long description of the command
			 * 
			 * @param long_description: a vector of strings containing the new long description of the command
			 */
			virtual inline void setLongDescription(const std::vector<std::string>& long_description) final { this->long_description = long_description; }
			/**
			 * @brief Set the Long Description object
			 * 
			 * @param long_description: a string containing the new long description of the command
			 * @note The string will be split on '\\n' characters
			 */
			virtual inline void setLongDescription(const std::string& long_description) final { this->long_description = split(long_description, '\n'); }
			/**
			 * @brief Set the Usage of the command
			 * @param usage: a string containing the new usage of the command
			 * @note: this string will be parsed to extract the arguments and keyword arguments
			 */
			virtual inline void setUsage(const std::string& usage) final { this->usage = usage; parse_usage(); }

			/**
			 * @brief Set a default value for the given argument
			 * 
			 * @param arg: the name of the argument
			 * @param value: the default value of the argument
			 */
			virtual void set_default_value(const std::string& arg, const std::string& value) final;
			/**
			 * @brief Set the default value of the given argument
			 * 
			 * @param arg: the name of the argument
			 * @param value: the default value of the argument
			 */
			virtual void set_default_value(const char* arg, const char* value) final;

			/**
			 * @brief Get the default value of the given argument
			 * 
			 * @param arg: the name of the argument
			 * @return a constant reference to the default value of the argument
			 */
			virtual inline const std::string& get_default_value(const std::string& arg) const final { return default_values.at(arg); }
			/**
			 * @brief Get the default value of the given argument
			 * 
			 * @param arg: the name of the argument
			 * @return a constant char* containing the default value of the argument
			 */
			virtual inline const char* get_default_value(const char* arg) const final { return default_values.at(arg).c_str(); }
	
			/**
			 * @brief Return if the passed string is the name a required or optional argument
			 * @return 0 if it's not an argument, 1 if it's a required argument, 2 if it's an optional argument
			 */
			virtual char is_argument(const std::string& arg) const final;
	
	}; // class Command

	/**
	 * @brief A class that manage the commands; you should make an instance of this class in your main function, then add commands to it
	 */

	using CommandMap = std::map<std::string, Command*>;



	namespace PreDefinedCmd{
		/**
		 * @brief Command that will print the help of all commands of the CommandManager
		 * @note to enable this command, you have to use the enableHelp() method
		 */
		class HelpCommand : public Command{
			private:
				std::ostream& out;
			public:
				HelpCommand(std::ostream& _out);
				~HelpCommand();

				void execute(const Kwargs& kwargs);
		};

		/**
		 * @brief Commant that will exit the program
		 * @note to enable this command, you have to use the enableExit() method
		 */
		class ExitCommand : public Command{
			public:
				inline ExitCommand() : Command("exit", "Exits the program", "Get back to the previous interpreter,\nor stop the program if it's the lowest", "exit") {}
				~ExitCommand() = default;

				void execute(const Kwargs&) final;
		};

		/**
		 * @brief Command that will print the current working directory
		 * @note to enable this command, you have to use the enableFs() method
		 */
	}


	class CommandManager{

		private:
		
			std::string question = "(%name) ";
			/**
			 * @brief The input stream of the CommandManager
			 * @note by default, it's std::cin
			 */
			std::istream& in;
			/**
			 * @brief The output stream of the CommandManager
			 * @note by default, it's std::cout
			 */
			std::ostream& out;
			/**
			 * @brief The error stream of the CommandManager
			 * @note by default, it's std::cerr
			 */
			std::ostream& err;
			/**
			 * @brief The name of the CommandManager
			 */
			std::string name;

			/**
			 * @brief if this variable is true and the command is not found but it's a file, the manager will try to execute it
			 */
			bool allow_execution = true;

			/**
			 * @brief store the returned value if the program stop
			 */
			int return_value = EXIT_SUCCESS;

			std::string parse_question();

		protected:
			/**
			 * @brief The map containing all the commands of the CommandManager
			 */
			CommandMap commands;
			/**
			 * @brief true if the mainloop is running, false otherwise
			 */
			bool mainloop_running = false;
		public:
			/**
			 * @brief Construct a new Command Manager object
			 * 
			 * @param name: the name of the CommandManager
			 * @param in: the input stream of the CommandManager (can be a file (ifstream))
			 * @param out: the output stream of the CommandManager (can be a file (ofstream))
			 * @param err: the error stream of the CommandManager (can be a file (ofstream))
			 */
			CommandManager(std::string name = "main", std::istream& in = std::cin, std::ostream& out = std::cout, std::ostream& err = std::cerr);
			~CommandManager();

			inline void set_exit_code(int code) { return_value = code; }
			inline int get_exit_code() const { return return_value; }
			

			/**
			 * @brief Add a command to the CommandManager
			 * @param command: a pointer to the command to add
			 */
			void addCommand(Command* command);
			/**
			 * @brief remove the command with the given name from the CommandManager
			 * @param name: the name of the command to remove
			 */
			void removeCommand(const std::string& name);
			/**
			 * @brief remove the given command from the CommandManager
			 * @param command: a pointer to the command to remove
			 */
			void removeCommand(const char* name);
			/**
			 * @brief remove the given command from the CommandManager
			 * @param command: a pointer to the command to remove
			 */
			void removeCommand(Command* command);

			/**
			 * @brief get the command with the given name
			 * 
			 * @param name: the name of the command to get
			 * @return a pointer to the command with the given name
			 */
			inline Command* getCommand(const std::string& name) const { return commands.at(name); }

			/**
			 * @brief return the command with the given name
			 * 
			 * @param name: the name of the command to get
			 * @return a pointer to the command with the given name
			 */
			inline Command* operator[](const std::string& name) const { return getCommand(name); }

			/**
			 * @brief give all the commands having a name with 'max' or less differences with the given name
			 * 
			 * @param name: the name of the command to find
			 * @param max: the maximum number of differences between the given name and the name of the commands
			 * @return a vector of strings containing the names of the commands found
			 */
			std::vector<std::string> similar(const std::string& name, unsigned int max = 5) const;

			/**
			 * @brief execute one of the commands of the CommandManager where the name of the input is matching the name of the command
			 * @param input: the input to interpret and execute
			 */
			void execute(Input input);
			/**
			 * @brief execute one of the commands of the CommandManager where the name of the input is matching the name of the command
			 * @param s: the string to interpret and execute
			 * @note the string will be converted to an Input
			 */
			void execute(const std::string& s);
			/**
			 * @brief execute one of the commands of the CommandManager where the name of the input is matching the name of the command
			 * @param s: the string to interpret and execute
			 * @note the string will be converted to an Input
			 */
			void execute(const char* s);


			/**
			 * @brief execute the file with the given name
			 * @param filename: the name of the file to execute
			 * @param args: the arguments to give to the file
			 */
			void execute_file(const fs::path& filename, const std::vector<std::string>& args = {});

			/**
			 * @brief execute one of the commands of the CommandManager where the name of the input is matching the name of the command
			 * @param input: the input to interpret and execute
			 */
			void operator()(Input& input);
			/**
			 * @brief execute one of the commands of the CommandManager where the name of the input is matching the name of the command
			 * @param s: the string to interpret and execute
			 * @note the string will be converted to an Input
			 */
			void operator()(const std::string& s);
			/**
			 * @brief execute one of the commands of the CommandManager where the name of the input is matching the name of the command
			 * @param s: the string to interpret and execute
			 * @note the string will be converted to an Input
			 */
			void operator()(const char* s);

			/**
			 * @brief print the help of the CommandManager
			 * @note it will print the usage and the description of all the commands
			 */
			void printHelp() const;
			/**
			 * @brief print the help of the CommandManager
			 * @param name: the name of the command to print the help of
			 * @note it will print the usage and the long description it exists of the command with the given name
			 * @note if the command doesn't have a long description, it will print the short description
			 */
			void printHelp(const std::string& name) const;

			/**
			 * @brief set the question of the CommandManager
			 * @param question: the question of the CommandManager
			 * @note by default, it's "(%name)"
			 * @note you can use "%name" to print the name of the CommandManager
			*/
			inline void setQuestion(const std::string& question) { this->question = question; }

			/**
			 * @brief enable the help command
			 */
			inline void enableHelp() { addCommand(new PreDefinedCmd::HelpCommand(out)); }
			/**
			 * @brief disable the help command
			 */
			inline void disableHelp() { removeCommand("help"); }

			/**
			 * @brief enable the exit command
			 */
			inline void enableExit() { addCommand(new PreDefinedCmd::ExitCommand()); }
			/**
			 * @brief disable the exit command
			 */
			inline void disableExit() { removeCommand("exit"); }

			/**
			 * @brief enter in the mainloop of the CommandManager
			 * @note it will read the input and execute the command corresponding to the input while we call stopMainloop()
			 * @return int: the exit code of the mainloop
			 */
			int mainloop();
			/**
			 * @brief stop the mainloop of the CommandManager
			 */
			inline void stopMainloop() { mainloop_running = false; }


			inline void enable_executable(){ allow_execution = true; }
			inline void disable_executable(){ allow_execution = false; }

	}; // class CommandManager





	/**
	 * @brief the exception class throw when an error occured during the execution of a command
	 */
	class CommandException : public std::exception{
		protected:
			/**
			 * @brief the message of the exception
			 */
			std::string message;
		public:
			/**
			 * @brief construct a CommandException with the given message
			 * @param message: the message of the exception
			 */
			CommandException(const std::string& message);
			/**
			 * @brief construct a CommandException with the given message
			 * @param message: the message of the exception
			 */
			CommandException(const char* message);
			/**
			 * @brief copy constructor
			 * @param e: the CommandException to copy
			 */
			CommandException(const CommandException& e);
			/**
			 * @brief Construct a new Command Exception object without message
			 */
			CommandException();
			virtual ~CommandException() throw();

			/**
			 * @brief return the message of the exception
			 * @return const char*: the message of the exception
			 */
			virtual const char* what() const throw();
	}; // class CommandException

	using Kwargs = ::Command::Command::Kwargs;

} // namespace cmd


#endif