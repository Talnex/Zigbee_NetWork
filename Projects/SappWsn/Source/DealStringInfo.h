#pragma once
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "List.h"


char substr2[255] = "";

List* splitStr(char *str);
uint8 judgeInfo_list(List *info, uint8 pos);

uint8 judgeInfo(char str[]);
char* substr(char str[], uint8 begin, uint8 end);
uint8 hasChar(char str[], char ch);

uint8 OK_ERROR(List* list);
char* getInfoBeforeChar(char str[], char end);
char* getInfoBeforeCharR(char str[], char* length);
//char* getInfoBeforeCharR();
char* deleteBeforeChar(char str[], char ch);
char* firstAlphaStr(char str[]);
uint8 tenSqure(uint8 time);

char* intToStr(uint8 temp) {
	 uint8 i = 0;
	 for (i = 5; i >= 0; i--) {
		 if (temp/(tenSqure(i)) < 10 && temp/tenSqure(i) > 0) {
			 break;
		 }
		 else {
			 continue;
		 }
	 }
	 char tempChr;
	 if (-1 == i) {
		 return "0";
	 }
	 else {
		 uint8 j = 0, s = 0;
		 for (j = i; j >= 0; j--) {
			 tempChr = (temp / tenSqure(j)) + '0';
			 temp = temp % tenSqure(j);
			 substr2[s++] = tempChr;
		 }
                 substr2[s] = '\0';
		 return substr2;
	 }
 }

uint8 doSomething(char* str) {

	List* info;
	List* result;

	init(&info);
        
	info = splitStr(str);
	trim(&info);
	//printList(&info);
	init(&result);
	uint8 id = judgeInfo_list(info, 0);         //
        
        return id;
        
}

char at[] = "AT";		//OK/ERROR
char rst[] = "RST";		//OK/ERROR
char cwmode_1[] = "CWMODE=1";		//OK/ERROR
char cwmode_2[] = "CWMODE=2";		//OK/ERROR
char cwmode_3[] = "CWMODE=3";		//OK/ERROR
char cwmode_question_mark[] = "CWMODE?";		//+CWMODE:1/2/3
char cipmux[] = "CIPMUX=1";		//OK/ERROR
char cipserver[] = "CIPSERVER";		//“=”得到mode， “，”得到port		
char cipstart[] = "CIPSTART";		//“=”得到id， “，""”得到type（连接类型）， “，""”得到IP， “，”得到port
char cipstatus[] = "CIPSTATUS";
char cipsend[] = "CIPSEND";		//“=”得到id， “，”得到length
char sendok[] = "SENDOK";
char error[] = "ERROR";
char arrow[] = ">";
char cwlif[] = "CWLIF";		//得到一堆IP
char cwlap[] = "CWLAP";		//OK/ERROR
char cifsr[] = "CIFSR";		//得到一个IP
char ipd[] = "+IPD";		//得到用户数据， “，”得到id， “，”得到length， “:”得到数据（最后一个“，”之前）
char connect[] = "0,CONNECT";

uint8 judgeInfo(char str[]) {
	char* type = getInfoBeforeChar(str, '+');  
                
	if (strcmp(type, at) == 0) {          
		type = deleteBeforeChar(str, '+');
		//type = firstAlphaStr(type);
		if (strcmp(type, rst) == 0) {
			return 0;
		}
		else if (strcmp(type, cwmode_1) == 0) {
			return 1;
		}
		else if (strcmp(type, cwmode_2) == 0) {
			return 2;
		}
		else if (strcmp(type, cwmode_3) == 0) {
			return 3;
		}
		else if (strcmp(type, cwmode_question_mark) == 0) {
			return 4;
		}
		else if (strcmp(type, cipmux) == 0) {
			return 5;
		}
		else if (strcmp(firstAlphaStr(type), cipserver) == 0) {
			return 6;
		}
		else if (strcmp(firstAlphaStr(type), cipstart) == 0) {
			return 7;
		}
		else if (strcmp(firstAlphaStr(type), cipstatus) == 0) {
			return 8;
		}
		else if (strcmp(firstAlphaStr(type), cipsend) == 0) {
			return 9;
		}
		else if (strcmp(firstAlphaStr(type), cwlif) == 0) {
			return 10;
		}
		else if (strcmp(firstAlphaStr(type), cwlap) == 0) {
			return 11;
		}
		else if (strcmp(firstAlphaStr(type), cifsr) == 0) {
			return 12;
		}
		else {
			//printf("不能识别的AT+指令\n");
			return 255;
		}
	}
	else if (strcmp(str, connect) == 0){
                //HalUARTWrite(0, str, strlen(str));
                return 17;
        }
        else if (strcmp(getInfoBeforeChar(str, ','), ipd) == 0) {
                //HalUARTWrite(0, str, strlen(str));
		return 13;
          }
	else if (strcmp(str, sendok) == 0) {
		return 14;
	}
	else if (strcmp(str, error) == 0) {
		return 15;
	}
	else if (strcmp(str, arrow) == 0) {
		return 16;
	}
	else {
		//printf("不能识别的指令\n");
                
		return 255;
	}

}


