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

//�����ڵ�
typedef struct List
{
	DataType data[100];
	struct List* next;

} List;

// ��ʼ�������ڵ�
void init(List **head) {
	assert(head);
	*head = NULL;
}

//�����½��
List* creatNode(DataType data[])
{
	List* pNewNode = (List*)osal_mem_alloc(sizeof(List));
	assert(pNewNode);

	strcpy(pNewNode->data, data);
	pNewNode->next = NULL;
	return pNewNode;
}

//β�庯��
void append(List** head, DataType data[])
{
	List* NewNode = creatNode(data);

	assert(head);
	//�������Ϊ�գ���ֱ����ͷָ��ָ��������Ľڵ㼴��
	if (*head == NULL)
	{
		*head = NewNode;
		return;
	}
	//�����ͷ��ʼ��������ֱ����ǰ�ڵ��ָ����ָ��NULL��Ȼ���õ�ǰ��
	//���ָ����ָ��������Ľڵ㼴��
	List* pTail = *head;
	while (pTail->next)
	{
		pTail = pTail->next;
	}
	pTail->next = NewNode;
}

//���ٽ��  //�������нڵ�
void destroyAll(List** pHead)
{
	assert(pHead);
	List* pDpre = NULL;
	List* pD = *pHead;
	//��������ָ��ͬʱ����β�ߣ�pD��Ϊ��ʱ����pDpre���pD
	//pD���������ͬʱ�ͷ�pDpre��ֱ��������������ȫ���ͷ�
	while (pD)
	{
		pDpre = pD;
		pD = pD->next;
		osal_mem_free(pDpre);
	}
	*pHead = NULL;
}

//������ӡ����
void printList(List** pHead)
{
	List* pCur = *pHead;
	for (pCur; pCur != NULL; pCur = pCur->next)
	{
              HalUARTWrite(HAL_UART_PORT_0, pCur->data, sizeof(pCur->data) - 1);

		//printf("%s\n", pCur->data);
	}
}


//��������
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


//��ȡ����ĳһλ�õ���Ϣ
List* get(List** pHead, uint8 position)
{
	assert(pHead);
	//��������Ϊ�յ����
	if (*pHead == NULL)
	{
		//printf("����Ϊ�գ��޷����ң�\n");
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
		//printf("����Խ��");
		return NULL;
	}
}



//��ӡ����ڵ�
void printListNode(List** pHead, uint8 pos)
{
	List* pCur = get(pHead, pos);
	//printf("this node data:%s\n", pCur->data);

}

//�п�
uint8 isEmpty(List** pHead)
{
	assert(pHead);
	return ((*pHead) == NULL);
}



//ɾ�����������ڵ���Ϣ�ո�
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

//ɾ���ַ����ո�
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

