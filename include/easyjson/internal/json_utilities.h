/*********************************************************************
* json utilities                               								*
*                                                                    *
* Date:    23-09-2025                                                *
* Author:  Dan Machado                                               *                                         *
**********************************************************************/
#ifndef JSON_UTILITIES_H
#define JSON_UTILITIES_H

#include <cstring>
#include <string>

//====================================================================

namespace easyjson
{

class JsonObj;
class JsonImpl;
class JsonParser;

enum class JSON_TYPES : unsigned char
{
	_NA,
	_STR,
	_NUM,
	_INT,
	_DOUBLE,
	_BOOL,
	_NULL,
	_JSON_ARRAY,
	_JSON_OBJ,
};


//====================================================================

#define B0 1<<7
#define B1 1<<11
#define B2 1<<16
#define B3 1<<21

enum class JsonDataObjects
{
	_NULL,
	_OBJ,
	_ARRAY,
};

template<JsonDataObjects T>
struct jsonDataObjects
{
	enum {val=int(T)};
};

typedef jsonDataObjects<JsonDataObjects::_NULL> json_null;
typedef jsonDataObjects<JsonDataObjects::_OBJ> json_obj;
typedef jsonDataObjects<JsonDataObjects::_ARRAY> json_array;

//====================================================================

template<typename T>
struct ToString
{
};

template<>
struct ToString<json_null>
{
	static constexpr JSON_TYPES modifier{JSON_TYPES::_NULL};

	static std::string toStr(json_null)
	{
		return std::string("null");
	}
};

template<>
struct ToString<json_array>
{
	static constexpr JSON_TYPES modifier{JSON_TYPES::_JSON_ARRAY};

	static std::string toStr(json_array)
	{
		return std::string("");
	}
};

template<>
struct ToString<json_obj>
{
	static constexpr JSON_TYPES modifier{JSON_TYPES::_JSON_OBJ};

	static std::string toStr(json_obj)
	{
		return std::string("");
	}
};

template<>
struct ToString<size_t>
{
	static constexpr JSON_TYPES modifier{JSON_TYPES::_NUM};

	static std::string toStr(size_t t)
	{
		return std::to_string(t);
	}
};

template<>
struct ToString<float>
{
	static constexpr JSON_TYPES modifier{JSON_TYPES::_NUM};

	static std::string toStr(float t)
	{
		return std::to_string(t);
	}
};

template<>
struct ToString<double>
{
	static constexpr JSON_TYPES modifier{JSON_TYPES::_NUM};

	static std::string toStr(double t)
	{
		return std::to_string(t);
	}
};

template<>
struct ToString<int>
{
	static constexpr JSON_TYPES modifier{JSON_TYPES::_NUM};
	
	static std::string toStr(int t)
	{
		return std::to_string(t);
	}
};

template<>
struct ToString<long>
{
	static constexpr JSON_TYPES modifier{JSON_TYPES::_NUM};
	
	static std::string toStr(long t)
	{
		return std::to_string(t);
	}
};

template<>
struct ToString<long long>
{
	static constexpr JSON_TYPES modifier{JSON_TYPES::_NUM};
	
	static std::string toStr(long long t)
	{
		return std::to_string(t);
	}
};

template<>
struct ToString<std::string>
{
	static constexpr JSON_TYPES modifier{JSON_TYPES::_STR};

	static std::string toStr(const std::string& str)
	{
		return str;
	}
};

template<>
struct ToString<const char*>
{
	static constexpr JSON_TYPES modifier{JSON_TYPES::_STR};

	static std::string toStr(const char* cstr)
	{
		return cstr;
	}
};

template<>
struct ToString<const char8_t*>
{
	static constexpr JSON_TYPES modifier{JSON_TYPES::_STR};

	static std::string toStr(const char8_t* cstr)
	{
		return reinterpret_cast<const char*>(cstr);
	}
};

template<>
struct ToString<bool>
{
	static constexpr JSON_TYPES modifier{JSON_TYPES::_BOOL};

