/*********************************************************************
* JsonObjBuffer class                          								*
* NodeJson struct                                                    *
*                                                                    *
* Version: 1.0                                                       *
* Date:    23-09-2019                                                *
* Author:  Dan Machado                                               *                                         *
**********************************************************************/
#ifndef JSON_CORE_H
#define JSON_CORE_H

#include <string>
#include <cstring>
#include <vector>
#include <functional>

#include "easyjson/internal/custom_allocator.h"

#include "easyjson/internal/json_utilities.h"


namespace easyjson
{
namespace internal
{

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

		int compare(size_t leftOffset, size_t rightOffset) const __attribute__((always_inline)) __attribute__((hot))
		{
			const char* data2 = m_buffer.data();
			return std::strcmp(data2+leftOffset, data2+rightOffset);
		}

		int comparing(const char* val, size_t offset) const __attribute__((always_inline)) __attribute__((hot))
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

		JsonObjBuffer(const char* str, size_t bufferSize=0)
		{
			if(bufferSize){
				m_buffer.resize(bufferSize);
			}
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

	friend class easyjson::JsonImpl;
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

	friend class easyjson::JsonImpl;
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
	m_mode=NodeMode::Array;
	m_dataMode=JSON_TYPES::_NA;

	m_child=reinterpret_cast<NodeJson*>(s_vectPool.construct());
}

//--------------------------------------------------------------------

// Responsibility of caller to check if the node is array
inline NodeJson* NodeJson::addArrayItem()
{
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

//====================================================================

}//internal
} // microjson namespace

#endif
