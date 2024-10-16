#pragma once

static const char* types_old[] = {
    "char", "double", "float",
    "int", "long", "short",
    "void", "size_t", "auto",
    "break", "case", "const",
    "continue", "default", "do",
    "else", "enum", "extern",
    "for", "goto", "if", "register",
    "return", "signed", "sizeof",
    "static","struct", "switch", 
    "typedef", "union", "unsigned", 
    "volatile", "while", "namespace",
    "inline", "bool",
    nullptr  
};

static const char*  brackets[] = {
    "(", ")", "{", "}", "[", "]", nullptr
};

static const char* preprocessor_directives[] = {
    "#include",    
    "#define",     
    "#undef",     
    "#ifdef",      
    "#ifndef",     
    "#if",        
    "#else",       
    "#elif",     
    "#endif",      
    "#error",     
    "#pragma",    
    "#line",      
    "#warning",    
    nullptr
};