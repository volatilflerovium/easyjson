/*********************************************************************
* Template class MemPool                               					*
*                                                                    *
* Version: 1.0                                                       *
* Date:    23-08-2025                                                *
* Author:  Dan Machado                                               *                                         *
**********************************************************************/
#ifndef EASYJSON_ONE_FILE
#define EASYJSON_ONE_FILE

#include <vector>
#include <mutex>
#include <cassert>
#include <string>
#include <cstring>
#include <functional>

#include <fstream>
#include <optional>
#include <initializer_list>

#include "easyjson/internal/custom_allocator.h"


#include "utilities/debug_utils.h"


namespace easyjson
{

//====================================================================

enum class JSON_TYPES : unsigned char
{
	_NA,
	_STR,
	_NUM,
	_INT,
	_DOUBLE,
	_BOOL,
	_NULL,
};

//====================================================================

namespace 
{
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
}

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

	friend class easyjson::JsonObj;
	friend class easyjson::JsonParser;
};

//====================================================================

class JsonObjBuffer final
{
	public:
		~JsonObjBuffer()=default;

		const char* getDataAt(size_t offset) const
		{
			return m_buffer.data()+offset;
		}

		size_t addData(const char* data, size_t inOffset=0);

		size_t getLengthAt(size_t offset) const
		{
			return std::strlen(m_buffer.data()+offset);
		}

		int compare(size_t leftOffset, size_t rightOffset) const __attribute__((always_inline))
		{
			//return std::strcmp(m_buffer.data()+leftOffset, m_buffer.data()+rightOffset);

			const char* data2 = m_buffer.data();
			return std::strcmp(data2+leftOffset, data2+rightOffset);
		}

		int comparing(const char* val, size_t offset) const __attribute__((always_inline))
		{
			return std::strcmp(val, m_buffer.data()+offset);
		}

		size_t bufferSize() const
		{
			return m_position;
		}

	private:
		std::string m_buffer;
		size_t m_position{0};

		JsonObjBuffer(const char* str)
		{
			m_buffer=str;
			m_position=m_buffer.length();
		}

		JsonObjBuffer()
		: JsonObjBuffer(" ")
		{}

		void resizeBuffer(size_t bufferSize)
		{
			m_buffer.resize(bufferSize);
		}

	friend class easyjson::JsonObj;
	friend class easyjson::JsonParser;
};

//====================================================================

class NodeJson
{
	enum NodeMode : unsigned char
	{
		None= 0,
		Key=	1<<0,
		Array=1<<1,
		Obj=	1<<2,
	};

	public:
		NodeJson()=default;

		~NodeJson();

		size_t getOffset() const
		{
			return m_offset;
		}

		void setOffset(size_t offset)
		{
			m_offset=offset;
		}
		
		void setData(size_t offset, JSON_TYPES dataMode)
		{
			m_offset=offset;
			m_dataMode=dataMode;
		}

		void setData(JsonObjBuffer& jsonBufferRef, const char* data, JSON_TYPES dataMode);

		JSON_TYPES getDataMode() const
		{
			return m_dataMode;
		}

		void setDataMode(JSON_TYPES dataMode)
		{
			m_dataMode=dataMode;
		}

		bool isDangling() const
		{
			return !isKeyObjArr() && (m_offset==0);
		}

		bool isKey() const __attribute__((always_inline))
		{
			return m_mode==NodeMode::Key;
		}
		
		/*void setAsKey() __attribute__((always_inline)) no needed
		{
			m_mode=NodeMode::Key;
			m_dataMode=JSON_TYPES::_STR;
		}*/

		bool isObj() const __attribute__((always_inline))
		{
			return m_mode==NodeMode::Obj;
		}

		void setAsObj() __attribute__((always_inline))
		{
			m_mode=NodeMode::Obj;
			m_dataMode=JSON_TYPES::_NA;
		}

		void setNone() __attribute__((always_inline))
		{
			m_mode=NodeMode::None;
			m_dataMode=JSON_TYPES::_NA;
		}

		bool isArray() const __attribute__((always_inline))
		{
			return m_mode==NodeMode::Array;
		}

		void setAsArray() __attribute__((always_inline));
		NodeJson* addArrayItem() __attribute__((always_inline));

		bool isKeyObjArr() const
		{
			return (m_mode & NodeMode::Key) | (m_mode & NodeMode::Obj) | (m_mode & NodeMode::Array);
		}

		bool hasData() const
		{
			return m_offset!=0;
		}
		
		bool isNumeric() const
		{
			return m_dataMode==JSON_TYPES::_NUM || m_dataMode==JSON_TYPES::_INT || m_dataMode==JSON_TYPES::_DOUBLE;
		}
		
		bool isString() const
		{
			return m_dataMode==JSON_TYPES::_STR;
		}
		
		bool isBoolean() const
		{
			return m_dataMode==JSON_TYPES::_BOOL;
		}
		
		bool isNull() const
		{
			return m_dataMode==JSON_TYPES::_NULL;
		}

		bool isDouble() const
		{
			return m_dataMode==JSON_TYPES::_DOUBLE;
		}

		NodeJson* addChild() __attribute__((always_inline));
		NodeJson* addBlankChild() __attribute__((always_inline));
		NodeJson* addKeyNode() __attribute__((always_inline));
		NodeJson* addKeyNode(JsonObjBuffer& jsonBufferRef, const char* data) __attribute__((always_inline));

		// Notice that key nodes are never clear
		void clear();
		void clearArray();

		void print(const JsonObjBuffer& jsonBufferRef, std::string& str, bool pretty) const;

		static NodeJson* allocateNode() __attribute__((always_inline)) __attribute__((hot))
		{
			return s_allocator.construct();
		}

		static void freeNode(NodeJson* node) __attribute__((always_inline))// no dealocating the root... because it is not being called from JObj
		{
			s_allocator.freeMem(node);
		}

	private:
		static inline Allocator::Custom_Allocator<NodeJson> s_allocator;
		
		NodeJson* m_left{nullptr};  //for avl tree structure
		NodeJson* m_right{nullptr}; //for avl tree structure
		NodeJson* m_child{nullptr};

		uint32_t m_offset{0};
		int16_t m_height{0}; //for avl tree structure
		JSON_TYPES m_dataMode{JSON_TYPES::_STR};
		NodeMode m_mode{NodeMode::Key};

		class AVL_Tree
		{
			public:
				AVL_Tree(JsonObjBuffer& jsonBuffer)
				: m_jsonBufferRef(jsonBuffer)
				{
				}

				~AVL_Tree()=default;

				bool insertAt(NodeJson* obj, NodeJson* node) __attribute__((always_inline));
				NodeJson* find(NodeJson* root, const char* key) __attribute__((always_inline)) __attribute__((hot));
				void remove(const char* key, NodeJson* obj);

				static NodeJson* find(const JsonObjBuffer& jsonBufferRef, NodeJson* obj, const char* key) __attribute__((always_inline)) __attribute__((hot));

			private:
				JsonObjBuffer& m_jsonBufferRef;
				NodeJson* m_root{nullptr};

				enum LR{LEFT, RIGHT};
				NodeJson* NodeJson::* branch[2]={&NodeJson::m_left, &NodeJson::m_right};

				void balance(NodeJson* node, int diff, NodeJson* top) __attribute__((hot));
				int16_t insert(NodeJson* root, NodeJson* node, NodeJson* top);

				bool removeNode(const char* key, NodeJson* node, NodeJson* top);
				void removeNode(NodeJson* node, NodeJson* top);
		};

		//----------- start AVL functionality -----------------

		int diff();
		void bridge(NodeJson* tmp, NodeJson* top);
		int getLHeight()const;
		int getRHeight() const;
		void updateHeight();

		class VectWrapper
		{
			public:
				VectWrapper() __attribute__((always_inline))
				{
					m_container.reserve(1);
				}

				virtual ~VectWrapper()
				{
					clear();
				}

				void push_back(NodeJson* node) __attribute__((always_inline))
				{
					m_container.emplace_back(node);
				}

				size_t size() __attribute__((always_inline))
				{
					return m_container.size();
				}
				
