#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern "C"
{
#include "sqlite3.h"
};


char * current_filename = (char *)malloc(sizeof(char) * 20);
char * current_table = (char *)malloc(sizeof(char) * 20);

/*
	
*/
int LoadMyInfo( void * para, int n_column, char ** column_value, char ** column_name )
{
   //para是你在 sqlite3_exec 里传入的 void * 参数
   //通过para参数，你可以传入一些特殊的指针（比如类指针、结构指针），然后在这里面强制转换成对应的类型（这里面是void*类型，必须强制转换成你的类型才可用）。然后操作这些数据
   //n_column是这一条记录有多少个字段 (即这条记录有多少列)
   // char ** column_value 是个关键值，查出来的数据都保存在这里，它实际上是个1维数组（不要以为是2维数组），每一个元素都是一个 char * 值，是一个字段内容（用字符串来表示，以/0结尾）
   //char ** column_name 跟 column_value是对应的，表示这个字段的字段名称  
   //这里，我不使用 para 参数。忽略它的存在.
 
    int i;
	FILE * file_output = NULL;
	file_output = fopen(current_filename, "ab+");
	if(file_output == NULL)
	{
		printf("can not create %s output file\n", current_filename);
		return 0;
	}
	
	fprintf(file_output, "记录包含 %d 个字段\n", n_column );
	for( i = 0 ; i < n_column; i ++ )
	{
	    fprintf(file_output, "字段名:%s   字段值:%s\n",  column_name[i], column_value[i] );
	}
	fclose(file_output);
	return 0;
	
}
/*
	save the database sql query result to file
	@parameter db_info the table name of current table.
	@return -1 for failed. 0 for success.
*/
int SaveDbToFile(const char ** db_info)
{
	sqlite3 * db = NULL; //声明sqlite关键结构指针
	int result;
	char ** errmsg = NULL;
	current_filename = (char *)db_info[2];
	current_table = (char *)db_info[2];
	char sqlite_sql[40] = "select * from ";
	strcat(sqlite_sql, current_table);
	char **dbResult;
	int nRow, nColumn;

	
	//打开数据库
	//需要传入 db 这个指针的指针，因为 sqlite3_open 函数要为这个指针分配内存，还要让db指针指向这个内存区
	result = sqlite3_open( db_info[0], &db );
	//result = sqlite3_open("d:\\canyou.db", &db);
	if( result != SQLITE_OK )
	{
	//数据库打开失败
		printf(" db file open failed!\n");
		return -1;
	}
	//数据库操作代码
	

	//result = sqlite3_get_table( db, "select * from memo", &dbResult, &nRow, &nColumn, errmsg );
    
	result = sqlite3_exec( db, sqlite_sql, LoadMyInfo, NULL, errmsg );
	//result = sqlite3_exec(db, "select * from memo", LoadMyInfo, NULL, errmsg);
	if(result != SQLITE_OK )
	{
	    printf( "错误码:%d，错误原因:%s\t", result, *errmsg );
		printf("%s read failed! Please access db file manually! File Path: %s\n\n", db_info[1], db_info[0]);
		sqlite3_close(db);
		return 0;
	}

	printf("%s open success! File Saved at: ./%s\n\n", current_filename, current_filename);
	//数据库打开成功
	//关闭数据库
	sqlite3_close(db);
}
/*
	show file in Byte format. Print a Byte table to dashboard.
	@parameter pBuf, the raw content array. uOffset, the first offset of content which is going to display in dashboard.
*/
void ShowByteInform(unsigned char * pBuf, unsigned int uOffset)
{
	int k;
	int j = 1;
	unsigned int i = uOffset;
	unsigned int uCount = 0;
	// data scope which is shown on the screen
	unsigned int uSize = 640;
	printf("**********************************************************\n");
	//print first row
	printf("          ");
	for(k = 0; k < 16 ;k++)
	{
		printf("%0x  ",k);
		if((k + 1) % 8 == 0)
        {
            printf(" ");
        }
	}
	printf("\n");
    for(i; uCount < uSize; i++,uCount++)
    {
		//print first cloumn
		if(i % 16 == 0)
		{
			printf("%08xh ",i);
		}
		//print data
		
        printf("%02x ",pBuf[i]);
		//print data in format
		if((i + 1) % 16 == 0)
		{
			printf("| ");
			for(j = i - 15; uCount < uSize; j++)
			{
				printf("%c",pBuf[j]);
				//show(&pBuf[j]);
				if((j + 1) % 16 == 0)
				{
					//j++;
					break;
				}
			}
		}
		//print \n for second row
        if((i + 1) % 16 == 0)
        {
            printf("\n");
        }
        else if((i + 1) % 8 == 0)
        {
            printf(" ");
        }
    }
    
	printf("**********************************************************");
	printf("\n\n");
    //test for UTF-8 printf
    /*int data_offset_i = uOffset;
    for(data_offset_i; uCount < uSize; data_offset_i++, uCount++ )
    {
        printf("%c", pBuf[data_offset_i]);
    }*/

}

