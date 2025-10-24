/*********************************************************************
* JsonImpl class                               								*
*                                                                    *
* Date:    14-06-2025                                                *
* Author:  Dan Machado                                               *                                         *
**********************************************************************/
#include "easyjson/easyjson.h"
#include "easyjson/internal/json_utilities.h"
#include "easyjson/internal/json_core.h"

namespace easyjson
{

using namespace internal;

void setData(const JsonValue& jsonValue, NodeJson* node, JsonObjBuffer& jsonBuffer)
{
	if(JSON_TYPES::_JSON_ARRAY==jsonValue.m_modifier){
		node->setAsArray();
	}
	else if(JSON_TYPES::_JSON_OBJ==jsonValue.m_modifier){
		node->setAsObj();
	}
	else{
		node->setNone();
		node->setData(jsonBuffer, jsonValue.m_val.c_str(), jsonValue.m_modifier);
	}
}

void setData(const JsonPair& jsonPair, NodeJson* node, JsonObjBuffer& jsonBuffer)
{
	node->setData(jsonBuffer, jsonPair.m_key, JSON_TYPES::_STR);
	node=node->addChild();
	
	if(JSON_TYPES::_JSON_ARRAY==jsonPair.m_modifier){
		node->setAsArray();
	}
	else if(JSON_TYPES::_JSON_OBJ==jsonPair.m_modifier){
		node->setAsObj();
	}
	else{	
		node->setNone();
		node->setData(jsonBuffer, jsonPair.m_val.c_str(), jsonPair.m_modifier);
	}
}

//====================================================================

class JsonImpl
{
	public:
		~JsonImpl();

		bool hasKey(const char* key) const;

		/*const JsonImpl* operator[](const char* key) __attribute__((always_inline)) __attribute__((hot))
		{
			const NodeJson* node=find(key);
			if(node){
				return new JsonImpl{const_cast<NodeJson*>(node), m_jsonBufferPtr, m_errorHandler.getMode()};
			}
			return new JsonImpl(m_errorHandler.getMode(), ErrorCode::error24);
		}*/

		JsonImpl* operator[](const char* key) __attribute__((always_inline)) __attribute__((hot))
		{
			if(NodeJson* node=find(key)){
				return new JsonImpl{node, m_jsonBufferPtr, m_errorHandler.getMode()};
			}
			return new JsonImpl(m_errorHandler.getMode(), ErrorCode::error24);
		}

		bool hasKey(const char8_t* key) const __attribute__((always_inline)) __attribute__((hot))
		{
			return hasKey(reinterpret_cast<const char*>(key));
		}

		/*const JsonImpl* operator[](const char8_t* key) __attribute__((always_inline)) __attribute__((hot))
		{
			return operator[](CAST_TO_CHAR(key));
		}*/

		JsonImpl* operator[](const char8_t* key) __attribute__((always_inline))
		{
			return operator[](reinterpret_cast<const char*>(key));
		}

		//to query arrays
		/*const JsonImpl* operator[](int idx) const __attribute__((always_inline)) __attribute__((hot))
		{
			return new JsonImpl(nodeAt(idx), m_jsonBufferPtr, m_errorHandler.getMode());
		}	*/

		JsonImpl* operator[](int idx) __attribute__((always_inline)) __attribute__((hot))
		{
			// copy the state of the handler
			return new JsonImpl(nodeAt(idx), m_jsonBufferPtr, m_errorHandler.getMode());
		}

		JsonImpl* follow(const char* str);
		
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

		template<typename T>
		std::optional<T> getValue() const;

		const char* getRawData() const;

		void removeKey(const char* key);
		