				void removeLast() __attribute__((always_inline))
				{
					if(m_container.size()>0){
						NodeJson::freeNode(*(--m_container.end()));
						m_container.pop_back();
					}
				}
				
				void remove(size_t idx, bool shift)
				{
					if(idx<m_container.size()){
						if(idx==m_container.size()-1){
							removeLast();
							return;
						}

						NodeJson::freeNode(m_container[idx]);						
						if(!shift){
							size_t last=m_container.size()-1;	
							m_container[idx]=m_container[last];
						}
						else{
							for(size_t i=idx+1; i<m_container.size(); i++){
								m_container[i-1]=m_container[i];
							}
						}
						m_container.pop_back();
					}
				}

				NodeJson* get(size_t idx) __attribute__((always_inline))
				{
					return m_container[idx];
				}
				
				NodeJson* getLast() __attribute__((always_inline))
				{
					if(m_container.size()>0){
						return get(m_container.size()-1);
					}
					return nullptr;
				}

				void clear()
				{
					size_t sz=size();
					for(size_t i=0; i<sz; i++){				
						NodeJson::freeNode(m_container[i]);
					}
					m_container.clear();
				}

				template<typename FUNC>
				void loop(FUNC cbk)
				{
					size_t sz=size();
					for(size_t i=0; i<sz; i++){				
						cbk(i, m_container[i]);
					}
				}

				void loop2(std::function<bool(size_t, const NodeJson*)> cbk) const
				{
					for(size_t i=0; i<m_container.size(); i++){			
						if(!cbk(i, m_container[i])){
							break;
						}
					}
				}

			private:
				std::vector<NodeJson*, Allocator::Custom_Allocator<NodeJson*>> m_container;
		};

		static inline Allocator::Custom_Allocator<VectWrapper> s_vectPool;

		void printMore(const JsonObjBuffer& jsonBufferRef, const std::string& spacer, const int padding, std::string& str, int indentation) const;
		void prettify(const JsonObjBuffer& jsonBufferRef, std::string& str, int indentation, const std::string& spacer, const int padding) const;

	friend class easyjson::JsonObj;
	friend class easyjson::JsonParser;
};

//====================================================================

inline void NodeJson::setData(JsonObjBuffer& jsonBufferRef, const char* data, JSON_TYPES dataMode)
{
	size_t offset=jsonBufferRef.addData(data, m_offset);
	setData(offset, dataMode);
}

//--------------------------------------------------------------------

inline NodeJson* NodeJson::addChild()
{
	m_child=allocateNode();
	return m_child;
}

//--------------------------------------------------------------------

inline NodeJson* NodeJson::addBlankChild()
{
	m_child=allocateNode();
	m_child->m_mode=NodeMode::None;
	m_child->m_dataMode=JSON_TYPES::_NA;
	
	return m_child;
}

//--------------------------------------------------------------------

inline NodeJson* NodeJson::addKeyNode()
{
	return allocateNode();
}

//--------------------------------------------------------------------

inline NodeJson* NodeJson::addKeyNode(JsonObjBuffer& jsonBufferRef, const char* data)
{
	NodeJson* keyNode=allocateNode();
	//keyNode->m_mode=NodeMode::Key;
	//keyNode->m_dataMode=JSON_TYPES::_STR;
	keyNode->m_offset=jsonBufferRef.addData(data);
	
	return keyNode;
}
//--------------------------------------------------------------------

inline void NodeJson::clear()
{
	if(m_child){
		if(isArray()){
			VectWrapper* vect=reinterpret_cast<VectWrapper*>(m_child);
			s_vectPool.freeMem(vect);
			//delete vect;
		}
		else{
			NodeJson::freeNode(m_child);
		}
		m_child=nullptr;
	}

	m_dataMode=JSON_TYPES::_NA;
	m_offset=0;
	setNone();
}

//--------------------------------------------------------------------

// Responsibility of caller to check if the node is array
inline void NodeJson::clearArray()
{
	if(m_child){
		VectWrapper* triePtr=reinterpret_cast<VectWrapper*>(m_child);
		triePtr->clear();
	}
}

//--------------------------------------------------------------------

inline void NodeJson::setAsArray()
{
	//profiler2.start();
	m_mode=NodeMode::Array;
	m_dataMode=JSON_TYPES::_NA;

	//VectWrapper* triePtr=s_vectPool.allocateNode();
	//VectWrapper* triePtr=new VectWrapper;

	/*
	 * reinterpret_cast nooooooooooooooooooooooooooooooooooooooooo!
	 * 
	 * It is very dangerous! So dangerous that at the moment this code runs, 
	 * your hair will start falling and you will suffer 10 days of diarrhoea... 
	 * And to make things worse Tutankhamun will be back to life and Genghis Khan too...
	 * 
	 * */
	//m_child=reinterpret_cast<NodeJson*>(s_vectPool.allocate(1));
	m_child=reinterpret_cast<NodeJson*>(s_vectPool.construct());
	//profiler2.stop();
	//static int p=0;
	//dbg("array count: ", ++p);
}

//--------------------------------------------------------------------

// Responsibility of caller to check if the node is array
inline NodeJson* NodeJson::addArrayItem()
{
	/*
	 * Oh no reinterpret_cast again... Another 10 days with diarrhoea!
	 * 
	 * */ 
	//VectWrapper* vect=reinterpret_cast<VectWrapper*>(m_child);
	VectWrapper& vect=reinterpret_cast<VectWrapper&>(*m_child);
	NodeJson& node=*NodeJson::allocateNode();
	node.setNone();
	vect.push_back(&node);
	return &node;
}

//--------------------------------------------------------------------

inline void NodeJson::print(const JsonObjBuffer& jsonBufferRef, std::string& str, bool pretty) const
{
	std::string spacer="";
	int padding=0;
	if(pretty){
		spacer="\n";
		padding=3;
	}
	prettify(jsonBufferRef, str, 0, spacer, padding);
}

//--------------------------------------------------------------------

inline int NodeJson::diff()
{
	int a=0;
	if(m_left){
		a=-1*(m_left->m_height+1);
	}
	if(m_right){
		a=a+(m_right->m_height+1);
	}
	return a;
}

//--------------------------------------------------------------------

inline void NodeJson::bridge(NodeJson* tmp, NodeJson* top)
{
	if(!top){
		return;
	}

	if(top->m_right==this){
		top->m_right=tmp;
		return;
	}
	top->m_left=tmp;
}

//--------------------------------------------------------------------

inline int NodeJson::getLHeight()const
{
	if(m_left){
		return m_left->m_height;
	}
	return 0;
}

//--------------------------------------------------------------------

inline int NodeJson::getRHeight() const
{
	if(m_right){
		return m_right->m_height;
	}
	return 0;
}

//--------------------------------------------------------------------

inline void NodeJson::updateHeight()
{
	if(m_left || m_right){
		m_height=std::max(getLHeight(), getRHeight())+1;	
	}
	else{
		m_height=0;
	}
}

//--------------------------------------------------------------------

inline bool NodeJson::AVL_Tree::insertAt(NodeJson* obj, NodeJson* node)
{
	m_root=obj->m_child;

	if(!m_root){
		obj->m_child=node;
		return true;
	}

	int x=insert(m_root, node, nullptr);
	obj->m_child=m_root;
	return x!=-1;
}

//--------------------------------------------------------------------

inline NodeJson* NodeJson::AVL_Tree::find(NodeJson* obj, const char* key)
{
	NodeJson* node=obj->m_child;
	int y;
	while(node){
		y=m_jsonBufferRef.comparing(key, node->m_offset);
		if(y<0){
			node=node->m_left;
		}
		else if(y>0){
			node=node->m_right;
		}
		else{
			break;
		}
	}
	return node;
}

//--------------------------------------------------------------------