/*
	Search target string in the raw content.
	@parameter target_str: the string that is going to be searched. raw_content: raw content. begin_offset: the offset that start search. raw_content_length: the file size of raw_content
	@return the offset of target string in raw content. -1 for no search result.
*/
long Search(unsigned char * target_str, unsigned char * raw_content, unsigned long * begin_offset,unsigned long raw_content_length)
{
	unsigned char * input_str = NULL;
    	int target_str_length = strlen((char *)target_str);
    	//unsigned long raw_content_length = strlen((char *)raw_content);
	long raw_i = *begin_offset;
	unsigned int target_i = 0;
    	int search_result = 1;

    	//input_str = malloc(sizeof(char) * 200);
    	//printf("pleas input the str you want to search:\n");
   	//scanf("%s", input_str);
   	//printf("length of input_str is:%d\n",strlen(input_str));
   	//input_str_length = strlen(input_str);
    	for(; raw_i < (raw_content_length - target_str_length + 1); raw_i++) //begin search at begin_offset
    	{
       		search_result = 1;
        	if(raw_content[raw_i] == target_str[0])
        	for(target_i = 1; target_i < target_str_length; target_i++)
        	{
           		if(raw_content[target_i + raw_i] != target_str[target_i])
           		{
                		search_result = 0;
                		break;
            		}
        	}
        		else
        		{
            			search_result = 0;
        		}
        		if(search_result == 1)
        		{
           			*begin_offset = (unsigned long)raw_i + 1;
            			return raw_i;
        		}
    	}
   
    	return -1;
}
/*
	Find the Table Root page
	@parameter raw_content: the raw content. table_name: the table name of current sql db file. raw_content_length: the raw content length in byte.
	@return the huffman code of the root page.
*/
long FindTableRootPage(unsigned char * raw_content, char * table_name, unsigned long raw_content_length){
	unsigned char * search_string = (unsigned char *)malloc(sizeof(char) * 400);
	//memcpy(unsigned_table_name, table_name, 200);
	unsigned long search_offset = 0;
	char * table_name_tmp = (char *)malloc(sizeof(char) * 400);					//everything about pointer data, store format : hight bits rights
	strcpy(table_name_tmp, "CREATE TABLE ");							//						low bits left
	strcat(table_name_tmp, table_name);								//everything about symbol data, sotre format : hight bits left
	long search_result = -1;									//					       low bits right
	memcpy(search_string, table_name_tmp, 400);
	search_result = Search(search_string, raw_content, &search_offset, raw_content_length);
	if(search_result == -1){
		return search_result;
	}
	return (search_result - 1);
}

int HuffmanCodeTransition(unsigned char * raw_content, long huffman_offset){
	int huffman_length = 0;
	int int_huffman_code = 0;
	unsigned char huffman_code[2] = {0x0, 0x0};
	unsigned char huffman_tmp;
	int huffman_iter = 0;
	int temp = 0;
	//printf("raw_content[%0x]:%0x\n", huffman_offset, raw_content[huffman_offset]);	//debug for testing only
	//printf("raw_content[%0x]:%0x\n", huffman_offset + 1, raw_content[huffman_offset + 1]);	//debug for testing only
	for(huffman_length, huffman_offset; huffman_length < 2; huffman_length++, huffman_offset++){
		huffman_code[huffman_length] = raw_content[huffman_offset];
		memcpy(&int_huffman_code, huffman_code, 1);
		if(int_huffman_code < 0x80){
			huffman_length = huffman_length + 1;
			break;
		}
	}

	if(huffman_length == 2){
		unsigned char huffman_code0 = huffman_code[0];
		huffman_code0 = huffman_code0 << 7;
		huffman_code0 = huffman_code0 >> 7;
		memcpy(&temp, &huffman_code0, 1);
		//printf("temp:%d\n", temp);	//debug for testing only
		memcpy(&int_huffman_code, &huffman_code[1], 1);
		int_huffman_code += temp * 0x80;
		memcpy(&huffman_code[1], &int_huffman_code, 1);
		huffman_code[0] = huffman_code[0] << 1;
		huffman_code[0] = huffman_code[0] >> 2;	
	}
	if(huffman_length == 1){
		huffman_code[0];
		huffman_code[1] = 0;
	}
	int i = 0;
	//printf("huffmancode:");			//debug for testing only
	/*for(i; i < huffman_length; i++){	//debug for testing only
		printf("%0x ,", huffman_code[i]);
	}*/
	int long_huffman_code = 0;
	
	if(huffman_length == 2){
		huffman_tmp = huffman_code[0];
		//printf("huffman_temp:%0x\n", huffman_tmp);//debug for testing only
		huffman_code[0] = huffman_code[1];
		huffman_code[1] = huffman_tmp;
		memcpy(&long_huffman_code, huffman_code, huffman_length);
	}

	if(huffman_length == 1){
		memcpy(&long_huffman_code, huffman_code, huffman_length);
	}
	//printf("long_huffman_code:%0x\n",long_huffman_code);	//debug for testing
	return (int)long_huffman_code;
	
}

