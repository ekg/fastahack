#include "split.h"

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while(std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    return split(s, delim, elems);
}

std::vector<std::string> &split(const std::string &s, const std::string& delims, std::vector<std::string> &elems) {
    char* tok;
    char* save = NULL;  // strtok_r rather than strtok, which is not thread-safe
    char cchars [s.size()+1];
    char* cstr = &cchars[0];
    strcpy(cstr, s.c_str());
    tok = strtok_r(cstr, delims.c_str(), &save);
    while (tok != NULL) {
        elems.push_back(tok);
        tok = strtok_r(NULL, delims.c_str(), &save);
    }
    return elems;
}

std::vector<std::string> split(const std::string &s, const std::string& delims) {
    std::vector<std::string> elems;
    return split(s, delims, elems);
}
