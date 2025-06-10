#include <iostream>
#include <cassert>
#include <vector>
#include <cstring>
#include <sstream>
#include <functional>
#include <algorithm>

#include "../arg.hpp"

// Simple test framework
class TestRunner {
private:


    std::string current_test;

public: // variables
    int tests_run = 0;    
    int tests_passed = 0; 
public: // functions

    void run_test(const std::string& name, std::function<void()> test_func) {
        current_test = name;
        tests_run++;
        try {
            test_func();
            tests_passed++;
            std::cout << "✓ " << name << std::endl;
        } catch (const std::exception& e) {
            std::cout << "✗ " << name << " - " << e.what() << std::endl;
        } catch (...) {
            std::cout << "✗ " << name << " - Unknown exception" << std::endl;
        }
    }

    void assert_true(bool condition, const std::string& message = "") {
        if (!condition) {
            throw std::runtime_error("Assertion failed in " + current_test + ": " + message);
        }
    }

    void assert_equals(const std::string& expected, const std::string& actual) {
        if (expected != actual) {
            throw std::runtime_error("Expected '" + expected + "', got '" + actual + "'");
        }
    }

    template<typename T>
    void assert_equals(T expected, T actual) {
        if (expected != actual) {
            std::ostringstream oss;
            oss << "Expected " << expected << ", got " << actual;
            throw std::runtime_error(oss.str());
        }
    }

    void assert_throws(std::function<void()> func, const std::string& expected_message = "") {
        bool thrown = false;
        try {
            func();
        } catch (const std::exception& e) {
            thrown = true;
            if (!expected_message.empty() && std::string(e.what()).find(expected_message) == std::string::npos) {
                throw std::runtime_error("Expected exception with message containing '" + expected_message + 
                                       "', got '" + e.what() + "'");
            }
        }
        if (!thrown) {
            throw std::runtime_error("Expected exception to be thrown");
        }
    }

    void print_summary() {
        std::cout << "\n" << tests_passed << "/" << tests_run << " tests passed";
        if (tests_passed == tests_run) {
            std::cout << " ✓" << std::endl;
        } else {
            std::cout << " ✗" << std::endl;
        }
    }
};

// Helper function to create argv from vector of strings
char** create_argv(const std::vector<std::string>& args) {
    char** argv = new char*[args.size()];
    for (size_t i = 0; i < args.size(); ++i) {
        argv[i] = new char[args[i].length() + 1];
        strcpy(argv[i], args[i].c_str());
    }
    return argv;
}

void cleanup_argv(char** argv, int argc) {
    for (int i = 0; i < argc; ++i) {
        delete[] argv[i];
    }
    delete[] argv;
}