char* substr(char str[], uint8 begin, uint8 end) {
	//int substrlen = end - begin + 1;
	//char *substr = (char*)malloc(sizeof(char)*substrlen);
	char substr[255] = "";
	//char* p = substr;
	//p = substr;
	uint8 i = 0, j = 0, s = 0;
        s = end-'0';
	for (i = begin-'0'; i <= s; i++) {
		//*(p++) = str[i];
		substr[j++] = str[i];
	}
	//*(substr + substrlen) = '\0';
	//substr[j++] = '\0';
	return substr;
}


uint8 hasChar(char* str, char ch) {
	uint8 i;
	char* p = str;
	for (i = 0; i < strlen(str); i++) {
		if (*(p++) == ch) {
			return 1;
		}
		else {
			continue;
		}
	}
	if (i == strlen(str)) {
		return 0;
	}
}


List* splitStr(char *str) {

	List* info;
	char newStr[100];
	char* p = str;
	char* temp = str;
	uint8 flag = hasChar(str, '\n');
	uint8 i;
	uint8 j;

	init(&info);
	while (flag == 1) {
		newStr[0] = '\0';

		for (i = 0; i < strlen(temp);) {
			while (*(p++) != '\n') {
				i++;
			}
			break;
		}
		p = temp;
		for (j = 0; j < i; j++) {
			newStr[j] = *(p + j);
		}
		newStr[j++] = '\0';
		append(&info, newStr);

		temp = p + j;
		p = temp;
		flag = hasChar(temp, '\n');

	};
	for (i = 0; i < strlen(temp); i++) {
		newStr[i] = *(temp + i);
	}
	newStr[i++] = '\0';
	append(&info, newStr);

	return info;

}

