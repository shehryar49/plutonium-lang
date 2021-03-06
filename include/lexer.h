#ifndef LEXER_H_
#define LEXER_H_
#include "plutonium.h"
using namespace std;
extern size_t line_num;
extern string source_code;
extern string filename;
extern vector<string> files;
extern vector<string> sources;

const char* keywords[] = {"var","if","else","while","dowhile","import","return","break","continue","function","nil","for","to","step","foreach","namespace","class","private","public","extends","try","catch","throw","yield"};//23 keywords right now
bool isKeyword(string s)
{
  for(size_t k=0;k<sizeof(keywords)/sizeof(char*);k+=1)
  {
    if(s==(string)keywords[k])
    {
      return true;
    }
  }
  return false;
}
string lstrip(string);
void removeUselessNewlines(vector<Token>&);
void lexErr(string type,string msg,bool dummy)
  {
    printf("\nFile %s\n",filename.c_str());
    printf("%s at line %ld\n",type.c_str(),line_num);
    auto it = std::find(files.begin(),files.end(),filename);
    size_t i = it-files.begin();
    string source_code = sources[i];
    int l = 1;
    string line = "";
    int k = 0;
    while(l<=line_num)
    {

        if(source_code[k]=='\n')
        {
            l+=1;
        }
        else if(l==line_num)
            line+=source_code[k];
        k+=1;
        if(k>=source_code.length())
        {
          break;
        }
    }
    printf("%s\n",lstrip(line).c_str());
    printf("%s\n",msg.c_str());
    exit(0);
  }