	static std::string toStr(bool a)
	{
		if(a){
			return std::string("true");
		}
		return std::string("false");
	}
};

//====================================================================

template<typename T>
struct FromString
{
};

template<>
struct FromString<int>
{
	static int getFrom(const char* str)
	{
		return std::atoi(str);
	}
};

template<>
struct FromString<long>
{
	static long getFrom(const char* str)
	{
		return std::atol(str);
	}
};

template<>
struct FromString<long long>
{
	static long long getFrom(const char* str)
	{
		return std::atoll(str);
	}
};

template<>
struct FromString<float>
{
	static float getFrom(const char* str)
	{
		return std::atof(str);
	}
};

template<>
struct FromString<double>
{
	static double getFrom(const char* str)
	{
		return std::atof(str);
	}
};

template<>
struct FromString<const char*>
{
	static const char* getFrom(const char* str)
	{
		return str;
	}
};

template<>
struct FromString<std::string>
{
	static std::string getFrom(const char* str)
	{
		return std::string(str);
	}
};

template<>
struct FromString<bool>
{
	static bool getFrom(const char* str)
	{
		return std::strcmp(str, "true")==0;
	}
};

template<>
struct FromString<std::u8string>
{
	static std::u8string getFrom(const char* str)
	{
		return reinterpret_cast<const char8_t*>(str);
	}
};

template<>
struct FromString<const char8_t*>
{
	static const char8_t* getFrom(const char* str)
	{
		return reinterpret_cast<const char8_t*>(str);
	}
};

template<>
struct FromString<json_null>
{
	static json_null getFrom(const char* str)
	{
		return json_null();
	}
};

//====================================================================

inline int hexChar(int b)
{
	if(b<103){//f
		if(b>96){ //a
			return b-87;
		}
		if(b<71){//F
			if(b>64){//A
				return b-55;
			}
			if(b>47){//0
				if(b<58){//9
					return b-48;
				}
			}
		}
	}
	return -1;
}


//====================================================================

struct JsonValue
{
	template<typename T>
	JsonValue(T t)
	: m_val(ToString<T>::toStr(t))
	, m_modifier(ToString<T>::modifier)
	{
	}

	std::string m_val; // const char?
	const JSON_TYPES m_modifier;
};

struct JsonPair
{
	template<typename T>
	JsonPair(const char* c, T t)
	: m_val(ToString<T>::toStr(t))
	, m_key(c)
	, m_modifier(ToString<T>::modifier)
	{}

	template<typename T>
	JsonPair(std::pair<const char*, T> data)
	:JsonPair(data.first, data.second)
	{}

	template<typename T>
	JsonPair(const char8_t* c, T t)
	:JsonPair(reinterpret_cast<const char*>(c), t)
	{}

	template<typename T>
	JsonPair(std::pair<const char8_t*, T> data)
	:JsonPair(data.first, data.second)
	{}

	std::string m_val;
	const char* m_key;
	const JSON_TYPES m_modifier;
};

struct JsonBulkList
{
	JsonBulkList(jsonDataObjects<JsonDataObjects::_OBJ>, std::initializer_list<JsonPair>&& list)
	: m_list(list)
	{}

	std::initializer_list<JsonPair> m_list;
};

//====================================================================

class JsonImpl;
class JsonObj;
class JsonParser;

enum class ErrorHandlerMode
{
	Exception,
	Quiet,
};

namespace internal
{
enum class ErrorCode
{
	error0,
	error1,
	error2,
	error3,
	error4,
	error5,
	error6,
	error7,
	error8,
	error9,
	error10,
	error11,
	error12,
	error13,
	error14,
	error15,
	error16,
	error17,
	error18,
	error19,
	error20,
	error21,
	error22,
	error23,
	error24,
	error25,
	error26,
	last,
};

class ErrorReporting
{
	public:
		ErrorReporting()=delete;
		
		ErrorReporting(ErrorHandlerMode errorHandlerMode)
		: c_errorHandlerMode(errorHandlerMode)
		{
		}
		
		ErrorReporting(const ErrorReporting& other)
		:m_errorCode(other.m_errorCode)
		, c_errorHandlerMode(other.c_errorHandlerMode)
		{
		}

		const char* getErrorMsg() const
		{
			return ErrorMessages[int(m_errorCode)];
		}

		bool isValid() const
		{
			return ErrorCode::error0==m_errorCode;
		}

		void reset()
		{
			m_errorCode=ErrorCode::error0;
		}

	private:
		ErrorCode m_errorCode{ErrorCode::error0};
		const ErrorHandlerMode c_errorHandlerMode;

