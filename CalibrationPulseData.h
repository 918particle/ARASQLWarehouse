#ifndef CALIBRATIONPULSEDATA_H
#define CALIBRATIONPULSEDATA_H

#include <string>
#include <vector>
#include <algorithm>

class CalibrationPulseData
{
public:
    CalibrationPulseData(std::string title)
    {
        data_type = title.find("noise")!=std::string::npos ? "noise" : "pulse";
        if(data_type=="noise")
        {
            int pieces_of_data = 4;
            int count=0;
            std::string delim = "_";
            std::string current_string = title.erase(0,title.find("/")+1);
            current_string = current_string.erase(current_string.find("."),std::string::npos);
            std::vector<std::string> results;
            while (count!=pieces_of_data)
            {
                switch(count)
                {
                    case 0:
                        number = current_string.substr(0,current_string.find(delim));
                        break;
                    case 1:
                        letter = current_string.substr(0,current_string.find(delim));
                        break;
                    case 2:
                        break;
                    case 3:
                        temp_setting = current_string;
                        break;
                }
                current_string.erase(0,current_string.find(delim)+delim.length());
                ++count;
            }
        }
        else
        {
            int pieces_of_data = 5;
            int count=0;
            std::string delim = "_";
            std::string current_string = title.erase(0,title.find("/")+1);
            current_string = current_string.erase(current_string.find("."),std::string::npos);
            std::vector<std::string> results;
            while (count!=pieces_of_data)
            {
                switch(count)
                {
                    case 0:
                        number = current_string.substr(0,current_string.find(delim));
                        break;
                    case 1:
                        letter = current_string.substr(0,current_string.find(delim));
                        break;
                    case 2:
                        time_setting = current_string.substr(0,current_string.find(delim));
                        break;
                    case 3:
                        mode = current_string.substr(0,current_string.find(delim));
                        break;
                    case 4:
                        temp_setting = current_string.substr(0,current_string.find(delim));
                        break;
                }
                current_string.erase(0,current_string.find(delim)+delim.length());
                ++count;
            }
        }
    }
    std::string GetNumber(){return number;}
    std::string GetLetter(){return letter;}
    std::string GetTimeSetting(){return time_setting;}
    std::string GetMode(){return mode;}
    std::string GetTempSetting(){return temp_setting;}
    std::string GetDataType(){return data_type;}
    int GetDataLength(){return column_1.size();}
    std::string GetDataX(int i){return column_1[i];}
    std::string GetDataY(int i){return column_2[i];}
    void SetData(std::vector<std::string> c1,std::vector<std::string> c2)
    {
        column_1.clear();
        column_2.clear();
        column_1.assign(c1.begin(),c1.end());
        column_2.assign(c2.begin(),c2.end());
    }
    std::string PrintNameString()
    {
        if(data_type=="noise")
        {
            return number+"."+letter+"."+temp_setting;
        }
        else
        {
            return number+"."+letter+"."+time_setting+"."+mode+"."+temp_setting;
        }
    }
private:
    std::string number;
    std::string letter;
    std::string time_setting;
    std::string mode;
    std::string temp_setting;
    std::string data_type;
    std::vector<std::string> column_1;
    std::vector<std::string> column_2;
};

#endif


