#include <sqlite3.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include "CalibrationPulseDataSet.h"

static int callback(void *NotUsed, int argc, char **argv, char **azColName)
{
    int i;
    for(i=0;i<argc;i++)
    {
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}

int main(int argc, char **argv)
{
    //Declare variables.
    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;
    std::string sqlCurrent;
    std::string dataTitle;
    std::vector<std::string> statements;
    int totalActions=0;
    int performedActions=0;
    float c1,c2;
    std::vector<std::string> c1s,c2s;
    std::stringstream tString,sString;
    CalibrationPulseDataSet *pulser_data_set = new CalibrationPulseDataSet();
    std::string current_file = "";
    int N,M,j,k;
    N = M = j = k = 0;
    
    //Handle arguments.
    if(argc!=3)
    {
        fprintf(stderr,"Usage: %s DATABASE-NAME INPUT-FILE-NAME NUMBER_LETTER\n",argv[0]);
        return(1);
    }
    
    //Read data from the list of files, and create CalibrationPulseDataSet objects for the different pulser units.
    std::ifstream all_files(argv[2]);
    while (all_files.good() && !all_files.eof())
    {
        std::getline(all_files,current_file);
        if(current_file=="") break;
        std::cout<<"Incorporating this file: "<<current_file<<std::endl;
        CalibrationPulseData *current_pulse = new CalibrationPulseData(current_file);
        std::ifstream infile(current_file);
        while(infile.good() && !infile.eof())
        {
            infile>>c1; tString<<c1;
            infile>>c2; sString<<c2;
            c1s.push_back(tString.str());
            c2s.push_back(sString.str());
            tString.str(std::string());
            sString.str(std::string());
        }
        infile.close();
        current_pulse->SetData(c1s,c2s);
        pulser_data_set->AddData(*current_pulse);
        delete current_pulse;
        c1s.clear();
        c2s.clear();
    }
    all_files.close();
    
    //Define SQL actions based on the pieces of data.  Eventually, there will be several pulser_data_sets and this should be a loop over a vector of them.
    N = pulser_data_set->NumberOfPulses();
    for(j=0;j<N;++j)
    {
        if(pulser_data_set->GetData(j).GetDataType()=="noise")
        {
            statements.push_back("create table '"+pulser_data_set->GetData(j).PrintNameString()+"'(frequencies number(3.3), power number(3.6));");
        }
        else
        {
            statements.push_back("create table '"+pulser_data_set->GetData(j).PrintNameString()+"'(times number(3.3), voltages number(3.6));");
        }
        M = pulser_data_set->GetData(j).GetDataLength();
        for (k=0;k<M;++k)
        {
            sqlCurrent = "insert into '"+pulser_data_set->GetData(j).PrintNameString()
                        +"' values("+pulser_data_set->GetData(j).GetDataX(k)
                        +","+pulser_data_set->GetData(j).GetDataY(k)+");";
            statements.push_back(sqlCurrent);
        }
    }
    totalActions=statements.size()-1;
    
    //Open the SQL database.
    rc = sqlite3_open(argv[1], &db);
    if(rc)
    {
        fprintf(stderr,"Can't open database: %s\n",sqlite3_errmsg(db));
        sqlite3_close(db);
        return(1);
    }
    
    //Perform list of SQL actions.
    while(performedActions!=totalActions)
    {
        rc = sqlite3_exec(db,statements[performedActions].c_str(),callback,0,&zErrMsg);
        if(rc!=SQLITE_OK)
        {
            fprintf(stderr, "SQL error: %s, action #%i\n",zErrMsg,performedActions);
            sqlite3_free(zErrMsg);
            break;
        }
        else
        {
            ++performedActions;
        }
    }
    
    //Close the SQL database.
    sqlite3_close(db);
    return 0;
}
