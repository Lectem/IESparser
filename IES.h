#pragma once

#include <vector>
#include <string>
#include <fstream>
#include <cassert>
#include <iostream>
#include <stdexcept>

using namespace std;

namespace IES {
    enum Fmt : int16_t {
        NUMBER = 0,
        STRING = 1,
        STRING2 =2
    };
    void XORStr(char * t,int length);
    string readXORStr(istream& in);

    struct Header {
        char Filename[64];

        uint32_t version;
        uint32_t offset_hint_a; // subtract from file_size to get ptr
        uint32_t offset_hint_b;
        uint32_t file_size;
        Header(){}
        Header(istream & in)
        {
            in.read((char*)this, sizeof(*this));
        }


        friend std::ostream& operator<<( std::ostream& out, const Header & h)
        {
            out << "====HEADER====" << endl;
            out << "Name:\t\t" << h.Filename << endl;
            out << "IES ver:\t"<< h.version << endl;
            out << "offset_a:\t"<< h.offset_hint_a << endl;
            out << "offset_b:\t"<< h.offset_hint_b << endl;
            out << "file size:\t"<< h.file_size << endl<<endl;
        }



    };

    struct Info {
        uint16_t val_1; // always 1?
        uint16_t Rows; // number of rows
        uint16_t Columns; // total number of int and string columns
        uint16_t ColInt; // cols which are ints
        uint16_t ColString; // cols which are strings
        Info(){}
        Info(istream & in)
        {
            in.read((char*)this, sizeof(*this));
        }

        friend std::ostream& operator<<( std::ostream& out, const Info & i)
        {
            out << "===INFO===" << endl;
            out << "val1 :\t\t" << i.val_1 << endl;
            out << "Rows :\t\t"<< i.Rows << endl;
            out << "Columns:\t"<< i.Columns << endl;
            out << "ColInt:\t\t"<< i.ColInt << endl;
            out << "ColString:\t"<< i.ColString << endl<<endl;
            return out;
        }
    };

    struct Column
    {
        char name1[64] ={};
        char name2[64] ={};
        Fmt fmtType = NUMBER;
        uint32_t index =0;
        Column(){}
        Column(istream & in,Info & info)
        {
            in.read((char*)this, sizeof(name1)+sizeof(name2)+sizeof(fmtType)+ sizeof(index));
            XORStr(name1,64);
            XORStr(name2,64);
            switch (fmtType)
            {
                case NUMBER:
                assert(info.ColInt > index);
                    break;
                case STRING:
                case STRING2:
                    assert(info.ColString > index);
                    break;
            }
        }

        friend std::ostream& operator<<( std::ostream& out, const Column & c)
        {
            out << "===Column===" << endl;
            out << "name1 :\t\t" <<     c.name1 << endl;
            out << "name2 :\t\t"<<      c.name2 << endl;
            out << "fmtType:\t"<<       c.fmtType <<endl;
            out << "index:\t\t"<<    c.index << endl;
            return out;
        }



    };

    struct Row
    {
        int index;
        string optional;
        vector<double> numbers;
        vector<string> strings;
        vector<string> strings2;
        Row(){}
        Row(istream & in,Info & info)
        {
            in.read((char*)&index, sizeof(index));
            optional = readXORStr(in);
            float tmpFloat;
            double tmpDouble=0;
            in.read((char*)&tmpFloat, sizeof(float));
            bool is64bit = false;
            if(!tmpFloat)is64bit=true;

            if(is64bit)
            {
                in.read((char*)&tmpDouble+4, 4);
                numbers.emplace_back(tmpDouble);

            }
            else
                numbers.emplace_back(tmpFloat);

            for(int i=1;i<info.ColInt;++i)
            {
                if(is64bit)
                {
                    in.read((char*)&tmpDouble, sizeof(tmpDouble));
                    numbers.emplace_back(tmpDouble);
                }
                else
                {
                    in.read((char*)&tmpFloat, sizeof(tmpFloat));
                    numbers.emplace_back(tmpFloat);
                }
            }
            for(int i=0;i<info.ColString;++i)
            {
                strings.push_back(readXORStr(in));
            }
        }
        friend std::ostream& operator<< ( std::ostream& out, const Row & r)
        {
            out << "===Row===" << endl;
            out << "index:\t\t" <<      r.index << endl;
            out << "optional:\t"<<    r.optional << endl;
            for(int i=0;i<r.numbers.size();++i)
            {
                out << "numbers["<<i<<"]=\t"<<r.numbers[i]<< endl;
            }
            for(int i=0;i<r.strings.size();++i)
            {
                out << "strings["<<i<<"]=\t"<<r.strings[i]<< endl;
            }
            for(int i=0;i<r.strings2.size();++i)
            {
                out << "strings2["<<i<<"]=\t"<<r.strings2[i]<< endl;
            }
            return out;
        }

    };


