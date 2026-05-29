#ifndef GCCCOMPILER_HPP
#define GCCCOMPILER_HPP

#include <string>
#include <iostream>
#include <cstdlib>

namespace Paper5 {

class GCCCompiler {
private:
    std::string compilerPath;
    std::string extraFlags;
    
public:
    GCCCompiler(const std::string& compiler = "g++", const std::string& flags = "")
        : compilerPath(compiler), extraFlags(flags) {}
    
    void setCompilerPath(const std::string& path) {
        compilerPath = path;
    }
    
    void addFlags(const std::string& flags) {
        if (!extraFlags.empty()) extraFlags += " ";
        extraFlags += flags;
    }
    
    bool compile(const std::string& cppFile, const std::string& exeFile) {
        std::string command = compilerPath + " \"" + cppFile + "\" -o \"" + exeFile + "\"";
        command += " -lgdi32 -lcomctl32 -static";
        
#ifdef _WIN32
        command += " -mwindows";
#endif
        
        if (!extraFlags.empty()) {
            command += " " + extraFlags;
        }
        
        std::cout << "\nRunning: " << command << std::endl;
        
        int result = system(command.c_str());
        
        if (result == 0) {
            std::cout << "Successfully compiled to: " << exeFile << std::endl;
            return true;
        } else {
            std::cerr << "Compilation failed (error code: " << result << ")" << std::endl;
            return false;
        }
    }
    
    bool compileOnly(const std::string& cppFile, const std::string& objFile) {
        std::string command = compilerPath + " -c \"" + cppFile + "\" -o \"" + objFile + "\"";
        
        if (!extraFlags.empty()) {
            command += " " + extraFlags;
        }
        
        std::cout << "Running: " << command << std::endl;
        
        int result = system(command.c_str());
        return result == 0;
    }
    
    bool isAvailable() const {
#ifdef _WIN32
        std::string command = compilerPath + " --version > NUL 2>&1";
#else
        std::string command = compilerPath + " --version > /dev/null 2>&1";
#endif
        int result = system(command.c_str());
        return result == 0;
    }
    
    std::string getCompilerPath() const {
        return compilerPath;
    }
};

} // namespace Paper5

#endif // GCCCOMPILER_HPP