#include "StringTokenizer.hpp"

StringTokenizer::StringTokenizer(string strIn) {
    this->strIn = strIn;
    this->tokenIndex = 0;

    string str(strIn);
    string tmpstr;
    stringstream ss(str);

    while(ss >> tmpstr)
        tokens.push_back(tmpstr);
}

string StringTokenizer::nextToken() {
    string token = tokens[tokenIndex];
    tokenIndex++;
    return token;
}

void StringTokenizer::setTokenIndex(int index) {
    tokenIndex = index;
}

bool StringTokenizer::hasMoreTokens() {
    return tokenIndex < tokens.size();
}

const vector<string>& StringTokenizer::getTokens() {
    return tokens;
}

string StringTokenizer::getToken() {
    return tokens[tokenIndex];
}

string StringTokenizer::getToken(int shift) {
    return tokens[tokenIndex + shift];
}

int StringTokenizer::getSize() {
    return tokens.size();
}

