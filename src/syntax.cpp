#include "../include/syntax.hpp"
#include <fstream>
#include <sstream>

namespace fs = std::filesystem;

// Helper to split strings by spaces
static std::vector<std::string> split(const std::string& str) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(str);
    while (tokenStream >> token) {
        tokens.push_back(token);
    }
    return tokens;
}

// Helper to trim spaces
static std::string trim_val(const std::string& str) {
    size_t first = str.find_first_not_of(" \t");
    if (std::string::npos == first) return "";
    size_t last = str.find_last_not_of(" \t");
    return str.substr(first, (last - first + 1));
}

void SyntaxHighlighter::loadLanguages() {
    languages.clear();
    
    // Path to the languages directory (relative to executable)
    // You might want to make this an absolute path or configurable in the future
    std::string langPath = "languages"; 

    if (!fs::exists(langPath) || !fs::is_directory(langPath)) {
        ErrorHandler::instance().report(ErrorLevel::WARNING, "Languages directory not found: " + langPath);
        return;
    }

    for (const auto& entry : fs::directory_iterator(langPath)) {
        if (entry.path().extension() == ".mvimlang") {
            parseLanguageFile(entry.path());
        }
    }
}

void SyntaxHighlighter::parseLanguageFile(const fs::path& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        ErrorHandler::instance().report(ErrorLevel::ERROR, "Failed to load language file: " + path.string());
        return;
    }

    Language lang;
    std::string line;
    
    // Prepare groups (0 = keywords, 1 = preprocessor)
    // You can extend this logic to be more dynamic if needed
    lang.syntaxGroups.push_back({{}, keyWordColor});
    lang.syntaxGroups.push_back({{}, preprocessorColor});

    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;

        size_t delimiterPos = line.find('=');
        if (delimiterPos == std::string::npos) continue;

        std::string key = trim_val(line.substr(0, delimiterPos));
        std::string value = trim_val(line.substr(delimiterPos + 1));

        if (key == "name") {
            lang.name = value;
        } else if (key == "extensions") {
            lang.extensions = split(value);
        } else if (key == "single_line_comment") {
            lang.singleLineComment = value;
        } else if (key == "multi_line_comment_start") {
            lang.multiLineCommentStart = value;
        } else if (key == "multi_line_comment_end") {
            lang.multiLineCommentEnd = value;
        } else if (key == "brackets") {
            lang.brackets = value;
        } else if (key == "keywords") {
            std::vector<std::string> words = split(value);
            lang.syntaxGroups[0].keywords.insert(lang.syntaxGroups[0].keywords.end(), words.begin(), words.end());
        } else if (key == "preprocessor") {
            std::vector<std::string> words = split(value);
            lang.syntaxGroups[1].keywords.insert(lang.syntaxGroups[1].keywords.end(), words.begin(), words.end());
        }
    }

    if (!lang.name.empty() && !lang.extensions.empty()) {
        languages.push_back(lang);
        ErrorHandler::instance().report(ErrorLevel::INFO, "Loaded language module: " + lang.name);
    }
}

std::string SyntaxHighlighter::getExtension(const std::string& filename) {
    size_t pos = filename.rfind('.');
    if (pos == std::string::npos) return "";
    return filename.substr(pos);
}

void SyntaxHighlighter::setLanguageFromFile(const std::string& filename) {
    std::string ext = getExtension(filename);
    currentLanguage = nullptr;

    for (const auto& lang : languages) {
        for (const auto& supportedExt : lang.extensions) {
            if (supportedExt == ext) {
                currentLanguage = &lang;
                ErrorHandler::instance().report(ErrorLevel::INFO, "Active Language: " + lang.name);
                return;
            }
        }
    }
}

const Language* SyntaxHighlighter::getCurrentLanguage() const {
    return currentLanguage;
}