		void removeKey(const char8_t* key)
		{
			removeKey(reinterpret_cast<const char*>(key));
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

		bool isNull()
		{
			return m_node->getDataMode()==JSON_TYPES::_NULL || 0==m_jsonBufferPtr->comparing("null", m_node->getOffset());
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
		
		bool isString() const
		{
			return m_node->isString();
		}

		bool isBoolean() const
		{
			return m_node->isBoolean();
		}

		bool isNumeric() const
		{
			return m_node->isNumeric();
		}
		
		size_t size() const;

	private:
		JsonObjBuffer* m_jsonBufferPtr;
		NodeJson* m_node;
		mutable ErrorReporting m_errorHandler;
		bool m_isRoot;

		JsonImpl(JsonImpl&& other) __attribute__((always_inline))
		: m_jsonBufferPtr(other.m_jsonBufferPtr)
		, m_node(other.m_node)
		, m_errorHandler(other.m_errorHandler)
		, m_isRoot(other.m_isRoot)
		{
			other.m_jsonBufferPtr=nullptr;
			other.m_node=nullptr;
			other.m_isRoot=false;
		}

		explicit JsonImpl(const char* data, ErrorHandlerMode mode=ErrorHandlerMode::Exception);
		explicit JsonImpl(size_t bufferSize, ErrorHandlerMode mode=ErrorHandlerMode::Exception);

		JsonImpl(NodeJson* node, JsonObjBuffer* jsonBufferPtr, ErrorHandlerMode mode=ErrorHandlerMode::Exception) __attribute__((always_inline))
		: m_jsonBufferPtr(jsonBufferPtr)
		, m_node(node)
		, m_errorHandler(mode)
		, m_isRoot(false)
		{
			if(!node){
				m_errorHandler.setError(ErrorCode::error20);
			}
		}

		explicit JsonImpl(ErrorHandlerMode mode);

		JsonImpl(const JsonImpl& other)=delete;
		JsonImpl& operator=(const JsonImpl&)=delete;

		JsonImpl(ErrorHandlerMode mode, ErrorCode error)
		: JsonImpl(mode)
		{
			m_errorHandler.setError(error);
		}

		NodeJson* nodeAt(uint idx) const __attribute__((always_inline));

		template<typename T, typename FUNC>
		void initArray(std::initializer_list<T>&& list, FUNC cbk);
		
		NodeJson* find(const char* key) const;
		
		template<typename T, typename FUNC>
		void pushBackData(T&& data, FUNC cbk);

		JsonImpl* onPath(JsonImpl* jsonObj, std::string& path, size_t offset);
		
		bool failWhen(bool a, ErrorCode errorCode) const	__attribute__((always_inline));

	friend JsonParser;
};

//--------------------------------------------------------------------

class JsonParser
{
	public:
		static JsonImpl* parse(const char* str, ErrorHandlerMode mode=ErrorHandlerMode::Exception)
		{
			JsonImpl* JsonImplPtr=new JsonImpl(str, mode);

			parserLoop(JsonImplPtr->m_jsonBufferPtr, JsonImplPtr->m_node, JsonImplPtr->m_errorHandler);

			return JsonImplPtr;
		}

		static JsonImpl* initObj(ErrorHandlerMode mode=ErrorHandlerMode::Exception)
		{
			return new JsonImpl(mode);
		}
		
		static JsonImpl* parse(const char8_t* str, ErrorHandlerMode mode=ErrorHandlerMode::Exception)
		{
			return parse(reinterpret_cast<const char*>(str), mode);
		}

		static JsonImpl* openJsonFile(const char* jsonFileName, ErrorHandlerMode mode=ErrorHandlerMode::Exception);
		
		static std::string utf8Encode(const char* cstr);

	private:	

		class SyntaxRules
		{
			public:
				void setRuleInitial() __attribute__((always_inline))
				{
					m_rule=Rule::initial;
				}

				bool syntaxRuleInitial() __attribute__((always_inline))
				{
					return m_rule==Rule::initial;
				}

				void setRuleKey() __attribute__((always_inline))
				{
					m_rule=Rule::key;
				}

				bool syntaxRuleKey() __attribute__((always_inline))
				{
					return m_rule & Rule::key;
				}

				void setRuleObj() __attribute__((always_inline))
				{
					m_rule=Rule::obj;
				}

				bool syntaxRuleObj() __attribute__((always_inline))
				{
					return (m_rule & Rule::ready) | (m_rule & Rule::curl);
				}

				void setRuleArr() __attribute__((always_inline))
				{
					m_rule=Rule::arr;
				}

				bool syntaxRuleArr() __attribute__((always_inline))
				{
					return (m_rule & Rule::ready) | (m_rule & Rule::squrt);
				}

				void setRuleValue() __attribute__((always_inline))
				{
					m_rule=Rule::value;
				}

				bool syntaxRuleValue() __attribute__((always_inline))
				{
					return m_rule & Rule::value;
				}

				void setRuleReady(bool factor=true) __attribute__((always_inline))
				{
					m_rule=Rule::closing;
					if(factor){
						m_rule=Rule::ready;
					}
				}

				bool syntaxRuleReady() __attribute__((always_inline))
				{
					return m_rule & Rule::ready;
				}

				void setRuleComma() __attribute__((always_inline))
				{
					m_rule=Rule::comma;
				}

				bool syntaxRuleComma() __attribute__((always_inline))
				{
					return m_rule==Rule::comma;
				}

				void setRuleColon() __attribute__((always_inline))
				{
					m_rule=Rule::colon;
				}

				bool syntaxRuleColon() __attribute__((always_inline))
				{
					return m_rule==Rule::colon;
				}
				
				/*bool syntaxRuleValueInitial() __attribute__((always_inline))
				{
					return m_rule & (Rule::value | Rule::initial);
				}*/

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

		static int parseSpecial(char* cstr, ErrorReporting& errorHandler, NodeJson* node);
		static int parseNumeric(char* cstr, ErrorReporting& errorHandler, NodeJson* node);

		static int parseValueData(char* cstr, ErrorReporting& errorHandler, NodeJson* node);
		static void removeEmptyIndex(NodeJson* node);
};

//--------------------------------------------------------------------

void JsonParser::removeEmptyIndex(NodeJson* node)
{
	/*
	 * An array that has at least one element
	 * */

	//if(node->isArray()){// we already check that it is an array
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

inline JsonImpl::JsonImpl(const char* data, ErrorHandlerMode mode)
: m_jsonBufferPtr(new JsonObjBuffer(data))
, m_node(NodeJson::allocateNode())
, m_errorHandler(mode)
, m_isRoot(true)
{
	m_node->setAsObj();
}

//--------------------------------------------------------------------

inline JsonImpl::JsonImpl(ErrorHandlerMode mode)
: JsonImpl(" ", mode)
{
}

//--------------------------------------------------------------------

inline JsonImpl::JsonImpl(size_t bufferSize, ErrorHandlerMode mode)
: m_jsonBufferPtr(new JsonObjBuffer(" ", bufferSize))
, m_node(NodeJson::allocateNode())
, m_errorHandler(mode)
, m_isRoot(true)
{
	m_node->setAsObj();
}

//--------------------------------------------------------------------

inline JsonImpl::~JsonImpl()
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

inline bool JsonImpl::failWhen(bool a, ErrorCode errorCode) const
{
	if(a){
		m_errorHandler.setError(errorCode);
		return true;
	}
	return false;
}

//--------------------------------------------------------------------

inline std::string JsonImpl::toString(bool pretty) const
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

inline bool JsonImpl::hasKey(const char* key) const
{
	if(!failWhen(m_node->isArray(), ErrorCode::error13)){
		NodeJson::AVL_Tree tree(*m_jsonBufferPtr);
		return tree.find(m_node, key)!=nullptr;
	}
	return false;
}

//--------------------------------------------------------------------

inline const char* JsonImpl::getRawData() const
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
std::optional<T> JsonImpl::getValue() const
{
	const char* data=getRawData();
	if(!data){
		return std::nullopt;
	}

	return FromString<T>::getFrom(data);
}
// */
//--------------------------------------------------------------------

template<>
std::optional<json_null> JsonImpl::getValue<json_null>() const [[maybe_unused]]
{
	const char* data=getRawData();

	if(!data || m_node->getDataMode()!=JSON_TYPES::_NULL || 0!=m_jsonBufferPtr->comparing("null", m_node->getOffset())){
		m_errorHandler.setError(ErrorCode::error23);
		return std::nullopt;
	}

	return FromString<json_null>::getFrom(data);
}

//--------------------------------------------------------------------

inline void JsonImpl::operator=(easyjson::JsonValue&& val)
{
	m_node->clear();
	easyjson::setData(val, m_node, *m_jsonBufferPtr);
}

//--------------------------------------------------------------------

/* Attaches array to key
 * obj[a]={b, c, 2, 3} --> a:[b, c, 2, 3] 
 * */
inline void JsonImpl::operator=(std::initializer_list<easyjson::JsonValue>&& list)
{
	initArray(std::move(list), [this](const JsonValue& data, NodeJson* node){
		easyjson::setData(data, node, *m_jsonBufferPtr);
	});
}

//--------------------------------------------------------------------

/* Attaches array of objs to key
 *  obj[a]={{b, 1}, {c, 2}, {d, 3}} --> a:[{b:1}, {c:2}, {d:3}] 
 * */
inline void JsonImpl::operator=(std::initializer_list<easyjson::JsonPair>&& list)
{	
	NodeJson::AVL_Tree tree(*m_jsonBufferPtr);
	initArray(std::move(list), [&tree, this](const JsonPair& data, NodeJson* node){
		node->setAsObj();
		NodeJson* keyNode=node->addKeyNode();
		easyjson::setData(data, keyNode, *m_jsonBufferPtr);
		tree.insertAt(node, keyNode);
	});
}

//--------------------------------------------------------------------

template<typename T, typename FUNC>
void JsonImpl::initArray(std::initializer_list<T>&& list, FUNC cbk)
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
inline void JsonImpl::pushBack(easyjson::JsonValue&& data)
{
	if(!failWhen(m_node->isObj() || m_node->m_offset>0, ErrorCode::error15)){
		if(!m_node->isArray()){
			m_node->setAsArray();
		}

		NodeJson* itemNode=m_node->addArrayItem();

		easyjson::setData(data, itemNode, *m_jsonBufferPtr);
	}
}

//--------------------------------------------------------------------

// Insert an array into an array
inline void JsonImpl::pushBack(std::initializer_list<easyjson::JsonValue>&& list)
{
	pushBackData(std::move(list), [this](std::initializer_list<easyjson::JsonValue>&& list, NodeJson* node){
		node->setAsArray();
		JsonImpl obj(node, m_jsonBufferPtr);
		obj=std::move(list);
	});
}

//--------------------------------------------------------------------

// append objects to array
inline void JsonImpl::pushBack(easyjson::JsonBulkList&& data)
{
	pushBackData(std::move(data), [this](easyjson::JsonBulkList&& data, NodeJson* node){
		node->setAsObj();
		JsonImpl obj(node, m_jsonBufferPtr);
		obj=std::move(data);
	});
}

//--------------------------------------------------------------------

template<typename T, typename FUNC>
inline void JsonImpl::pushBackData(T&& data, FUNC cbk)
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
inline void JsonImpl::pushBack(json_array)
{
	pushBack({});
}
//--------------------------------------------------------------------

// append empty object to array
inline void JsonImpl::pushBack(json_obj)
{
	pushBack({JSON_OBJ, {}});
}

//--------------------------------------------------------------------

//add {key:val} to obj
inline void JsonImpl::append(easyjson::JsonPair&& data)
{
	if(!failWhen(m_node->isArray(), ErrorCode::error16)){
		if(!failWhen(hasKey(data.m_key),ErrorCode::error17)){
			m_node->setAsObj();
			NodeJson* node=m_node->addKeyNode();
	
			easyjson::setData(data, node, *m_jsonBufferPtr);
			NodeJson::AVL_Tree tree(*m_jsonBufferPtr);
			tree.insertAt(m_node, node); // we already check for the key
		}
	}
}

//--------------------------------------------------------------------

// User responsibility to check vector size/range
inline NodeJson* JsonImpl::nodeAt(uint idx) const
{
	if(failWhen(!m_node->isArray(), ErrorCode::error14)){
		return nullptr;
	}

	NodeJson::VectWrapper* vect=reinterpret_cast<NodeJson::VectWrapper*>(m_node->m_child);

	return vect->get(idx);
}

//--------------------------------------------------------------------

inline void JsonImpl::removeKey(const char* key)
{
	if(failWhen(!m_node->isObj(), ErrorCode::error13)){
		return;
	}

	NodeJson::AVL_Tree tree(*m_jsonBufferPtr);
	tree.remove(key, m_node);
}

//--------------------------------------------------------------------

inline void JsonImpl::removeFromArray(size_t idx, bool shift)
{
	if(failWhen(!m_node->isArray(), ErrorCode::error13)){
		return;
	}

	NodeJson::VectWrapper* vect=reinterpret_cast<NodeJson::VectWrapper*>(m_node->m_child);

	vect->remove(idx, shift);
}

//--------------------------------------------------------------------

inline size_t JsonImpl::size() const
{
	if(failWhen(!m_node->isArray(), ErrorCode::error14)){
		return size_t(-1);
	}

	NodeJson::VectWrapper* vect=reinterpret_cast<NodeJson::VectWrapper*>(m_node->m_child);

	return vect->size();
}

//====================================================================

}// simplejson namespace

//====================================================================

using namespace easyjson;
using namespace easyjson::internal;


//--------------------------------------------------------------------

std::string JsonParser::utf8Encode(const char* cstr)
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

inline int JsonParser::parseValueData(char* cstr, ErrorReporting& errorHandler, NodeJson* node)
{
	if(cstr[0]!='-' && (int(cstr[0])<48 || int(cstr[0])>57)){
		size_t k=0;
		bool a=true;
		if(cstr[0]=='n'){
			k=4;
			a=cstr[1]=='u' && cstr[2]=='l' && cstr[3]=='l';
			node->setDataMode(JSON_TYPES::_NULL); 
		}
		else if(cstr[0]=='t'){
			k=4;
			a=cstr[1]=='r' && cstr[2]=='u' && cstr[3]=='e';
			node->setDataMode(JSON_TYPES::_BOOL);
		}
		else if(cstr[0]=='f'){
			k=5;
			a=cstr[1]=='a' && cstr[2]=='l' && cstr[3]=='s' && cstr[4]=='e';
			node->setDataMode(JSON_TYPES::_BOOL);
		}
		else{ //k==0
			errorHandler.setError(ErrorCode::error1);
			return -1;
		}

		if(cstr[k]==0 || !a){
			errorHandler.setError(ErrorCode::error1);
			return -1;
		}

		if(int(cstr[k])<33){// && std::isspace(static_cast<unsigned char>(cstr[k]))){
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
			//i++;
			continue;
		}

		if(cstr[i]=='+'){
			if(0==(format & NumFormat::eCount) || format & NumFormat::bad){
				errorHandler.setError(ErrorCode::error5);
				return -1;
			}
			format=format|NumFormat::pst;
			//i++;
			continue;
		}

		if(cstr[i]=='e' || cstr[i]=='E'){
			if(format & NumFormat::eCount){
				errorHandler.setError(ErrorCode::error5);
				return -1;
			}
			format=format&~NumFormat::neg;
			format=format | NumFormat::eCount | NumFormat::dots;// dots are not allowed after e/E
			//i++;
			continue;
		}

		if(cstr[i]=='.'){
			if(format & NumFormat::dots){
				errorHandler.setError(ErrorCode::error5);
				return -1;
			}
			format=format|NumFormat::dots;
			node->setDataMode(JSON_TYPES::_DOUBLE);
			//i++;
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

void JsonParser::parserLoop(JsonObjBuffer* jsonObjBufferPtr, NodeJson* node, ErrorReporting& errorHandler)
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
	//*
	int i=0;
	while(buffer[i]){ // ok...
		if(buffer[i]<33){ // we discart any white character outside a key or value
			i++;
			continue;
		}// */
	/*
	int i=-1;
	START_LOOP:
	{
		while(buffer[++i]<33 && buffer[i]>0);
		if(buffer[i]==0){
			goto FINISH_JSON;
		}
		
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

JsonImpl* JsonParser::openJsonFile(const char* jsonFileName, ErrorHandlerMode mode)
{
	JsonImpl* JsonImplPtr=nullptr;

	size_t fileLength=0;
	//profiler.start();
	
	//=================
	#if 0
	FILE* file = fopen(jsonFileName, "rb");
	if (!file) {
		fprintf(stderr, "Failed to open file\n");
		//return 1;
	}
	fseek(file, 0, SEEK_END);
	size_t length = ftell(file);
	fseek(file, 0, SEEK_SET);

			JsonImplPtr=new JsonImpl(length+1, mode);
			
			char* buffer=JsonImplPtr->m_jsonBufferPtr->m_buffer.data();
			buffer[fileLength]=0;
	
	//char* buffer = new char[length];
	if (length == fread(buffer, 1, length, file)) {
		fclose(file);
		parserLoop(JsonImplPtr->m_jsonBufferPtr, JsonImplPtr->m_node, JsonImplPtr->m_errorHandler);
		return JsonImplPtr;
	}
	fclose(file);
	#endif
	//=================
	
	#if 1
	std::ifstream jsonFile(jsonFileName);
	if(jsonFile.is_open()){
		jsonFile.seekg(0, std::ios_base::end);
		std::ifstream::pos_type endPos = jsonFile.tellg();
		jsonFile.seekg(0, std::ios_base::beg);

		fileLength=endPos-jsonFile.tellg();
	
      if(fileLength>1){
			JsonImplPtr=new JsonImpl(fileLength+1, mode);
			
			char* buffer=JsonImplPtr->m_jsonBufferPtr->m_buffer.data();

			jsonFile.read(buffer, fileLength);
			buffer[fileLength]=0;
			
			if(jsonFile){
				jsonFile.close();
				parserLoop(JsonImplPtr->m_jsonBufferPtr, JsonImplPtr->m_node, JsonImplPtr->m_errorHandler);
				//profiler.stop();
				return JsonImplPtr;
			}
		}
		jsonFile.close();
	}
	#endif

	JsonImplPtr=new JsonImpl(mode);
	ErrorCode error=ErrorCode::error22;
	if(fileLength<2){
		error=ErrorCode::error25;
	}
	JsonImplPtr->m_errorHandler.setError(error);

	return JsonImplPtr;
}

//--------------------------------------------------------------------

/* find will return the node attached (child) to the node with the passed key 
 * if there exists, otherwhise it will create a new entry with empty child
 * */
NodeJson* JsonImpl::find(const char* key) const
{
	if(failWhen(m_node->isArray(), ErrorCode::error13)){
		return nullptr;
	}
	
	if(m_node->isDangling()){
		m_node->setAsObj();
	}

	NodeJson* node=NodeJson::AVL_Tree::find(*m_jsonBufferPtr, m_node, key);
	
	if(node){
		return node->m_child;
	}
	
	NodeJson::AVL_Tree tree(*m_jsonBufferPtr);

	node=m_node->addKeyNode(*m_jsonBufferPtr, key);

	if(failWhen(!tree.insertAt(m_node, node), ErrorCode::error17)){
		return nullptr;
	}

	return node->addBlankChild();
}

//--------------------------------------------------------------------

//obj[key]={JSO::OBJ, {{a, b}, {c,d}, {e:f}} -> {key: {a:b, c:d, e:f}}
//m_node->m_child->m_child(obj)
void JsonImpl::operator=(easyjson::JsonBulkList&& data)
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

		easyjson::setData(nodeData, node, *m_jsonBufferPtr);

		tree.insertAt(m_node, node);
	}
}

//--------------------------------------------------------------------

JsonImpl* JsonImpl::onPath(JsonImpl* jsonObj, std::string& path, size_t offset)
{
	const char* key=path.data()+offset;
	size_t pos=path.find_first_of('/', offset);
	if(pos!=std::string::npos){
		path[pos++]=0;
		if(jsonObj->isArray()){
			return onPath(jsonObj->operator[](std::atoi(key)), path, pos);
		}
		return onPath(jsonObj->operator[](key), path, pos);
	}

	if(jsonObj->isArray()){
		return jsonObj->operator[](std::atoi(key));
	}

	return jsonObj->operator[](key);
}

//--------------------------------------------------------------------

JsonImpl* JsonImpl::follow(const char* str)
{
	std::string path=str;
	size_t pos=path.find_first_of('/');
	if(pos!=std::string::npos){
		path[pos++]=0;
		const char* key=path.data();
		JsonImpl* jsonObj=operator[](key);
		return onPath(jsonObj, path, pos);
	}
	return operator[](str);
}

//====================================================================
//====================================================================

JsonObj::~JsonObj()
{
	if(m_impl){
		delete m_impl;
		m_impl=nullptr;
	}
}

JsonObj JsonObj::parse(const char* str, ErrorHandlerMode mode)
{
	return JsonParser::parse(str, mode);
}

JsonObj JsonObj::initObj(ErrorHandlerMode mode)
{
	return JsonParser::initObj(mode);
}

JsonObj JsonObj::parseJsonFile(const char* jsonFileName, ErrorHandlerMode mode)
{
	
	return {JsonParser::openJsonFile(jsonFileName, mode)};
}

bool JsonObj::hasKey(const char* key) const
{
	return m_impl->hasKey(key);	
}

const JsonObj JsonObj::operator[](const char* key) const
{
	return const_cast<JsonImpl*>(m_impl->operator[](key));
}

JsonObj JsonObj::operator[](const char* key)
{
	return m_impl->operator[](key);	
}

//to query arrays
const JsonObj JsonObj::operator[](int idx) const
{
	return m_impl->operator[](idx);
}

JsonObj JsonObj::operator[](int idx)
{
	return m_impl->operator[](idx);	
}

JsonObj JsonObj::follow(const char* str)
{
	return m_impl->follow(str);	
}

void JsonObj::operator=(json_obj obj)
{
	m_impl->operator=(obj);		
}

void JsonObj::operator=(json_array array)
{
	m_impl->operator=(array);	
}

void JsonObj::operator=(easyjson::JsonValue&& val)
{
	m_impl->operator=(std::forward<easyjson::JsonValue>(val));
}

void JsonObj::operator=(std::initializer_list<easyjson::JsonValue>&& list)
{
	m_impl->operator=(std::forward<std::initializer_list<easyjson::JsonValue>>(list));
}

void JsonObj::operator=(std::initializer_list<easyjson::JsonPair>&& list)
{
	m_impl->operator=(std::forward<std::initializer_list<easyjson::JsonPair>>(list));
}

void JsonObj::operator=(easyjson::JsonBulkList&& data)
{
	m_impl->operator=(std::forward<easyjson::JsonBulkList>(data));
}

//for arrays
void JsonObj::pushBack(easyjson::JsonValue&& data)
{
	m_impl->pushBack(std::forward<easyjson::JsonValue>(data));
}

void JsonObj::pushBack(std::initializer_list<easyjson::JsonValue>&& list)
{
	m_impl->pushBack(std::forward<std::initializer_list<easyjson::JsonValue>>(list));
}

void JsonObj::pushBack(easyjson::JsonBulkList&& data)
{
	m_impl->pushBack(std::forward<easyjson::JsonBulkList>(data));
}

void JsonObj::pushBack(json_obj jsonObj)
{
	m_impl->pushBack(jsonObj);
}

void JsonObj::pushBack(json_array jsonArray)
{
	m_impl->pushBack(jsonArray);
}

//for obj
void JsonObj::append(easyjson::JsonPair&& data)
{
	m_impl->append(std::forward<easyjson::JsonPair>(data));
}

const char* JsonObj::getRawData() const
{
	return m_impl->getRawData();
}

void JsonObj::removeKey(const char* key)
{
	m_impl->removeKey(key);
}

void JsonObj::removeFromArray(size_t idx, bool shift)
{
	m_impl->removeFromArray(idx, shift);
}

bool JsonObj::isObj() const
{
	return m_impl->isObj();
}

bool JsonObj::isArray() const
{
	return m_impl->isArray();
}

bool JsonObj::isNull() const
{
	return m_impl->isNull();
}

std::string JsonObj::toString(bool prettyStr) const
{
	return m_impl->toString(prettyStr);
}

bool JsonObj::isString() const
{
	return m_impl->isString();
}

bool JsonObj::isBoolean() const
{
	return m_impl->isBoolean();
}

bool JsonObj::isNumeric() const
{
	return m_impl->isNumeric();
}

bool JsonObj::isValid() const
{
	return m_impl->isValid();
}

std::string JsonObj::getErrorMsg() const
{
	return m_impl->getErrorMsg();
}

size_t JsonObj::size() const
{
	return m_impl->size();
}

//--------------------------------------------------------------------

std::string JsonObj::utf8Encode(const char* cstr)
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

//====================================================================
