#pragma once
#include <stdio.h>
#include <stddef.h>
#include <assert.h>
#include <stdlib.h>
//#include <corecrt_malloc.h>
#include <string.h>
//#include <cassert>

typedef char DataType;
char* charTrim(char* str);

//单链节点
typedef struct List
{
	DataType data[100];
	struct List* next;

} List;

// 初始化单链节点
void init(List **head) {
	assert(head);
	*head = NULL;
}

//创建新结点
List* creatNode(DataType data[])
{
	List* pNewNode = (List*)osal_mem_alloc(sizeof(List));
	assert(pNewNode);

	strcpy(pNewNode->data, data);
	pNewNode->next = NULL;
	return pNewNode;
}

//尾插函数
void append(List** head, DataType data[])
{
	List* NewNode = creatNode(data);

	assert(head);
	//如果链表为空，则直接让头指针指向新申请的节点即可
	if (*head == NULL)
	{
		*head = NewNode;
		return;
	}
	//否则从头开始遍历链表，直到当前节点的指针域指向NULL，然后让当前节
	//点的指针域指向新申请的节点即可
	List* pTail = *head;
	while (pTail->next)
	{
		pTail = pTail->next;
	}
	pTail->next = NewNode;
}

//销毁结点  //销毁所有节点
void destroyAll(List** pHead)
{
	assert(pHead);
	List* pDpre = NULL;
	List* pD = *pHead;
	//定义两个指针同时向链尾走，pD不为空时，用pDpre标记pD
	//pD不断向后移同时释放pDpre，直至将整个链表结点全部释放
	while (pD)
	{
		pDpre = pD;
		pD = pD->next;
		osal_mem_free(pDpre);
	}
	*pHead = NULL;
}

//遍历打印链表
void printList(List** pHead)
{
	List* pCur = *pHead;
	for (pCur; pCur != NULL; pCur = pCur->next)
	{
              HalUARTWrite(HAL_UART_PORT_0, pCur->data, sizeof(pCur->data) - 1);

		//printf("%s\n", pCur->data);
	}
}


//求单链表长度
uint8 listSize(List** pHead)
{
	assert(pHead);
	List* pSize = *pHead;
	uint8 count = 0;
	while (pSize != NULL)
	{
		pSize = pSize->next;
		count++;
	}
	return count;
}


//获取链表某一位置的信息
List* get(List** pHead, uint8 position)
{
	assert(pHead);
	//考虑链表为空的情况
	if (*pHead == NULL)
	{
		//printf("链表为空！无法查找！\n");
		return NULL;
	}
	List* pFind = *pHead;
	uint8 i;
	if (position <= listSize(pHead)) {
		for (i = 0; i < position; i++) {
			pFind = pFind->next;
		}
		return pFind;
	}
	else {
		//printf("链表越界");
		return NULL;
	}
}



//打印链表节点
void printListNode(List** pHead, uint8 pos)
{
	List* pCur = get(pHead, pos);
	//printf("this node data:%s\n", pCur->data);

}

//判空
uint8 isEmpty(List** pHead)
{
	assert(pHead);
	return ((*pHead) == NULL);
}



//删除链表空行与节点信息空格
void trim(List** pHead) {
	assert(pHead);
	uint8 i = 0;
	while (pHead) {
		char* dataT = charTrim((*pHead)->data);
		if (strcmp(dataT, "") != 0) {
			break;
		}
		else {
			*pHead = (*pHead)->next;
		}
	}

	List* pBefore = *pHead;
	List* pAfter = pBefore->next;
	i = 0;
	while (pAfter) {
		char* dataT = charTrim(pAfter->data);
		if (strcmp(dataT, "") != 0) {
			pBefore = pAfter;
			pAfter = pAfter->next;
		}
		else {
			if (pAfter->next) {
				pBefore->next = pAfter->next;
			}
			else {
				pBefore->next = NULL;
			}
			pAfter = pAfter->next;
		}
	}

}

//删除字符串空格
char* charTrim(char* str) {
	uint8 i, j = 0;
	//char* result = str;
	char* p = str;
	char* r = str;
	for (i = 0; i < strlen(str);) {
		if (*p != ' ') {
			*(r++) = *(p++);
		}
		else {
			while (*p == ' ') {
				p++;
				i++;
			}
			*(r++) = *(p++);
		}
		i++;
	}
	*(r++) = '\0';
	return str;

}
/*
void strcat(char* des, char* src) {
	char* d = des;
	char* s = src;
	//char result[999] = "";
	while (*d != NULL) {
		d++;
	}
	while (*s != NULL) {
		if (!d) {
			break;
		}
		else {
			*d++ = *s++;
		}
	}
	*d++ = '\0';
}
*/
char* toString(List** pHead) {

	List* pCur = *pHead;
	char str[100] = "";
	uint8 size = 0;

	if (pHead != NULL) {
		//for (pCur; pCur != NULL; pCur = pCur->next)
		//{
		//	 size += strlen(pCur->data);
		// }
		// size++;
		// pCur = *pHead;
		//str = (char*)malloc(sizeof(char)*size);
		//*str = 0;
		for (; pCur != NULL; pCur = pCur->next)
		{
			//strcat_s(str, strlen(str) + strlen(pCur->data) + 1, pCur->data);
			strcat(str, pCur->data);
		}
		return str;

	}
	else
	{
		return NULL;
	}

}