/*

5	"AT+CIPMUX=1";		//OK/ERROR
6	"AT+CIPSERVER";		//“=”得到mode， “，”得到port
7	"AT+CIPSTART";		//“=”得到id， “，""”得到type（连接类型）， “，""”得到IP， “，”得到port
9	"AT+CIPSEND";		//“=”得到id， “，”得到length
14	"SENDOK";
10	"AT+CWLIF";		//得到一堆IP
13	"+IPD";		//得到用户数据， “，”得到id， “，”得到length， “:”得到数据（最后一个“，”之前）

*/
uint8 judgeInfo_list(List* list, uint8 pos) {

	List* info;
	init(&info);
	char* info_line;
	uint8 position;
	char* mode;
	char* port;
	char* ip;
	char* id;
	char* type;
	char* data;
	char* length;
	uint8 num;
	uint8 sum;
	char comma[] = ",";

        //HalUARTWrite(0,"How dare you!",20);
	uint8 info_id = judgeInfo(get(&list, pos)->data);
	//printf("info_id:%d\n", info_id);
	switch (info_id) {
        case 13:		//得到用户数据， “，”得到id， “，”得到length， “:”得到数据（最后一个“，”之前）
                info_line = deleteBeforeChar(get(&list, 1)->data, ',');
		append(&info, getInfoBeforeChar(info_line, ','));		//info(0) = id
                
                
		info_line = deleteBeforeChar(info_line, ',');
		append(&info, getInfoBeforeChar(info_line, ':'));		//info(1) = length
                
                
                length = getInfoBeforeChar(info_line, ':');
		info_line = deleteBeforeChar(info_line, ':');
                info_line = getInfoBeforeCharR(info_line, length);
                //info_line = getInfoBeforeCharR();
		append(&info, getInfoBeforeCharR(info_line, length));		//info(2) = data
                SendData(TRANSFER_ENDPOINT,get(&info, 2)->data,0x0000,1,strlen(get(&info, 2)->data));
															//id = get(&info, 0)->data;
                //printList(&info);
              break;
          
	case 0:
		switch (OK_ERROR(list)) {
		case 0:	//AT+RST ERROR
				//printf("ERROR\n");
			append(&info, "ERROR");
			break;
		case 1:	//AT+RST OK
				//printf("OK\n");
			append(&info, "OK");
			break;
		default:
			//printf("NOT OK OR ERROR\n");
			break;
		}
		break;
	case 1:
		switch (OK_ERROR(list)) {
		case 0:	//AT+CWMODE=1 ERROR
				//printf("ERROR\n");
			append(&info, "ERROR");
			break;
		case 1:	//AT+CWMODE=1 OK
				//printf("OK\n");
			append(&info, "OK");
			break;
		default:
			//printf("NOT OK OR ERROR\n");
			break;
		}
		break;
	case 2:		//OK/ERROR
		switch (OK_ERROR(list)) {
		case 0:	//AT+CWMODE=2 ERROR
				//printf("ERROR\n");
			append(&info, "ERROR");
			break;
		case 1:	//AT+CWMODE=2 OK
				//printf("OK\n");
			append(&info, "OK");
			break;
		default:
			//printf("NOT OK OR ERROR\n");
			break;
		}
		break;
	case 3:		//OK/ERROR
		switch (OK_ERROR(list)) {
		case 0:	//AT+CWMODE=3 ERROR
				//printf("ERROR\n");
			append(&info, "ERROR");
			break;
		case 1:	//AT+CWMODE=3 OK
				//printf("OK\n");
			append(&info, "OK");
			break;
		default:
			//printf("NOT OK OR ERROR\n");
			break;
		}
		break;

	case 4:		//AT+CWMODE?
		switch (OK_ERROR(list)) {
		case 0:	//AT+CWMODE? ERROR
				//printf("ERROR\n");
			append(&info, "ERROR");
			break;

		case 1:	//AT+CWMODE? OK
				//printf("OK\n");
			append(&info, "OK");
			info_line = get(&list, 1)->data;
			append(&info, deleteBeforeChar(info_line, ':'));
			//得到mode
			//mode = get(&info, 1)->data;
			//printf("THE MODE:%s\n", mode);
			break;
		default:
			//printf("NOT OK OR ERROR\n");
			break;
		}
		break;
                //***************************************CIPMUX***********************************************
	case 5:		//CIPMUX=1
		switch (OK_ERROR(list)) {
		case 0:	//AT+CIPMUX=1 ERROR
				//printf("ERROR\n");
			append(&info, "ERROR");
			break;
		case 1:	//AT+CIPMUX=1 OK
				//printf("OK\n");
			append(&info, "OK");
			break;
		default:
			//printf("NOT OK OR ERROR\n");
			break;
		}
                
                //HalUARTWrite(0, get(&info, 0)->data, 6);
		break;
                //*************************************CIPSERER*************************************************
	case 6:		//AT+CIPSERVER“=”得到mode， “，”得到port
                	
		switch (OK_ERROR(list)) {
		case 0:
			//printf("ERROR\n");
			append(&info, "ERROR");
			break;
		case 1:
			//printf("OK\n");
			append(&info, "OK");
                              		
			info_line = deleteBeforeChar(get(&list, 0)->data, '=');
			if (hasChar(info_line, ',')) {
				append(&info, getInfoBeforeChar(info_line, ','));		//info(1) = mode
                          
				info_line = deleteBeforeChar(info_line, ',');
				append(&info, info_line);		//info(2) = port
												//得到mode、port:
												//mode = get(&info, 1)->data;
					                                    
                                                                                                //port = get(&info, 2)->data;

			}
			else {
				append(&info, getInfoBeforeChar(info_line, ' '));		//info(1) = mode
																		//得到port
																		//mode = get(&info, 1)->data;
			}
			//printf("所得数据：\n");
			//printList(&info);
			break;
		default:
			//printf("NOT OK OR ERROR\n");
			//
			break;
		}
                
                //HalUARTWrite(0, get(&info, 2)->data, strlen(get(&info, 2)->data));
		break;
                //*****************************************CIPSTART*************************************************
	case 7:		//AT+CIPSTART“=”得到id， “，""”得到type（连接类型）， “，""”得到IP， “，”得到port

		switch (OK_ERROR(list)) {
		case 0:
			//printf("ERROR\n");
			append(&info, "ERROR");
			break;
		case 1:
			//printf("OK\n");
			append(&info, "OK");
			info_line = deleteBeforeChar(get(&list, 0)->data, '=');
			//printf(info_line);
			//printf("\n");
			append(&info, getInfoBeforeChar(info_line, ','));		//info(1) = id
			info_line = deleteBeforeChar(info_line, '"');
			append(&info, getInfoBeforeChar(info_line, '"'));		//info(2) = type
			info_line = deleteBeforeChar(info_line, '"');
			info_line = deleteBeforeChar(info_line, '"');
			append(&info, getInfoBeforeChar(info_line, '"'));		//info(3) = ip
			info_line = deleteBeforeChar(info_line, ',');
			append(&info, info_line);		//info(4) = port
											//得到id、type、ip、port
											//id = get(&info, 1)->data;
											//type = get(&info, 2)->data;
											//ip = get(&info, 3)->data;
											//port = get(&info, 4)->data;
											//printf("所得数据：\n");
											//printList(&info);

			break;
		default:
			//printf("ALREADY CONNECT!\n");
			//ALREADY CONNECT
			break;
		}
		break;
                //*********************************CIPSEND*****************************************************
	case 8:
		break;
	case 9:		//AT+CIPSEND“=”得到id， “，”得到length
		
		//printList(&info);

		//‘>’直接跟在这条指令后面？单独跳出来一条指令判断？
		info_line = get(&list, listSize(&list)-1)->data;
		if (hasChar(info_line, '>')) {
		//	printf("GET '>'");
		//	//得到“>”
                      append(&info, ">");	
                      info_line = deleteBeforeChar(get(&list, 0)->data, '=');
                      if (hasChar(info_line, ',')) {		//多路连接
                              append(&info, getInfoBeforeChar(info_line, ','));		//info(1) = id
                              info_line = deleteBeforeChar(info_line, ',');
                              append(&info, info_line);		//info(2) = length
											//得到id、length
											//id = get(&info, 0)->data;
											//length = get(&info, 1)->data;
                       }
                      else {		//单路连接
                              append(&info, info_line);		//info(1) = length
											//得到length
											//length = get(&info, 0)->data;
                      }
                  
		}
		else {
		//	printf("NO '>'");
                  
                //HalUARTWrite(0, "no '>'", 6);
		}
                
		break;
	case 14:
		//SENDOK
		append(&info, "SENDOK");
                //HalUARTWrite(0, get(&info, 0)->data, 6);
		break;
	case 15:
		//ERROR
		append(&info, "ERROR");
		break;
	case 16:
		//>
		append(&info, ">");
		break;
                //*********************************CWLIF*****************************************************
	case 10:		//AT+CWLIF 得到一堆IP

		switch (OK_ERROR(list)) {
		case 0:
			append(&info, "ERROR");
			break;
		case 1:
			append(&info, "OK");
			for (num = 1; num < listSize(&list) - 1; num++) {
				append(&info, getInfoBeforeChar(get(&list, num)->data, ','));		//info(num) = ip
			}
			sum = listSize(&info);		//接入的IP数量
										//get(&info,id)//IP的id 与 IP的获取
                        
                        //HalUARTWrite(0, get(&info, 3)->data, 20);

										//char* str = "";
										//str = itoa(sum, str, 10);
										//append(&info, str);

										//printf("得到的所有IP：\n");
										//printList(&info);
			break;
		default:
			//printf("NOT OK OR ERROR\n");
			break;
		}
		break;

	case 11:		//AT + CWLAP
		switch (OK_ERROR(list)) {
		case 0:	//AT+CWLAP ERROR
				//printf("I got error!\n");
			append(&info, "ERROR");
			break;
		case 1:	//AT+CWLAP OK
				//printf("I got ok!\n");
			append(&info, "OK");
			//当前无线路由器列表
			for (num = 1; num < listSize(&list) - 1; num++) {
				append(&info, get(&list, num)->data);
			}

			//printf("路由信息：\n");
			//printList(&info);
			break;
		default:
			//printf("JUDGE ERROR\n");
			break;
		}
		break;
	case 12:		//AT+CIFSR 得到一个IP
		switch (OK_ERROR(list)) {
		case 0:
			//printf("I got error!\n");
			//ERROR
			append(&info, "ERROR");
			break;
		case 1:
			//printf("I got ok!\n");
			//OK
			append(&info, "OK");
			info_line = get(&list, 1)->data;
			if (hasChar(info_line, ':')) {
				info_line = deleteBeforeChar(info_line, ':');
				append(&info, info_line);
			}
			else {
				append(&info, info_line);
			}
			//printf("所得IP：\n");
			//printList(&info);
			break;
		default:
			//printf("ALREADY CONNECT!\n");
			//ALREADY CONNECT
			break;
		}

		break;
                
                //**************************************************************************************+IPD
	
        case 17:
                
                info_id = judgeInfo_list(list, 1);
	case 255:		//其它指令
		break;
	default:
		break;

	}
        
        return info_id;
	

}