/*
	ergodic all B tree page.
	@parameter raw_content: the raw content. page_offset: the offset of the B tree page. page_amount: the amount of the total b tree page.
	@return the page number.
*/
int * ErgodicAllBTreePage(unsigned char * raw_content, int * page_offset, int page_amount){
	unsigned char page_type[200];
	unsigned char first_free_block[200][2];
	unsigned char cell_number[200][2];
	unsigned char first_cell[200][2];
	unsigned char fragment_number[200];
	unsigned char cell_offset[200][100][2];
	int int_cell_offset[200][100];
	int int_cell_number[200];
	unsigned char * ergodic_result = (unsigned char *)malloc(sizeof(char) * 1000);
	int * int_ergodic_result = (int *)malloc(sizeof(int) * 1000);
	for(int i = 0; i < 200; i++){
		int_cell_number[i] = 0;
	}
	//unsigned char right_most_pointer[4];	//BTree Leaf Page does not have right_most_pointer;
	unsigned char cell_content[200][4];
	int k = 0;
	int cell_offset_iter = 0;
	int ergodic_iter = 0;
	for(int page_iter = 0; page_iter < page_amount; page_iter++){
		page_type[page_iter] = raw_content[page_offset[page_iter]];
		cell_number[page_iter][1] = raw_content[page_offset[page_iter] + 3];
		cell_number[page_iter][0] = raw_content[page_offset[page_iter] + 4];
		memcpy(&int_cell_number[page_iter], &cell_number[page_iter], 2);
		//printf("page[%0x]:%0x\n", page_iter, page_offset[page_iter]);	//debug for testing only

		for(cell_offset_iter = 0, k = 0; k < int_cell_number[page_iter]; cell_offset_iter += 2, k++){
		//	printf("int_cell_number[%d]:%0x\n", page_iter, int_cell_number[page_iter]);
			cell_offset[page_iter][k][0] = raw_content[page_offset[page_iter] + 8 + cell_offset_iter + 1];
			cell_offset[page_iter][k][1] = raw_content[page_offset[page_iter] + 8 + cell_offset_iter];
			memcpy(&int_cell_offset[page_iter][k], cell_offset[page_iter][k], 2);
			int_cell_offset[page_iter][k] += page_offset[page_iter];
		//	printf("cell_offset[%d][0]:%0x\n", page_iter, cell_offset[page_iter][k][0]);//debug for testing only
		//	printf("cell_offset[%d][1]:%0x\n", page_iter, cell_offset[page_iter][k][1]);//debug for testing only
			//printf("int_cell_offset[%d][%d]:%0x\n",page_iter, k, int_cell_offset[page_iter][k]);//debug for testing only
			//break;	//debug for testing only
			int_ergodic_result[ergodic_iter] = HuffmanCodeTransition(raw_content, (long)int_cell_offset[page_iter][k]);
			if(int_ergodic_result[ergodic_iter] < 0x80){
				int_ergodic_result[ergodic_iter] = HuffmanCodeTransition(raw_content, (long)(int_cell_offset[page_iter][k] + 1));
				ergodic_iter++;
			}
			if(int_ergodic_result[ergodic_iter] > 0x80 && int_ergodic_result[ergodic_iter] < 0x20000000){
				int_ergodic_result[ergodic_iter] = HuffmanCodeTransition(raw_content, (long)(int_cell_offset[page_iter][k] + 2));
				ergodic_iter++;
			}
			//printf("int_ergodic_result(_id)[%d]:%d\n", ergodic_iter - 1, int_ergodic_result[ergodic_iter - 1]); //debug for testing only
			if(k > 0){
				if(int difference = (int_ergodic_result[ergodic_iter - 1] - int_ergodic_result[ergodic_iter - 2]) != 1){
					printf("The Offset of Page may contain deleted information which _id = ");
					for(int n = 0; n < difference; n++){
						printf("%d ", int_ergodic_result[ergodic_iter - 2] + n + 1);
					}
					printf("\n is: 0x%0x\n",page_offset[page_iter]);
				}
			}
			
		}
	//	break;	//debug for testing only	
	}
}

