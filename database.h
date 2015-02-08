#pragma once

#include <map>
#include <string>
using namespace std;
class Database
{

    Database(){
        files.emplace("test","aaaa");
    }
    Database(Database const &)=delete ;

    map<string,string> files;
public:
    static Database& getInstance()
    {
        static Database _instance;
        return _instance;
    }

    static string getAssociatedTable(string file)
    {
        Database& db = getInstance();
        auto it = db.files.find(file);
        if(it != db.files.end())return it->second;
        else return "TABLE";
    }

};