#include <functional>
#include <fstream>

#include "easyjson/easyjson.h"

#include "utilities/profiler.h"
Profiler<std::chrono::microseconds> profiler;

using namespace easyjson;

//====================================================================

void loadDataFileJson(const char* fileName, std::function<void(const char*)> cbk)
{
	std::ifstream commandFiles;
	commandFiles.open(fileName, std::ifstream::in);
	if(commandFiles.is_open()){
		std::string commandLine;
		while(std::getline(commandFiles, commandLine)){
			try{
				if(commandLine.length()==0){
					continue;
				}

				cbk(commandLine.c_str());				
			}
			catch(...){
				dbg(commandLine);
				break;
			}
		}
		commandFiles.close();
	}
	else{
		dbgW("could not open file: ", fileName);
	}
}

//====================================================================

void loadTestFileJson(const char* fileName, std::function<void(const char*)> cbk)
{
	std::ifstream commandFiles;
	commandFiles.open(fileName, std::ifstream::in);
	if(commandFiles.is_open()){
		std::string commandLine;
		while(std::getline(commandFiles, commandLine)){
			try{
				if(commandLine.length()==0){
					continue;
				}

				cbk(commandLine.c_str());				
			}
			catch(...){
				dbg(commandLine);
				break;
			}
		}
		commandFiles.close();
	}
}

//====================================================================

void pressKey()
{
	dbg("Press any key to continue: ");
	static char inBuff[200];
	std::cin>>inBuff;
}

void checkResult(const std::string& jsonStr, const char* expected)
{
	if(jsonStr!=expected){
		dbgW("Parse Error?: ");
		dbg("Should get: ", expected);
		dbg("got: ", jsonStr);
		pressKey();
	}
	else{
		dbg(jsonStr);
		dbgG("Passed");
	}
}

//====================================================================

struct TestData
{
	const char* m_jsonStr;
	bool m_testStatus;
};

#define TEST_FILE TEST_FILE_PATH // /test_data.txt

//====================================================================