char* getInfoBeforeChar(char str[], char end) {

	uint8 i, j = 0;
	//char *substr = (char*)malloc(sizeof(char)*strlen(str));
	char substr[255] = "";
	//char* p = substr;
	for (i = 0; i < strlen(str); i++) {
		if (str[i] != end) {
			//*(p++) = str[i];
			substr[j++] = str[i];
		}
		else {
			//*(p++) = '\0';
			//substr[j++] = '\0';
			break;
		}
	}
	return substr;
}


uint8 tenSqure(uint8 time){
    uint8 result = 1;
    uint8 i=0;
    for(i=0; i<time; i++){
      result = result*10;
    }
    return result;
}

char* getInfoBeforeCharR(char str[], char length[]) {

	uint8 i=0, j=0, len=0;
        uint8 temp=0;
	
                //HalUARTWrite(0, "How dare you!", 20);
        
          for (i = 0; i < strlen(length); i++) {
                temp = length[i]-'0';
		len = len + temp*tenSqure(strlen(length)-i-1);
          
          }
	for (j = 0; j < len; j++) {
		substr2[j] = str[j];
	}
        //HalUARTWrite(0, substr2, 20);
	return substr2;
}

/*
char* getInfoBeforeCharR() {

        HalUARTWrite(0, "How dare you!", 20);
	
	return "";
}
*/

char* deleteBeforeChar(char str[], char ch) {
	uint8 i=0;
	char* p = str;
	for (i = 0; i < strlen(str); i++) {
		if (str[i] != ch) {
			p++;
		}
		else {
			p++;
			break;
		}
	}
	return p;
}

char* firstAlphaStr(char str[]) {
	uint8 i, j = 0;
	char substr[255] = "";
	for (i = 0; i < strlen(str); i++) {
		if (isalpha(str[i])) {
			substr[j++] = str[i];
		}
		else {
			break;
		}
	}
	substr[j++] = '\0';
	return substr;
}

uint8 OK_ERROR(List* list) {
	char ok[] = "OK";
	char error[] = "ERROR";

	char* result = get(&list, listSize(&list) - 1)->data;		//键盘模拟最后一行#结尾，串口数据须改为-1
	if (strcmp(result, ok) == 0) {
		return 1;
	}
	else if (strcmp(result, error) == 0) {
		return 0;
	}
	else {
		return 255;
	}

}