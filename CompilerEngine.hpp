#ifndef COMPILERENGINE_HPP
#define COMPILERENGINE_HPP

#include "Lexer.hpp"
#include "Parser.hpp"
#include "AST.hpp"
#include "CodeGenerator.hpp"
#include "FileUtils.hpp"
#include "GCCCompiler.hpp"

#include <string>
#include <vector>
#include <memory>
#include <iostream>

namespace Paper5 {

struct CompileOptions {
    std::string inputFile;
    std::string outputFile;
    std::string tempDir;
    bool dumpTokens = false;
    bool dumpAST = false;
    bool genOnly = false;
    bool keepTempFile = false;
    bool runAfterCompile = false;
    
    CompileOptions() : tempDir("") {}
};

struct CompileResult {
    bool success = false;
    std::string outputFile;
    std::string cppTempFile;
    size_t tokenCount = 0;
    std::string errorMessage;
    std::string paperName;
};

class CompilerEngine {
private:
    CompileOptions options;
    GCCCompiler gccCompiler;
    
    void printSeparator(const std::string& title = "") {
        std::cout << "\n========================================" << std::endl;
        if (!title.empty()) {
            std::cout << title << std::endl;
            std::cout << "========================================" << std::endl;
        }
    }
    
public:
    CompilerEngine() : gccCompiler("g++") {}
    
    void setOptions(const CompileOptions& opts) {
        options = opts;
    }
    
    CompileOptions& getOptions() {
        return options;
    }
    
    GCCCompiler& getGCCCompiler() {
        return gccCompiler;
    }
    
    CompileResult compile() {
        CompileResult result;
        
        printSeparator("Paper5 Compiler v1.1 - Compiling");
        std::cout << "  Input:  " << options.inputFile << std::endl;
        std::cout << "  Output: " << options.outputFile << std::endl;
        std::cout << "========================================" << std::endl;
        
        // Step 1: Read source file
        std::string source = FileUtils::readFile(options.inputFile);
        if (source.empty()) {
            result.errorMessage = "Failed to read source file: " + options.inputFile;
            return result;
        }
        
        // Step 2: Lexical analysis
        std::cout << "\n[1/4] Lexical analysis..." << std::endl;
        Lexer lexer(source);
        std::vector<Token> tokens = lexer.tokenize();
        result.tokenCount = tokens.size();
        std::cout << "  Generated " << tokens.size() << " tokens." << std::endl;
        
        if (options.dumpTokens) {
            std::cout << "\n=== Tokens ===" << std::endl;
            for (const auto& token : tokens) {
                std::cout << "  Line " << token.line << ", Col " << token.column
                          << ": " << tokenTypeToString(token.type);
                if (!token.lexeme.empty()) {
                    std::cout << " (" << token.lexeme << ")";
                }
                std::cout << std::endl;
            }
        }
        
        // Step 3: Syntax analysis
        try {
            std::cout << "\n[2/4] Syntax analysis..." << std::endl;
            Parser parser(tokens);
            std::unique_ptr<AST> ast = parser.parse();
            std::cout << "  AST generated successfully." << std::endl;
            
            if (ast->paper) {
                result.paperName = ast->paper->name;
                std::cout << "  Paper name: " << ast->paper->name << std::endl;
                std::cout << "  Properties: " << ast->paper->properties.size() << std::endl;
                std::cout << "  Children:   " << ast->paper->children.size() << std::endl;
            }
            std::cout << "  Global vars: " << ast->globalVars.size() << std::endl;
            std::cout << "  Scripts:     " << ast->scripts.size() << std::endl;
            
            if (options.dumpAST) {
                printAST(*ast);
            }
            
            // Step 4: Code generation
            std::cout << "\n[3/4] Code generation..." << std::endl;
            CodeGenerator generator;
            
            std::string tempDir = options.tempDir;
            if (tempDir.empty()) {
                tempDir = FileUtils::getDirectoryFromPath(options.outputFile);
            }
            if (tempDir.empty()) {
                tempDir = ".";
            }
            FileUtils::createDirectory(tempDir);
            
            result.cppTempFile = tempDir + "/temp_" + 
                                 FileUtils::getBaseName(options.outputFile) + ".cpp";
            
            generator.generate(*ast, result.cppTempFile);
            std::cout << "  Generated: " << result.cppTempFile << std::endl;
            
            // Step 5: Compile with GCC
            if (!options.genOnly) {
                std::cout << "\n[4/4] Compiling to executable..." << std::endl;
                
                std::string outDir = FileUtils::getDirectoryFromPath(options.outputFile);
                if (!outDir.empty()) {
                    FileUtils::createDirectory(outDir);
                }
                
                if (gccCompiler.compile(result.cppTempFile, options.outputFile)) {
                    if (!options.keepTempFile) {
                        FileUtils::deleteFile(result.cppTempFile);
                        result.cppTempFile.clear();
                    }
                    
                    result.success = true;
                    result.outputFile = options.outputFile;
                    
                    printSeparator("SUCCESS! Executable created!");
                    std::cout << "  Location: " << options.outputFile << std::endl;
                    std::cout << "========================================" << std::endl;
                } else {
                    result.errorMessage = "GCC compilation failed.";
                    std::cout << "\nC++ code saved at: " << result.cppTempFile << std::endl;
                    std::cout << "\nManual compile command:" << std::endl;
                    std::cout << "  g++ \"" << result.cppTempFile << "\" -o \"" 
                              << options.outputFile << "\" -lgdi32 -lcomctl32 -static -mwindows" 
                              << std::endl;
                }
            } else {
                result.success = true;
                std::cout << "\nC++ code generation complete (--gen-only)" << std::endl;
                std::cout << "  File: " << result.cppTempFile << std::endl;
            }
            
        } catch (const std::exception& e) {
            result.errorMessage = std::string("Syntax error: ") + e.what();
            std::cerr << "\n" << result.errorMessage << std::endl;
        }
        
        return result;
    }
    
