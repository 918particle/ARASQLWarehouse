#include <sqlite3.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cctype>
#include <cstddef>
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

std::string SplitFilename(std::string str)
{
  std::size_t found = str.find_last_of("/\\");
  return str.substr(found+1);
}

std::string RemoveFileExtension(std::string str)
{
	std::size_t found = str.find_last_of(".");
	return str.substr(0,found);
}

void FindDataInFile(std::ifstream infile,std::string thisLine)
{
	do {std::getline(infile,thisLine);}
	while(thisLine.find("#")==std::string::npos);
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
    std::vector<std::vector<std::string> > cs; cs.clear();
    std::vector<std::string> thisRow; thisRow.clear();
    TouchStoneDataSet *touchstone_data_set = new TouchStoneDataSet();
    std::string current_file = "";
    std::string sub = "";
    std::stringstream iss;
    int N,M,j,k,q,count;
    N = M = j = k = q = count = 0;
    const char split_char = '\t';
    const int TouchStoneRowLength = 9;
    int verbosityLevel;
        
    //Handle arguments.
    if(argc==3)
    {
		verbosityLevel = 0;
	}
	if(argc==4)
	{
		verbosityLevel = atoi(argv[3]);
	}
	if(argc<=2)
	{
        fprintf(stderr,"Usage: %s DATABASE-NAME INPUT-FILE-NAME VERBOSITY(0/1)\n",argv[0]);
        return(1);
    }
    
    //Read data from the list of files, and create TouchStoneDataSet objects.
    //void ProcessAllFiles(std::ifstream,std::string);
    std::ifstream all_files(argv[2]);
    while (std::getline(all_files,current_file))
    {
		std::string thisLine="";
        if(verbosityLevel) std::cout<<"Incorporating this file: "<<current_file<<std::endl;
        std::ifstream infile(current_file);
        current_file = SplitFilename(RemoveFileExtension(current_file));
        FindDataInFile(infile,thisLine);
        while(std::getline(infile,thisLine))
        {
			iss.str(thisLine);
			for(sub=" ";std::getline(iss,sub,split_char);++q)
			{
				if(sub.size()==0) continue;
				if(!isspace(sub.at(0)) && count<TouchStoneRowLength)
				{
					thisRow.push_back(sub);
					++count;
				}
			}
			q=0;
			count=0;
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
    if(verbosityLevel) std::cout<<"Read all Touchstone files from list."<<std::endl;
    
    //Define SQL actions based on the pieces of data.
    N = touchstone_data_set->NumberOfMeasurements();
    if(verbosityLevel) std::cout<<"Defining "<<N<<" SQL tables."<<std::endl;
    for(j=0;j<N;++j)
    {
		if(verbosityLevel) std::cout<<"Table "<<j<<std::endl;
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
    if(sqlite3_open(argv[1], &db))
    {
        fprintf(stderr,"Can't open database: %s\n",sqlite3_errmsg(db));
        sqlite3_close(db);
        return(1);
    }
    
    //Perform list of SQL actions.
    while(performedActions!=totalActions)
    {
        if(sqlite3_exec(db,statements[performedActions].c_str(),callback,0,&zErrMsg)!=SQLITE_OK)
        {
            fprintf(stderr, "SQL error: %s, action #%i\n",zErrMsg,performedActions);
            sqlite3_free(zErrMsg);
            break;
        }
        else ++performedActions;
        if(verbosityLevel) std::cout<<"Performed actions: "<<performedActions<<" of "<<totalActions<<std::endl;
    }
    
    //Close the SQL database.
    sqlite3_close(db);
    return 0;
}
