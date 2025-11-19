/*********************************************************************
* class JsonObj                                       					*
*                                                                    *
* Date:    14-06-2025                                                *
* Author:  Dan Machado                                               *                                         *
**********************************************************************/
#ifndef _EASYJSON_H
#define _EASYJSON_H

#include <optional>
#include <fstream>
#include <initializer_list>

#include "easyjson/internal/json_utilities.h"

//====================================================================

namespace easyjson
{

#define JSON_NULL json_null()
#define JSON_OBJ json_obj()
#define JSON_ARRAY json_array()

class JsonImpl;

//====================================================================

class JsonObj
{
	public:
		~JsonObj();
		
		static JsonObj parse(const char* str, ErrorHandlerMode mode=ErrorHandlerMode::Exception);
		
		static JsonObj initObj(ErrorHandlerMode mode=ErrorHandlerMode::Exception);
		
		static JsonObj parse(const char8_t* str, ErrorHandlerMode mode=ErrorHandlerMode::Exception)
		{
			return parse(reinterpret_cast<const char*>(str), mode);
		}

		static JsonObj parseJsonFile(const char* jsonFileName, ErrorHandlerMode mode=ErrorHandlerMode::Exception);
		
		static std::string utf8Encode(const char* cstr);

		bool isValid() const;

		std::string getErrorMsg() const;

		bool hasKey(const char* key) const;

		const JsonObj operator[](const char* key) const;// __attribute__((always_inline)) __attribute__((hot));

		JsonObj operator[](const char* key);// __attribute__((always_inline)) __attribute__((hot));

		bool hasKey(const char8_t* key) const __attribute__((always_inline)) __attribute__((hot))
		{
			return hasKey(reinterpret_cast<const char*>(key));
		}

		const JsonObj operator[](const char8_t* key) const __attribute__((always_inline)) __attribute__((hot))
		{
			return operator[](reinterpret_cast<const char*>(key));
		}

		JsonObj operator[](const char8_t* key) __attribute__((always_inline)) __attribute__((hot))
		{
			return operator[](reinterpret_cast<const char*>(key));
		}

		//to query arrays
		const JsonObj operator[](int idx) const;// __attribute__((always_inline)) __attribute__((hot));	

		JsonObj operator[](int idx);// __attribute__((always_inline)) __attribute__((hot));

		JsonObj follow(const char* str);
		
		void operator=(json_obj);

		void operator=(json_array);

		void operator=(easyjson::JsonValue&& val);

		void operator=(std::initializer_list<easyjson::JsonValue>&& list);

		void operator=(std::initializer_list<easyjson::JsonPair>&& list);

		void operator=(easyjson::JsonBulkList&& data);

		//for arrays
		void pushBack(easyjson::JsonValue&& data);
		void pushBack(std::initializer_list<easyjson::JsonValue>&& list);
		void pushBack(easyjson::JsonBulkList&& data);
		void pushBack(json_obj);
		void pushBack(json_array);
		
		//for obj
		void append(easyjson::JsonPair&& data);

		const char* getRawData() const;

		template<typename T>
		std::optional<T> getValue() const
		{
			const char* data=getRawData();
			if(!data){
				return std::nullopt;
			}

			return FromString<T>::getFrom(data);
		}

		void removeKey(const char* key);
		
		void removeKey(const char8_t* key)
		{
			removeKey(reinterpret_cast<const char*>(key));
		}
		
		void removeFromArray(size_t idx, bool shift=false);

		bool isObj() const;

		bool isArray() const;

		bool isValue() const
		{
			return !(isArray() || isObj());
		}
		
		bool isNull() const;
		
		bool isString() const;
		bool isBoolean() const;
		bool isNumeric() const;

		size_t size() const;
		
		std::string toString(bool prettyStr=false) const;

	private:
		
		JsonImpl* m_impl{nullptr};

		JsonObj(JsonObj&& other) __attribute__((always_inline))
		: m_impl(other.m_impl)
		{
			other.m_impl=nullptr;
		}

		JsonObj(JsonImpl* impl) __attribute__((always_inline))
		: m_impl(impl)
		{
		}

		JsonObj(const JsonObj& other)=delete;
		JsonObj& operator=(const JsonObj&)=delete;
};

template<>
inline std::optional<json_null> JsonObj::getValue<json_null>() const [[maybe_unused]]
{
	const char* data=getRawData();

	if(!data || !isNull()){
		return std::nullopt;
	}

	return FromString<json_null>::getFrom(data);
}

}// easyjson namespace

//====================================================================

#endif