    struct IES {
        Header header;
        Info info;
        vector<Row> rows;
        vector<Column> columns;

        IES( const char * file)
        {
            try {
                ifstream input(file, ios::binary);
                if(input.is_open()) {
                    auto fsize = input.tellg();
                    input.seekg(0, ios::end);
                    fsize = input.tellg() - fsize;
                    input.seekg(0, ios::beg);

                    header = input;
                    if(header.file_size != fsize) throw runtime_error("header.file_size != fsize");
                    info = input;

                    if(info.ColString + info.ColInt != info.Columns)throw runtime_error("info.ColString + info.ColInt != info.Columns");

                    uint32_t OffsetRows = header.file_size - header.offset_hint_b;
                    uint32_t OffsetColumns = header.file_size - (header.offset_hint_b + header.offset_hint_a);
                    uint16_t unknown =0;
                    //input.read((char*)&unknown, sizeof(unknown));
                    input.seekg(OffsetColumns, ios::beg);

                    int count_int = 0, count_str = 0,count_str2=0 ,count_unkw=0;
                    for (int i = 0; i < info.Columns; ++i) {
                        columns.emplace_back(input,info);
                        switch (columns.back().fmtType) {
                            case NUMBER:
                                count_int++;
                                break;
                            case STRING:
                                count_str++;
                                break;
                            case STRING2:
                                count_str2++;
                                break;
                            default:
                                count_unkw++;
                                cout << "In file "<< file << ":"<<endl;
                                cout << "New column format (col " << columns.size() << " "<< columns.back().name1 <<") : " << columns.back().fmtType << endl;
                                break;
                        }
                    }
                    if(count_unkw)cout << count_unkw << " columns of unknwon type"<<endl;
                    if(count_int != info.ColInt) throw runtime_error("count_int != info.ColInt");
                    if(count_str + count_str2 != info.ColString) throw runtime_error("count_str + count_str2 != info.ColString");
                    if(input.tellg() != OffsetRows) throw runtime_error("input.tellg() != OffsetRows");
                    for (int i = 0; i < info.Rows; ++i) {
                        rows.emplace_back(input, info);
                        input.seekg(info.ColString, ios::cur);
                    }
                }
                else
                {
                    cerr << "Can't open file "<< file << endl;
                }
            } catch (exception const &  e )
            {
                cerr << "Exception caught : " << e.what() << endl;
            }
        }

        void toSQL(string file, string table);

        void toXML(string file);

        friend std::ostream& operator<<( std::ostream& out, const IES & ies)
        {
            out << ies.header << ies.info;
            out << "=======COLUMNS:"<< ies.columns.size() <<"=======" << endl;
            for(auto &c : ies.columns)
            {
                out << c;
            }
            out << "=======ROWS:"<< ies.rows.size() <<"=======" << endl;
            /*for(auto &r : ies.rows)
            {
                out << r;
            }*/
            out << ies.rows.front();
            return out;
        }
        std::string escapeSQL(std::string const &s);
        std::string escapeXML(std::string const &s);

    };

}



