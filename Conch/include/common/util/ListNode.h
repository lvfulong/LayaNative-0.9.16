/**
@file			ListNode.h
@brief			
@author			James
@version		1.0
@date			2016_5_12
*/

#ifndef __ListNode_H__
#define __ListNode_H__

#include <stdio.h>

namespace laya
{
	class ListNode
    {
	public:
        ListNode* _Prev;
        ListNode* _Next;
		int id; // for debug
        ListNode(){
			_Prev=_Next=this;
			static int n=0;
			id=n++;
		}
		virtual ~ListNode(){
			//�������������ɾ��
            ListNode* pPrev = _Prev;
            ListNode* pNext = _Next;
			pPrev->_Next = _Next;
			pNext->_Prev = _Prev;
			_Next = this;
			_Prev = this;
		}
		inline bool notInChain(){
			return _Prev==this && _Next==this;
		}
	};

    /** 
     * @brief ��Ϊstl��list��ɾ�������ʱ��̫���㣬��Ҫ�����ҵ�iterator���������Լ�дһ��
    */
	class simpList
    {
	protected:
        ListNode* pHead;	//pHeadֻ�Ǳ�ʾͷ��������Ч����
		int		nSize;
	public:
		simpList(){
			pHead = new ListNode();
			nSize = 0;
		}
		~simpList(){
			delete pHead;
			pHead = 0;
			nSize = 0;
		}
		int size(){
			return nSize;
		}
		//�����Ҫ��ʹ���ˡ���Ϊ��stl�ı�׼��һ�¡�
        ListNode* getHead(){
			return pHead;
		}
		//begin����Чֵ����������listû�����ݣ���������end()
        ListNode* begin(){
			return pHead->_Next;
		}
		//end����Чֵ������ָ��pHead
        ListNode* end(){
			return pHead;
		}
		void push_back(ListNode* pNode ){
            ListNode* pTail = pHead->_Prev;
			if( pTail==pNode)
				return;
			pTail->_Next = pNode;
			pNode->_Prev = pTail;
			pNode->_Next = pHead;
			pHead->_Prev = pNode;
			nSize++;
		}
        ListNode* pop_back(){
            ListNode* pTail = pHead->_Prev;
            ListNode* pPrev = pTail->_Prev;
			//pnext = phead
			pPrev->_Next = pHead;
			pHead->_Prev = pPrev;
			pTail->_Next= pTail;
			pTail->_Prev= pTail;
			nSize--;
			if(nSize<0)nSize=0;
			return pTail;
		}
		void push_front(ListNode* pNode){
			if( pNode==pHead->_Next)
				return;
			pNode->_Next = pHead->_Next;
			pHead->_Next->_Prev=pNode;
			pHead->_Next=pNode;
			pNode->_Prev = pHead;

			nSize++;
		}
        ListNode* pop_front(){
            ListNode* pDel = pHead->_Next;
			pDel->_Next->_Prev = pHead;
			pHead->_Next = pDel->_Next;
			pDel->_Prev=pDel;
			pDel->_Next=pDel;
			nSize--;
			if(nSize<0)nSize=0;
			return pDel;
		}
		//������һ���ڵ�
        ListNode* delNode(ListNode* pNode){
			if( pNode->notInChain() )
				return NULL;
            ListNode* pPrev = pNode->_Prev;
            ListNode* pNext = pNode->_Next;
			pPrev->_Next = pNode->_Next;
			pNext->_Prev = pNode->_Prev;
			pNode->_Next = pNode;
			pNode->_Prev = pNode;
			nSize--;
			if(nSize<0)nSize=0;
			return pNext;
		}
		void clear(){
		}
	};
}
//------------------------------------------------------------------------------


#endif //__ListNode_H__

//-----------------------------END FILE--------------------------------