inline NodeJson* NodeJson::AVL_Tree::find(const JsonObjBuffer& jsonBufferRef, NodeJson* obj, const char* key)
{
	NodeJson* node=obj->m_child;
	int y;
	while(node){
		y=jsonBufferRef.comparing(key, node->m_offset);
		if(y<0){
			node=node->m_left;
		}
		else if(y>0){
			node=node->m_right;
		}
		else{
			break;
		}
	}
	return node;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//--------------------------------------------------------------------
//--------------------------------------------------------------------

inline size_t JsonObjBuffer::addData(const char* data, size_t inOffset)
{
	size_t offset=inOffset;
	if(offset>0){
		size_t lengthOld=getLengthAt(offset);
		size_t lengthNew=std::strlen(data);
		if(lengthOld<=lengthNew){
			std::memset(&m_buffer[offset], 0, lengthOld*sizeof(char));
			std::memcpy(&m_buffer[offset], data, lengthNew*sizeof(char));
			return offset;
		}
	}
	
	m_buffer+=" "; // optimise here reserving 128 characters
	m_buffer[m_position]=0;
	offset=m_position+1;
	m_buffer+=data;
	m_position=m_buffer.length();
	return offset;
}

//--------------------------------------------------------------------

NodeJson::~NodeJson()
{
	if(m_left){
		NodeJson::freeNode(m_left);
		m_left=nullptr;
	}

	if(m_right){
		NodeJson::freeNode(m_right);
		m_right=nullptr;
	}
	
	if(isArray()){
		VectWrapper* vectPtr=reinterpret_cast<VectWrapper*>(m_child);
		s_vectPool.freeMem(vectPtr);
		m_child=nullptr;
	}
	else if(m_child){
		NodeJson::freeNode(m_child);
	}

	m_child=nullptr;
}

//--------------------------------------------------------------------

inline void unescape(std::string& str, size_t initialPos)
{
		size_t pos=initialPos;
		while(pos!=std::string::npos){
			pos=str.find_first_of('"', pos);
			if(pos!=std::string::npos){
				str.insert(pos, "\\");
				pos+=2;
			}
		}
}

inline void NodeJson::printMore(const JsonObjBuffer& jsonBufferRef, const std::string& spacer, const int padding, std::string& str, int indentation) const
{
	str+=std::string(indentation, ' ')+"\"";
	size_t p=str.length();
	str+=jsonBufferRef.getDataAt(m_offset);
	unescape(str, p);
	str+="\"";
	str+=": ";
	if(m_child){
		m_child->prettify(jsonBufferRef, str, indentation, spacer, padding);	
	}
};

inline void NodeJson::prettify(const JsonObjBuffer& jsonBufferRef, std::string& str, int indentation, const std::string& spacer, const int padding) const
{
	if(!isArray() && !isObj()){
		if(isKey()){
			if(m_left){
				m_left->printMore(jsonBufferRef, spacer, padding, str, indentation);
				str+=", "+spacer;
			}
			printMore(jsonBufferRef, spacer, padding, str, indentation);
			if(m_right){
				str+=", "+spacer;
				m_right->printMore(jsonBufferRef, spacer, padding, str, indentation);
			}
		}
		else{
			if(m_offset>0){
				if(m_dataMode==JSON_TYPES::_STR){
					str+="\"";
				}
				size_t p=str.length();
				str+=jsonBufferRef.getDataAt(m_offset);
				unescape(str, p);
				if(m_dataMode==JSON_TYPES::_STR){
					str+="\"";
				}
			}
			else{
				throw "Incompleted...";
			}
		}
	}
	else if(isObj()){
		if(m_child){
			str+="{"+spacer;
			if(m_child->m_left){
				m_child->m_left->prettify(jsonBufferRef, str, indentation+padding, spacer, padding);
				str+=", "+spacer;
			}
			m_child->printMore(jsonBufferRef, spacer, padding, str, indentation+padding);
			if(m_child->m_right){
				str+=", "+spacer;
				m_child->m_right->prettify(jsonBufferRef, str, indentation+padding, spacer, padding);
			}
			str+=spacer+std::string(indentation, ' ')+"}";
		}
		else{
			str+="{}";
		}
	}
	else if(isArray()){
		if(m_child){
			VectWrapper* triePtr=reinterpret_cast<VectWrapper*>(m_child);
			if(triePtr->size()>0){
				str+="["+spacer;
				triePtr->loop([&str, jsonBufferRef, &spacer, padding, indentation](size_t i, NodeJson* node){
					if(i==0)
					{
						str+=std::string(indentation+padding, ' ');
					}
					else {
						str+=", "+spacer+std::string(indentation+padding, ' ');
					}
					node->prettify(jsonBufferRef, str, indentation+padding, spacer, padding);
				});
				str+=spacer+std::string(indentation, ' ')+"]";
			}
			else{
				str+="[]";
			}
		}
		else{
			str+="[]";
		}
	}
}



//--------------------------------------------------------------------

inline void NodeJson::AVL_Tree::balance(NodeJson* node, int diff, NodeJson* top)
{
	int lr=LR::LEFT;
	int clr=LR::RIGHT;
	int ft=1;
	
	if(diff<0){
		lr=LR::RIGHT;
		clr=LR::LEFT;
		ft=-1;
	}

	NodeJson* tmp=node->*branch[clr];
	NodeJson* tmp2=tmp->*branch[lr];

	bool isRoot=(m_root==node);

	if(!tmp2){
		node->bridge(tmp, top);
		node->*branch[clr]=nullptr;
		tmp->*branch[lr]=node;	
		node->m_height-=2;
		tmp2=tmp;
	}
	else{
		if(!(tmp->*branch[clr])){
			node->bridge(tmp2, top);
			
			tmp2->*branch[lr]=node;
			tmp2->*branch[clr]=tmp;
			
			tmp->*branch[lr]=nullptr;
			node->*branch[clr]=nullptr;
			node->m_height=0;
			tmp->m_height=0;
			tmp2->m_height=1;
		}
		else if(ft*tmp->diff()>0){
			node->bridge(tmp, top);

			tmp->*branch[lr]=node;
			node->*branch[clr]=tmp2;

			node->updateHeight();
			tmp->updateHeight();
			tmp2=tmp;
		}
		else{
			node->bridge(tmp2, top);
			node->*branch[clr]=tmp2->*branch[lr];

			tmp2->*branch[lr]=node;
			tmp->*branch[lr]=tmp2->*branch[clr];
			tmp2->*branch[clr]=tmp;

			node->updateHeight();
			tmp->updateHeight();
			tmp2->updateHeight();
		}
	}
	
	if(isRoot){
		m_root=tmp2;
	}
}

//--------------------------------------------------------------------

inline int16_t NodeJson::AVL_Tree::insert(NodeJson* root, NodeJson* node, NodeJson* top)
{
	int y=m_jsonBufferRef.compare(node->m_offset, root->m_offset);

	int16_t x=1;
	//int br=0;
	if(y<0){	
		if(root->m_left){
			x=x+insert(root->m_left, node, root);
		}
		else{
			root->m_left=node;
		}
	}
	else if(y>0){
		if(root->m_right){
			x=x+insert(root->m_right, node, root);
		}
		else{
			root->m_right=node;
		}
	}
	else{
		return -1;
	}

	/*int x=1;
	if(root->*branch[br]){
		x+=insert(root->*branch[br], node, root);
	}
	else{
		root->*branch[br]=node;
	}*/
	
	root->m_height=std::max(root->m_height, x);

	int diff=root->diff();
	if(std::abs(diff)>1){
		balance(root, diff, top);
	}
	
	return root->m_height;
}



//--------------------------------------------------------------------

inline void NodeJson::AVL_Tree::removeNode(NodeJson* node, NodeJson* top)
{
	int diff=node->diff();
	int lr, clr;
	if(diff>0 || (diff==0 && node->m_right)){
		lr=LR::LEFT;
		clr=LR::RIGHT;
	}
	else{ //if(diff<0 || (diff==0 && !node->right)){
		lr=LR::RIGHT;
		clr=LR::LEFT;
	}

	bool isRoot=(m_root==node);
	
	NodeJson* tmp=nullptr;

	if(!node->m_right && !node->m_left){
		if(isRoot){
			m_root=nullptr;
			NodeJson::freeNode(node);
			return;
		}

		if(top->m_right==node){
			top->m_right=nullptr;
		}
		else{
			top->m_left=nullptr;
		}
		tmp=top;
	}
	else{
		NodeJson* tmpTop=node;
		tmp=node->*branch[clr];
		while(tmp->*branch[lr]){
			tmpTop=tmp;
			tmp=tmp->*branch[lr];
		}

		if(tmpTop==node){
			node->bridge(tmp, top);
			tmp->*branch[lr]=node->*branch[lr];
		}
		else{
			tmpTop->*branch[lr]=tmp->*branch[clr];

			node->bridge(tmp, top);
			tmp->*branch[lr]=node->*branch[lr];
			tmp->*branch[clr]=node->*branch[clr];
		}

		node->*branch[lr]=nullptr;
		node->*branch[clr]=nullptr;
	}

	if(isRoot){
		m_root=tmp;
	}

	if(tmp!=top){
		tmp->updateHeight();
		diff=tmp->diff();
		if(tmp!=top && std::abs(diff)>1){
			balance(tmp, diff, top);
		}
	}

	NodeJson::freeNode(node);
}

//--------------------------------------------------------------------

inline bool NodeJson::AVL_Tree::removeNode(const char* key, NodeJson* node, NodeJson* top)
{
	int y=m_jsonBufferRef.comparing(key, node->m_offset);

	int br=-1;
	if(y<0){
		br=LR::LEFT;
	}
	else if(y>0){
		br=LR::RIGHT;
	}
	
	if(br<0){
		removeNode(node, top);
		return true;
	}

	if(m_root && node->*branch[br]){
		if(!removeNode(key, node->*branch[br], node)){
			node->updateHeight();
			int diff=node->diff();
			if(node!=top && std::abs(diff)>1){
				balance(node, diff, top);
			}
		}
	}
	return false;
}

//--------------------------------------------------------------------

inline void NodeJson::AVL_Tree::remove(const char* key, NodeJson* obj)
{
	m_root=obj->m_child;
	if(m_root){
		removeNode(key, m_root, nullptr);
		obj->m_child=m_root;
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//--------------------------------------------------------------------
//--------------------------------------------------------------------
//--------------------------------------------------------------------
//--------------------------------------------------------------------
//--------------------------------------------------------------------
//--------------------------------------------------------------------
//--------------------------------------------------------------------
//--------------------------------------------------------------------
//--------------------------------------------------------------------

}
}
//--------------------------------------------------------------------
//--------------------------------------------------------------------


namespace easyjson
{

using namespace internal;

#define CAST_TO_CHAR(x) reinterpret_cast<const char*>(x)

#define JSON_NULL json_null()
#define JSON_OBJ json_obj()
#define JSON_ARRAY json_array()

/*
enum class ErrorHandlerMode
{
	Exception,
	Quiet,
};
*/
//====================================================================

class JsonObj final
{
	private:
		struct JsonValue
		{
			template<typename T>
			JsonValue(T t)
			: m_val(ToString<T>::toStr(t))
			, m_modifier(ToString<T>::modifier)
			{
			}
			
			void setData(NodeJson* node, JsonObjBuffer& jsonBuffer) const
			{
				node->setNone();
				node->setData(jsonBuffer, m_val.c_str(), m_modifier);
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
			:JsonPair(CAST_TO_CHAR(c), t)
			{}

			template<typename T>
			JsonPair(std::pair<const char8_t*, T> data)
			:JsonPair(data.first, data.second)
			{}

			void setData(NodeJson* node, JsonObjBuffer& jsonBuffer) const
			{
				node->setData(jsonBuffer, m_key, JSON_TYPES::_STR);
				node=node->addChild();
				node->setNone();
				node->setData(jsonBuffer, m_val.c_str(), m_modifier);
			}

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
	
	public:
		~JsonObj();

		bool hasKey(const char* key) const;

		const JsonObj operator[](const char* key) const
		{
			const NodeJson* node=find(key);
			if(node){
				return JsonObj{const_cast<NodeJson*>(node), m_jsonBufferPtr, m_errorHandler.getMode()};
			}
			return badJson(ErrorCode::error24);
		}

		JsonObj operator[](const char* key) __attribute__((always_inline))
		{
			if(NodeJson* node=find(key)){
				return {node, m_jsonBufferPtr, m_errorHandler.getMode()};
			}
			return badJson(ErrorCode::error24);
		}

		bool hasKey(const char8_t* key) const __attribute__((always_inline))
		{
			return hasKey(CAST_TO_CHAR(key));
		}

		const JsonObj operator[](const char8_t* key) const __attribute__((always_inline))
		{
			return operator[](CAST_TO_CHAR(key));
		}

		JsonObj operator[](const char8_t* key) __attribute__((always_inline))
		{
			return operator[](CAST_TO_CHAR(key));
		}

		//to query arrays
		const JsonObj operator[](int idx) const __attribute__((always_inline))
		{
			return JsonObj(nodeAt(idx), m_jsonBufferPtr, m_errorHandler.getMode());
		}	

		JsonObj operator[](int idx) __attribute__((always_inline))
		{
			// copy the state of the handler
			return JsonObj(nodeAt(idx), m_jsonBufferPtr, m_errorHandler.getMode());
		}

		JsonObj follow(const char* str);
		
		void operator=(json_obj)
		{
			m_node->clear();
			m_node->setAsObj();
		}

		void operator=(json_array)
		{
			if(m_node->isArray()){
				m_node->clearArray();
				return;
			}
			m_node->clear();
			m_node->setAsArray();
		}

		void operator=(JsonObj::JsonValue&& val);

		void operator=(std::initializer_list<JsonObj::JsonValue>&& list);

		void operator=(std::initializer_list<JsonObj::JsonPair>&& list);

		void operator=(JsonObj::JsonBulkList&& data);

		//for arrays
		void pushBack(JsonObj::JsonValue&& data);
		void pushBack(std::initializer_list<JsonObj::JsonValue>&& list);
		void pushBack(JsonObj::JsonBulkList&& data);
		void pushBack(json_obj);
		void pushBack(json_array);
		
		//for obj
		void append(JsonObj::JsonPair&& data);

		template<typename T>
		std::optional<T> getValue() const;

		const char* getRawData() const;

		void removeKey(const char* key);
		
		void removeKey(const char8_t* key)
		{
			removeKey(CAST_TO_CHAR(key));
		}
		
		void removeFromArray(size_t idx, bool shift=false);

		bool isObj() const
		{
			return m_node->isObj();
		}

		bool isArray() const
		{
			return m_node->isArray();
		}

		bool isValue() const
		{
			return !(isArray() || isObj());
		}

		std::string toString(bool prettyStr=false) const;

		bool isValid() const
		{
			return m_errorHandler.isValid();
		}

		std::string getErrorMsg() const
		{
			return m_errorHandler.getErrorMsg();
		}
		
		size_t size() const;

	private:
		JsonObjBuffer* m_jsonBufferPtr;
		NodeJson* m_node;
		mutable ErrorReporting m_errorHandler;
		bool m_isRoot;

		JsonObj(JsonObj&& other) __attribute__((always_inline))
		: m_jsonBufferPtr(other.m_jsonBufferPtr)
		, m_node(other.m_node)
		, m_errorHandler(other.m_errorHandler)
		, m_isRoot(other.m_isRoot)
		{
			other.m_jsonBufferPtr=nullptr;
			other.m_node=nullptr;
			other.m_isRoot=false;
		}

		explicit JsonObj(const char* data, ErrorHandlerMode mode=ErrorHandlerMode::Exception);
		explicit JsonObj(size_t bufferSize, ErrorHandlerMode mode=ErrorHandlerMode::Exception);

		JsonObj(NodeJson* node, JsonObjBuffer* jsonBufferPtr, ErrorHandlerMode mode=ErrorHandlerMode::Exception) __attribute__((always_inline))
		: m_jsonBufferPtr(jsonBufferPtr)
		, m_node(node)
		, m_errorHandler(mode)
		, m_isRoot(false)
		{
			if(!node){
				m_errorHandler.setError(ErrorCode::error20);
			}
		}

		explicit JsonObj(ErrorHandlerMode mode);

		JsonObj(const JsonObj& other)=delete;
		JsonObj& operator=(const JsonObj&)=delete;

		JsonObj badJson(ErrorCode error) const
		{
			JsonObj obj(m_errorHandler.getMode());

			obj.m_errorHandler.setError(error);
			return obj;
		}

		NodeJson* nodeAt(uint idx) const;

		template<typename T, typename FUNC>
		void initArray(std::initializer_list<T>&& list, FUNC cbk);
		
		NodeJson* find(const char* key) const;
		
		template<typename T, typename FUNC>
		void pushBackData(T&& data, FUNC cbk);

		JsonObj onPath(const JsonObj& jsonObj, std::string& path, size_t offset);
		
		bool failWhen(bool a, ErrorCode errorCode) const	__attribute__((always_inline));

	friend JsonParser;
};

//--------------------------------------------------------------------

class JsonParser
{
	public:
		static JsonObj parse(const char* str, ErrorHandlerMode mode=ErrorHandlerMode::Exception);
		
		static JsonObj initObj(ErrorHandlerMode mode=ErrorHandlerMode::Exception)
		{
			return JsonObj(mode);
		}
		
		static JsonObj parse(const char8_t* str, ErrorHandlerMode mode=ErrorHandlerMode::Exception)
		{
			return parse(reinterpret_cast<const char*>(str), mode);
		}

		static JsonObj openJsonFile(const char* jsonFileName, ErrorHandlerMode mode=ErrorHandlerMode::Exception);
		
		static std::string utf8Encode(const char* cstr);

	private:	

		class SyntaxRules
		{
			public:
				void setRuleInitial();
				bool syntaxRuleInitial();
				void setRuleKey();
				bool syntaxRuleKey();
				void setRuleValue();
				bool syntaxRuleValue();

				void setRuleReady(bool factor=true);
				bool syntaxRuleReady();

				void setRuleComma();
				bool syntaxRuleComma();
				void setRuleColon();
				bool syntaxRuleColon();

				void setRuleObj();
				bool syntaxRuleObj();
				void setRuleArr();
				bool syntaxRuleArr();

			private:
				enum Rule : unsigned char
				{
					closing=0,
					initial=1,
					value=1<<1,
					ready=1<<2,
					curl=1<<3,
					squrt=1<<4,
					key=1<<5,
					obj=(curl | key),
					arr=(squrt | value),
					comma=1<<6,
					colon=1<<7,
				};
				
				Rule m_rule{Rule::initial};
		};

		static void parserLoop(JsonObjBuffer* jsonObjBuffer, NodeJson* node, ErrorReporting& errorHandler);

		JsonParser()=delete;

		static bool pushUTF8CodePoint(std::string& str, char ch1, char ch2, char ch3, char ch4);

		static bool isHexValid(const char* cstr);
		static int parseValueData(char* cstr, ErrorReporting& errorHandler, NodeJson* node);
		static void removeEmptyIndex(NodeJson* node);
};

//--------------------------------------------------------------------

inline JsonObj JsonParser::parse(const char* str, ErrorHandlerMode mode)
{
	JsonObj jObj(str, mode);

	parserLoop(jObj.m_jsonBufferPtr, jObj.m_node, jObj.m_errorHandler);

	return jObj;
}

//--------------------------------------------------------------------

inline void JsonParser::SyntaxRules::setRuleInitial()
{
	m_rule=Rule::initial;
}

//--------------------------------------------------------------------

inline bool JsonParser::SyntaxRules::syntaxRuleInitial()
{
	return m_rule==Rule::initial;
}

//--------------------------------------------------------------------

inline void JsonParser::SyntaxRules::setRuleKey()
{
	m_rule=Rule::key;
}

//--------------------------------------------------------------------

inline bool JsonParser::SyntaxRules::syntaxRuleKey()
{
	return m_rule & Rule::key;
}

//--------------------------------------------------------------------

inline void JsonParser::SyntaxRules::setRuleObj()
{
	m_rule=Rule::obj;
}

//--------------------------------------------------------------------

inline bool JsonParser::SyntaxRules::syntaxRuleObj()
{
	return (m_rule & Rule::ready) | (m_rule & Rule::curl);
}

//--------------------------------------------------------------------

inline void JsonParser::SyntaxRules::setRuleArr()
{
	m_rule=Rule::arr;
}

//--------------------------------------------------------------------

inline bool JsonParser::SyntaxRules::syntaxRuleArr()
{
	return (m_rule & Rule::ready) | (m_rule & Rule::squrt);
}

//--------------------------------------------------------------------

inline void JsonParser::SyntaxRules::setRuleValue()
{
	m_rule=Rule::value;
}

//--------------------------------------------------------------------

inline bool JsonParser::SyntaxRules::syntaxRuleValue()
{
	return m_rule & Rule::value;
}

//--------------------------------------------------------------------

inline void JsonParser::SyntaxRules::setRuleReady(bool factor)
{
	m_rule=Rule::closing;
	if(factor){
		m_rule=Rule::ready;
	}
}

//--------------------------------------------------------------------

inline bool JsonParser::SyntaxRules::syntaxRuleReady()
{
	return m_rule & Rule::ready;
}

//--------------------------------------------------------------------

inline void JsonParser::SyntaxRules::setRuleComma()
{
	m_rule=Rule::comma;
}

//--------------------------------------------------------------------

inline bool JsonParser::SyntaxRules::syntaxRuleComma()
{
	return m_rule==Rule::comma;
}

//--------------------------------------------------------------------

inline void JsonParser::SyntaxRules::setRuleColon()
{
	m_rule=Rule::colon;
}

//--------------------------------------------------------------------

inline bool JsonParser::SyntaxRules::syntaxRuleColon()
{
	return m_rule==Rule::colon;
}

//--------------------------------------------------------------------

inline void JsonParser::removeEmptyIndex(NodeJson* node)
{
	/*
	 * An array that has at least one element
	 * */

	//if(node->isArray()){// we already check that it is an array
	//node->m_offset=NodeJson::VectWrapper::cow(node->m_offset);
	NodeJson::VectWrapper* vectPtr=reinterpret_cast<NodeJson::VectWrapper*>(node->m_child);

	if(vectPtr->size()==1){
		NodeJson* item=vectPtr->getLast();
		if(item){
			if(!item->isArray() && !item->isObj()){
				if(item->m_offset==0){
					vectPtr->removeLast();
				}
			}
		}
	}
}

//--------------------------------------------------------------------

inline JsonObj::JsonObj(const char* data, ErrorHandlerMode mode)
: m_jsonBufferPtr(new JsonObjBuffer(data))
, m_node(NodeJson::allocateNode())
, m_errorHandler(mode)
, m_isRoot(true)
{
	m_node->setAsObj();
}

//--------------------------------------------------------------------

inline JsonObj::JsonObj(ErrorHandlerMode mode)
: JsonObj(" ", mode)
{
}

//--------------------------------------------------------------------

inline JsonObj::JsonObj(size_t bufferSize, ErrorHandlerMode mode)
:JsonObj(mode)
{
	m_jsonBufferPtr->resizeBuffer(bufferSize);
}

//--------------------------------------------------------------------

inline JsonObj::~JsonObj()
{
	if(m_isRoot){
		if(m_node){
			NodeJson::freeNode(m_node);
			m_node=nullptr;
		}
		if(m_jsonBufferPtr){
			delete m_jsonBufferPtr;
		}
	}
}

//--------------------------------------------------------------------

inline bool JsonObj::failWhen(bool a, ErrorCode errorCode) const
{
	if(a){
		m_errorHandler.setError(errorCode);
		return true;
	}
	return false;
}

//--------------------------------------------------------------------

inline std::string JsonObj::toString(bool pretty) const
{
	if(m_node && isValid()){
		std::string jsonStr;
		jsonStr.reserve(m_jsonBufferPtr->bufferSize());
		m_node->print(*m_jsonBufferPtr, jsonStr, pretty);
		return jsonStr;
	}

	return "";
}


//--------------------------------------------------------------------

inline bool JsonObj::hasKey(const char* key) const
{
	if(!failWhen(m_node->isArray(), ErrorCode::error13)){
		NodeJson::AVL_Tree tree(*m_jsonBufferPtr);
		return tree.find(m_node, key)!=nullptr;
	}
	return false;
}

//--------------------------------------------------------------------

inline const char* JsonObj::getRawData() const
{
	if(!failWhen(!m_node || !m_node->hasData(), ErrorCode::error19)){
		if(!failWhen(m_node->isKeyObjArr(), ErrorCode::error14)){
			return m_jsonBufferPtr->getDataAt(m_node->getOffset());
		}
	}
	return nullptr;
}

//--------------------------------------------------------------------

template<typename T>
std::optional<T> JsonObj::getValue() const
{
	const char* data=getRawData();
	if(!data){
		return std::nullopt;
	}

	return FromString<T>::getFrom(data);
}

//--------------------------------------------------------------------

template<>
std::optional<json_null> JsonObj::getValue<json_null>() const
{
	const char* data=getRawData();

	if(!data || m_node->getDataMode()!=JSON_TYPES::_NULL || 0!=m_jsonBufferPtr->comparing("null", m_node->getOffset())){
		m_errorHandler.setError(ErrorCode::error23);
		return std::nullopt;
	}

	return FromString<json_null>::getFrom(data);
}

//--------------------------------------------------------------------

inline void JsonObj::operator=(JsonObj::JsonValue&& val)
{
	m_node->clear();
	val.setData(m_node, *m_jsonBufferPtr);
}

//--------------------------------------------------------------------

/* Attaches array to key
 * obj[a]={b, c, 2, 3} --> a:[b, c, 2, 3] 
 * */
inline void JsonObj::operator=(std::initializer_list<JsonObj::JsonValue>&& list)
{
	initArray(std::move(list), [this](const JsonValue& data, NodeJson* node){
		data.setData(node, *m_jsonBufferPtr);
	});
}

//--------------------------------------------------------------------

/* Attaches array of objs to key
 *  obj[a]={{b, 1}, {c, 2}, {d, 3}} --> a:[{b:1}, {c:2}, {d:3}] 
 * */
inline void JsonObj::operator=(std::initializer_list<JsonObj::JsonPair>&& list)
{	
	NodeJson::AVL_Tree tree(*m_jsonBufferPtr);
	initArray(std::move(list), [&tree, this](const JsonPair& data, NodeJson* node){
		node->setAsObj();
		NodeJson* keyNode=node->addKeyNode();
		data.setData(keyNode, *m_jsonBufferPtr);
		tree.insertAt(node, keyNode);
	});
}

//--------------------------------------------------------------------

template<typename T, typename FUNC>
void JsonObj::initArray(std::initializer_list<T>&& list, FUNC cbk)
{
	NodeJson* arrayNode=m_node;
	
	if(arrayNode->isArray()){
		arrayNode->clearArray();
	}
	else{
		arrayNode->clear();
		arrayNode->setAsArray();
	}

	if(list.size()==0){
		return;
	}

	NodeJson* itemNode=arrayNode->addArrayItem();

	size_t i=0;
	for(auto& nodeData : list){
		cbk(nodeData, itemNode);

		if(++i<list.size()){
			itemNode=arrayNode->addArrayItem();
		}
	}
}

//--------------------------------------------------------------------

// insert an element (number or string) into array: 
inline void JsonObj::pushBack(JsonObj::JsonValue&& data)
{
	if(!failWhen(m_node->isObj() || m_node->m_offset>0, ErrorCode::error15)){
		if(!m_node->isArray()){
			m_node->setAsArray();
		}

		NodeJson* itemNode=m_node->addArrayItem();

		data.setData(itemNode, *m_jsonBufferPtr);
	}
}

//--------------------------------------------------------------------

// Insert an array into an array
inline void JsonObj::pushBack(std::initializer_list<JsonObj::JsonValue>&& list)
{
	pushBackData(std::move(list), [this](std::initializer_list<JsonObj::JsonValue>&& list, NodeJson* node){
		node->setAsArray();
		JsonObj obj(node, m_jsonBufferPtr);
		obj=std::move(list);
	});
}

//--------------------------------------------------------------------

// append objects to array
inline void JsonObj::pushBack(JsonObj::JsonBulkList&& data)
{
	pushBackData(std::move(data), [this](JsonObj::JsonBulkList&& data, NodeJson* node){
		node->setAsObj();
		JsonObj obj(node, m_jsonBufferPtr);
		obj=std::move(data);
	});
}

//--------------------------------------------------------------------

template<typename T, typename FUNC>
inline void JsonObj::pushBackData(T&& data, FUNC cbk)
{
	if(failWhen(m_node->isObj(), ErrorCode::error14)){
		return;
	}

	if(!m_node->isArray()){
		m_node->setAsArray();
	}

	NodeJson* itemNode=m_node->addArrayItem();

	cbk(std::move(data), itemNode);
}

//--------------------------------------------------------------------

// add an empty array to an array
inline void JsonObj::pushBack(json_array)
{
	pushBack({});
}
//--------------------------------------------------------------------

// append empty object to array
inline void JsonObj::pushBack(json_obj)
{
	pushBack({JSON_OBJ, {}});
}

//--------------------------------------------------------------------

//add {key:val} to obj
inline void JsonObj::append(JsonObj::JsonPair&& data)
{
	if(!failWhen(m_node->isArray(), ErrorCode::error16)){
		if(!failWhen(hasKey(data.m_key),ErrorCode::error17)){
			m_node->setAsObj();
			NodeJson* node=m_node->addKeyNode();
	
			data.setData(node, *m_jsonBufferPtr);
			NodeJson::AVL_Tree tree(*m_jsonBufferPtr);
			tree.insertAt(m_node, node); // we already check for the key
		}
	}
}

//--------------------------------------------------------------------

// User responsibility to check vector size/range
inline NodeJson* JsonObj::nodeAt(uint idx) const
{
	if(failWhen(!m_node->isArray(), ErrorCode::error14)){
		return nullptr;
	}

	NodeJson::VectWrapper* vect=reinterpret_cast<NodeJson::VectWrapper*>(m_node->m_child);

	return vect->get(idx);
}

//--------------------------------------------------------------------

inline void JsonObj::removeKey(const char* key)
{
	if(failWhen(!m_node->isObj(), ErrorCode::error13)){
		return;
	}

	NodeJson::AVL_Tree tree(*m_jsonBufferPtr);
	tree.remove(key, m_node);
}

//--------------------------------------------------------------------

inline void JsonObj::removeFromArray(size_t idx, bool shift)
{
	if(failWhen(!m_node->isArray(), ErrorCode::error13)){
		return;
	}

	NodeJson::VectWrapper* vect=reinterpret_cast<NodeJson::VectWrapper*>(m_node->m_child);

	vect->remove(idx, shift);
}

//--------------------------------------------------------------------

inline size_t JsonObj::size() const
{
	if(failWhen(!m_node->isArray(), ErrorCode::error14)){
		return size_t(-1);
	}

	NodeJson::VectWrapper* vect=reinterpret_cast<NodeJson::VectWrapper*>(m_node->m_child);

	return vect->size();
}

//====================================================================

inline bool JsonParser::pushUTF8CodePoint(std::string& str, char ch1, char ch2, char ch3, char ch4)
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

//--------------------------------------------------------------------

inline std::string JsonParser::utf8Encode(const char* cstr)
{
	std::string result;
	char* q=0;
	char* p=const_cast<char*>(cstr);
	while(*p) {
		if(*p=='\\'){
			q=p;
			q++;
			if(*q=='u'){
				q++;
				
				if(!pushUTF8CodePoint(result, q[0], q[1], q[2], q[3])){
					return "";
				}
				p+=6;
				continue;
			}
		}
		
		result.push_back(*p);

		++p;
	}
	
	return result;
}

//--------------------------------------------------------------------

inline bool JsonParser::isHexValid(const char* cstr)
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

//--------------------------------------------------------------------

namespace
{
	enum NumFormat : unsigned char
	{
		none=0,
		neg=1,
		pst=1<<1,
		dots=1<<2,
		eCount=1<<3,
		bad=neg | pst,
		bad2=eCount | neg | pst,
	};
}

//--------------------------------------------------------------------


inline int JsonParser::parseValueData(char* cstr, ErrorReporting& errorHandler, NodeJson* node)
{
	if(cstr[0]!='-' && (int(cstr[0])<48 || int(cstr[0])>57)){
		size_t k=0;
		bool a=true;
		if(cstr[0]=='n'){
			k=4;
			a=cstr[1]=='u' || cstr[2]=='l' || cstr[3]=='l';
			node->setDataMode(JSON_TYPES::_NULL); 
		}
		else if(cstr[0]=='t'){
			k=4;
			a=cstr[1]=='r' || cstr[2]=='u' || cstr[3]=='e';
			node->setDataMode(JSON_TYPES::_BOOL);
		}
		else if(cstr[0]=='f'){
			k=5;
			a=cstr[1]=='a' || cstr[2]=='l' || cstr[3]=='s' || cstr[4]=='e';
			node->setDataMode(JSON_TYPES::_BOOL);
		}

		if(k==0 || cstr[k]==0 || !a){
			errorHandler.setError(ErrorCode::error1);
			return -1;
		}

		if(int(cstr[k])<33 && std::isspace(static_cast<unsigned char>(cstr[k]))){
			cstr[k]=0;
		}
		return k-1;
	}

	// Leading zeroes
	// notice that cstr[1] is valid since it could be '\0'
	if(cstr[0]==48 && (cstr[1]==48 || cstr[1]=='e' || cstr[1]=='E')){
		errorHandler.setError(ErrorCode::error5);
		return -1;
	}

	node->setDataMode(JSON_TYPES::_NUM);

	unsigned char format=0;

	size_t i=0;

	if(cstr[i]=='-'){
		format=NumFormat::neg;
		i++;
	}

	i--; // !!
	while(true){
		while(cstr[++i]>47 && cstr[i]<58);// ;-)

		if(cstr[i]==',' || cstr[i]=='}' || cstr[i]==']'){
			break;
		}

		if(int(cstr[i])<33){// As we are parsing a numeric value, we don't need to check for specific white characters
			cstr[i]=0;
			break;
		}

		if(cstr[i]=='-'){
			if(format & NumFormat::bad){
				errorHandler.setError(ErrorCode::error5);
				return -1;
			}
			format=format|NumFormat::neg;
			
			continue;
		}

		if(cstr[i]=='+'){
			if(0==(format & NumFormat::eCount) || format & NumFormat::bad){
				errorHandler.setError(ErrorCode::error5);
				return -1;
			}
			format=format|NumFormat::pst;
			
			continue;
		}

		if(cstr[i]=='e' || cstr[i]=='E'){
			if(format & NumFormat::eCount){
				errorHandler.setError(ErrorCode::error5);
				return -1;
			}
			format=format&~NumFormat::neg;
			format=format | NumFormat::eCount | NumFormat::dots;// dots are not allowed after e/E
			i++;
			continue;
		}

		if(cstr[i]=='.'){
			if(format & NumFormat::dots){
				errorHandler.setError(ErrorCode::error5);
				return -1;
			}
			format=format|NumFormat::dots;
			node->setDataMode(JSON_TYPES::_DOUBLE);
			
			continue;
		}

		errorHandler.setError(ErrorCode::error5);
		return -1;
	}

	if(format & NumFormat::eCount){
		if(cstr[i-1]=='e' || cstr[i-1]=='E'){
			errorHandler.setError(ErrorCode::error5);
			return -1;
		}
	}

	if(cstr[i-1]=='.'){
		errorHandler.setError(ErrorCode::error6);
		return -1;
	}

	return i-1;
}

//--------------------------------------------------------------------

namespace 
{
	/*
	* -"But but, you could have used a smart pointer"
	* -"Right, but I don't want to include a header with 100's of lines
	*  of code and NodeGard does the job that needs to be done... Simple."
	* */
	struct NodeGard
	{
		~NodeGard()
		{
			if(m_nodePtr){
				if(m_nodePtr->isKey()){
					NodeJson::freeNode(m_nodePtr);
					m_nodePtr=nullptr;
				}
			}
		}

		NodeJson* m_nodePtr{nullptr};
	};

	class NodeDeck
	{
		public:
			~NodeDeck()=default;

			void addContainer(NodeJson* node) __attribute__((always_inline))
			{
				if(m_conA>0){
					m_conVect[m_conQ]=node;
					m_conA--;
					m_conQ++;
				}
				else{
					m_conVect.push_back(node);
					m_conQ=m_conVect.size();
				}	
			}
			
			NodeJson* closeScope() __attribute__((always_inline))
			{
				m_conA++;
				m_conQ--;

				if(m_conQ>0){
					return m_conVect[m_conQ-1];
				}
				return nullptr;
			};
			
			bool hasNodes() 
			{
				return m_conQ>0;
			}
	
		private:
			std::vector<NodeJson*> m_conVect;
			int m_conQ{0};
			int m_conA{0};
	};
}

inline void JsonParser::parserLoop(JsonObjBuffer* jsonObjBufferPtr, NodeJson* node, ErrorReporting& errorHandler)
{	
	char* buffer=jsonObjBufferPtr->m_buffer.data();

	NodeJson* rootNode=node;
	
	SyntaxRules rule;
	rule.setRuleInitial();

	NodeGard nodeGard;

	NodeJson::AVL_Tree tree(*jsonObjBufferPtr);

	NodeJson* activeContainer=nullptr;

	NodeDeck nodeDeck;

	bool parsingString=false;
	int k=0;
	int i=0;
	while(buffer[i]){ // ok...
		if(buffer[i]<33){ // we discart any white character outside a key or value
			i++;
			continue;
		}
	/*
	START_LOOP:
	{
		//--i;
		//i++;
		--i;
		while(buffer[++i]<33 && buffer[i]>0);
		if(buffer[i]==0){
			goto FINISH_JSON;
		}//*
		
		/*
		--i;
		while(buffer[++i]>0 && buffer[i]<33);
		if(buffer[i]==0){
			goto FINISH_JSON;
		}// */

		/*
		if(int(buffer[i])<33){
			bool a=true;
			while(a){
				switch(buffer[i]){
					case ' ':
					case '\n':
					case '\r':
					case '\t':
						i++;
						break;
					default:
						a=false;
						break;
				}
			}
			if(buffer[i]==0){
				goto FINISH_JSON;
			}
		}// */

		switch(buffer[i]){
			case '"':
				{
					if(!node->isKey() && !node->isObj()){
						if(!rule.syntaxRuleValue()){
							errorHandler.setError(ErrorCode::error1);
							goto FINISH_JSON;
						}
					}
					else{
						if(!rule.syntaxRuleKey()){
							errorHandler.setError(ErrorCode::error9);
							goto FINISH_JSON;
						}

						node=node->addKeyNode();
						nodeGard.m_nodePtr=node;
					}

					i++;
					node->m_offset=i;				

					parsingString=true;

					while(buffer[i]!=0){
						if(buffer[i]=='"'){
							break;
						}

						if(buffer[i]=='\\'){
							if(buffer[i+1]=='\\'){
								i++;
							}
							else if(buffer[i+1]=='u' || buffer[i+1]=='U'){
								if(isHexValid(buffer+i+1)){
									i+=5; // u(H1)(H2)(H3)(H4) 5 characters for a valid hex
								}
								else{
									errorHandler.setError(ErrorCode::error1);
									goto FINISH_JSON;
								}
							}
							else if(buffer[i+1]=='"'){
								i++;
								k++;
							}
							else if(buffer[i+1]=='/' || buffer[i+1]=='b' 
									|| buffer[i+1]=='f' || buffer[i+1]=='n' || buffer[i+1]=='r' 
									|| buffer[i+1]=='t')
							{
								i++;
							}
							else{
								errorHandler.setError(ErrorCode::error1);
								goto FINISH_JSON;
							}	
						}
						else {
							if(buffer[i]<14 && buffer[i]>0){//control characters//13='\n', 12 || buffer[i]=='\t' || buffer[i]=='\b' || buffer[i]=='\r' || buffer[i]=='\f'){
								errorHandler.setError(ErrorCode::error1);
								goto FINISH_JSON;
							}
						}
						
						if(k>0){
							buffer[i-k]=buffer[i];
						}

						i++;
					}

					if(parsingString){
						parsingString=false;
						buffer[i]='\0';
						buffer[i-k]='\0';					

						node->setDataMode(JSON_TYPES::_STR);

						if(node->isKey()){
							nodeGard.m_nodePtr=nullptr;
							if(!tree.insertAt(activeContainer, node)){
								NodeJson::freeNode(node);
								errorHandler.setError(ErrorCode::error17);
								goto FINISH_JSON;
							}
							rule.setRuleColon();
						}
						else{
							rule.setRuleReady();
						}
						k=0;
					}
					break;
				}
			case '{':
				{
					if(!rule.syntaxRuleValue() && !rule.syntaxRuleInitial()){
						errorHandler.setError(ErrorCode::error9);
						goto FINISH_JSON;
					}

					node->setAsObj();
					
					nodeDeck.addContainer(node);
					
					activeContainer=node;
					rule.setRuleObj();
					break;
				}
			case '}':
				{
					if(!rule.syntaxRuleObj()){
						errorHandler.setError(ErrorCode::error3);
						goto FINISH_JSON;
					}

					if(!activeContainer->isObj()){
						errorHandler.setError(ErrorCode::error11);
						goto FINISH_JSON;
					}
					
					buffer[i]=0;

					node=nodeDeck.closeScope();
					activeContainer=node;

					rule.setRuleReady(activeContainer);
					break;
				}
			case '[':
				{
					if(!rule.syntaxRuleValue() && !rule.syntaxRuleInitial()){
						errorHandler.setError(ErrorCode::error2);
						goto FINISH_JSON;
					}

					node->setAsArray();

					nodeDeck.addContainer(node);
					activeContainer=node;
					node=node->addArrayItem();

					rule.setRuleArr();
					break;
				}
			case ']':
				{
					if(!rule.syntaxRuleArr()){
						errorHandler.setError(ErrorCode::error1);
						goto FINISH_JSON;
					}

					if(!activeContainer->isArray()){
						errorHandler.setError(ErrorCode::error11);
						goto FINISH_JSON;
					}
					
					buffer[i]=0;

					removeEmptyIndex(activeContainer);
					
					node=nodeDeck.closeScope();
					
					activeContainer=node;
					rule.setRuleReady(activeContainer);
					break;
				}
			case ',':
				{
					if(!rule.syntaxRuleReady()){
						errorHandler.setError(ErrorCode::error1);
						goto FINISH_JSON;
					}

					buffer[i]=0;

					if(activeContainer->isArray()){
						node=activeContainer->addArrayItem();
						rule.setRuleValue();
					}
					else{
						node=activeContainer;
						rule.setRuleKey();
					}
					break;
				}
			case ':':
				{
					if(!rule.syntaxRuleColon()){
						errorHandler.setError(ErrorCode::error1);
						goto FINISH_JSON;
					}
					buffer[i]=0;
					node=node->addBlankChild();
					rule.setRuleValue();
					break;
				}
			default:
				{
					if(!rule.syntaxRuleValue()){
						errorHandler.setError(ErrorCode::error1);
						goto FINISH_JSON;
					}

					node->setOffset(i);

					int a=parseValueData(&buffer[i], errorHandler, node);

					if(a<0){
						goto FINISH_JSON;
					}
					i+=a;
					if(buffer[i+1]=='\0'){
						i++;
					}
					rule.setRuleReady();
					break;
				}
		}
		i++;
		//goto START_LOOP;
	}

	FINISH_JSON:

	if(errorHandler.isValid()){
		if(nodeDeck.hasNodes()){// We should end with the same node as we began
			errorHandler.setError(ErrorCode::error3);
		}
		
		if(!rootNode->m_child && rule.syntaxRuleInitial()){
			errorHandler.setError(ErrorCode::error21);
		}
	}
}

//--------------------------------------------------------------------

inline JsonObj JsonParser::openJsonFile(const char* jsonFileName, ErrorHandlerMode mode)
{
	size_t fileLength=0;
	std::ifstream jsonFile(jsonFileName);
	if(jsonFile.is_open()){
		jsonFile.seekg(0, std::ios_base::end);
		std::ifstream::pos_type endPos = jsonFile.tellg();
		jsonFile.seekg(0, std::ios_base::beg);

		fileLength=endPos-jsonFile.tellg();
	
      if(fileLength>1){
			JsonObj jObj(fileLength+1, mode);
			
			char* buffer=jObj.m_jsonBufferPtr->m_buffer.data();

			jsonFile.read(buffer, fileLength);
			buffer[fileLength]=0;
			
			if(jsonFile){
				jsonFile.close();
				parserLoop(jObj.m_jsonBufferPtr, jObj.m_node, jObj.m_errorHandler);
				return jObj;
			}
		}
		jsonFile.close();
	}

	JsonObj badJson(mode);
	ErrorCode error=ErrorCode::error22;
	if(fileLength<2){
		error=ErrorCode::error25;
	}
	badJson.m_errorHandler.setError(error);

	return badJson;
}


//--------------------------------------------------------------------

/* find will return the node attached (child) to the node with the passed key 
 * if there exists, otherwhise it will create a new entry with empty child
 * */
inline NodeJson* JsonObj::find(const char* key) const
{
	if(failWhen(m_node->isArray(), ErrorCode::error13)){
		return nullptr;
	}

	NodeJson* node=NodeJson::AVL_Tree::find(*m_jsonBufferPtr, m_node, key);
	
	if(node){
		return node->m_child;
	}
	
	NodeJson::AVL_Tree tree(*m_jsonBufferPtr);

	node=m_node->addKeyNode(*m_jsonBufferPtr, key);
	
	tree.insertAt(m_node, node);

	return node->addBlankChild();
	// */
}

//--------------------------------------------------------------------

//obj[key]={JSO::OBJ, {{a, b}, {c,d}, {e:f}} -> {key: {a:b, c:d, e:f}}
//m_node->m_child->m_child(obj)
inline void JsonObj::operator=(JsonObj::JsonBulkList&& data)
{
	m_node->clear();
	m_node->setAsObj();

	NodeJson* node=nullptr;	

	NodeJson::AVL_Tree tree(*m_jsonBufferPtr);

	bool isIn=false;
	for(auto& nodeData : data.m_list){
		isIn=false;
		
		if(m_node->m_child){
			isIn=tree.find(m_node, nodeData.m_key);
		}
		
		if(isIn){
			continue;
		}
		
		node=m_node->addKeyNode();

		nodeData.setData(node, *m_jsonBufferPtr);

		tree.insertAt(m_node, node);
	}
}

//--------------------------------------------------------------------

inline JsonObj JsonObj::onPath(const JsonObj& jsonObj, std::string& path, size_t offset)
{
	const char* key=path.data()+offset;
	size_t pos=path.find_first_of('/', offset);
	if(pos!=std::string::npos){
		path[pos++]=0;
		if(jsonObj.isArray()){
			return onPath(jsonObj[std::atoi(key)], path, pos);
		}
		return onPath(jsonObj[key], path, pos);
	}

	if(jsonObj.isArray()){
		return jsonObj[std::atoi(key)];
	}

	return jsonObj[key];
}

//--------------------------------------------------------------------

inline JsonObj JsonObj::follow(const char* str)
{
	std::string path=str;
	size_t pos=path.find_first_of('/');
	if(pos!=std::string::npos){
		path[pos++]=0;
		const char* key=path.data();
		JsonObj jsonObj=operator[](key);
		return onPath(jsonObj, path, pos);
	}
	return operator[](str);
}

/*
template<typename... Args>
inline NodeJson* JsonObj::follow(NodeJson* node, int idx, Args... args)
{
	NodeJson::VectWrapper* vect=reinterpret_cast<NodeJson::VectWrapper*>(node->m_child);

	return follow(vect->get(idx), args...);
}

template<typename... Args>
inline NodeJson* JsonObj::follow(NodeJson* node, const char* key, Args... args)
{
	NodeJson* node=NodeJson::AVL_Tree::find(*m_jsonBufferPtr, node, key);
	
	return follow(node, args...);
	
}

template<typename... Args>
inline JsonObj JsonObj::follow(const char* key, Args... args)
{
	NodeJson* node=NodeJson::AVL_Tree::find(*m_jsonBufferPtr, m_node, key);
	
	follow(node, args...);
	
}

// */
//====================================================================

}// simplejson namespace


//--------------------------------------------------------------------
//--------------------------------------------------------------------
//--------------------------------------------------------------------

#endif
