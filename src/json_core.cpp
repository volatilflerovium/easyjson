/*********************************************************************
* Implementation of NodeJson struct                            		*
*                                                                    *
* Version: 1.0                                                       *
* Date:    23-09-2019                                                *
* Author:  Dan Machado                                               *
**********************************************************************/
#include "easyjson/internal/json_core.h"

namespace easyjson{
using namespace easyjson::internal;

//--------------------------------------------------------------------

size_t JsonObjBuffer::addData(const char* data, size_t inOffset)
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
	
	m_buffer+=" ";
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

void unescape(std::string& str, size_t initialPos)
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

void NodeJson::printMore(const JsonObjBuffer& jsonBufferRef, const std::string& spacer, const int padding, std::string& str, int indentation) const
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

void NodeJson::prettify(const JsonObjBuffer& jsonBufferRef, std::string& str, int indentation, const std::string& spacer, const int padding) const
{
	if(isKey()){
		if(m_left){
			m_left->prettify(jsonBufferRef, str, indentation, spacer, padding);	
			str+=", "+spacer;
		}
		printMore(jsonBufferRef, spacer, padding, str, indentation);
		if(m_right){
			str+=", "+spacer;
			m_right->prettify(jsonBufferRef, str, indentation, spacer, padding);
		}
	}
	else if(!isArray() && !isObj()){
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
	else if(isObj()){
		if(m_child){
			str+="{"+spacer;
			m_child->prettify(jsonBufferRef, str, indentation+padding, spacer, padding);
			/*if(m_child->m_left){
				m_child->m_left->prettify(jsonBufferRef, str, indentation+padding, spacer, padding);
				str+=", "+spacer;
			}
			m_child->printMore(jsonBufferRef, spacer, padding, str, indentation+padding);
			if(m_child->m_right){
				str+=", "+spacer;
				m_child->m_right->prettify(jsonBufferRef, str, indentation+padding, spacer, padding);
			}// */
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

void NodeJson::AVL_Tree::balance(NodeJson* node, int diff, NodeJson* top)
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

int16_t NodeJson::AVL_Tree::insert(NodeJson* root, NodeJson* node, NodeJson* top)
{
	int y=m_jsonBufferRef.compare(node->m_offset, root->m_offset);

	int16_t x=1;

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
	
	root->m_height=std::max(root->m_height, x);

	int diff=root->diff();
	if(std::abs(diff)>1){
		balance(root, diff, top);
	}
	
	return root->m_height;
}

//--------------------------------------------------------------------

void NodeJson::AVL_Tree::removeNode(NodeJson* node, NodeJson* top)
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

bool NodeJson::AVL_Tree::removeNode(const char* key, NodeJson* node, NodeJson* top)
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

void NodeJson::AVL_Tree::remove(const char* key, NodeJson* obj)
{
	m_root=obj->m_child;
	if(m_root){
		removeNode(key, m_root, nullptr);
		obj->m_child=m_root;
	}
}

//====================================================================
}
