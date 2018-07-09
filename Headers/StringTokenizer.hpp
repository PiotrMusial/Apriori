#ifndef STRINGTOKENIZER_HPP_
#define STRINGTOKENIZER_HPP_

#include <vector>
#include <sstream>
using namespace std;

class StringTokenizer {
private:
    vector<string> tokens;
    vector<string> vecIn;
    string strIn;
    int vecIndex;
    int tokenIndex;
public:
    StringTokenizer(string strIn);
    string nextToken();
    void setTokenIndex(int index);
    bool hasMoreTokens();
    const vector<string>& getTokens();
    string getToken();
    string getToken(int shift);
    int getSize();
};

#endif /* STRINGTOKENIZER_HPP_ */
