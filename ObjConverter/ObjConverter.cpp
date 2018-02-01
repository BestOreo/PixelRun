#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <direct.h>
#include <vector>
#include <io.h>
#include <algorithm>
#include <windows.h>
#include <fstream>

using namespace std;

/*
* @function: 获取cate_dir目录下的所有文件名
* @param: cate_dir - string类型
* @result：vector<string>类型
*/
vector<string> getFiles(string path)
{
    vector<string> files;//存放文件名

    _finddata_t file;
    long lf;
    //输入文件夹路径
    if ((lf = _findfirst(path.c_str(), &file)) == -1) {
        cout << path << " not found!!!" << endl;
    }
    else {
        while (_findnext(lf, &file) == 0) {
            //输出文件名
            //cout<<file.name<<endl;
            if (strcmp(file.name, ".") == 0 || strcmp(file.name, "..") == 0)
                continue;
            files.push_back(file.name);
        }
    }
    _findclose(lf);
    //排序，按从小到大排序
    sort(files.begin(), files.end());
    return files;
}

void ConvertFolder(string folderPath);
void DeleteFolder(string folderPath);
void ModifyObjFile(string folderPath, string fileName);
void modifyContentInFile(string fileName, int lineNum, char *content);

int main() {

    string path = "..\\project\\_model\\";
    ConvertFolder(path);


    system("PAUSE");
    return 0;
}

void ConvertFolder(string folderPath) {
    vector<string> files = getFiles(folderPath + "*");
    for (int i = 0; i < files.size(); i++)
    {
        size_t pos = files[i].find_last_of(".");
        if (pos != string::npos) {
            string postfix = files[i].substr(pos + 1);
            if (postfix == "meta") {
                // delete
                if (remove((folderPath + files[i]).c_str()) == 0) {
                    cout << "delete meta file: " << files[i] << endl;
                }
                else
                {
                    cout << "!!! delete meta file faild: " << files[i] << endl;
                    cout << stderr << endl;
                }
            }
            if (postfix == "obj") {
                // change .obj
                // add .mtl
                ModifyObjFile(folderPath, files[i].substr(0, files[i].size() - 4));
            }
        }
        else {
            if (files[i] == "Materials") {
                // delete
                DeleteFolder(folderPath + files[i] + "\\");
            }
            else {
                ConvertFolder(folderPath + files[i] + "\\");
            }
        }
    }
}

void DeleteFolder(string folderPath) {
    vector<string> files = getFiles(folderPath + "*");
    for (int i = 0; i < files.size(); i++)
    {
        size_t pos = files[i].find_last_of(".");
        if (pos != string::npos) {
                // delete
            if (remove((folderPath + files[i]).c_str()) == 0) {
                cout << "delete meta file: " << files[i] << endl;
            }
            else
            {
                cout << "!!! delete meta file faild: " << files[i] << endl;
                cout << stderr << endl;
            }
        }
        else {
            DeleteFolder(folderPath + files[i]);
        }
    }

    if (_rmdir(folderPath.c_str()) == 0) {
        cout << "delete folder: " << folderPath << endl;
    }
    else
    {
        cout << "!!! delete folder faild: " << folderPath << endl;
        cout << errno << endl;
    }
}

void ModifyObjFile(string folderPath, string fileName) {
    string file = folderPath + fileName;
    modifyContentInFile(file + ".obj", 2, "usemtl palette\n");

    ofstream out;
    out.open(file + ".mtl");
    out.flush();
    out << "newmtl palette\nillum 1\nKa 0.000 0.000 0.000\nKd 1.000 1.000 1.000\nKs 0.000 0.000 0.000\nmap_Kd " << fileName << ".png\n\0";
    out.close();
}

/************************************************************************/
/* char*tostr  字符串转化str类型
输入：char * 字符串地址
无输出
返回值： str类型的字符串
*/
/************************************************************************/
string charToStr(char * contentChar)
{
    string tempStr;
    for (int i = 0; contentChar[i] != '\0'; i++)
    {
        tempStr += contentChar[i];
    }
    return tempStr;
}


/************************************************************************/
/* 修改文件某行内容
输入：文件名 fileName   行号   lineNum ,修改的内容 content
输出：文件名 fileName
无返回值
tip：1,lineNum从第一行开始 2.content需要加上换行符
*/
/************************************************************************/
void modifyContentInFile(string fileName, int lineNum, char *content)
{
    ifstream in;
    char line[1024] = { '\0' };
    in.open(fileName);
    int i = 0;
    string tempStr;
    while (in.getline(line, sizeof(line)))
    {
        i++;
        if (lineNum != i)
        {
            tempStr += charToStr(line);
        }
        else
        {
            tempStr += charToStr(content);
        }
        tempStr += '\n';
    }
    in.close();
    ofstream out;
    out.open(fileName);
    out.flush();
    out << tempStr;
    out.close();


}