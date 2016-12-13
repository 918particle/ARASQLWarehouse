#include <sqlite3.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cctype>
#include "TouchStoneDataSet.h"

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
    const int columns=9;
    std::string sqlCurrent;
    std::vector<std::string> statements;
    int totalActions=0;
    int performedActions=0;
    float c;
    std::vector<std::vector<std::string> > cs; cs.clear();
    std::vector<std::string> thisRow; thisRow.clear();
    std::string thisLine;
    TouchStoneDataSet *touchstone_data_set = new TouchStoneDataSet();
    std::string current_file = "";
    std::string sub = "";
    std::stringstream iss;
    int N,M,j,k,q;
    N = M = j = k = q = 0;
    const char split_char = ' ';
    const std::string errorChar1 = " ";
    const std::string errorChar2 = "\\n";
    const int TouchStoneRowLength = 9;
    
    //Handle arguments.
    if(argc!=3)
    {
        fprintf(stderr,"Usage: %s DATABASE-NAME INPUT-FILE-NAME\n",argv[0]);
        return(1);
    }
    
    //Read data from the list of files, and create TouchStoneDataSet objects.
    std::ifstream all_files(argv[2]);
    while (all_files.good() && !all_files.eof())
    {
        std::getline(all_files,current_file);
        if(current_file=="") break;
        std::cout<<"Incorporating this file: "<<current_file<<std::endl;
        std::ifstream infile(current_file);
        current_file = current_file.erase(current_file.find("."),std::string::npos);
        do {std::getline(infile,thisLine);} while(thisLine.find("#")==std::string::npos);
        while(std::getline(infile,thisLine))
        {
			iss.str(thisLine);
			for(sub=" ";std::getline(iss,sub,split_char);++q)
			{
				if(sub!=errorChar1 && q<TouchStoneRowLength)
				{
					thisRow.push_back(sub);
				}
				else continue;
			}
			q=0;
            iss.clear();
            cs.push_back(thisRow);
            thisRow.clear();
        }
        infile.close();
        TouchStoneData *touchstone_data = new TouchStoneData(current_file,cs.size(),cs[0].size());
        touchstone_data->SetData(cs);
        touchstone_data_set->AddData(*touchstone_data);
        delete touchstone_data;
        cs.clear();
    }
    all_files.close();
    std::cout<<"Read all Touchstone files from list."<<std::endl;
    
    //Define SQL actions based on the pieces of data.
    N = touchstone_data_set->NumberOfMeasurements();
    for(j=0;j<N;++j)
    {
        sqlCurrent = "create table '"+touchstone_data_set->GetData(j).PrintPartNumber()
        +"'(frequencies number(3.3), "
        +"S11dB number(3.3), S11angle number(3.3), "
        +"S21dB number(3.3), S21angle number(3.3), "
        +"S12dB number(3.3), S12angle number(3.3), "
        +"S22dB number(3.3), S22angle number(3.3));";
        statements.push_back(sqlCurrent);
        M = touchstone_data_set->GetData(j).GetDataLength();
        for (k=0;k<M;++k)
        {
            sqlCurrent = "insert into '"+touchstone_data_set->GetData(j).PrintPartNumber()
                        +"' values("+touchstone_data_set->GetData(j).GetDataRow(k)+");";
            statements.push_back(sqlCurrent);
        }
    }
    totalActions=statements.size();

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
