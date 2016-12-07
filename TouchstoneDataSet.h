#ifndef TOUCHSTONEDATASET_H
#define TOUCHSTONEDATASET_H

#include "TouchStoneData.h"

class TouchStoneDataSet
{
public:
    TouchStoneDataSet(){}
    void AddData(TouchStoneData q){data.push_back(q);}
    TouchStoneData GetData(int i){return data[i];}
    int NumberOfMeasurements(){return data.size();}
private:
    std::vector<TouchStoneData> data;
};

#endif
