#ifdef ONE_FILE_LIB
	#include "easyjson/easyjson_one_file.h"
#else
	#include "easyjson/easyjson.h"
#endif

#include <iostream>
#include <functional>
#include <fstream>

using namespace easyjson;

//====================================================================

void loadTestFileJson(const char* fileName, std::function<void(const char*)> cbk)
{
	std::ifstream files;
	files.open(fileName, std::ifstream::in);
	if(files.is_open()){
		std::string line;
		while(std::getline(files, line)){
			try{
				if(line.length()==0){
					continue;
				}
				
				cbk(line.c_str());		
			}
			catch(...){
				dbg(line);
				break;
			}
		}
		files.close();
	}
}

//====================================================================

void pressKey()
{
	std::cout<<"Press any key to continue: "<<std::endl;
	static char inBuff[200];
	std::cin>>inBuff;
}

//====================================================================

int main(int argc, char* argv[])
{
	std::ios_base::sync_with_stdio(false);

	std::string fileData=TEST_DATA_PATH;
	fileData+="/json_files.txt";

	loadTestFileJson(fileData.c_str(), [](const std::string& line){
		if(line[0]=='/'){
			return;
		}
		size_t pos=line.find_first_of(" ");
		if(pos!=std::string::npos){
			bool testResult=line.substr(0, pos)=="true";
			std::string jsonFile=TEST_DATA_PATH;
			jsonFile+="/"+line.substr(pos+1);
			std::cout<<"File: "<< jsonFile<<std::endl;

			#ifdef ONE_FILE_LIB
				auto obj=JsonParser::openJsonFile(jsonFile.c_str(), ErrorHandlerMode::Quiet);
			#else
				auto obj=JsonObj::parseJsonFile(jsonFile.c_str(), ErrorHandlerMode::Quiet);
			#endif

			if(obj.isValid()){	
				if(!testResult){
					std::cout<<"Test should have failed..."<<std::endl;
					pressKey();
				}
				else{
					std::cout<<"Test passed: JSON is valid!\n";
				}
			}	
			else{
				if(testResult){
					std::cout<<"Test should have passed..."<<std::endl;
					std::cout<<"Exception: "<<obj.getErrorMsg()<<std::endl;
					pressKey();
				}
				else{
					std::cout<<"Test passed: invalid JSON syntax detected: "<<obj.getErrorMsg()<<"\n";
				}			
			}
		}
	});

	return 0;
};
