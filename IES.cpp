#include "IES.h"

namespace IES {
    void XORStr(char *t, int length) {
        int i = 0;
        while (i < length) {
            if (t[i] == 0) break;
            t[i] ^= 0x1;
            ++i;
        }
    }

    string readXORStr(istream &in) {
        string res;
        bool end = false;
        uint16_t length = 0;
        in.read((char *) &length, sizeof(length));
        while (!end && length-- > 0) {
            char tmp = 0;
            in.read(&tmp, 1);
            if (tmp == 0) end = true;
            res += tmp ^ 0x1;
        }
        return res;
    }

    std::string IES::escapeSQL(std::string const &s)
    {
        std::size_t n = s.length();
        std::string escaped;
        escaped.reserve(n * 2);

        for (std::size_t i = 0; i < n; ++i) {
            if ( s[i] == '\'')
                escaped += '\\';
            escaped += s[i];
        }
        return escaped;
    }

    void IES::toSQL(string file, string table)

    {
        ofstream out(file);
        if(!out.is_open())
        {
            cerr << "can't open "<< file << endl;
        }
        else {

            for (Row &r: rows) {
                //out << "-- row index" << r.index << endl;
                out << "REPLACE INTO " << table << "(";
                int first = true;
                for (Column &c : columns) {
                    if (first)first = false;
                    else out << ",";
                    out << c.name1;
                }
                out << ")" << " VALUES (";
                first = true;
                int count_int = 0;
                int count_str = 0;
                for (Column &c : columns) {
                    if (first)first = false;
                    else out << ",";

                    switch (c.fmtType) {

                        case NUMBER:
                            out << r.numbers[count_int];
                            count_int++;
                            break;
                        case STRING:
                            if (r.strings[count_str].empty())out << "None";
                            else out << "'" << escapeSQL(r.strings[count_str]) << "'";
                            count_str++;
                            break;
                        default:
                            out << "NULL";
                            break;
                    }
                }
                out << ");" << endl;
            }
        }
    }


    std::string IES::escapeXML(std::string const &s)
    {
        std::size_t n = s.length();
        std::string escaped;
        escaped.reserve(n * 2);

        for (std::size_t i = 0; i < n; ++i) {
            if ( s[i] == '\"')
            {
                escaped += "&quot;";
            }
            else
            {
                escaped += s[i];
            }
        }
        return escaped;
    }

    void IES::toXML(string file) {
        ofstream out(file);
        if (!out.is_open()) {
            cerr << "can't open " << file << endl;
        }
        else {
            out << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>" << endl;
            out << "<idspace id=\""<< header.Filename <<"\">" << endl;
            for (Row &r: rows) {
                out << "\t<Class ";

                int count_int = 0;
                int count_str = 0;
                int count_str2 = 0;
                for (Column &c : columns) {
                    out <<" "<< c.name1 << "=";

                    switch (c.fmtType) {

                        case NUMBER:
                            out << "\"" << r.numbers[count_int] <<"\"";
                            count_int++;
                            break;
                        case STRING:
                            if (r.strings[count_str].empty())out << "\"None\"";
                            else out << "\"" << escapeXML(r.strings[count_str]) << "\"";
                            count_str++;
                            break;
                        case STRING2:
                            if (r.strings2[count_str2].empty())out << "\"None\"";
                            else out << "\"" << escapeXML(r.strings2[count_str2]) << "\"";
                            count_str2++;
                            break;
                        default:
                            cout << "unknown type, using NULL " << endl;
                            out << "NULL";
                            break;
                    }
                }
                out << "/>  " << endl;
            }
            out << "</idspace>" << endl;
        }
    }
}