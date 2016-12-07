#ifndef CALIBRATIONPULSEDATASET_H
#define CALIBRATIONPULSEDATASET_H

#include "CalibrationPulseData.h"

class CalibrationPulseDataSet
{
public:
    CalibrationPulseDataSet(){}
    void AddData(CalibrationPulseData q){data.push_back(q);}
    CalibrationPulseData GetData(int i){return data[i];}
    int NumberOfPulses(){return data.size();}
private:
    std::vector<CalibrationPulseData> data;
};

#endif