vector<Token> generateTokens(const string& s) //generates tokens for a plutonium program
{
    size_t k = 0;
    std::unordered_map<string,Token> lexMacros;
    Token macro;
    macro.type = NUM_TOKEN;
    macro.content = to_string(SEEK_CUR);
    lexMacros.emplace("SEEK_CUR",macro);
    macro.content = to_string(SEEK_SET);
    lexMacros.emplace("SEEK_SET",macro);
    macro.content = to_string(SEEK_END);
    lexMacros.emplace("SEEK_END",macro);
    macro.type = FLOAT_TOKEN;
    macro.content = "3.14159";
    lexMacros.emplace("pi",macro);
    macro.content = "2.718";
    lexMacros.emplace("e",macro);
    macro.content=to_string(CLOCKS_PER_SEC);
    lexMacros.emplace("clocks_per_sec",macro);
    macro.content = to_string(-FLT_MAX);
    lexMacros.emplace("FLT_MIN",macro);
    macro.content = to_string(FLT_MAX);
    lexMacros.emplace("FLT_MAX",macro);
    macro.type = NUM_TOKEN;
    macro.content = to_string(INT_MIN);
    lexMacros.emplace("INT_MIN",macro);
    macro.content = to_string(INT_MAX);
    lexMacros.emplace("INT_MAX",macro);
    macro.content = to_string(LLONG_MIN);
    lexMacros.emplace("INT64_MIN",macro);
    macro.content = to_string(LLONG_MAX);
    lexMacros.emplace("INT64_MAX",macro);
    macro.content = "1";
    lexMacros.emplace("TypeError",macro);
    macro.content = "2";
    lexMacros.emplace("ValueError",macro);
    macro.content = "3";
    lexMacros.emplace("MathError",macro);
    macro.content = "4";
    lexMacros.emplace("NameError",macro);
    macro.content = "5";
    lexMacros.emplace("IndexError",macro);
    macro.content = "6";
    lexMacros.emplace("ArgumentError",macro);
    macro.content = "7";
    lexMacros.emplace("UnknownError",macro);
    macro.content = "8";
    lexMacros.emplace("FileIOError",macro);
    macro.content = "9";
    lexMacros.emplace("KeyError",macro);
    macro.content = "10";
    lexMacros.emplace("OverflowError",macro);
    macro.content = "11";
    lexMacros.emplace("FileOpenError",macro);
    macro.content = "12";
    lexMacros.emplace("FileSeekError",macro);
    macro.content = "13";
    lexMacros.emplace("ImportError",macro);
    macro.content = "14";
    lexMacros.emplace("ThrowError",macro);
    macro.content = "15";
    lexMacros.emplace("MaxRecursionError",macro);
    macro.content = "16";
    lexMacros.emplace("AccessError",macro);
    char c;
    vector<Token> tokenlist;
    int ln = 1;
    while(k<s.length())
    {
        c = s[k];
        if(c=='"')
        {

            int j = k+1;
            string t;
            bool match;
            bool escaped = false;
            while(j<s.length())
            {
                if(s[j]=='"')
                {
                    if(escaped)
                    {
                        escaped = false;
                    }
                    else
                    {
                      match = true;
                      break;
                    }
                    t+=s[j];
                }
                else if(s[j]=='\\' && !escaped)
                {
                    escaped = true;
                    t+=s[j];
                }
                else if(s[j]=='\n')
                {
                    t+=s[j];
                    ln+=1;
                }
                else
                {
                  t+=s[j];
                  if(escaped)
                  {
                      escaped=false;
                  }

                }
                j+=1;
            }
            if(!match)
            {
                line_num = ln;
                lexErr("SyntaxError","Error expected a '\"' at the end!",false);
            }
            Token i;
            line_num = ln;
            i.content = addlnbreaks(t);
            i.type = STRING_TOKEN;
            i.ln = ln;
            tokenlist.push_back(i);
            // = "string";
          //  printf("here\n%d\n",j);
            k = j;
        //    printf("k  = %d\nstrlen : %ld",k,s.length());
        }
        else if(c=='@')
        {
            if(k==s.length()-1)
            {
                line_num = ln;
                lexErr("SyntaxError","Invalid macro name",false);
            }
            int j = k+1;
            string t = "";
            while(j<s.length())
            {

                if(!isalpha(s[j]) && !isdigit(s[j]) && s[j]!='_')
                  break;
                if(j==k+1 && isdigit(s[j]))
                {
                  line_num = ln;
                  lexErr("SyntaxError","Invalid macro name",false);
                }
                t+=s[j];
                j+=1;
            }
            if(lexMacros.find(t)==lexMacros.end())
            {
                line_num = ln;
                lexErr("NameError","Unknown macro "+t,false);
            }
            tokenlist.push_back(lexMacros[t]);
            k = j;
            continue;
        }
        else if(c=='#')
        {
            int j;
            for(j=k+1;j<s.length();j+=1)
            {
                if(s[j]=='\n')
                {
                    break;
                }
            }
            k = j-1;

        }
        else if(isdigit(c))
        {
            if(c=='0')
            {
                if(k!=s.length()-1)
                {
                    if(s[k+1]=='x')
                    {
                        k+=1;
                        if(k+1+1>s.length()-1)
                        {
                          line_num = ln;
                          lexErr("SyntaxError","Invalid Syntax",false);
                        }
                        k+=1;
                        string b = s.substr(k,1+1);
                        Token i;
                        i.type = BYTE_TOKEN;
                        i.ln = ln;
                        i.content = b;
                        tokenlist.push_back(i);
                        //="byte";
                        k+=1+1;
                        continue;
                    }
                }
            }
            int j = k+1;
            string t;
            t+=c;
            bool decimal = false;
            bool exp = false;
            int l = 0;
            int m = 0;
            int n = 0;
            bool expSign = false;
            while(j<s.length())
            {
                if(!isdigit(s[j]) && s[j]!='.' && s[j]!='e' && s[j]!='-' && s[j]!='+')
                {
                    j = j-1;
                    break;
                }
                if(s[j]=='.')
                {
                    if(decimal || exp)
                    {
                        j = j-1;
                        break;
                    }
                    decimal = true;
                }
                else if(s[j]=='e')
                {
                    if(exp)
                    {
                        j = j-1;
                        break;
                    }
                    exp = true;
                }
                else if(s[j]=='+' || s[j]=='-')
                {
                    if(expSign || !exp)
                    {
                        j = j-1;
                        break;
                    }
                    expSign = true;
                }
                t+=s[j];
                j+=1;
            }
            if(t[t.size()-1]=='e')
            {
                t = t.substr(0,t.length()-1);
                j-=1;
            }
            Token i;
              i.content = t;
            if(!decimal && !exp)
              i.type = NUM_TOKEN;
            else
              i.type = FLOAT_TOKEN;
            i.ln = ln;
            tokenlist.push_back(i);
           k = j;
        }
        else if(c=='>' || c=='<')
        {

           if(k==s.length()-1)
           {
                line_num = ln;
                lexErr("SyntaxError","Invalid Syntax",false);
           }
           if(s[k+1]=='=')
           {
               Token i;
               i.type = TokenType::OP_TOKEN;
               i.content+=c;
               i.content+="=";
               i.ln = ln;
               tokenlist.push_back(i);
               k = k+1;
           }
           else if(c=='<' && s[k+1]=='<')
           {
             Token i;
             i.type=TokenType::OP_TOKEN;
             i.content ="<<";
             tokenlist.push_back(i);
             k+=1;
           }
           else if(c=='>' && s[k+1]=='>')
           {
             Token i;
             i.type=TokenType::OP_TOKEN;
             i.content =">>";
             tokenlist.push_back(i);
             k+=1;
           }
           else
           {
               Token i;
               i.type = TokenType::OP_TOKEN;
               i.content+=c;
               i.ln = ln;
               tokenlist.push_back(i);

           }
        }
        else if(c=='.')
        {
           if(k==s.length()-1)
           {
                line_num = ln;
                lexErr("SyntaxError","Invalid Syntax",false);
           }
           if(s[k+1]=='.')
           {
               Token i;
               i.type = TokenType::Ellipsis_TOKEN;
               i.content="..";
               i.ln = ln;
               tokenlist.push_back(i);
               k = k+1;
           }
           else
           {
               Token i;
               i.type = TokenType::OP_TOKEN;
               i.content+=c;
               i.ln = ln;
               tokenlist.push_back(i);
           }
        }
        else if(c=='+' || c=='-')
        {
            if(k!=s.length()-1)
            {
                if(s[k+1]=='=')
                {
                  Token i;
                  i.ln = ln;
                  i.content += c;
                  i.content+="=";
                  i.type = TokenType::OP_TOKEN;
                  tokenlist.push_back(i);
                  k+=2;
                  continue;
                }
            }
            Token i;
            i.ln = ln;
            i.content += c;
            i.type = TokenType::OP_TOKEN;
            tokenlist.push_back(i);
            // = "TokenType::OP_TOKEN";
        }
        else if(c=='/' || c=='*' || c=='%' || c=='^' || c=='&' || c=='|' || c=='~')
        {

            if(k!=s.length()-1)
            {
                if(s[k+1]=='=')
                {
                  Token i;
                  i.ln = ln;
                  i.content += c;
                  i.content+="=";
                  i.type = TokenType::OP_TOKEN;
                  tokenlist.push_back(i);
                  // = "TokenType::OP_TOKEN";
                  k+=2;
                  continue;
                }
            }
            Token i;
            i.ln = ln;
            i.content += c;
            i.type = TokenType::OP_TOKEN;
            tokenlist.push_back(i);
            // = "TokenType::OP_TOKEN";


        }
        else if(c=='(')
        {
            //("found TokenType::LParen_TOKEN\n");
            Token i;
            i.content += c;
            i.type = TokenType::LParen_TOKEN;
            i.ln = ln;
            tokenlist.push_back(i);
            // = "TokenType::LParen_TOKEN";
        }
        else if(c==')')
        {
            //("here\n");
            Token i;
            i.content += c;
            i.type = TokenType::RParen_TOKEN;
                        i.ln = ln;
            // = "TokenType::RParen_TOKEN";
            tokenlist.push_back(i);
            //("done\n");
        }
        else if(c=='!')
        {
            if(k!=s.length()-1)
            {
                if(s[k+1]=='=')
                {
                    Token i;
                    i.content = "!=";
                    i.type = TokenType::OP_TOKEN;
                    // = "TokenType::OP_TOKEN";
                                i.ln = ln;
                    k+=1;
                    tokenlist.push_back(i);
                }
                else
                {
                Token i;
                i.content = "!";
                i.type = TokenType::OP_TOKEN;
                            i.ln = ln;

                tokenlist.push_back(i);
                }
            }
            else
            {
                Token i;
                i.content = "!";
                i.type = TokenType::OP_TOKEN;
                            i.ln = ln;
                tokenlist.push_back(i);
            }
        }
        else if(c=='=')
        {
            if(s[k+1]=='=')
            {
              Token i;
              i.content = "==";
              i.type = TokenType::OP_TOKEN;
              // = "TokenType::OP_TOKEN";
                          i.ln = ln;
              k+=1;
              tokenlist.push_back(i);
            }
            else
            {
              Token i;
              i.content = "=";
              i.type = TokenType::OP_TOKEN;
              // = "equal";
                          i.ln = ln;
              tokenlist.push_back(i);
            }
        }

        else if(c=='[')
        {
            Token i;
            i.content = "[";
            i.type = BEGIN_LIST_TOKEN;
            i.ln = ln;
            tokenlist.push_back(i);
        }
        else if(c==']')
        {

                Token i;
                i.content = "]";
                i.type = END_LIST_TOKEN;
                        i.ln = ln;
                tokenlist.push_back(i);
        }
        else if(c=='{')
        {

            Token i;
            i.content = "{";
            i.type = L_CURLY_BRACKET_TOKEN;
                        i.ln = ln;
            tokenlist.push_back(i);
        }
        else if(c=='}')
        {
            Token i;
            i.content = "}";
            i.type = R_CURLY_BRACKET_TOKEN;
            i.ln = ln;
            tokenlist.push_back(i);
        }
        else if(isalpha(c) || c=='_')
        {
            int j = k+1;
            string t;
            t+=c;
            while(j<s.length())
            {
                if((j!=s.length()-1 && s[j]==':' && s[j+1]==':'))
                {
                  t+="::";
                  j+=2;
                }
                else if(!isalpha(s[j]) && !isdigit(s[j]) && s[j]!='_')
                {
                    j = j-1;
                    break;
                }
                else
                {
                  t+=s[j];
                  j+=1;
                }
            }

            Token i;
            if(isKeyword(t))
            {
             if(t=="if" && k!=0 && tokenlist.size()!=0)
             {
                 if(tokenlist[tokenlist.size()-1].type == KEYWORD_TOKEN && tokenlist[tokenlist.size()-1].content=="else")
                 {
                    tokenlist[tokenlist.size()-1].content+=" if";
                    k = j+1;
                    continue;
                 }
             }
             //  Token i;
            i.type = KEYWORD_TOKEN;
            // = "keyword";
            i.content = t;
            i.ln = ln;
            //printf("keyword ln = %ld\n",i.ln);
            }
            else if(t=="or" || t=="and" || t=="is")
            {
            i.content = t;
            i.type = TokenType::OP_TOKEN;
                        i.ln = ln;
            // = "TokenType::OP_TOKEN";
            }
            else if(t=="true" || t=="false")
            {
            i.content = t;
            i.type = BOOL_TOKEN;
                        i.ln = ln;
            // = "bool";
            }
            else
            {
              i.type = ID_TOKEN;
              // = "id";
              i.content = t;
              i.ln = ln;
            }
            tokenlist.push_back(i);
            k = j;
        }
        else if(c==',')
        {
            Token i;
            i.type = COMMA_TOKEN;
            i.content = ",";
            i.ln = ln;
            // = "comma";
            tokenlist.push_back(i);
        }
        else if(c==':')
        {
            Token i;
            i.type = COLON_TOKEN;
            i.content = ":";
            i.ln = ln;
            // = "colon";
            tokenlist.push_back(i);
        }
        else if(c=='\n' )
        {
            Token i;
            i.content = "\n";
            i.type = NEWLINE_TOKEN;;
            // = "newline";
            i.ln = -1;
            tokenlist.push_back(i);
            ln+=1;
        }
        else if(c=='\t' )
        {

            //do nothing
        }
        else if(c==' ' )
        {
            //do nothing
        }
        else if(c=='\r')
        {

        }
        else
        {
            //error;
                if(c>127 || c<0)
                {
                  printf("Error: The given file is non ascii\n");
                  exit(0);
                }
                line_num = ln;
                lexErr("SyntaxError","Invalid Syntax",false);
        }
        k+=1;
    }
    removeUselessNewlines(tokenlist);
    if (tokenlist.size() == 0)
        return tokenlist;
    Token t;
    t.type = NEWLINE_TOKEN;
    t.content = "\n";
    tokenlist.push_back(t);
    t.content = "EOP";
    t.type = EOP_TOKEN;
    tokenlist.push_back(t);
    t.type = NEWLINE_TOKEN;
    t.content = "\n";
    tokenlist.push_back(t);
    return tokenlist;
}
#endif // LEXER_H_
