//   █████╗ ██████╗  ██████╗    ██╗  ██╗██████╗ ██████╗ 
//  ██╔══██╗██╔══██╗██╔════╝    ██║  ██║██╔══██╗██╔══██╗
//  ███████║██████╔╝██║  ███╗   ███████║██████╔╝██████╔╝
//  ██╔══██║██╔══██╗██║   ██║   ██╔══██║██╔═══╝ ██╔═══╝ 
//  ██║  ██║██║  ██║╚██████╔╝██╗██║  ██║██║     ██║     
//  ╚═╝  ╚═╝╚═╝  ╚═╝ ╚═════╝ ╚═╝╚═╝  ╚═╝╚═╝     ╚═╝     
//                                                      
// small header-only class for options 

#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <sstream>
#include <stdexcept>
#include <iostream>

class arg_parser
{
private:
    struct option
    {
        std::string description;
        std::string value;
        bool has_value = false;
        bool required = false;
    };

    std::unordered_map<std::string, option> options_;
    std::vector<std::string> positional_args_;

public:
    void add_option(const std::string &name, const std::string &description, bool required = false)
    {
        options_[name] = {description, "", false, required};
    }

    template <typename T>
    void add_option(const std::string &name, const std::string &description, const T &default_value, bool required = false)
    {
        std::ostringstream oss;
        oss << default_value;
        options_[name] = {description, oss.str(), true, required};
    }

    void parse(int argc, char *argv[])
    {
        for (int i = 1; i < argc; ++i)
        {
            std::string arg = argv[i];

            if (arg.substr(0, 2) == "--")
            {

                auto it = options_.find(arg);
                if (it != options_.end())
                {
                    if (i + 1 < argc && argv[i + 1][0] != '-')
                    {

                        it->second.value = argv[++i];
                        it->second.has_value = true;
                    }
                    else
                    {
                        throw std::runtime_error("Option " + arg + " expects a value");
                    }
                }
                else
                {
                    throw std::runtime_error("Unknown option: " + arg);
                }
            }
            else if (arg[0] == '-' && arg.length() > 1)
            {
                throw std::runtime_error("Short options not supported: " + arg);
            }
            else
            {

                positional_args_.push_back(arg);
            }
        }

        for (const auto &[name, option] : options_)
        {
            if (option.required && !option.has_value)
            {
                throw std::runtime_error("Required option missing: " + name);
            }
        }
    }

    bool has(const std::string &name) const
    {
        auto it = options_.find(name);
        return it != options_.end() && it->second.has_value;
    }

    template <typename T>
    T get(const std::string &name) const
    {
        auto it = options_.find(name);
        if (it == options_.end())
        {
            throw std::runtime_error("Unknown option: " + name);
        }

        if (!it->second.has_value)
        {
            throw std::runtime_error("Option " + name + " has no value");
        }

        return convert_value<T>(it->second.value);
    }

    template <typename T>
    T get_or(const std::string &name, const T &default_value) const
    {
        if (has(name))
        {
            return get<T>(name);
        }
        return default_value;
    }

    const std::vector<std::string> &positional() const
    {
        return positional_args_;
    }

    void print_help(const std::string &program_name = "") const
    {
        if (!program_name.empty())
        {
            std::cout << "Usage: " << program_name << " [options]\n\n";
        }

        std::cout << "Options:\n";
        for (const auto &[name, option] : options_)
        {
            std::cout << "  " << name << "\t" << option.description;
            if (option.required)
            {
                std::cout << " (required)";
            }
            else if (option.has_value)
            {
                std::cout << " (default: " << option.value << ")";
            }
            std::cout << "\n";
        }
    }

private:
    template <typename T>
    T convert_value(const std::string &value) const
    {
        std::istringstream iss(value);
        T result;
        if (!(iss >> result) || !iss.eof())
        {
            throw std::runtime_error("Cannot convert '" + value + "' to requested type");
        }
        return result;
    }
};

template <>
inline std::string arg_parser::convert_value<std::string>(const std::string &value) const
{
    return value;
}

template <>
inline bool arg_parser::convert_value<bool>(const std::string &value) const
{
    std::string lower_value = value;
    std::transform(lower_value.begin(), lower_value.end(), lower_value.begin(), ::tolower);

    if (lower_value == "true" || lower_value == "1" || lower_value == "yes")
    {
        return true;
    }
    else if (lower_value == "false" || lower_value == "0" || lower_value == "no")
    {
        return false;
    }
    else
    {
        throw std::runtime_error("Cannot convert '" + value + "' to bool");
    }
}
