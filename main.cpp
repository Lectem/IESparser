#include <iostream>
#include "IES.h"
#include "dirent.h"
#include "database.h"

#include <queue>
#include <sys/stat.h>
#include <unistd.h>
using namespace std;


void parseDirectoryRec(string dir)
{
    queue<string> dirs;
    dirs.push(dir);
    while(!dirs.empty())
    {
        string curdir = dirs.front();
        dirs.pop();
        DIR * dir =opendir(curdir.c_str());
        if(dir)
        {
            struct dirent * entry = readdir(dir);
            cout << "Parsing files of directory " << curdir << endl;
            while(entry)
            {
                if(DT_REG == entry->d_type)
                {
                    string filename (entry->d_name);
                    if(!filename.substr(filename.length()-4, filename.length()).compare(".ies")) {
                        cout << entry->d_name << endl;
                        string s(entry->d_name);
                        IES::IES ies((curdir + ("/" + s)).c_str());
                        rmdir("output/");
                        mkdir("output/");
                        ies.toSQL("output/" + string(entry->d_name).substr(0, string(entry->d_name).length() - 4)+ ".sql", Database::getAssociatedTable(entry->d_name));
                        ies.toXML(curdir+"/" + string(entry->d_name).substr(0, string(entry->d_name).length() - 4) + ".xml");
                    }
                }
                else if(DT_DIR == entry->d_type && entry->d_name[0] != '.')
                {
                    dirs.push(string(curdir)+"/" +entry->d_name);
                }
                else
                {
                    cout << "ignoring non regular file " << entry->d_name << endl;
                }
                entry = readdir(dir);

            }

            closedir(dir);
        }
        else {
            cout << "can't open dir "<< curdir<<":";
            perror("");
        }
    }
}



int main(int argc, char * argv[]) {
    if(argc>2)
    {
        struct stat buf;
        stat(argv[1],&buf);
        if(S_ISDIR(buf.st_mode))
        {
            parseDirectoryRec(argv[1]);
        }
        else if(S_ISREG(buf.st_mode))
        {
            IES::IES ies(argv[1]);
            ies.toSQL(string(argv[1]) +".sql ", argv[2]);
        }
    }
    else
    {
        cout << "Usage : " << argv[0] << " file.ies" <<" tablename" <<endl;
    }

    return 0;
}