/*
	find the deleted information id.
	@parameter raw_content: the raw content array. root_page: the number of root page
	@return 0
*/
unsigned long * FindLostId(unsigned char * raw_content, unsigned long root_page){
	//unsigned char * raw_content = (unsigned char *)malloc(sizeof(char) * 500);
	unsigned char page_type;
	unsigned char first_free_block[2];
	unsigned char cell_number[2];
	unsigned char first_cell[2];
	unsigned char fragment_number;
	unsigned char right_most_pointer[4];
	unsigned char cell_content[200][4];
	int ergodic_array[200];
	for(int k = 0; k < 200; k++)
		ergodic_array[k] = 0;
	int int_cell_content[200];
	unsigned char cell_offset[200][2];
	unsigned int int_cell_offset[200];
	unsigned char cell_offset_iter = 0;
        long page_offset = 0;
	page_offset = (raw_content[root_page] - 1) * 0x400;
	//printf("PageOffset: %0x\n", page_offset);	//for debug test

	page_type = raw_content[page_offset];
	first_free_block[1] = raw_content[page_offset + 1];
	first_free_block[0] = raw_content[page_offset + 2];
	cell_number[1] = raw_content[page_offset + 3];
	cell_number[0] = raw_content[page_offset + 4];
	first_cell[1] = raw_content[page_offset + 6];
	first_cell[0] = raw_content[page_offset + 5];
	fragment_number = raw_content[page_offset + 7];
	if(page_type == 0x05){
		right_most_pointer[3] = raw_content[page_offset + 11];
		right_most_pointer[2] = raw_content[page_offset + 10];
		right_most_pointer[1] = raw_content[page_offset + 9];
		right_most_pointer[0] = raw_content[page_offset + 8];
	//	printf("page_type:%0x\n", page_type);			//debug for testing only
	}else{
		cell_offset_iter = 8;
	}
		
	int int_cell_number = 0;
	memcpy(&int_cell_number, cell_number, 2);
	//printf("cell_number[0]:%0x\n", cell_number[0]);		//debug for testing only
	//printf("cell_number[1]:%0x\n", cell_number[1]);		//debug for testing only
	//printf("cell_number: %d\n", int_cell_number);			//debug for testing only
	int i = 0;	
	for(cell_offset_iter,i; i < int_cell_number; cell_offset_iter += 2, i++){
		cell_offset[i][0] = raw_content[page_offset + 12 + cell_offset_iter + 1];
		cell_offset[i][1] = raw_content[page_offset + 12 + cell_offset_iter];
		memcpy(&int_cell_offset[i], cell_offset[i], 2);
	//	printf("cell_offset[%d][0]:%0x\n", i, cell_offset[i][0]);//debug for testing only
	//	printf("cell_offset[%d][1]:%0x\n", i, cell_offset[i][1]);//debug for testing only
	//	printf("int_cell_offset[%d]:%0x\n", i, int_cell_offset[i]);//debug for testing only
		
	}
	for(i = 0; i < int_cell_number; i++){
		int_cell_offset[i] += page_offset;
	//	printf("\n");		//debug for testing only
	//	printf("int_cell_offset[%d]%0x\n:", i, int_cell_offset[i]);//debug for testing only
	}
	for(i =0; i < int_cell_number; i++){
		int_cell_content[i] = 0;
		cell_content[i][0] = raw_content[int_cell_offset[i] + 3];
		cell_content[i][1] = raw_content[int_cell_offset[i] + 2];
		cell_content[i][2] = raw_content[int_cell_offset[i] + 1];
		cell_content[i][3] = raw_content[int_cell_offset[i] + 0];
		memcpy(&int_cell_content[i], cell_content[i], 4);
	//	printf("int_cell_content[%d]:%0x\n", i, int_cell_content[i]);//debug for testing only
		ergodic_array[i] = (int_cell_content[i] - 1) * 0x400;
	//	printf("ergodic_array[%d]:%0x\n", i, ergodic_array[i]);

	}
	
	ErgodicAllBTreePage(raw_content, ergodic_array, int_cell_number);
	//HuffmanCodeTransition(raw_content, 0x1962c);
	return 0;
}
//unsigned char * FindLostPage(int * id, unsigned char * raw_content);
