int main() {
    TestRunner runner;

    // Test basic string parsing
    runner.run_test("Basic string option parsing", [&]() {
        arg_parser parser;
        parser.add_option("--file", "Input file");
        
        std::vector<std::string> args = {"program", "--file", "test.txt"};
        char** argv = create_argv(args);
        
        parser.parse(args.size(), argv);
        
        runner.assert_true(parser.has("--file"));
        runner.assert_equals(std::string("test.txt"), parser.get<std::string>("--file"));
        
        cleanup_argv(argv, args.size());
    });

    // Test integer parsing
    runner.run_test("Integer option parsing", [&]() {
        arg_parser parser;
        parser.add_option("--count", "Number of items");
        
        std::vector<std::string> args = {"program", "--count", "42"};
        char** argv = create_argv(args);
        
        parser.parse(args.size(), argv);
        
        runner.assert_true(parser.has("--count"));
        runner.assert_equals(42, parser.get<int>("--count"));
        
        cleanup_argv(argv, args.size());
    });

    // Test double parsing
    runner.run_test("Double option parsing", [&]() {
        arg_parser parser;
        parser.add_option("--value", "Floating point value");
        
        std::vector<std::string> args = {"program", "--value", "123.45"};
        char** argv = create_argv(args);
        
        parser.parse(args.size(), argv);
        
        runner.assert_true(parser.has("--value"));
        runner.assert_equals(123.45, parser.get<double>("--value"));
        
        cleanup_argv(argv, args.size());
    });

    // Test boolean parsing
    runner.run_test("Boolean option parsing", [&]() {
        arg_parser parser;
        parser.add_option("--verbose", "Enable verbose output");
        
        std::vector<std::string> args = {"program", "--verbose", "true"};
        char** argv = create_argv(args);
        
        parser.parse(args.size(), argv);
        
        runner.assert_true(parser.has("--verbose"));
        runner.assert_equals(true, parser.get<bool>("--verbose"));
        
        cleanup_argv(argv, args.size());
    });

    // Test boolean variants
    runner.run_test("Boolean variants parsing", [&]() {
        std::vector<std::string> true_values = {"true", "1", "yes", "TRUE", "YES"};
        std::vector<std::string> false_values = {"false", "0", "no", "FALSE", "NO"};
        
        for (const auto& val : true_values) {
            arg_parser parser;
            parser.add_option("--flag", "Test flag");
            
            std::vector<std::string> args = {"program", "--flag", val};
            char** argv = create_argv(args);
            
            parser.parse(args.size(), argv);
            runner.assert_equals(true, parser.get<bool>("--flag"));
            
            cleanup_argv(argv, args.size());
        }
        
        for (const auto& val : false_values) {
            arg_parser parser;
            parser.add_option("--flag", "Test flag");
            
            std::vector<std::string> args = {"program", "--flag", val};
            char** argv = create_argv(args);
            
            parser.parse(args.size(), argv);
            runner.assert_equals(false, parser.get<bool>("--flag"));
            
            cleanup_argv(argv, args.size());
        }
    });

    // Test default values
    runner.run_test("Default values", [&]() {
        arg_parser parser;
        parser.add_option("--count", "Number of items", 10);
        parser.add_option("--name", "Name", std::string("default"));
        
        std::vector<std::string> args = {"program"};
        char** argv = create_argv(args);
        
        parser.parse(args.size(), argv);
        
        runner.assert_equals(10, parser.get<int>("--count"));
        runner.assert_equals(std::string("default"), parser.get<std::string>("--name"));
        
        cleanup_argv(argv, args.size());
    });

    // Test overriding default values
    runner.run_test("Override default values", [&]() {
        arg_parser parser;
        parser.add_option("--count", "Number of items", 10);
        
        std::vector<std::string> args = {"program", "--count", "20"};
        char** argv = create_argv(args);
        
        parser.parse(args.size(), argv);
        
        runner.assert_equals(20, parser.get<int>("--count"));
        
        cleanup_argv(argv, args.size());
    });

    // Test get_or method
    runner.run_test("get_or method", [&]() {
        arg_parser parser;
        parser.add_option("--optional", "Optional value");
        
        std::vector<std::string> args = {"program"};
        char** argv = create_argv(args);
        
        parser.parse(args.size(), argv);
        
        runner.assert_equals(99, parser.get_or<int>("--optional", 99));
        runner.assert_equals(std::string("fallback"), parser.get_or<std::string>("--optional", std::string("fallback")));
        
        cleanup_argv(argv, args.size());
    });

    // Test required options
    runner.run_test("Required option provided", [&]() {
        arg_parser parser;
        parser.add_option("--required", "Required option", true);
        
        std::vector<std::string> args = {"program", "--required", "value"};
        char** argv = create_argv(args);
        
        parser.parse(args.size(), argv);
        runner.assert_equals(std::string("value"), parser.get<std::string>("--required"));
        
        cleanup_argv(argv, args.size());
    });

    // Test missing required option
    runner.run_test("Missing required option throws", [&]() {
        arg_parser parser;
        parser.add_option("--required", "Required option", true);
        
        std::vector<std::string> args = {"program"};
        char** argv = create_argv(args);
        
        runner.assert_throws([&]() {
            parser.parse(args.size(), argv);
        }, "Required option missing");
        
        cleanup_argv(argv, args.size());
    });

    // Test positional arguments
    runner.run_test("Positional arguments", [&]() {
        arg_parser parser;
        parser.add_option("--flag", "Test flag");
        
        std::vector<std::string> args = {"program", "--flag", "value", "pos1", "pos2"};
        char** argv = create_argv(args);
        
        parser.parse(args.size(), argv);
        
        const auto& positional = parser.positional();
        runner.assert_equals(size_t(2), positional.size());
        runner.assert_equals(std::string("pos1"), positional[0]);
        runner.assert_equals(std::string("pos2"), positional[1]);
        
        cleanup_argv(argv, args.size());
    });

    // Test unknown option
    runner.run_test("Unknown option throws", [&]() {
        arg_parser parser;
        
        std::vector<std::string> args = {"program", "--unknown", "value"};
        char** argv = create_argv(args);
        
        runner.assert_throws([&]() {
            parser.parse(args.size(), argv);
        }, "Unknown option");
        
        cleanup_argv(argv, args.size());
    });

    // Test option without value
    runner.run_test("Option without value throws", [&]() {
        arg_parser parser;
        parser.add_option("--file", "Input file");
        
        std::vector<std::string> args = {"program", "--file"};
        char** argv = create_argv(args);
        
        runner.assert_throws([&]() {
            parser.parse(args.size(), argv);
        }, "expects a value");
        
        cleanup_argv(argv, args.size());
    });

    // Test getting non-existent option
    runner.run_test("Get non-existent option throws", [&]() {
        arg_parser parser;
        
        std::vector<std::string> args = {"program"};
        char** argv = create_argv(args);
        
        parser.parse(args.size(), argv);
        
        runner.assert_throws([&]() {
            parser.get<std::string>("--nonexistent");
        }, "Unknown option");
        
        cleanup_argv(argv, args.size());
    });

    // Test getting option with no value
    runner.run_test("Get option with no value throws", [&]() {
        arg_parser parser;
        parser.add_option("--optional", "Optional value");
        
        std::vector<std::string> args = {"program"};
        char** argv = create_argv(args);
        
        parser.parse(args.size(), argv);
        
        runner.assert_throws([&]() {
            parser.get<std::string>("--optional");
        }, "has no value");
        
        cleanup_argv(argv, args.size());
    });

    // Test invalid type conversion
    runner.run_test("Invalid type conversion throws", [&]() {
        arg_parser parser;
        parser.add_option("--number", "A number");
        
        std::vector<std::string> args = {"program", "--number", "not_a_number"};
        char** argv = create_argv(args);
        
        parser.parse(args.size(), argv);
        
        runner.assert_throws([&]() {
            parser.get<int>("--number");
        }, "Cannot convert");
        
        cleanup_argv(argv, args.size());
    });

    // Test invalid boolean conversion
    runner.run_test("Invalid boolean conversion throws", [&]() {
        arg_parser parser;
        parser.add_option("--flag", "A boolean flag");
        
        std::vector<std::string> args = {"program", "--flag", "maybe"};
        char** argv = create_argv(args);
        
        parser.parse(args.size(), argv);
        
        runner.assert_throws([&]() {
            parser.get<bool>("--flag");
        }, "Cannot convert");
        
        cleanup_argv(argv, args.size());
    });

    // Test complex scenario
    runner.run_test("Complex parsing scenario", [&]() {
        arg_parser parser;
        parser.add_option("--input", "Input file", true);
        parser.add_option("--output", "Output file", std::string("output.txt"));
        parser.add_option("--count", "Number of iterations", 100);
        parser.add_option("--verbose", "Enable verbose output", false);
        
        std::vector<std::string> args = {
            "program", 
            "--input", "data.txt",
            "--count", "50",
            "--verbose", "true",
            "extra1", "extra2"
        };
        char** argv = create_argv(args);
        
        parser.parse(args.size(), argv);
        
        runner.assert_equals(std::string("data.txt"), parser.get<std::string>("--input"));
        runner.assert_equals(std::string("output.txt"), parser.get<std::string>("--output")); // default
        runner.assert_equals(50, parser.get<int>("--count"));
        runner.assert_equals(true, parser.get<bool>("--verbose"));
        
        const auto& positional = parser.positional();
        runner.assert_equals(size_t(2), positional.size());
        runner.assert_equals(std::string("extra1"), positional[0]);
        runner.assert_equals(std::string("extra2"), positional[1]);
        
        cleanup_argv(argv, args.size());
    });

    runner.print_summary();
    return runner.tests_passed == runner.tests_run ? 0 : 1;
}
