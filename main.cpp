#include <iostream>
#include <string>
#include <cstdlib>

#include "include/Token.hpp"
#include "include/Lexer.hpp"
#include "include/Parser.hpp"
#include "include/AST.hpp"
#include "include/CodeGenerator.hpp"
#include "include/FileUtils.hpp"
#include "include/GCCCompiler.hpp"
#include "include/CompilerEngine.hpp"

using namespace Paper5;

void printUsage() {
    std::cout << "Paper5 Compiler v1.1" << std::endl;
    std::cout << "Usage: paper5c <input.paper> [options]" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  -o <file>       Output executable file" << std::endl;
    std::cout << "  --dump-tokens   Dump tokens from source file" << std::endl;
    std::cout << "  --dump-ast      Dump AST after parsing" << std::endl;
    std::cout << "  --gen-only      Only generate C++ code" << std::endl;
    std::cout << "  --keep-temp     Keep temporary C++ file" << std::endl;
    std::cout << "  --run           Run after successful compilation" << std::endl;
    std::cout << "  --help          Show this help message" << std::endl;
    std::cout << std::endl;
    std::cout << "Interactive mode (no arguments):" << std::endl;
    std::cout << "  The compiler will prompt for input file and options." << std::endl;
}

void createTestFile() {
    std::string testDir = FileUtils::getDefaultOutputDir();
    FileUtils::createDirectory(testDir);
    
    std::string testFile = testDir + "test.paper";
    std::string content = 
        "<paper> mainWin {\n"
        "    width = 800;\n"
        "    height = 600;\n"
        "    title = \"Paper5 Window\";\n"
        "    background = \"#F0F0F0\";\n"
        "}\n"
        "\n"
        "mainWin.script {\n"
        "    <code> init {\n"
        "        // Application starts here\n"
        "        print(\"Hello from Paper5!\");\n"
        "    }\n"
        "    init();\n"
        "}\n";
    
    if (FileUtils::writeFile(testFile, content)) {
        std::cout << "Created test.paper file: " << testFile << std::endl;
    }
}

bool askYesNo(const std::string& prompt) {
    std::cout << prompt << " (y/N): ";
    std::string input;
    std::getline(std::cin, input);
    input = FileUtils::trim(input);
    return (input == "y" || input == "Y" || input == "yes" || input == "Yes");
}

int main(int argc, char* argv[]) {
    CompileOptions options;
    bool interactive = false;
    
    if (argc < 2) {
        interactive = true;
        std::cout << "Paper5 Compiler v1.1 - Interactive Mode\n" << std::endl;
        
        std::cout << "Enter path to .paper file (or press Enter for test file): ";
        std::string input;
        std::getline(std::cin, input);
        input = FileUtils::trim(input);
        
        if (input.empty()) {
            createTestFile();
            options.inputFile = FileUtils::getDefaultOutputDir() + "test.paper";
        } else {
            options.inputFile = input;
        }
        
        options.dumpTokens = askYesNo("Dump tokens?");
        options.dumpAST = askYesNo("Dump AST?");
        
        std::string defaultDir = FileUtils::getDefaultOutputDir();
        std::string defaultOut = defaultDir + "Untitled.exe";
        FileUtils::createDirectory(defaultDir);
        
        std::cout << "\nOutput path (press Enter for " << defaultOut << "): ";
                std::cout << "\nOutput path (press Enter for " << options.outputFile << "): ";
        std::string output;
        std::getline(std::cin, output);
        output = FileUtils::trim(output);
        
        if (!output.empty()) {
            options.outputFile = FileUtils::ensureExtension(output, ".exe");
        }
        
        options.runAfterCompile = askYesNo("Run after compilation?");
        
    } else {
        options.inputFile = argv[1];
		std::string baseName = FileUtils::getBaseName(options.inputFile);
		std::string outDir = FileUtils::getDirectoryFromPath(options.inputFile);
		if (outDir.empty()) outDir = FileUtils::getDefaultOutputDir();
		options.outputFile = outDir + "/" + baseName + ".exe";
        
        for (int i = 2; i < argc; i++) {
            std::string arg = argv[i];
            if (arg == "-o" && i + 1 < argc) {
                options.outputFile = argv[++i];
            } else if (arg == "--dump-tokens") {
                options.dumpTokens = true;
            } else if (arg == "--dump-ast") {
                options.dumpAST = true;
            } else if (arg == "--gen-only") {
                options.genOnly = true;
            } else if (arg == "--keep-temp") {
                options.keepTempFile = true;
            } else if (arg == "--run") {
                options.runAfterCompile = true;
            } else if (arg == "--help") {
                printUsage();
                return 0;
            }
        }
    }
    
    options.outputFile = FileUtils::ensureExtension(options.outputFile, ".exe");
    
    CompilerEngine engine;
    engine.setOptions(options);
    
    if (!options.genOnly) {
        engine.checkCompiler();
    }
    
    CompileResult result = engine.compile();
    
    if (result.success && !options.genOnly && options.runAfterCompile) {
        std::string runCmd = "start \"\" \"" + result.outputFile + "\"";
        system(runCmd.c_str());
    }
    
    system("pause");
    
    return result.success ? 0 : 1;
}