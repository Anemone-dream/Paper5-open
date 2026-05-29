#ifndef FILEUTILS_HPP
#define FILEUTILS_HPP

#include <string>
#include <fstream>
#include <iostream>
#include <sys/stat.h>

#ifdef _WIN32
#include <direct.h>
#define MKDIR(path) _mkdir(path)
#else
#define MKDIR(path) mkdir(path, 0755)
#endif

namespace Paper5 {

class FileUtils {
public:
    static std::string readFile(const std::string& filename) {
        std::ifstream file(filename, std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "Error: Cannot open file " << filename << std::endl;
            return "";
        }
        std::string content;
        std::string line;
        while (std::getline(file, line)) {
            content += line + "\n";
        }
        file.close();
        return content;
    }
    
    static bool writeFile(const std::string& filename, const std::string& content) {
        std::ofstream file(filename, std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "Error: Cannot write to " << filename << std::endl;
            return false;
        }
        file << content;
        file.close();
        return true;
    }
    
    static bool createDirectory(const std::string& path) {
        if (path.empty()) return true;
        
        struct stat info;
        if (stat(path.c_str(), &info) == 0) {
            return true;
        }
        
        std::string currentPath;
        for (char c : path) {
            currentPath += c;
            if (c == '\\' || c == '/') {
                if (currentPath.length() > 1) {
                    std::string dirPath = currentPath;
                    dirPath.pop_back();
                    MKDIR(dirPath.c_str());
                    currentPath += c;
                }
            }
        }
        MKDIR(path.c_str());
        
        return stat(path.c_str(), &info) == 0;
    }
    
    static bool deleteFile(const std::string& filename) {
        return std::remove(filename.c_str()) == 0;
    }
    
    static std::string getDirectoryFromPath(const std::string& path) {
        size_t lastSlash = path.find_last_of("\\/");
        if (lastSlash != std::string::npos) {
            return path.substr(0, lastSlash);
        }
        return "";
    }
    
    static std::string getBaseName(const std::string& path) {
        size_t lastSlash = path.find_last_of("\\/");
        size_t lastDot = path.find_last_of(".");
        
        std::string filename;
        if (lastSlash != std::string::npos) {
            filename = path.substr(lastSlash + 1);
        } else {
            filename = path;
        }
        
        if (lastDot != std::string::npos && lastDot > lastSlash) {
            filename = filename.substr(0, lastDot - 
                (lastSlash != std::string::npos ? lastSlash + 1 : 0));
        }
        
        return filename;
    }
    
    static std::string getExtension(const std::string& path) {
        size_t lastDot = path.find_last_of(".");
        if (lastDot != std::string::npos) {
            return path.substr(lastDot);
        }
        return "";
    }
    
    static std::string ensureExtension(const std::string& path, const std::string& ext) {
        std::string dotExt = ext;
        if (!dotExt.empty() && dotExt[0] != '.') {
            dotExt = "." + dotExt;
        }
        
        if (path.length() >= dotExt.length() && 
            path.substr(path.length() - dotExt.length()) == dotExt) {
            return path;
        }
        return path + dotExt;
    }
    
    static std::string getDefaultOutputDir() {
#ifdef _WIN32
        struct stat info;
        std::string dDrive = "D:\\Paper5file\\papers\\";
        if (stat("D:\\", &info) == 0 && createDirectory(dDrive)) {
            return dDrive;
        }
#endif
        return "papers/";
    }
    
    static std::string trim(const std::string& str) {
        size_t first = str.find_first_not_of(" \t\r\n");
        if (first == std::string::npos) return "";
        size_t last = str.find_last_not_of(" \t\r\n");
        return str.substr(first, last - first + 1);
    }
};

} // namespace Paper5

#endif // FILEUTILS_HPP