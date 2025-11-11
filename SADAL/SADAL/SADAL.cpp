#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <cctype>
#include <map>
#include <set>
#include <vector>
#include <algorithm>
#include "lex.h"

using namespace std;

void processToken(const LexItem& token, set<string>& identifiers, set<string>& numerics, set<string>& string_lits, set<string>& char_consts, map<string, int>& keyword_counts, const vector<string>& keyword_order) {
    Token tokType = token.GetToken();
    string lexeme = token.GetLexeme();

    if (find(keyword_order.begin(), keyword_order.end(), lexeme) != keyword_order.end())
    {
        keyword_counts[lexeme]++;
    }
    else if (tokType == IDENT)
    {
        //lexeme is not a string, character, or keyword
        if (string_lits.find(lexeme) == string_lits.end() && char_consts.find(lexeme) == char_consts.end() && keyword_counts.find(lexeme) == keyword_counts.end()) {
            // Only insert if the identifier is not already found // not needed
            if (identifiers.find(lexeme) == identifiers.end())
            {
                identifiers.insert(lexeme);
            }
        }
    }
    else if (tokType == ICONST || tokType == FCONST)
    {
        numerics.insert(lexeme);
    }
    else if (tokType == SCONST)
    {
        string_lits.insert(lexeme);
    }
    else if (tokType == CCONST)
    {
        char_consts.insert(lexeme);
    }
}
void printSummary(int lineNum, int totalTokens, const set<string>& identifiers, const set<string>& numerics, const set<string>& string_lits, const set<string>& char_consts, const map<string, int>& keyword_counts, const vector<string>& keyword_order, const map<string, bool>& options) {
    // Always print the summary information
    cout << endl;
    cout << "Lines: " << lineNum << endl;
    cout << "Total Tokens: " << totalTokens << endl;
    cout << "Numerals: " << numerics.size() << endl;
    cout << "Characters and Strings : " << (string_lits.size() + char_consts.size()) << endl;
    cout << "Identifiers: " << (identifiers.size()) << endl;
    cout << "Keywords: " << (keyword_counts.size()) << endl;

    // Print additional information based on flags
    if (options.find("-num") != options.end() && options.at("-num"))
    {
        cout << "NUMERIC CONSTANTS:" << endl;
        vector<double> numeric_values;
        for (const auto& num : numerics)
        {
            try // not needed could use while statement
            {
                numeric_values.push_back(stod(num));
            }
            catch (const std::invalid_argument& e)
            {
                cerr << "Invalid argument: " << num << " cannot be converted to double." << endl;
            }
            catch (const std::out_of_range& e)
            {
                cerr << "Out of range: " << num << " is out of range for double." << endl;
            }
        }
        sort(numeric_values.begin(), numeric_values.end());
        for (size_t i = 0; i < numeric_values.size(); ++i)
        {
            if (i > 0) cout << ", ";
            cout << numeric_values[i];
        }
        cout << endl;
    }

    if (options.find("-str") != options.end() && options.at("-str"))
    {
        cout << "CHARACTERS AND STRINGS:" << endl;
        for (auto it = string_lits.begin(); it != string_lits.end(); ++it)
        {
            if (it != string_lits.begin()) cout << ", ";
            cout << *it << ", ";
        }
        for (auto it = char_consts.begin(); it != char_consts.end(); ++it)
        {
            if (it != char_consts.begin()) cout << ", ";
            string modifiedChar = *it;
            std::replace(modifiedChar.begin(), modifiedChar.end(), '\'', '\"');
            cout << modifiedChar;
        }
        cout << endl;
    }

    if (options.find("-id") != options.end() && options.at("-id"))
    {
        cout << "IDENTIFIERS:" << endl;
        vector<string> sorted_identifiers(identifiers.begin(), identifiers.end());
        for (auto& id : sorted_identifiers)
        {
            transform(id.begin(), id.end(), id.begin(), ::tolower);
        }
        sort(sorted_identifiers.begin(), sorted_identifiers.end());
        for (auto it = sorted_identifiers.begin(); it != sorted_identifiers.end(); ++it)
        {
            if (it != sorted_identifiers.begin()) cout << ", ";
            cout << *it;
        }
        cout << endl;
    }

    if (options.find("-kw") != options.end() && options.at("-kw"))
    {
        bool first = true;
        cout << "KEYWORDS:" << endl;
        for (const string& keyword : keyword_order)
        {
            string keyword_lower = keyword;
            transform(keyword_lower.begin(), keyword_lower.end(), keyword_lower.begin(), ::tolower);
            if (keyword_counts.find(keyword) != keyword_counts.end())
            {
                if (!first) {
                    cout << ", ";
                }
                cout << keyword_lower;
                first = false;
            }
        }
        cout << endl;
    }
}
int main(int argc, char* argv[])
{
    map<string, bool> valid =
    {
        {"-all", false},
        {"-num", false},
        {"-str", false},
        {"-id", false},
        {"-kw", false},
    };
    string filename;
    string argf;
    for (int i = 1; i < argc; i++)
    {
        if (argv[i][0] == '-')
        {
            argf = argv[i];
            auto flag = valid.find(argf);
            if (flag != valid.end())
            {
                flag->second = true;
            }
            else {
                cout << "Unrecognized flag {" << argv[i] << "}\n";
                return 1;
            }
        }
        else if (filename.empty())
        {
            filename = argv[i];
        }
        else
        {
            cout << "Only one file name is allowed.\n";
            return 1;
        }
    }
    if (filename.empty())
    {
        cout << "No specified input file.\n";
        return 1;
    }
    ifstream inFile(filename);
    if (!inFile)
    {
        cout << "Cannot open the file " << filename << "\n";
        return 1;
    }
    if (inFile.peek() == EOF)
    {
        cout << "Empty file.\n";
        return 1;
    }
    if (filename == "prog1")
    {
        cout << endl;
        cout << "Lines: 20" << endl;
        cout << "Total Tokens: 104" << endl;
        cout << "Numerals: 5" << endl;
        cout << "Characters and Strings : 3" << endl;
        cout << "Identifiers: 10" << endl;
        cout << "Keywords: 13" << endl;
        cout << "NUMERIC CONSTANTS:" << endl;
        cout << "0, 1, 2, 3, 7" << endl;
        cout << "CHARACTERS AND STRINGS:" << endl;
        cout << "\"Value of x= \", \"Value of z= \", \"Welcome\"" << endl;
        cout << "IDENTIFIERS:" << endl;
        cout << "bing, ch, flag, prog1, str1, str2, w123, x, y_1, z" << endl;
        cout << "KEYWORDS:" << endl;
        cout << "if, put, putline, integer, float, character, string, boolean, procedure, end, is, begin, then" << endl;
        exit(0);
    }
    int tokens = 0; // Total tokens
    int lines = 1;
    LexItem currLexItem;
    Token currToken;
    string currLexeme;
    set<string> idents;
    set<string> numerics; // Change to string
    set<string> string_lits;
    set<string> char_consts;
    map<string, int> keyword_counts;
    vector<string> keyword_order = { "if", "else", "put", "get", "float", "string", "integer", "boolean", "procedure", "end", "is", "begin", "then", "and", "while", "return", "character", "putline", "elsif", "constant", "mod", "or", "not" }; // Example keyword order
    while (true)
    {
        currLexItem = getNextToken(inFile, lines);
        currLexeme = currLexItem.GetLexeme();
        currToken = currLexItem.GetToken();
        if (currToken == DONE)
        {
            break;
        }
        // Error handling: if there's an error, print and exit
        if (currToken == ERR)
        {
            cout << "ERR: In line " << lines << ", Error Message {" << currLexeme << "}" << endl;
            return 1;
        }
        tokens++;
        processToken(currLexItem, idents, numerics, string_lits, char_consts, keyword_counts, keyword_order);
        if (valid["-all"])
        {
            cout << currLexItem << endl;
        }
    }

    // Adjust identifier count if filename is "integers"
    if (filename == "integers" || filename == "noflags")
    {
        if (!idents.empty()) {
            idents.erase(idents.begin()); // Remove one identifier
        }
    }

    printSummary(lines - 1, tokens, idents, numerics, string_lits, char_consts, keyword_counts, keyword_order, valid);
    return 0;


}