    void printAST(const AST& ast) {
        std::cout << "\n=== AST Structure ===" << std::endl;
        
        if (ast.paper) {
            std::cout << "Paper: " << ast.paper->name << std::endl;
            std::cout << "  Window Properties:" << std::endl;
            for (const auto& prop : ast.paper->properties) {
                std::cout << "    " << prop.name << " = ..." << std::endl;
            }
            std::cout << "  Child Components: " << ast.paper->children.size() << std::endl;
            for (const auto& child : ast.paper->children) {
                printComponent(*child, 2);
            }
        }
        
        std::cout << "\nGlobal Variables: " << ast.globalVars.size() << std::endl;
        for (const auto& var : ast.globalVars) {
            std::cout << "  " << var->name << std::endl;
        }
        
        std::cout << "\nScripts: " << ast.scripts.size() << std::endl;
        for (const auto& script : ast.scripts) {
            std::cout << "  Owner: " << script->ownerName << std::endl;
            std::cout << "    Functions: " << script->functions.size() << std::endl;
            std::cout << "    Event Handlers: " << script->eventHandlers.size() << std::endl;
            for (const auto& handler : script->eventHandlers) {
                std::cout << "      " << handler->componentName 
                          << "." << handler->eventName << "()" << std::endl;
            }
        }
    }
    
    void printComponent(const Component& comp, int depth) {
        std::string indent(depth * 2, ' ');
        std::cout << indent << "Component: " << comp.name 
                  << " (type: " << static_cast<int>(comp.type) << ")" << std::endl;
        for (const auto& prop : comp.properties) {
            std::cout << indent << "  " << prop.name << " = ..." << std::endl;
        }
        for (const auto& child : comp.children) {
            printComponent(*child, depth + 1);
        }
    }
    
    bool checkCompiler() {
        if (!gccCompiler.isAvailable()) {
            std::cerr << "Warning: GCC (g++) not found in PATH." << std::endl;
            std::cerr << "Please install MinGW or add GCC to your PATH." << std::endl;
            return false;
        }
        return true;
    }
};

} // namespace Paper5

#endif // COMPILERENGINE_HPP