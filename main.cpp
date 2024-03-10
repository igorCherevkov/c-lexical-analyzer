    #include <iostream>
    #include <fstream>
    #include <sstream>
    #include <string>
    #include <vector>
    #include <cctype>
    #include <algorithm>

    using namespace std;

    enum Tokens {
        KEYWORD,
        IDENTIFIER,
        OPERATION,
        SEPARATOR,
        CONSTANT,
        PreprocessorDirective,
        COMMENT,
        LINE
    };

    struct Token {
        Tokens type;
        string value;
        int id;
    };

    class Lexer {
    public:
        const string inputLine;
        vector<string> preprocessorDirectives;
        vector<string> identifiers;
        vector<string> keywords;
        vector<string> constants;
        vector<string> operations = {
            "==", "!=", "<", ">", "-", "+", "*", "/", "=", "<<", ">>"
        };
        vector<string> separators = {
            "(", ")", "{", "}", ";", ","
        };

        const vector<string>& getPreprocessorDirectives() const {
            return preprocessorDirectives;
        }

        const vector<string>& getIdentifiers() const {
            return identifiers;
        }

        const vector<string>& getConstants() const {
            return constants;
        }

        void addToPreprocessorDirectives(const string& value) {
            if (find(preprocessorDirectives.begin(), preprocessorDirectives.end(), value) == preprocessorDirectives.end()) {
                preprocessorDirectives.push_back(value);
            }
        }

        void addToIdentifiers(const string& value) {
            if (find(identifiers.begin(), identifiers.end(), value) == identifiers.end()) {
                identifiers.push_back(value);
            }
        }

        void addToConstants(const string& value) {
            if (find(constants.begin(), constants.end(), value) == constants.end()) {
                constants.push_back(value);
            }
        }

        Lexer(const string& line,
            vector<string>& ppDirectives,
            vector<string>& iden,
            vector<string>& consts) : inputLine(line),
            preprocessorDirectives(ppDirectives),
            identifiers(iden),
            constants(consts) {
            loadKeyWords("keywords.txt");
        }

        static string tokenTypeToString(Tokens type) {
            switch (type) {
                case KEYWORD:
                    return "KEYWORD";
                case IDENTIFIER:
                    return "IDENTIFIER";
                case OPERATION:
                    return "OPERATION";
                case SEPARATOR:
                    return "SEPARATOR";
                case CONSTANT:
                    return "CONSTANT";
                case PreprocessorDirective:
                    return "PreprocessorDirective";
                case COMMENT:
                    return "COMMENT";
                case LINE:
                    return "LINE";
                default:
                    return "UNKNOWN";
            }
        }

        vector<Token> stringToWords(const string& inputLine) {
            vector<Token> tokens;
            string currentToken;

            for (char c : inputLine) {
                if (isspace(c) || find(separators.begin(), separators.end(), string(1, c)) != separators.end()) {
                    if (!currentToken.empty()) {
                        tokens.push_back(getToken(currentToken));
                        currentToken.clear();
                    }
                    if (find(separators.begin(), separators.end(), string(1, c)) != separators.end()) {
                        tokens.push_back(getToken(string(1, c)));
                    }
                } else {
                    currentToken += c;
                }
            }

            if (!currentToken.empty()) {
                tokens.push_back(getToken(currentToken));
            }

            return tokens;
        }

        vector<Token> isComment(const string& inputLine) {
            vector<Token> tokens;

            if (inputLine.size() >= 2) {
                if (inputLine.substr(0, 2) == "//") {
                    tokens.push_back({ COMMENT, inputLine });
                } else if (inputLine.substr(0, 2) == "/*" && inputLine.substr(inputLine.size() - 2, 2) == "*/") {
                    tokens.push_back({ COMMENT, inputLine });
                }
            }

            if (tokens.empty()) {
                tokens = stringToWords(inputLine);
            }

            return tokens;
        }

        Token getToken(const string& inputLine) {
            int tokenID;
            if (inputLine.compare(0, 1, "#") == 0) {
                addToPreprocessorDirectives(inputLine);
                tokenID = checkID(inputLine, 1);
                return { PreprocessorDirective, inputLine, tokenID };
            } else if (inputLine.compare(0, 1, "<") == 0 and inputLine.compare(inputLine.length() - 1, 1, ">") == 0) {
                addToIdentifiers(inputLine);
                tokenID = checkID(inputLine, 2);
                return { IDENTIFIER, inputLine, tokenID };
            } else if (isKeyWord(inputLine)) {
                tokenID = checkID(inputLine, 3);
                return { KEYWORD, inputLine, tokenID };
            } else if (isSeparator(inputLine)) {
                tokenID = checkID(inputLine, 4);
                return { SEPARATOR, inputLine, tokenID };
            } else if (isOperator(inputLine)) {
                tokenID = checkID(inputLine, 5);
                return { OPERATION, inputLine, tokenID };
            } else {
                addToConstants(inputLine);
                tokenID = checkID(inputLine, 6);
                return { CONSTANT, inputLine, tokenID };
            }
        }

        bool isKeyWord(const string& inputLine) {
            return find(keywords.begin(), keywords.end(), inputLine) != keywords.end();
        }

        bool isSeparator(const string& inputLine) {
            return find(separators.begin(), separators.end(), inputLine) != separators.end();
        }

        bool isOperator(const string& inputLine) {
            return find(operations.begin(), operations.end(), inputLine) != operations.end();
        }

        int checkID(const string& inputLine, int check) {
            switch (check) {
                case 1: {
                    auto it = find(preprocessorDirectives.begin(), preprocessorDirectives.end(), inputLine);
                    if (it != preprocessorDirectives.end()) {
                        return distance(preprocessorDirectives.begin(), it) + 1;
                    }
                    break;
                }
                case 2: {
                    auto it = find(identifiers.begin(), identifiers.end(), inputLine);
                    if (it != identifiers.end()) {
                        return distance(identifiers.begin(), it) + 1;
                    }
                    break;
                }
                case 3: {
                    auto it = find(keywords.begin(), keywords.end(), inputLine);
                    if (it != keywords.end()) {
                        return distance(keywords.begin(), it) + 1;
                    }
                    break;
                }
                case 4: {
                    auto it = find(separators.begin(), separators.end(), inputLine);
                    if (it != separators.end()) {
                        return distance(separators.begin(), it) + 1;
                    }
                    break;
                }
                case 5: {
                    auto it = find(operations.begin(), operations.end(), inputLine);
                    if (it != operations.end()) {
                        return distance(operations.begin(), it) + 1;
                    }
                    break;
                }
                case 6: {
                    auto it = find(constants.begin(), constants.end(), inputLine);
                    if (it != constants.end()) {
                        return distance(constants.begin(), it) + 1;
                    }
                    break;
                }
            };
        }

        void loadKeyWords(const string& keyWordsFilePath) {
            ifstream keywordsFile(keyWordsFilePath);

            if (keywordsFile.is_open()) {
                string keyword;
                while (getline(keywordsFile, keyword)) {
                    keywords.push_back(keyword);
                }
                keywordsFile.close();
            } else {
                cerr << endl << "error when opening file: " << keyWordsFilePath << endl << endl;
            }
        }

    };

    int main() {
        string mainCPPPath = "test.cpp";

        ifstream file(mainCPPPath);

        if (file.is_open()) {
            cerr << endl << "file was open " << mainCPPPath << endl << endl;
            vector<string> preprocessorDirectives;
            vector<string> identifiers;
            vector<string> constants;
            Lexer lexer("", preprocessorDirectives, identifiers, constants);

            string line;
            while (getline(file, line)) {
                vector<Token> tokens = lexer.isComment(line);

                for (const auto& token : tokens) {
                    if (token.id == 0) {
                        cout << "Value: " << token.value << ", type: " << lexer.tokenTypeToString(token.type) << endl;
                    } else {
                        cout << "Value: " << token.value << ", type: " << lexer.tokenTypeToString(token.type) << ", id: " << token.id << endl;
                    }
                }
            }

            file.close();
        } else {
            cerr << endl << "error when opening file: " << mainCPPPath << endl;
        }

        return 0;
    }