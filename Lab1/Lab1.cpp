#include <fstream>
#include <string>
#include <sstream>
#include <iostream>

using namespace std;

string replaceExtension(const string& filename) {
    size_t dotPos = filename.find_last_of('.');
    if (dotPos != string::npos && filename.substr(dotPos) == ".txt") {
        return filename.substr(0, dotPos) + "_html.txt";
    }
    return filename;
}

string markdown_to_html(string markdown) {
    if (markdown.length() == 0) {
        return markdown;
    }

    bool in_pre = false;
    bool in_strong = false;
    bool in_italic = false;
    bool in_monospaced = false;
    bool in_paragraph = false;
    string error = "Syntax Error";
    string html;
    string line;
    stringstream ss(markdown);

    while (getline(ss, line)) {
        if (line.empty()) {
            continue;
        }

        if (line.substr(0, 3) == "```") {
            if (!in_pre) {
                in_pre = true;
                html += "<p>```\n";
            }
            else {
                in_pre = false;
                html += "```</p>\n";
            }
            continue;
        }

        if (!in_pre) {
            html += "<p>" + line + "</p>\n";
        }
        else {
            html += line + "\n";
        }
    }
  
    markdown = html;

    for (int i = 0; i < markdown.length(); i++) {
        if (markdown[i] == '`' && markdown[i + 1] == '`' && markdown[i + 2] == '`') {
            if (in_pre  == false) {
                in_pre = true;
                markdown.replace(i, 3, "<pre>");
            }
            else {
                in_pre = false;
                markdown.replace(i, 3, "</pre>");
            }
        }

        if (in_pre == false) {
            if (markdown[i] == '*' && markdown[i + 1] == '*') {
                if (in_pre || in_italic || in_monospaced) {
                    cout << "ERROR! Already in STRONG";
                    return error;
                }
                else {
                    if (in_strong == false) {
                        in_strong = true;
                        markdown.replace(i, 2, "<strong>");
                    }
                    else {
                        in_strong = false;
                        markdown.replace(i, 2, "</strong>");
                    }
                }
            }
            else if (markdown[i] == '_' && markdown[i + 1] != '-') {
                if ((markdown[i-1] == '>' && markdown[i - 2] == 'p') || markdown[i - 1] == ' '|| markdown[i + 1] == ' ' || (markdown[i + 1] == '<' && markdown[i + 2] == '/')) {
                    if (in_italic == false) {
                        in_italic = true;
                        markdown.replace(i, 1, "<i>");
                    }
                    else {
                        in_italic = false;
                        markdown.replace(i, 1, "</i>");
                    }
                }
            }
            else if (markdown[i] == '`') {
                if (in_pre || in_strong || in_italic) { 
                    return error;
                }
                else {
                    if (in_monospaced == false) {
                        in_monospaced = true;
                        markdown.replace(i, 1, "<tt>");
                    }
                    else {
                        in_monospaced = false;
                        markdown.replace(i, 1, "</tt>");
                    }
                }
            }
        }
    } 
            if (markdown[markdown.length()-1] != '_' && in_italic == true) {
                return error;
            }
            if (markdown[markdown.length()-2] != '*' && markdown[markdown.length()-1] != '*' && in_strong == true) {
                return error;
            }
            if (markdown[markdown.length() - 1] != '`' && in_monospaced == true) {
                return error;
            }

    return markdown;
}

int main() {
    system("chcp 1251");
    string filePath;
    size_t lastSlashPos;
    string filename;
    string directory;
    string newFilename;
    ofstream output_file;
    ifstream input_file;
    string output_text;

  
    cout << "Enter filename: ";
    cin >> filePath;

    input_file.open(filePath);
    if (!input_file.is_open()) {
        cerr << "File does not exist!\n";
        return 0;
    }
    
    string input_text;
    getline(input_file, input_text, '\0');
    input_file.close();

    output_text = markdown_to_html(input_text);
    if (output_text == "Syntax Error") {
        cout << output_text;
        return 0;
    }

    lastSlashPos = filePath.find_last_of('\\');
    filename = filePath.substr(lastSlashPos + 1);
    directory = filePath.substr(0, lastSlashPos + 1);

    newFilename = replaceExtension(filename);

    directory += newFilename;
    
    output_file.open(directory);
   
    output_file << output_text;
    output_file.close();

    return 0;
}
