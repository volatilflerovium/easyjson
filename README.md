# easyjson
A C++ JSON library for parsing and writing
json strings. 

## Motivation

While developing km_recorderAndPlayer I used a simple naive serialization/deserialization
class for saving data to files. However, I thought replacing it with
one of the most popular json libraries (nlohmann json). The performance of nlohmann json
is really poor compared to the solution used in the km_recorderAndPlayer project;
so I stuck with the naive approach. Later I remembered an old unfinished idea
that I worked on while develop my project fpdf-easytable, which involved
parsing nested css-style tags. Implementing this idea for parsing json strings
gave rise to easyjson.

Despite the main algorithm is simple and with few optimization tricks,
easyjson run, in most of the cases, as faster as RapidJson.

## Features:

- Parse and generate JSON string according to the standard http://json.org/
  Nothing more nothing less.

- Read and write JSON strings in UTF-8. The consensus is increasingly moving 
  toward using UTF-8. Here two libraries for utf8 validation:

   https://github.com/simdutf/is_utf8

   https://github.com/nemtrif/utfcpp

- The generated JSON string can be compact or prettified 

- Errors are handled by throwing exception (default) or error reporting.

- Safe and explicit casts
```
	std::optional<T> value=obj["some_key"].getValue<T>();

	//or with more flexibility:

	const char* value=obj["some_key"].getRawData();
```
- Internally easyjson uses an AVL tree structure therefore lookup is done
  in O(ln N) time.

- Easy to use.


## Example
```
// Json string input:

auto obj=JsonObj::parse(c_json_string);

// Json file input:

auto obj=JsonParser::openJsonFile("some_file.json");

std::string jsonStr="{\"createnew\":{\"message\":\"\u054d\u054f\u0535\u0542\u053e\u0535\u053c \u0546\u0548\u0550\"},\"explanationofflinedisabled\":{\"message\":\"Google \u0553\u0561\u057d\u057f\u0561\u0569\u0572\u0569\u0565\u0580\u0568 \u0576\u0561\u0587 \u0561\u0576\u0581\u0561\u0576\u0581 \u057c\u0565\u056a\u056b\u0574\u0578\u0582\u0574 \u0585\u0563\u057f\u0561\u0563\u0578\u0580\u056e\u0565\u056c\u0578\u0582 \u0570\u0561\u0574\u0561\u0580 \u0574\u056b\u0561\u0581\u0565\u0584 \u0570\u0561\u0574\u0561\u0581\u0561\u0576\u0581\u056b\u0576, \u0562\u0561\u0581\u0565\u0584 \u056e\u0561\u057c\u0561\u0575\u0578\u0582\u0569\u0575\u0561\u0576 \u0563\u056c\u056d\u0561\u057e\u0578\u0580 \u0567\u057b\u0568, \u0561\u0576\u0581\u0565\u0584 \u056f\u0561\u0580\u0563\u0561\u057e\u0578\u0580\u0578\u0582\u0574\u0576\u0565\u0580 \u0587 \u0574\u056b\u0561\u0581\u0580\u0565\u0584 \u0561\u0576\u0581\u0561\u0576\u0581 \u0570\u0561\u0574\u0561\u056a\u0561\u0574\u0561\u0581\u0578\u0582\u0574\u0568:\"},\"explanationofflineenabled\":{\"message\":\"\u0534\u0578\u0582\u0584 \u0574\u056b\u0561\u0581\u0561\u056e \u0579\u0565\u0584 \u0570\u0561\u0574\u0561\u0581\u0561\u0576\u0581\u056b\u0576, \u057d\u0561\u056f\u0561\u0575\u0576 \u0564\u0561\u0580\u0571\u0575\u0561\u056c \u056f\u0561\u0580\u0578\u0572 \u0565\u0584 \u0583\u0578\u0583\u0578\u056d\u0565\u056c \u0561\u057c\u056f\u0561 \u0586\u0561\u0575\u056c\u0565\u0580\u0568 \u056f\u0561\u0574 \u057d\u057f\u0565\u0572\u056e\u0565\u056c \u0576\u0578\u0580 \u0583\u0561\u057d\u057f\u0561\u0569\u0572\u0569\u0565\u0580:\"},\"extdesc\":{\"message\":\"\u0553\u0578\u0583\u0578\u056d\u0565\u0584, \u057d\u057f\u0565\u0572\u056e\u0565\u0584 \u0587 \u0564\u056b\u057f\u0565\u0584 \u0571\u0565\u0580 \u0583\u0561\u057d\u057f\u0561\u0569\u0572\u0569\u0565\u0580\u0568, \u0561\u0572\u0575\u0578\u0582\u057d\u0561\u056f\u0576\u0565\u0580\u0576 \u0578\u0582 \u0576\u0565\u0580\u056f\u0561\u0575\u0561\u0581\u0578\u0582\u0574\u0576\u0565\u0580\u0568 \u0561\u057c\u0561\u0576\u0581 \u056b\u0576\u057f\u0565\u0580\u0576\u0565\u057f \u056f\u0561\u057a\u056b:\"},\"extname\":{\"message\":\"Google Docs Offline\"},\"learnmore\":{\"message\":\"\u0544\u0561\u0576\u0580\u0561\u0574\u0561\u057d\u0576\"},\"popuphelptext\":{\"message\":\"\u054d\u057f\u0565\u0572\u056e\u0565\u0584 \u0583\u0561\u057d\u057f\u0561\u0569\u0572\u0569\u0565\u0580 \u0587 \u056d\u0574\u0562\u0561\u0563\u0580\u0565\u0584 \u0564\u0580\u0561\u0576\u0584 \u0563\u0578\u0580\u056e\u0568\u0576\u056f\u0565\u0580\u0576\u0565\u0580\u056b \u0570\u0565\u057f \u0574\u056b\u0561\u057d\u056b\u0576\u055d \u0578\u0580\u057f\u0565\u0572 \u0567\u056c \u0578\u0580 \u056c\u056b\u0576\u0565\u0584, \u0561\u0576\u0563\u0561\u0574 \u0561\u057c\u0561\u0576\u0581 \u0581\u0561\u0576\u0581\u056b\u0576 \u0574\u056b\u0561\u0576\u0561\u056c\u0578\u0582\u0589\"}}";
		
auto obj=JsonObj::parse(jsonStr.c_str());

auto message=*obj["createnew"]["message"].getValue<std::u8string>();

std::cout<<"createnew:message: "<<reinterpret_cast<const char*>(message.c_str())<<std::endl;

// Output:

createnew:message: ՍՏԵՂԾԵԼ ՆՈՐ
```
## Quickstart

