#pragma once
#include <string>
#include <vector>
#include <map>
#include <ncurses.h>
#include <filesystem>
#include "globals/mvimResources.h"

struct SyntaxGroup {
    std::vector<std::string> keywords;
    short color;
};

struct Language {
    std::string name;
    std::vector<std::string> extensions;
    std::vector<SyntaxGroup> syntaxGroups;
    
    std::string singleLineComment; 
    std::string multiLineCommentStart; 
    std::string multiLineCommentEnd;   
    std::string brackets; 
};

class SyntaxHighlighter {
public:
    static SyntaxHighlighter& instance() {
        static SyntaxHighlighter instance;
        return instance;
    }

    // Load all .mvimlang files from the 'languages' directory
    void loadLanguages();

    void setLanguageFromFile(const std::string& filename);
    const Language* getCurrentLanguage() const;

private:
    SyntaxHighlighter() { 
        // Try to load immediately on startup
        loadLanguages(); 
    }
    
    std::vector<Language> languages;
    const Language* currentLanguage = nullptr;
    
    std::string getExtension(const std::string& filename);
    
    // Helper to parse a single file
    void parseLanguageFile(const std::filesystem::path& path);
};