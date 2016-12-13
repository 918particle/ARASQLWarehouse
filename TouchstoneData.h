#ifndef TOUCHSTONEDATA_H
#define TOUCHSTONEDATA_H

#include <string>
#include <vector>
#include <algorithm>

class TouchStoneData
{
public:
    TouchStoneData(std::string title,int r,int c) : part_number(title), rows(r), columns(c)
    {
        data.clear();
        for (int i=0;i<rows;++i)
        {
            std::string st = " ";
            std::vector<std::string> rs(columns,st);
            data.push_back(rs);
        }
    }
    void SetData(std::vector<std::vector<std::string> > c)
    {
        if(c.size()!=rows || c[0].size()!=columns)
        {
            std::cout<<"Incorrect dimensions for input arrays."<<std::endl;
        }
        else
        {
            for (int i=0;i<rows;++i)
            {
                for(int j=0;j<columns;++j)
                {
                    data[i][j] = c[i][j];
                }
            }
        }
    }
    std::string PrintPartNumber() {return part_number;}
    int GetDataLength(){return rows;}
    std::string GetDataRow(int k)
    {
        std::string result="";
        for (int i=0;i<columns;++i)
        {
            if(i<(columns-1))
            {
                result+=(data[k][i]+", ");
            }
            else
            {
                result+=data[k][i];
            }
        }
        return result;
    }
private:
    std::string part_number;
    std::string data_type;
    int rows;
    int columns;
    std::vector<std::vector<std::string> > data;
};

#endif