int main(int argc, char* argv[])
{	
	std::ios_base::sync_with_stdio(false);
	
	int testNum=-1;
	if(argc>1){
		testNum=std::atoi(argv[1]);
	}
	
	//-----------------------------------------------------------------

	try{

	#if 1
	if(testNum==-1 || testNum==0)
	{
		dbgW("\n Test: 0 ===========================================");
			
		auto obj=JsonObj::initObj();
		try{
			auto obj2=obj["a"];
			obj2="b";
			auto obj21=obj["c"];
			obj21["d"]="e";
			obj21["f"]="g";
			obj21["h"]["i"]="j";
			obj21["h"]["i"]="p";
			obj21["h2"];
			obj21["h2"];

			dbg(obj.toString());
			dbgW("This should fail! because the key \"h2\" does not hold a value...");
			/*
			 * This is an expected behaviour as operator[] will create a new key
			 * which should get its value via operator= 
			 * Therefore, keys with not value is a user error.
			 * */
		}
		catch(const char* msg){
			dbgW("Exception: ", msg);
			pressKey();
		}
	}
	
	#endif

	if(testNum==-1 || testNum==1)
	{
		dbgW("\n Test: 1 ===========================================");
		loadTestFileJson(TEST_FILE, [](const std::string& line){
			if(line[0]=='/'){
				return;
			}
			size_t pos=line.find_first_of(" ");
			if(pos!=std::string::npos){
				bool testResult=line.substr(0, pos)=="true";
				std::string jsonStr=line.substr(pos);
				dbg("json str: ", jsonStr);
				try{
					auto obj=JsonObj::parse(jsonStr.c_str());
					
					if(!testResult){
						dbgW("Test should have failed...");
						pressKey();
					}
					else{
						dbgG("Test passed: JSON is valid!\n");
					}
					
					//checkResult(obj.toString(), jsonStr.c_str());
					//dbg(obj.toString(true), "<<--\n\n");
				}	
				catch(const char* msg){
					if(testResult){
						dbgW("Test should have passed...");
						dbgW("Exception: ", msg);
						pressKey();
					}
					else{
						dbgG("Test passed: invalid JSON syntax detected: ", msg, "\n");
					}			
				}
			}
		});

	}

	#if 1
	if(testNum==-1 || testNum==2)
	{
		dbgW("\n Test: 2 ===========================================");
			
		const char* test="{\"a\":[false, null, \"false\", \"a\":3, {\"e\":true, \"d\":2} ], \"b\":{\"c\":false}}";
		//test="{\"a\":[false, null, \"false\", {\"a\":3}:4, {\"e\":true, \"d\":2} ], \"b\":{\"c\":false}}";
		//test="{\"a\":false, {\"a\":3}:4, \"e\":true, \"d\":2}, \"b\":{\"c\":false}}";
		//test="{\"a\":[\"b\":false]}";
		//test="{\"a\":[\"b\",false]}";
		//test="{\"a\":{\"b\":false}}";
		//test="{\"a\":\"false\"}";
		//test="{\"a\":false}";
		//test="{\"a\":{[\"b\":{}]}}";
		//test="{  \"message\": \"Coffee\", \"a\": , \"b\":\"\"}";
		test="{\"message\"}";
		
		dbg("\nCase ", test, ":\n");
		try{
			auto obj=JsonObj::parse(test);		
			dbg(obj.toString());
			dbgW("Test should have failed...");
			pressKey();
		}
		catch(const char* msg){
			dbgG("Test passed: invalid JSON syntax detected.\n");
		}
	}
	
	if(testNum==-1 || testNum==3)
	{
		dbgW("\n Test: 3 ===========================================");
		dbg("Test null\n");

		try{
			auto obj=JsonObj::parse("{\"a\":null, \"b\":[null]}");

			dbg(obj.toString());
			obj["c"]=JSON_NULL;
			dbg(obj.toString());
			obj["b"].pushBack(JSON_NULL);
			dbg(obj.toString());
			obj["b"].pushBack("null");
			dbg(obj.toString());
			obj.append({"d", JSON_NULL});
			dbg(obj.toString());
			obj["d"].getValue<json_null>();
			dbg("Getting obj[\"b\"][2] as json_null");
			obj["b"][2].getValue<json_null>();
			// */
		}
		catch(const char* msg){
			if(std::strcmp(msg, "Can not convert to null.")==0){
				dbgG("Passed: expected exception: ", msg);
			}
			else{
				pressKey();
			}
		}
	}

	if(testNum==-1 || testNum==4)
	{
		dbgW("\n Test: 4 ===========================================");
		
		try{
			const char* test="{\"a\":[false, null, \"false\",  {\"e\":true, \"d\":2}], \"b\":{\"c\":false}}";
			
			dbg("Test boolean\n");
			dbg("\nCase ", test, ":\n");
			//auto obj=JsonObj::parse("{\"a\":\"true\", \"b\":[false, null, false]}");		
			auto obj=JsonObj::parse(test);		
			//auto obj=JsonObj::parse("{\"a\":true, \"b\":{\"c\":false}}");		
			checkResult(obj.toString(), "{\"a\": [false, null, \"false\", {\"d\": 2, \"e\": true}], \"b\": {\"c\": false}}");
			dbg("Keys are re-order because of AVL tree");

			/*dbg("\nAdd c:false and d:\"false\"");
			obj["c"]=false;
			obj["d"]="false";
			dbg(obj.toString());*/
			//dbgG("Pased!");
		}
		catch(const char* msg){
			dbgW("Failed: ", msg);
			pressKey();
		}
	}
	
	if(testNum==-1 || testNum==5)
	{
		dbgW("\nTest: 5 ===========================================");
		dbg("Test empty object and empty array\n");

		{
			dbg("From empty object:\n");
			auto obj=JsonObj::initObj();
			dbg(obj.toString());
			
			obj["a"]=JSON_OBJ;
			obj["b"]=JSON_ARRAY;
			
			checkResult(obj.toString(), "{\"a\": {}, \"b\": []}");
			
			obj["a"]["c"]=JSON_OBJ;
			obj["a"]["d"]=JSON_ARRAY;
			obj["a"]["d"].pushBack(1);
			checkResult(obj.toString(), "{\"a\": {\"c\": {}, \"d\": [1]}, \"b\": []}");
			
			obj["b"].pushBack({JSON_OBJ, {}});		
			checkResult(obj.toString(), "{\"a\": {\"c\": {}, \"d\": [1]}, \"b\": [{}]}");

			obj["b"].pushBack(JSON_OBJ);
			checkResult(obj.toString(), "{\"a\": {\"c\": {}, \"d\": [1]}, \"b\": [{}, {}]}");

			obj["b"][0].append({"a", 2});
			checkResult(obj.toString(), "{\"a\": {\"c\": {}, \"d\": [1]}, \"b\": [{\"a\": 2}, {}]}");

			obj["b"].pushBack({});
			obj["b"].pushBack(JSON_ARRAY);
			checkResult(obj.toString(), "{\"a\": {\"c\": {}, \"d\": [1]}, \"b\": [{\"a\": 2}, {}, [], []]}");			

			dbg("--------------------------------------------");
		}

		{			
			dbg("From string:\n");
			{
				const char* test="{}";//"{ }" "  {   }  "
				dbg("\nCase ", test, ":\n");
				auto obj=JsonObj::parse(test);
				checkResult(obj.toString(), "{}");
				dbg("\n-----------------------------------\n");
			}
			
			
			{
				const char* test="{\"a\":{}}";
				dbg("\nCase ", test, ":\n");
				auto obj=JsonObj::parse(test);

				checkResult(obj.toString(), "{\"a\": {}}");
				
				dbg("appending {\"b\", 2} to a");
				obj["a"].append({"b", 2});
				checkResult(obj.toString(), "{\"a\": {\"b\": 2}}");

				dbg("\n-----------------------------------\n");
			}
			
			{
				const char* test="{\"a\":{\"b\":{\"c\":{}}}}";
				dbg("\nCase ", test, ":\n");
				try{
					auto obj=JsonObj::parse(test);
					checkResult(obj.toString(), "{\"a\": {\"b\": {\"c\": {}}}}");
					dbg("\n-----------------------------------\n");
				}
				catch(const char* msg){
					dbgW("Exception: ", msg, " * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *");
					pressKey();
				}
			}

			{
				const char* test="{\"a\":}";
				dbg("\nCase ", test, ":\n");
				try{
					auto obj=JsonObj::parse(test);
					dbg(obj.toString());
					dbgW("Test should have failed...");
					pressKey();
				}
				catch(const char* msg){
					dbgG("Test passed! invalid JSON syntax detected: ", msg);
				}
			}

			{
				const char* test="{\"a\":1, \"b\":}";
				dbg("\nCase ", test, ":\n");
				try{
					auto obj=JsonObj::parse(test);
					dbg(obj.toString());
					dbgW("Test should have failed...");
					pressKey();
				}
				catch(const char* msg){
					dbgG("Test passed! invalid JSON syntax detected: ", msg);
				}
			}

			{
				const char* test="{\"a\"}";
				dbg("\nCase ", test, ":\n");
				try{
					auto obj=JsonObj::parse(test);
					dbg(obj.toString());
					dbgW("Test should have failed...");
					pressKey();
				}
				catch(const char* msg){
					dbgG("Test passed! invalid JSON syntax detected: ", msg);
				}
			}
		
			{
				const char* test="{\"a\":{\"b\":{\"c\":{\"d\":\"e\", \"f\", \"g\":4}}}}";
				dbg("\nCase ", test, ":\n");
				try{
					auto obj=JsonObj::parse(test);
					dbg(obj.toString());
					dbgW("Test should have failed...");
					pressKey();
				}
				catch(const char* msg){
					dbgG("Test passed! invalid JSON syntax detected: ", msg);
				}
			}
			
			{
				const char* test="{\"a\":{\"b\":{\"c\":[\"d\"]}}}";
				dbg("\nCase ", test, ":\n");
				try{
					auto obj=JsonObj::parse(test);
					checkResult(obj.toString(), "{\"a\": {\"b\": {\"c\": [\"d\"]}}}");
					dbg("\n-----------------------------------\n");
				}
				catch(const char* msg){
					dbgW("Exception: ", msg, " * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *");
					pressKey();
				}
			}
			
			dbg("\n+ + + + + + + + \n");
			{
				const char* test="{[]}";
				dbg("\nCase ", test, ":\n");
				try{
					auto obj=JsonObj::parse(test);
					dbg(obj.toString());
					dbgW("Test should have failed...");
					pressKey();
				}
				catch(const char* msg){
					dbgG("Test passed! invalid JSON syntax detected: ", msg);
				}
			}
	
			{
				const char* test="{\"a\":[]}";
				dbg("\nCase ", test, ":\n");
				try{
				auto obj=JsonObj::parse(test);
					checkResult(obj.toString(), "{\"a\": []}");
				
					dbg("pushing \"b\" to a");
					obj["a"].pushBack("b");
					checkResult(obj.toString(), "{\"a\": [\"b\"]}");
				}
				catch(const char* msg){
					dbgW("Exception: ", msg, " * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *");
					pressKey();
				}
			}

			{
				const char* test="{\"a\":[1]}";
				dbg("\nCase ", test, ":\n");
				try{
				auto obj=JsonObj::parse(test);
					checkResult(obj.toString(), "{\"a\": [1]}");
				
					dbg("pushing \"b\" to a");
					obj["a"].pushBack("b");
					checkResult(obj.toString(), "{\"a\": [1, \"b\"]}");
					dbg("\n-----------------------------------\n");
				}
				catch(const char* msg){
					dbgW("Exception: ", msg, " * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *");
					pressKey();
				}
			}

			{
				const char* test="{\"a\":[[]]}";
				dbg("\nCase ", test, ":\n");
				try{
				auto obj=JsonObj::parse(test);
					checkResult(obj.toString(), "{\"a\": [[]]}");
				
					dbg("pushing \"b\" to a");
					obj["a"].pushBack("b");
					checkResult(obj.toString(), "{\"a\": [[], \"b\"]}");
					dbg("\n-----------------------------------\n");
				}
				catch(const char* msg){
					dbgW("Exception: ", msg, " * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *");
					pressKey();
				}
			}

			{
				const char* test="{\"a\":[{}]}";
				dbg("\nCase ", test, ":\n");
				try{
				auto obj=JsonObj::parse(test);
					checkResult(obj.toString(), "{\"a\": [{}]}");
				
					dbg("pushing \"b\" to a");
					obj["a"].pushBack("b");
					checkResult(obj.toString(), "{\"a\": [{}, \"b\"]}");
					
					dbg("append \"c\":2 to a[0]");
					obj["a"][0].append({"c", 2});
					checkResult(obj.toString(), "{\"a\": [{\"c\": 2}, \"b\"]}");
					dbg("\n-----------------------------------\n");
				}
				catch(const char* msg){
					dbgW("Exception: ", msg);
					pressKey();
				}
			}
		
			{
				const char* test="{\"a\":{[\"b\":{}]}}";
				dbg("\nCase ", test, ":\n");
				try{
					auto obj=JsonObj::parse(test);
					dbg(obj.toString());
					dbgW("Test should have failed...");
					pressKey();
				}
				catch(const char* msg){
					dbgG("Test passed! invalid JSON syntax detected: ", msg);
				}
			}

			{
				const char* test="{\"a\":[\"b\":{}}}";
				dbg("\nCase ", test, ":\n");
				try{
					auto obj=JsonObj::parse(test);
					dbg(obj.toString());
					dbgW("Test should have failed...");
					pressKey();
				}
				catch(const char* msg){
					dbgG("Test passed! invalid JSON syntax detected: ", msg);
				}
			}
		}

		
		{
			const char* test="{\"a\": [\"b\", {}, [], [{}], 2, 0.3, {\"c\": 4, \"d\": [1, 2]}, 4], \"d\": \"e\"}";
			dbg("\nCase ", test, ":\n");
			try{
				auto obj=JsonObj::parse(test);
				checkResult(obj.toString(), test);
				dbg("\n-----------------------------------\n");
			}
			catch(const char* msg){
				dbgW("Exception: ", msg);
				pressKey();
			}
		}

		{
			dbg("From string:\n");
			//const char* test="{\"a\":{}, \"b\":[], \"c\":{\"d\":1}, \"e\":{\"f\":[1]}}";
			const char* test="{\"a\": 1, \"b\": 2, \"c\": {\"d\": 1}, \"e\": {\"f\": [1]}}";
			dbg("Case: ", test);
			auto obj=JsonObj::parse(test);

			try{
				checkResult(obj.toString(), test);
				
				//*
				obj["a"]=JSON_OBJ;
				obj["b"]=JSON_ARRAY;
				obj["c"]="4";
				obj["d"]=4;
				checkResult(obj.toString(), "{\"a\": {}, \"b\": [], \"c\": \"4\", \"d\": 4, \"e\": {\"f\": [1]}}");// */

				obj["a"].append({"ae", 2});
				obj["b"].pushBack(3);
				obj["b"].pushBack({});// Pushing empty array
				obj["b"].pushBack({JSON_OBJ, {}});
				checkResult(obj.toString(), "{\"a\": {\"ae\": 2}, \"b\": [3, [], {}], \"c\": \"4\", \"d\": 4, \"e\": {\"f\": [1]}}");// */

				obj["b"][1].pushBack(4);
				obj["b"][2].append({"g", 5});
				checkResult(obj.toString(), "{\"a\": {\"ae\": 2}, \"b\": [3, [4], {\"g\": 5}], \"c\": \"4\", \"d\": 4, \"e\": {\"f\": [1]}}");// */

				//dbg("value at [b][0]: ", *obj["b"][0].getValue<int>());
				
			}
			catch(const char* msg){
				dbgW("Exception: ", msg);
				pressKey();
			}
		}
	}

	if(testNum==-1 || testNum==6)
	{
		dbgW("\n Test: 6 ===========================================");
		const char* data="{\"a\": \"b\", \"c\": \"2\", \"d\": 2}";
		dbg(data);
		auto obj=JsonObj::parse(data);
		checkResult(obj.toString(), data);
	}

	if(testNum==-1 || testNum==7)
	{
		dbgW("\n Test: 7 ===========================================");
		const char data[]={'{',
			'a',':',' ', 'b',',',
			'c',':','"','d',' ', '\\', '"', 'A', '\\', '"', ':', '{', 'w', '}', '"', ',',
			'e', 'E', 'e', ':', '{', 'f', ':', 'g', '}',
			'}', '\0'};
		dbg(data);
		try{
			auto obj=JsonObj::parse(data);//"{d:e,f:g},h:i,j:{k:{l:m},n:{o:p}}}");
			checkResult(obj.toString(), data);
			dbgW("Test should have failed...");
			pressKey();
		}
		catch(const char* msg){
			dbgG("Test passed! invalid JSON syntax detected: ", msg);
		}
	}

	if(testNum==-1 || testNum==8)
	{
		dbgW("\n Test: 8 ===========================================");
		
		const char data[]={
			'{', '"' , 'I', 'D', '"', ':', '"', '3', '"', ',', '"', 
			'd', 'e', 's', 'c', 'r', 'i', 'p', 't', 'i', 'o', 'n', '"', ':', 
			'"', 'e', 'c', 'h', 'o', ' ', '\\', '"', '1', ' ', '-', '1', ' ', '0', '\\', '"', ' ', '>', '>', '.', '.', '.', '\\', '"', '1', '2', '\\', '"', '?', '"', ',', 
			
			'"', 'r', 'u', 'n', '"', 
			':', 't', 'r', 'u', 'e',
			'}', '\0'
		};
		// */
		//const char* data="{\"ID\":\"3\",\"description\":\"echo 1 -1 0 >>...12?\",\"run\":true}";
		dbg("Test: ", data);

		auto obj=JsonObj::parse(data);
		dbg(obj.toString());
		try{
			dbg("value: ", *(obj["ID"].getValue<int>()));
			dbg("value: ", *(obj["description"].getValue<std::string>()));
		}
		catch(const char* msg){
			dbgW("Exception: ", msg);
			pressKey();
		}
	}

	if(testNum==-1 || testNum==9)
	{
		dbgW("\n Test: 9 ===========================================");
		std::string jsonStr="{\"createnew\":{\"message\":\"\u054d\u054f\u0535\u0542\u053e\u0535\u053c \u0546\u0548\u0550\"},\"explanationofflinedisabled\":{\"message\":\"Google \u0553\u0561\u057d\u057f\u0561\u0569\u0572\u0569\u0565\u0580\u0568 \u0576\u0561\u0587 \u0561\u0576\u0581\u0561\u0576\u0581 \u057c\u0565\u056a\u056b\u0574\u0578\u0582\u0574 \u0585\u0563\u057f\u0561\u0563\u0578\u0580\u056e\u0565\u056c\u0578\u0582 \u0570\u0561\u0574\u0561\u0580 \u0574\u056b\u0561\u0581\u0565\u0584 \u0570\u0561\u0574\u0561\u0581\u0561\u0576\u0581\u056b\u0576, \u0562\u0561\u0581\u0565\u0584 \u056e\u0561\u057c\u0561\u0575\u0578\u0582\u0569\u0575\u0561\u0576 \u0563\u056c\u056d\u0561\u057e\u0578\u0580 \u0567\u057b\u0568, \u0561\u0576\u0581\u0565\u0584 \u056f\u0561\u0580\u0563\u0561\u057e\u0578\u0580\u0578\u0582\u0574\u0576\u0565\u0580 \u0587 \u0574\u056b\u0561\u0581\u0580\u0565\u0584 \u0561\u0576\u0581\u0561\u0576\u0581 \u0570\u0561\u0574\u0561\u056a\u0561\u0574\u0561\u0581\u0578\u0582\u0574\u0568:\"},\"explanationofflineenabled\":{\"message\":\"\u0534\u0578\u0582\u0584 \u0574\u056b\u0561\u0581\u0561\u056e \u0579\u0565\u0584 \u0570\u0561\u0574\u0561\u0581\u0561\u0576\u0581\u056b\u0576, \u057d\u0561\u056f\u0561\u0575\u0576 \u0564\u0561\u0580\u0571\u0575\u0561\u056c \u056f\u0561\u0580\u0578\u0572 \u0565\u0584 \u0583\u0578\u0583\u0578\u056d\u0565\u056c \u0561\u057c\u056f\u0561 \u0586\u0561\u0575\u056c\u0565\u0580\u0568 \u056f\u0561\u0574 \u057d\u057f\u0565\u0572\u056e\u0565\u056c \u0576\u0578\u0580 \u0583\u0561\u057d\u057f\u0561\u0569\u0572\u0569\u0565\u0580:\"},\"extdesc\":{\"message\":\"\u0553\u0578\u0583\u0578\u056d\u0565\u0584, \u057d\u057f\u0565\u0572\u056e\u0565\u0584 \u0587 \u0564\u056b\u057f\u0565\u0584 \u0571\u0565\u0580 \u0583\u0561\u057d\u057f\u0561\u0569\u0572\u0569\u0565\u0580\u0568, \u0561\u0572\u0575\u0578\u0582\u057d\u0561\u056f\u0576\u0565\u0580\u0576 \u0578\u0582 \u0576\u0565\u0580\u056f\u0561\u0575\u0561\u0581\u0578\u0582\u0574\u0576\u0565\u0580\u0568 \u0561\u057c\u0561\u0576\u0581 \u056b\u0576\u057f\u0565\u0580\u0576\u0565\u057f \u056f\u0561\u057a\u056b:\"},\"extname\":{\"message\":\"Google Docs Offline\"},\"learnmore\":{\"message\":\"\u0544\u0561\u0576\u0580\u0561\u0574\u0561\u057d\u0576\"},\"popuphelptext\":{\"message\":\"\u054d\u057f\u0565\u0572\u056e\u0565\u0584 \u0583\u0561\u057d\u057f\u0561\u0569\u0572\u0569\u0565\u0580 \u0587 \u056d\u0574\u0562\u0561\u0563\u0580\u0565\u0584 \u0564\u0580\u0561\u0576\u0584 \u0563\u0578\u0580\u056e\u0568\u0576\u056f\u0565\u0580\u0576\u0565\u0580\u056b \u0570\u0565\u057f \u0574\u056b\u0561\u057d\u056b\u0576\u055d \u0578\u0580\u057f\u0565\u0572 \u0567\u056c \u0578\u0580 \u056c\u056b\u0576\u0565\u0584, \u0561\u0576\u0563\u0561\u0574 \u0561\u057c\u0561\u0576\u0581 \u0581\u0561\u0576\u0581\u056b\u0576 \u0574\u056b\u0561\u0576\u0561\u056c\u0578\u0582\u0589\"}}";
		dbg(jsonStr, "\n");
		
		profiler.start();
		auto obj=JsonObj::parse(jsonStr.c_str());
		profiler.stop();
		
		try{
			auto t=*obj["createnew"]["message"].getValue<std::u8string>();
			dbg("createnew:message: ", reinterpret_cast<const char*>(t.c_str()));
		}
		catch(const char* msg){
			dbgW("Exception: ", msg);
			pressKey();
		}
	}

	if(testNum==-1 || testNum==10)
	{
		dbgW("\n Test: 10 ===========================================");

		loadDataFileJson("../../../../km_recorder_player/parser_test/test_json", [](const char* line){
			try{
				profiler.start();
				JsonObj::parse(line);
				profiler.stop();
			}
			catch(const char* msg){
				dbg(line);
				dbgW("Exception: ", msg);
			}
			
			
		});
		profiler.totalTime(3);
	}

	if(testNum==-1 || testNum==11)
	{
		dbgW("\n Test: 11 ===========================================");
		const auto obj=JsonObj::parse("{\"c\":{\"d\":\"e\"},\"a\":\"b\"}");
		dbg(obj.toString());

		try{
			dbg("value at c:d: ", *(obj["c"]["d"].getValue<const char*>()));
		}
		catch(const char* ex){
			dbgW(ex);
			pressKey();
		}
	}

	if(testNum==-1 || testNum==12)	
	{
		dbgW("\n Test: 12 ===========================================");
		auto obj=JsonObj::initObj();
		auto obj2=obj["a"];
		obj2="b";
		auto obj21=obj["c"];
		obj21="d";
		
		checkResult(obj.toString(), "{\"a\": \"b\", \"c\": \"d\"}");

		try{
			auto obj3=obj["g"]; //by default obj["g"] should be an object 
			obj3["h"]="i";
			obj3["j"]="k";
			checkResult(obj.toString(), "{\"a\": \"b\", \"c\": \"d\", \"g\": {\"h\": \"i\", \"j\": \"k\"}}");

			obj3["l"]["m"]="n";
			obj3["l"]["o"]["p"]="q";
			checkResult(obj.toString(),"{\"a\": \"b\", \"c\": \"d\", \"g\": {\"h\": \"i\", \"j\": \"k\", \"l\": {\"m\": \"n\", \"o\": {\"p\": \"q\"}}}}");

			//obj3["l"]["o"]["p"]["q"]="r";// this should cause an error
			obj["e"]="f";

			checkResult(obj.toString(), "{\"a\": \"b\", \"c\": \"d\", \"e\": \"f\", \"g\": {\"h\": \"i\", \"j\": \"k\", \"l\": {\"m\": \"n\", \"o\": {\"p\": \"q\"}}}}");
		}
		catch(const char* msg){
			dbgW("Exception: ", msg);
			pressKey();
		}	
	}

	if(testNum==-1 || testNum==13)
	{
		dbgW("\n Test: 13 ===========================================");
		auto obj=JsonObj::parse("{\"c\":{\"d\":\"e\"},\"a\":\"b\"}");
		dbg(obj.toString());
		obj["a"]="d";
		dbg(obj.toString());
		obj["f"]["g"]="h";
		dbg(obj.toString());
	}

	if(testNum==-1 || testNum==14)
	{
		dbgW("\n Test: 14 ===========================================");
		// test constantness
		const auto obj=JsonObj::parse("{\"c\":{\"d\":\"e\"},\"a\":\"b\"}");
		dbg(obj.toString());
		//obj["a"]="d";
	}

	if(testNum==-1 || testNum==15)
	{
		dbgW("\n Test: 15 ===========================================");
		auto obj=JsonObj::initObj();
		auto obj2=obj["a"];
		obj2="b";
		obj["c"];
		//auto obj21=obj["c"];
		
		//dbg(obj.toString());

		try{
			dbgW("This should trigger an user error: ", obj.toString());
			dbgW("because the key \"c\" does not hold a value...");
			/*
			 * This is an expected behaviour as operator[] will create a new key
			 * which should get its value via operator= 
			 * Therefore, keys with not value is a user error.
			 * */
		}
		catch(const char* e){
			dbgW(e);
			pressKey();
		}
	}

	if(testNum==-1 || testNum==16)
	{
		dbgW("\n Test: 16 ===========================================");
		auto obj=JsonObj::initObj();
		auto obj2=obj["a"];
		obj2="b";
		obj["c"]="d";
		
		try{
			auto obj3=obj["e"];
			obj3={1,2,3};
			dbg(obj.toString());

			obj["f"]["g"]={1, 2, 3};
			obj["h"]="i";
			dbg(obj.toString());

			obj["f"]["g"].pushBack(7);
			dbg(obj.toString());
			
			obj["j"]={4, 5, 6};
			dbg(obj.toString());

			obj["j"].pushBack({"7", "8", "9"});
			dbg(obj.toString());

			obj["j"].pushBack(10);
			dbg(obj.toString());

			obj["k"]={JSON_OBJ, {}};
			//obj["k"].pushBack({JSON_OBJ, {{"m", 11}, {"n", 12}}});
			dbg(obj.toString());
		}
		catch(const char* msg){
			dbgW("Test should have passed... Exception: ", msg);
			pressKey();
		}
	}

	if(testNum==-1 || testNum==17)
	{
		dbgW("\n Test: 17 ===========================================");
		auto obj=JsonObj::initObj();
		auto obj2=obj["a"];
		obj2="b";
		auto obj21=obj["c"];
		obj21="d";
		
		try{
			auto obj4=obj["e"];
			obj4={{"f", 1}, {"g", 2}, {"h", 3}};
			obj["i"]="j";
			
			checkResult(obj.toString(), "{\"a\": \"b\", \"c\": \"d\", \"e\": [{\"f\": 1}, {\"g\": 2}, {\"h\": 3}], \"i\": \"j\"}");
		}
		catch(const char* msg){
			dbgW("Test should have passed... Exception: ", msg);
			pressKey();
		}
	}

	if(testNum==-1 || testNum==18)
	{
		dbgW("\nTest: 18 ===========================================");
		try{
			auto obj=JsonObj::initObj();
			auto obj2=obj["a"];
			obj2="b";
			auto obj21=obj["c"];
			obj21="d";
			//dbg(obj.toString());

			auto obj3=obj["e"];
			obj3={1,2,3};
			//dbg(obj.toString());
			obj["f"]="g";
			//dbg(obj.toString());
			obj["h"]={{"i", 4}, {"j", 5}, {"k", 6}};
			//dbg(obj.toString());
			obj["l"]="m";
			checkResult(obj.toString(), 
			"{\"a\": \"b\", \"c\": \"d\", \"e\": [1, 2, 3], \"f\": \"g\", \"h\": [{\"i\": 4}, {\"j\": 5}, {\"k\": 6}], \"l\": \"m\"}");
		}
		catch(const char* msg){
			dbgW("Test should have passed... Exception: ", msg);
			pressKey();
		}
	}

	if(testNum==-1 || testNum==19)
	{
		dbgW("\nTest: 19 ===========================================");
		auto obj=JsonObj::initObj();
		auto obj2=obj["a"];
		obj2="b";		
		try{
			auto obj3=obj["e"];
			obj3={1,2,3};
			
			obj["f"]="g";
						
			obj["h"]={{"i", 4}, {"j", 5}, {"k", 6}};
			dbg(obj.toString());
			
			dbg("obj[\"a\"]: ", *obj["a"].getValue<const char*>());
			dbg("obj[\"e\"][1]: ", *obj["e"][1].getValue<int>());
			dbg("obj[\"h\"][1][\"j\"]: ", *obj["h"][1]["j"].getValue<int>());
		}
		catch(const char* msg){
			dbgW("Test should have passed... Exception: ", msg);
			pressKey();
		}
	}

	if(testNum==-1 || testNum==20)
	{
		dbgW("\nTest: 20 ===========================================");
		// test constantness
		const auto obj=JsonObj::parse("{\"a\":{\"b\":\"c\"}, \"d\":[1,2,3], \"e\":\"f\"}");
		dbg(obj.toString());
		try{
			dbg(*obj["d"][1].getValue<int>());
			dbg(*obj["e"].getValue<const char*>());
		}
		catch(const char* msg){
			dbgW("Test should have passed... Exception: ", msg);
			pressKey();
		}
	}

	if(testNum==-1 || testNum==21)
	{
		bool a=true;
		std::string file=TEST_DATA_PATH;
		file+="/json_data.txt";
		loadDataFileJson(file.c_str(), [&a](const char* line){		
			if(a){
				
				a=false;
				profiler.start();
				std::string ee=line;
				auto obj=JsonObj::parse(ee.c_str());
				profiler.stop();
				auto t=*obj["createnew"]["message"].getValue<const char*>();
				
				dbg("createnew:message: ", JsonObj::utf8Encode(t));				
			}
		});
	}
	
	if(testNum==-1 || testNum==22)
	{
		dbgW("\nTest: 22 ===========================================");

		auto obj=JsonObj::parse("{\"a\":\"b\", \"c\":\"e\"}");
		
		dbg(obj.toString());
		try{
			dbg("a: ", *(obj["a"].getValue<const char*>()));
		}
		catch(const char* msg){
			dbgW("Test should have passed... Exception: ", msg);
			pressKey();
		}
	}

	if(testNum==-1 || testNum==23)
	{
		dbgW("\nTest: 23 ===========================================");
		const char* data="{a:b,c:{d:e,f:g},h:i,j:{k:{l:m\"},\"n\":{\"o\":34}}}";
		data="{\"a\":\"b\", \"c\":{\"d\":\"e\", \"f\":\"g\"}, \"h\":\"i\", \"j\":{\"k\":{\"l\":\"m\"}, \"n\":{\"o\":34}}}";
		dbg(data);
		auto obj=JsonObj::parse(data);
		
		dbg(obj.toString());
		//*
		try{
			auto tmp=obj["j"];
			auto num=tmp["n"]["o"];
			num.getValue<int>();
			dbg("j:n:o: ", *num.getValue<int>());
		}
		catch(const char* msg){
			dbgW("Test should have passed... Exception: ", msg);
			pressKey();
		}
	}

	if(testNum==-1 || testNum==24)
	{
		dbgW("\nTest: 24 ===========================================");

		auto obj=JsonObj::parse("{\"a\":\"b\", \"c\":\"d\"}");
		
		dbg(obj.toString());
		try{
			dbg("a: ", *(obj["a"].getValue<const char*>()));
			
			obj["e"]="f";
			dbg(obj.toString());
			obj["e"]="f-2";
			dbg(obj.toString());
		}
		catch(const char* msg){
			dbgW("Test should have passed... Exception: ", msg);
			pressKey();
		}
	}

	if(testNum==-1 || testNum==25)
	{
		dbgW("\nTest: 25 ===========================================");
		
		auto obj=JsonObj::initObj();

		try{
			obj["a"]={"b", "c", "d", 1};
			obj["e"]="f";
			dbg(obj.toString());
			dbg("\nadd new item to array \"a\"");
			obj["a"].pushBack(2);
			dbg(obj.toString());
			dbg("value [a][2]: ", *obj["a"][2].getValue<const char*>());
			dbg("value [a][3]: ", *obj["a"][3].getValue<int>());
			dbg("value [a][4]: ", *obj["a"][4].getValue<int>());
			
			dbg("\nAdd new array at \"g\"");
			obj["g"].pushBack("h");
			obj["g"].pushBack("i");
			obj["g"].pushBack(3);
			obj["g"].pushBack(4);
			dbg(obj.toString());

			dbg("value [g][1]: ", *obj["g"][1].getValue<const char*>());
			dbg("value [g][2]: ", *obj["g"][2].getValue<int>());
						
			dbg("\nAdd object to array g:");
			obj["g"].pushBack({JSON_OBJ,{{"j", 5}, {"k", 6}, {"l", 8.623}}});
			dbg(obj.toString());
			dbg("\nAdd val 7 to array g");
			obj["g"].pushBack(7);
			dbg(obj.toString());
			
			dbg("\nget value [g][4][k]: ", *obj["g"][4]["k"].getValue<int>());
			dbg("get value [g][4][l]: ", *obj["g"][4]["l"].getValue<float>());

			dbg("\nAdd an array to array \"a\":");
			obj["a"].pushBack({0,1,2,3,4});
			dbg(obj.toString());
			dbg("\npushing another element into \"a\"");
			obj["a"].pushBack(23);
			dbg(obj.toString());
			
			dbg("value [a][5][3]: ", *obj["a"][5][3].getValue<int>());

			dbg("\nAdd {\"m\", JSON_NULL} to object [g][4]");
			obj["g"][4].append({"m", JSON_NULL});
			dbg(obj.toString());
		
			dbg("\nadd an object to array [\"a\"][5]");
			obj["a"][5].pushBack({JSON_OBJ,{{"ab", 5}, {"ac", 6}, {"ad", 8.623}}});
			dbg(obj.toString());
			
			dbg("replacing a:");
			obj["a"]="5";
			dbg(obj.toString());

			dbg("\nadding m");
			obj["m"]=6.5345;
			dbg(obj.toString());

			dbg("\nreplacing m by an array with objects {\"n\", 7.12345} and {n1: 7.333}");
			obj["m"]={{"n", 7.12345}, {"n1", 7.33}};
			dbg(obj.toString());

			try{
				obj["e"].pushBack(23);			
				dbg(obj.toString());
				dbgW("Test should have failed...");
				pressKey();
			}
			catch(const char* msg){
				dbgG("Test passed, exception... Exception: ", msg);
				
			}
	
			dbg("\ncreating a new array at r");
			obj["r"].pushBack("s");
			obj["r"].pushBack("tt");
			obj["r"].pushBack({"u", 5, 6, 7, 8, 9.3});
			obj["r"].pushBack({JSON_OBJ,{{"v", 5}, {"w", 6}, {"x", 8.623}}});
			obj["r"].pushBack(6);
			dbg(obj.toString());
			dbg("value at [r][2][3]: ", *obj["r"][2][3].getValue<int>());
			dbg("value at [r][3][x]: ", *obj["r"][3]["x"].getValue<float>());
		}
		catch(const char* msg){
			dbgW("Test should have passed... Exception: ", msg);
			pressKey();
		}
	}

	if(testNum==-1 || testNum==26)
	{
		dbgW("\nTest 26 ===========================================");
		auto obj=JsonObj::initObj();

		try{
			//obj["a"]={JSON_OBJ, {{"b", 1}, {"c", 2}, {"c", 3}, {"d", 3}}};//, {"b", 3}}};
			obj["a"]={JSON_OBJ, {{"b", 1}, {"c", 2}, {"c", 3}, {"d", 3}, {"e", 4}}};//, {"b", 3}}};
			dbg(obj.toString());
			dbg("Appending {\"b\", 4} to obj[\"a\"]");
			obj["a"].append({"b", 4});
			dbg(obj.toString());
			dbgW("Test should have faild");
			pressKey();
		}
		catch(const char* msg){
			dbgG("Test passed! Exception: ", msg);
		}
	}
	
	if(testNum==-1 || testNum==27)
	{
		dbgW("\n Test: 27 ===========================================");

		TestData jsonStrs[]={
	 		{"{\"a\":\"b\", \"a\":\"b\"}", false},
			{"{\"a\":\"b\" \"c\":5 \"d\":\"2\", \"e\":\"f\"\"g\":3, \"h\":2:3,,\"i\":4,}", false},
			{"{\"a\":\"b\", \"c\":\"5\"}", true},
			{"{ ,\"a\":\"b\", \"c\":\"5\"}", false},
			{"{\"a\":[,\"b\"], \"c\":\"5\"}", false},
			{"{\"a\":[\"b\",...], \"c\":\"5\"}", false},
			{"{\"a\":\"b\", [1,2], \"c\":\"5\"}", false},
			{"{\"a\":\"b\", {\"1\":2}, \"c\":\"5\"}", false},
			{"{\"a\":\"b\", \"c\":[1, [2], 2], \"d\":\"5\"}", true},
			{"{\"a\":\"b\", \"c\":[1, [2], 2e], \"d\":\"5\"}", false},
			{"{\"a\":\"b\" \"c\":\"5\"}", false},
			{"{\"a\":\"b\", \"c\":5}", true},
			{"{\"a\":\"b\", \"c\":5:3}", false},
			{"{\"a\":\"b\" \"c\":5}", false},
			{"{\"a\":\"b\", \"c\":[5, \"2\"]}", true},
			{"{\"a\":\"b\" \"c\":[5, \"2\"]}", false},
			{"{\"a\":\"b\" \"c\":[5, 4, \"2\", 3, 4, \"2\"], \"s\":3}", false},
			{"{\"a\":[1, 1, \"2\"]}", true},
			{"{\"a\":[1, 1, ,\"2\"]}", false},
	/* */	 	{"{\"a\":[1, 1, \"2\"], \"\"s\":3}", false},
			{"{\"a\":[1, 1, \"\"2\"]}", false},
				{"{\"a\":[1, 1, \"2\"], \"b\":2}", true},
			{"{\"a\":\"b\", \"c\":[1, 1, \"2\"], \"d\":2}", true},
			{"{\"a\":\"b\", \"c\":[1, 1, \"2\"], \"d\":{\"e\":2}}", true},
			{"{\"a\":\"b\", , \"c\":[1, 1, \"2\"], \"d\":{\"e\":2}}", false},
			{"{\"a\":\"b\", \"c\":[1, 1, \"2\"], \"d\":{\"e\":2}, \"e\": :}", false},
			{"{\"a\":\"b\", \"c:\"5\"}", false},
			{"{\"a\":\"b\" \"c\":5}", false},
			{"{\"a\":\"b\", \"c\":5, \"d\":\"2\", \"e\":\"f\"\"g\":3, \"h\":2:3,,\"i\":4,}", false},
			{"{\"a\":\"b\", \"c\":5, \"d\":\"2\", \"e\":\"f\":\"g\":3, \"h\":2:3,,\"i\":4,}", false},
			{"{\"a\":\"5\", \"e\":\"f\", \"g\":[\"h\", \"i\", 3, 4, {\"j\":5, \"k\":6, \"l\":8.623000, \"m\":null}, 7], \"m\":[{\"n\":7.123450}, {\"n1\":7.330000}]}", true},
			{"{}", true},
			{"{-}", false},
			{"-", false},
	/*	*/	{"", false}
		};

		for(size_t i=0; i<sizeof(jsonStrs)/sizeof(TestData); i++){	
			dbgErr("json str: ", jsonStrs[i].m_jsonStr);
			try{
				auto obj=JsonObj::parse(jsonStrs[i].m_jsonStr);
				
				if(!jsonStrs[i].m_testStatus){
					dbgW("Test should have failed...");
					pressKey();
				}
				else{
					dbgG("Test passed: JSON is valid!\n");
				}
			}	
			catch(const char* msg){
				if(jsonStrs[i].m_testStatus){
					dbgW("Test should have passed...");
					dbgW("Exception: ", msg);
					pressKey();
				}
				else{
					dbgG("Test passed: invalid JSON syntax detected: ", msg, "\n");
				}			
			}
		}
		
	}
	
	if(testNum==-1 || testNum==28)
	{
		dbgW("\n Test: 28 ===========================================");

		TestData jsonStrs[]={
		{"{ \"name\": \"John\", \"age\": 30}", true},
		{"{ \"name\": \"John\", }", false},
		{"{ \"name\": \"John\", \" d\"age\": 30}", false},
		{"{ \"name\": \"John\", \"age\": [\"30\"]}", true},
		{"{ \"name\": \"John\", \"age\": [\"30\", 0.2, 0, 1000.002, 0.2], \"e\":3}", true},
		{"{ \"name\": \"John\", \"age\": [\"30\", 0.2, 0 , 1000.002, 0.2], \"e\":3}", true},
		{"{ \"name\": \"John\", \"age\": [\"30\", 0.2, 2., 1000.002, 0.2], \"e\":3}", false},
		{"{ \"name\": \"John\", \"age\": [\"30\", .2], \"e\":3}", false},
		{"{ \"name\": \"John\", \"age\": [\"30\", 2.2.2], \"e\":3}", false},
		{"{ \"name\": \"John\", \"age\": [\"30\", 1 2 \"b\"], \"e\":3}", false},
		{"{ \"name\": \"John\", \"age\": [\"30\", 2.2 2], \"e\":3}", false},
		{"{ \"name\": \"John\", \"age\": [\"30\", 00.2], \"e\":3}", false},
		{"{ \"name\": \"John\", \"age\": [\"30\", 0a.2], \"e\":3}", false},
		{"{ \"name\": \"John\", \"age\": : 30}", false},
		{"{ name: \"John\", \"age\": \"30\"}", false},
		{"{ name: \"John\", age: \"30\"}", false},
		{"{ name: \"John\", age: 30}", false},
		{"{ \"name\": \"John\", \"age\": 30,  \"city\": \"New York\",  }", false},
		{"{  \"name\": \"John\",  \"address\": {    \"street\": \"5th Avenue\",  \"city\": \"New York\"}",  false},
		{"{ \"quote\": \"She said, \"Hello!\"\" }", false},
			{"{ \"user\": {\"name\": \"John\", \"contact\": { \"phone\": \"123-456-7890\", \"email\": \"john@example.com\" }, }, \"userDetails\": [{ \"age\": 30, \"city\": \"New York\" }]}", false},
		{"{ \"user\": {\"name\": \"John\", \"contact\": { \"phone\": \"123-456-7890\", \"email\": \"john@example.com\" }}, \"userDetails\": [{ \"age\": 30, \"city\": \"New York\" }]}", true},
		{"{ \"name\": null,  \"age\": 30, \"city\": undefined  }", false},
		{"{ \"name\": null       ,  \"age\": 30, \"city\": null  }", true},
		{"{ \"name\": null,  \"age\": 30, \"city\": null true }", false},
	 	{"{  \"message\": \"Coffee\", \"a\":\"\", \"b\":\"\"}", true},
		{"{  \"message\": \"Coffee\", \"a\": , \"b\":\"\"}", false},
		{"{  \"message\": \"Café\"}", true},
		{"{'name': 'Bob'}", false},
		{"{\"name\": \"Eve\", \"items\": [1, 2, 3}", false},
		{"{\"name\": \"Eve\", \"items\": [1, 2, 3}]", false},
		{"{\"name\": \"Eve\", \"items\": [1, 2, 3,]}", false},
		{"{\"name\": \"Eve\", \"item\":1,}", false},
		{"{\"name\": \"John\", \"age\": 30 \"city\": \"New York\"}", false},
		{"{\"name\": \"John\", \"address\": {\"street\": \"123 Main St\", \"city\": \"New York\", \"zip\": \"10001\", \"state\": }", false},
		{"{name: \"John\", \"age\": 30}", false},
		{"{ name: Alice age: 30 }", false},
	 	{"{ name: \"Alice\" age: 30 }", false},
	 	{"{ name: \"Alice\" age: \"30\" }", false},
	 	{"{}", true}
		};

		for(size_t i=0; i<sizeof(jsonStrs)/sizeof(TestData); i++){	
			dbgErr("json str: ", jsonStrs[i].m_jsonStr);
			
			auto obj=JsonObj::parse(jsonStrs[i].m_jsonStr, ErrorHandlerMode::Quiet);
			if(obj.isValid()){
				if(!jsonStrs[i].m_testStatus){
					dbgW("Test should have failed...");
					pressKey();
				}
				else{
					dbg(obj.toString());
					dbgG("OK.\n");
				}
			}	
			else{
				if(jsonStrs[i].m_testStatus){
					dbgW("Test should have passed...");
					dbgW("Exception: ", obj.getErrorMsg());
					pressKey();
				}
				else{
					dbgG("Invalid JSON syntax detected: ", obj.getErrorMsg(), ".\n");
				}			
			}
		}
	}

	if(testNum==-1 || testNum==29)
	{
		dbgW("\n Test: 29 ===========================================");
		
		const char* data="{\"a \\\"b\\\"\": \"c: \\\"v0\\\" \\\"v1\\\" \\\"v2\\\"  \"}";
		dbg("Test: ", data);

		auto obj=JsonObj::parse(data);
		checkResult(obj.toString(), data);
		dbg("value: ", *(obj["a \"b\""].getValue<const char*>()));
	}

	if(testNum==-1 || testNum==30)
	{
		dbgW("\n Test: 29 ===========================================");
		
		const char* data="{\"a\":\"b \\\"v\\\"\"}";
		dbg("Test: ", data);

		auto obj=JsonObj::parse(data);
		dbg(obj.toString());
		try{
			dbgErr("Quering key ID and description...");
			dbg("value: ", *(obj["ID"].getValue<int>()));
			dbg("value: ", *(obj["description"].getValue<std::string>()));
		}
		catch(const char* msg){
			dbgG("Test passed. Exception: ID ", msg);
			pressKey();
		}
	}

	if(testNum==-1 || testNum==31)
	{
		const char* data="{ \"name\": \"John\", \"age\": [\"30\", 0.2, 0 , 1000.002, {\"b\":23}, [10, 11, 12, 13, 14], 0.35], \"e\":3}";
		auto obj=JsonObj::parse(data);
		dbg(obj.toString());
		try{
			obj.removeKey("e");
			dbg(obj.toString());
			obj["age"].removeFromArray(0, true);
			dbg(obj.toString());
			obj["age"].removeFromArray(0, false);
			dbg(obj.toString());
			dbg("age[0]: ", *obj["age"][0].getValue<double>());
			dbg("age/3/b: ", *(obj.follow("age/3/b").getValue<int>()));
			dbg("age/4/3: ", *(obj.follow("age/4/3").getValue<int>()));
		}
		catch(const char* msg){
			dbgW("Exception: ", msg);
			pressKey();
		}
			
	}

	if(testNum==-1 || testNum==32)
	{
		auto obj=JsonObj::initObj();

		try{
			obj["a"]={"b", "c", "d", JSON_NULL, JSON_ARRAY, JSON_OBJ, true, false};
			obj["b"]={JSON_OBJ, {{"b 1", 1}, {"b 2", JSON_ARRAY}, {"b 3", true}}};
			dbg(obj.toString());
			
			auto array=obj["a"];
			array.pushBack(123);
			dbg(obj.toString());
		}
		catch(const char* msg){
			dbgW("Exception: ", msg);
			pressKey();
		}
	}		

	if(testNum==-1 || testNum==33)
	{
		auto obj=JsonObj::initObj();

		//add some keys and values

		obj["a"]="hello";

		obj["b"]=true;
		obj["c"]=123.456;
		obj["d"]=JSON_NULL; // nullptr IS NOT JSON null!

		//Arrays:
		obj["e"]=JSON_ARRAY; // empty array

		obj["f"]={1, 2, 3, "world", JSON_NULL, JSON_ARRAY, JSON_OBJ, true, false};

		//an array of objects:
		obj["g"]={{"a 1", 1}, {"a 2", 2}, {"a 3", true}, {"a 4", JSON_NULL}};

		//Objects:

		obj["h"]=JSON_OBJ;   // empty json obj

		obj["i"]={JSON_OBJ, {{"b 1", 1}, {"b 2", JSON_ARRAY}, {"b 3", true}}};

		// creating new object on the fly
		obj["j"]["c 1"]="hello";
		obj["j"]["c 2"]={JSON_OBJ, JSON_ARRAY};

		//There is more:

		auto jsonArray=obj["f"];
		jsonArray.pushBack(123.00456);
		jsonArray.pushBack({JSON_OBJ, {{"d 1", "hello"}}});		
		jsonArray.pushBack({});

		auto jsonObj=obj["i"];

		jsonObj.append({"b 4", 2});
		jsonObj["b 2"].pushBack("hi");

		jsonArray[6].append({"a b c", 2});
		obj["f"][8]=true;	
		// */
		dbg(obj.toString(true));
	}



	#endif


	/*
	// potential memory leak cases

		{"{\"a\":\"b\", \"a\":\"b\"}"
		{"{\"a\":\"b\", [1,2], \"c\":\"5\"}", false},
		{"{\"a\":\"b\", {\"1\":2}, \"c\":\"5\"}", false},
		{"{\"a\":[1, 1, ,\"2\"]}", false},
		{"{\"a\":[1, 1, \"2\"], \"\"s\":3}"
	*/
	//-----------------------------------------------------------------
	}
	catch(const char* msg){
		dbgW("Exception: ", msg, " * * * *");
	}
	
	//#################################################################

	return 0;
};