		static inline const char* ErrorMessages[int(ErrorCode::last)]{
		/*error0*/  "JSON is valid",
		/*error1*/  "Expecting 'string', 'number', 'null', 'true', 'false', '{', '[', got 'undefined'",
		/*error2*/  "Expecting 'string', '}', got '['",
		/*error3*/  "Expecting 'string', '}', got 'undefined'",
		/*error4*/  "Expecting 'null', 'true' or 'false', got 'undefined'",
		/*error5*/  "Expecting 'number'",
		/*error6*/  "Expecting 'EOF', '}', ',', ']', got 'undefined'",
		/*error7*/  "Invalid JSON string: orphan array",
		/*error8*/  "Expecting 'string', 'number', 'null', 'true', 'false', '{', '[', ']', got ','",
		/*error9*/  "Expecting 'string' got 'undefined'",
		/*error10*/ "Expecting 'EOF', '}', ',', ']', got ':'",
		/*error11*/ "Expecting ']' got '}'",
		/*error12*/ "Expecting '}' got ']'",
		/*error13*/ "Array object is invalid for this operation.",
		/*error14*/ "Invalid operation on Object.",
		/*error15*/ "Trying to add an element into a no array object.",
		/*error16*/ "Invalid operation on array.",
		/*error17*/ "Error: Duplicate key.",
		/*error18*/ "Out of range",
		/*error19*/ "Key has not value",
		/*error20*/ "Invalid JSON object",
		/*error21*/ "Expecting 'string', 'number', 'null', 'true', 'false', '{', '[', got 'EOF'",
		/*error22*/ "Unable to open json file. Wrong name or no permisions on the file",
		/*error23*/ "Invalid data conversion",
		/*error24*/ "Key no found",
		/*error25*/ "File is empty",
		/*error26*/ "Expecting 'EOF', got 'undefined'"
		};

		void setError(ErrorCode errorCode)
		{
			m_errorCode=ErrorCode::error0;

			if(c_errorHandlerMode==ErrorHandlerMode::Exception){
				throw ErrorMessages[int(errorCode)];
			}
			else{
				
				m_errorCode=errorCode;
			}
		}

		ErrorHandlerMode getMode() const
		{
			return c_errorHandlerMode;
		}

	friend class easyjson::JsonImpl;
	friend class easyjson::JsonObj;
	friend class easyjson::JsonParser;
};

}

inline bool isHexValid(const char* cstr)
{
	if(cstr[0]!='u'){
		return false;
	}

	int i=1;
	while(cstr[i]){
		if(hexChar(int(cstr[i]))<0){
			return false;
		}

		if(i==4){
			return true;
		}
		i++;
	}

	return false;
}

inline bool pushUTF8CodePoint(std::string& str, char ch1, char ch2, char ch3, char ch4)
{
	uint32_t val=0;
	int tmp=hexChar(int(ch1)); // 16*3
	if(tmp<0){
		return false;
	}
	val+=4096*tmp; // 16*3

	tmp=hexChar(int(ch2)); // 16^2
	if(tmp<0){
		return false;
	}
	val+=256*tmp;

	tmp=hexChar(int(ch3));
	if(tmp<0){
		return false;
	}
	val+=16*tmp;
	
	tmp=hexChar(int(ch4));
	if(tmp<0){
		return false;
	}
	val+=tmp;

	if(val<B0){
		uint8_t data0=(63 & val);
		str.push_back(data0);
		return true;
	}
	else if(val<B1){
		uint8_t data1=(63 & val) | B0;
		uint8_t data0=(63 & (val>>6) ) | 6<<5;
		str.push_back(data0);
		str.push_back(data1);
		return true;
	}
	else if(val<B2){
		uint8_t data2=(63 & val) | B0;
		uint8_t data1=(63 & (val>>6)) | B0;
		uint8_t data0=(63 & (val>>12) ) | 14<<4;
		str.push_back(data0);
		str.push_back(data1);
		str.push_back(data2);
		return true;
	}
	else if(val<B3){
		uint8_t data3=(63 & val) | B0;
		uint8_t data2=(63 & (val>>6)) | B0;
		uint8_t data1=(63 & (val>>12) ) | B0;
		uint8_t data0=(63 & (val>>18) ) | 30<<3;
		str.push_back(data0);
		str.push_back(data1);
		str.push_back(data2);
		str.push_back(data3);
		return true;
	}
	return false;
}

}

#endif
