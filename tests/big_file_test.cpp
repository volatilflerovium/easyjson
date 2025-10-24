#ifdef ONE_FILE_LIB
	#include "easyjson/easyjson_one_file.h"
#else
	#include "easyjson/easyjson.h"
#endif

#include <iostream>
#include <functional>
#include <thread>
#include <ctime>

#include "time_profiler/time_profiler.h"

using Profiler=profiler::TimeProfiler<std::chrono::microseconds>;

using namespace easyjson;

//====================================================================

int main(int argc, char* argv[])
{
	#ifdef ONE_FILE_LIB
	Profiler watch("easyjson_one", "#9bddff");
	#else
	Profiler watch("easyjson", "#006994");
	#endif

	bool hotTest=false;
	int iterations=10;
	int idx=1;
	if(argc>1){
		iterations=std::atoi(argv[1]);
		if(argc>2){
			hotTest=std::atoi(argv[2])==1;
			if(argc>3){
				idx=std::atoi(argv[3]);
			}
		}
	}

	std::srand(time(0));

	std::string fileData=TEST_DATA_PATH;
	fileData+="/json_files_performace.txt";

	std::ifstream jsonFiles;
	jsonFiles.open(fileData, std::ifstream::in);
	if(jsonFiles.is_open()){
		std::string fileName;
		int lineNumber=0;
		while(std::getline(jsonFiles, fileName)){
			if(idx==0 || idx==++lineNumber){
				std::cout<<"File: "<<fileName<<std::endl;
	
				if(fileName.length()==0){
					continue;
				}
				std::string file=TEST_DATA_PATH;
				file+=fileName;

				for(int i=0; i<iterations; i++){
					if(!hotTest){
						std::this_thread::sleep_for(std::chrono::seconds(1));
					}

					try{
						watch.start();
						#ifdef ONE_FILE_LIB
							auto obj=JsonParser::openJsonFile(file.c_str());
						#else
							auto obj=JsonObj::parseJsonFile(file.c_str());
						#endif
						watch.takeSample();
					}
					catch(const char* msg){
						std::cout<<"Exception: "<< msg<<std::endl;
					}
				}
				//watch.takeAverageSample();
			}
		}
		jsonFiles.close();
	}

	return 0;
}
