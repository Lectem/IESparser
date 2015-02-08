#include <iostream>
#include "IES.h"
#include "dirent.h"
#include "database.h"


#include <sys/stat.h>
#include <unistd.h>
using namespace std;

int main(int argc, char * argv[]) {
    if(argc>2)
    {
        struct stat buf;
        stat(argv[1],&buf);
        if(S_ISDIR(buf.st_mode))
        {
            DIR * dir =opendir(argv[1]);
            if(dir)
            {
                struct dirent * entry = readdir(dir);
                cout << "Parsing files of directory " << argv[1] << endl;
                while(entry)
                {
                    if(DT_REG == entry->d_type)
                    {
                        string filename (entry->d_name);
                        if(!filename.substr(filename.length()-4, filename.length()).compare(".ies")) {
                            cout << entry->d_name << endl;
                            string s(entry->d_name);
                            IES::IES ies((argv[1] + ("/" + s)).c_str());
                            rmdir("output/");
                            mkdir("output/");
                            ies.toSQL("output/" + string(entry->d_name) + ".sql", argv[2]);
                            ies.toXML("ies/" + string(entry->d_name).substr(0, string(entry->d_name).length() - 4) + ".xml");
                        }
                        else cout << "ignoring " << filename << endl;
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
                cout << "can't open dir :";
                perror("");
            }
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

    cout << Database::getAssociatedTable("test");


    return 0;
}