```
// Create an empty object

auto obj=JsonObj::initObj();

// Add some keys/values

obj["a"]="hello";

obj["b"]=true;

obj["c"]=123.456;

obj["d"]=JSON_NULL;

// Add some arrays

obj["e"]=JSON_ARRAY; // empty array

obj["f"]={1, 2, 3, "world", JSON_NULL, JSON_ARRAY, JSON_OBJ, true, false};

// an array of objects:
obj["g"]={{"a 1", 1}, {"a 2", 2}, {"a 3", true}, {"a 4", JSON_NULL}};

// Add some JSON objects:

obj["h"]=JSON_OBJ;   // empty json obj

obj["i"]={JSON_OBJ, {{"b 1", 1}, {"b 2", JSON_ARRAY}, {"b 3", true}}};

// Creating a JSON object on the fly

obj["j"]["c 1"]="hello";
obj["j"]["c 2"]={JSON_OBJ, JSON_ARRAY};

// There is more:

auto jsonArray=obj["f"];
jsonArray.pushBack(123.00456);
jsonArray.pushBack({JSON_OBJ, {{"d 1", "hello"}}});		
jsonArray.pushBack({});

auto jsonObj=obj["i"];

jsonObj.append({"b 4", 2});
jsonObj["b 2"].pushBack("hi");

jsonArray[6].append({"a b c", 2});
obj["f"][8]=true;	

dbg(obj.toString(true));
```

## easyjson vs Others

How does SimpleJson compare to 3 of the most popular C++ Json Lib:

### Code lines:

- easyjson:   3494 ( 7 files)
- rapijson  +16000 (38 files)
- sajson      2606 ( 1 file)
- nlohmann: +30000 ( 1 file)

### Size of simple executable:

- easyjson:  47K
- rapijson   57K
- sajson:    57K
- nlohmann: 103K

### Speed

[![single-burst.png](https://i.postimg.cc/hvJQ0hT6/single-burst.png)](https://postimg.cc/5QVt44PS)

[![1000-iterations.png](https://i.postimg.cc/WzWLsbHH/1000-iterations.png)](https://postimg.cc/5QCk311B)

**Notice that in both cases the scale is logarithmic.


