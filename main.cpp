#include <stdio.h>
#include <string.h>
//#include "windows.h"
#include <sys/stat.h>
//#include <io.h>
//#include <FCNTL.H>
#include <wchar.h>
#include <locale.h>
#include <stdlib.h>
//#include <conio.h>

#include "Sqlite3Forensics.h"
#include <termios.h>
#include <unistd.h> // for STDIN_FILENO

//for one-click keyboard response working in Linux
int getch (void)
{
int ch;
struct termios oldt, newt;

tcgetattr(STDIN_FILENO, &oldt);
memcpy(&newt, &oldt, sizeof(newt));
newt.c_lflag &= ~( ECHO | ICANON | ECHOE | ECHOK |
ECHONL | ECHOPRT | ECHOKE | ICRNL);
tcsetattr(STDIN_FILENO, TCSANOW, &newt);
ch = getchar();
tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

return ch;
}

/*
	open target input file.
	@parameter file_absolute_path, the absolute file path
	@return return the FILE pointer
*/
FILE* OpenTargetFile(char * file_absolute_path)
{
	FILE * fInput = NULL;
	fInput = fopen(file_absolute_path, "rb");
	if(fInput == NULL)
	{
		printf("can not open target file\n");
		return 0;
	}
	return fInput;
}

/*
	open output file.
	@parameter file_absolute_path, the absolute file path
	@return the FILE pointer
*/
FILE* OpenOutputFile(char * file_absolute_path)
{
	FILE * fOutput = NULL;
	fOutput = fopen(file_absolute_path,"wb");
	if(fOutput == NULL)
	{
		printf("can not create output.txt\n");
		return 0;
	}
	return fOutput;

}
/*
	Copy inputfile to outputfile
	@parameter fInput, input file pointer. fOutput, output file pointer.
	@return 1;
*/
int CopyTargetFile(FILE * fInput, FILE * fOutput)
{
	int iReadBuffer = 0;
	iReadBuffer = fgetc(fInput);
	while(feof(fInput) == 0)
	{
		fputc(iReadBuffer,fOutput);
		iReadBuffer=fgetc(fInput);
	}
	return 1;
}

/*
	Get file size before loading to memory.
	@parameter the file absolute path
	@return the file size in byte.
*/
unsigned long GetFileSize(char * file_absolute_path)
{

    long int save_pos;
    unsigned long size_of_file;
    FILE * fInput;
    fInput = fopen(file_absolute_path, "rb");
    if(fInput == NULL)
	{
		printf("can not open target file\n");
		return 0;
	}
    /* Save the current position. */
    save_pos = ftell( fInput );

    /* Jump to the end of the file. */
    fseek( fInput, 0L, SEEK_END );

    /* Get the end position. */
    size_of_file = ftell( fInput );

    /* Jump back to the original position. */
    fseek( fInput, save_pos, SEEK_SET );

    return(size_of_file);

}
/*
	Read target input file to memory
	@parameter fInput, the input target file. file_absolute_path, the absolute path of input file.
	@return array of file content.
*/
unsigned char* ReadFileToMemory(FILE * fInput, char * file_absolute_path)
{
	int fileSize = 0;
	int i=0;
	//get file size
	fileSize = GetFileSize(file_absolute_path);
	unsigned char * bReadBuffer = (unsigned char *)malloc(sizeof(char)*fileSize);
	//read file to bReadBuffer
	bReadBuffer[0] = fgetc(fInput);
	for(i = 1; i < fileSize; i++)
	{
		if(feof(fInput) != 0)
			break;
		//while(feof(fInput) == 0)
		//{
			bReadBuffer[i] = fgetc(fInput);
		//}
	}
	return bReadBuffer;
}

/*
	Generate the hash code of file, print them to dash board.
*/
void HashGeneration(char * cfile_path)
{
	char * output_hash_file_path = (char *)malloc(sizeof(char) * 300);
	char * md5_command = (char *)malloc(sizeof(char) * 300);
       	char * sha1_command = (char *)malloc(sizeof(char) * 300);
	char * echo_command = (char *)malloc(sizeof(char) * 300);
	char * echo_command2 = (char *)malloc(sizeof(char) * 300);
	strcpy(md5_command, "openssl md5 ");
	strcpy(sha1_command, "openssl sha1 ");
	strcpy(echo_command, "echo \"file location:");
	strcpy(output_hash_file_path, ">>hash_digital_collections");
	strcpy(echo_command2, "echo \" \"");
	//printf("%s\n", md5_command);
	//printf("%s\n", cfile_path);	
	strcat(md5_command, cfile_path);
	strcat(sha1_command, cfile_path);
	strcat(md5_command, output_hash_file_path);
	strcat(sha1_command, output_hash_file_path);
	
	strcat(echo_command, cfile_path);
	strcat(echo_command, "\"");
	strcat(echo_command, output_hash_file_path);
	strcat(echo_command2, output_hash_file_path);

	//printf("%s\n", md5_command);
	//printf("%s\n", sha1_command);
	//printf("%s\n", echo_command);

	system(echo_command);	
	system(md5_command);
	system(sha1_command);
	system(echo_command2);
	printf("hash_signatures are saved in file:\"hash_digital_collections\"\n");
}

/*
	Copy and paste the raw image of raw content.
*/
int CaptuerFromImage(unsigned char * raw_content)
{
	FILE *foutput = NULL;
	unsigned long ulstart_offset,ulend_offset,ulfile_iter;
	//unsigned char * uccaptured_content;
	char * ucoutput_file_path = (char *)malloc(sizeof(char) * 100);
	char * offset_information = (char *)malloc(sizeof(char) * 100);
	strcpy(offset_information, "offset ");
	printf("please input\n");
	printf("starting offset:");
	scanf("%0xd", &ulstart_offset);
	printf("end offset:");
	scanf("%0xd", &ulend_offset);
	printf("please input output filen path:");
	scanf("%s", ucoutput_file_path);
	//ucoutput_file_path = "test";
	//printf("%s", ucoutput_file_path);
	foutput = fopen(ucoutput_file_path, "ab+");
	if(foutput == NULL)
	{
		printf("invalid output file path!\n");
		return 0;
	}
	fprintf(foutput, "%s", "offset ");
	fprintf(foutput, "%0xd-%0xd :", ulstart_offset, ulend_offset);
	//fprintf(foutput, "-");
	//fprintf(foutput, "%0xd", ulend_offset);
	for(ulfile_iter = ulstart_offset; ulfile_iter <= ulend_offset; ulfile_iter++)
	{
		fprintf(foutput, "%02x", raw_content[ulfile_iter]);
		//uccaptured[icopy_iter] = raw_content[ulstart_offset++];
	}
	fclose(foutput);
	printf("content capture success!\n");
	HashGeneration(ucoutput_file_path);
	return 1;
}


int main(int argc, char *argv[])
{
	//initial variable
	FILE *fInput,*fOutput;
	fInput = NULL;
	fOutput = NULL;
	int cReadBuffer;
	unsigned long uFileSize = 0;
	char * target_str_temp = (char *)malloc(sizeof(char) * 200); //target_str_temp store the target_str's last value for the comparison at next time
	unsigned char * raw_content = NULL;
	unsigned char * target_str = (unsigned char *)malloc(sizeof(char) * 200);
	char * target_str_char = (char *)malloc(sizeof(char) * 200);
	//char * target_str = (char *)malloc(sizeof(char) * 200);
	unsigned long search_offset = 0;
	long search_result = 0;
	unsigned long view_offset = 0;
	char view_data_ctr = '0';
	long root_page_offset = -1;
	char * table_name = (char *)malloc(sizeof(char) * 200);
	if(argv[1] == NULL)
	{
		printf("input --h or --help for help\n");
		return 0;
	}
	

	if(!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help"))
	{
		printf("Android_2.3.6 Samsung GT_S5820 Forensics Tool Mark4\n");
		printf("written by Le0n Hu ,University of Chongqing Posts & Telecommunications\n");
		printf("useage:\n");
		printf(" -h, --help\t\tgive the help list\n");
		printf(" -r, --read\t\tread the raw image file\n");
		printf(" -p, --pull\t\tpull /data/data from android phone to current directory\n");
		printf(" -d, --dig\t\tdig information from sms,contacts,call log,memo,browser history,etc to files to current directory\n");
		printf("     --recovery\t\t recovery specific Pages contains deleted datas\n");
		return 0;
		
	}

	if(!strcmp(argv[1], "--recovery"))
	{

		if(argv[2] == NULL)
		{
			printf("please input recovery file absolute path\n");
			return 0;
		}
		/*if(argv[3] == NULL)
		{
			printf("please input the table name which you want to recovery information from\n");
			return 0;
		}*/
		printf("\nplease input the table name which you want to recovery informaiton from\n");
		gets(table_name);
		fInput = OpenTargetFile(argv[2]);
		uFileSize = GetFileSize(argv[2]);
		raw_content = ReadFileToMemory(fInput, argv[2]);
		root_page_offset = FindTableRootPage(raw_content, table_name, uFileSize);
		if(root_page_offset == -1)
		{
			printf("table %s is not found!\nplease try again\n", table_name);
			return 0;
		}

		
		FindLostId(raw_content, root_page_offset);
		//printf("RootPage Offset: %0x\n", root_page_offset);
		return 0;
	}

	if(!strcmp(argv[1], "-r") || !strcmp(argv[1], "--read"))
	{
		if(argv[2] == NULL)
		{
			printf("please input file absolute path\n");
			return 0;
		}
		fInput = OpenTargetFile(argv[2]);
		uFileSize = GetFileSize(argv[2]);
		raw_content = ReadFileToMemory(fInput, argv[2]);
		ShowByteInform(raw_content, 0);
		while(/*view_data_ctr != 0x1b*/1)	//getch();
		{
			view_data_ctr = getch();
			
			switch(view_data_ctr)
			{
			case 'i':	if(view_offset == 0)
							ShowByteInform(raw_content, view_offset); 
						else
						{
							//ShowByteInform(raw_content, (view_offset = view_offset - 16 * 20));
							ShowByteInform(raw_content, (view_offset = view_offset - 16));
						}
						break;
			case 'k':	if(view_offset + 640 > uFileSize)	
						{
							ShowByteInform(raw_content, (view_offset = uFileSize - 624));
						}
						else
						{
							//ShowByteInform(raw_content, (view_offset = view_offset + 16 * 20));
							ShowByteInform(raw_content, (view_offset = view_offset + 16));
						}
						break;
			case 'g':	printf("please input the offset you want to move to:");
						scanf("%0xd", &view_offset);
						if(view_offset < 0)
						{
							ShowByteInform(raw_content, 0);
						}
						else
						{
							if(view_offset + 640 > uFileSize)
							{
								ShowByteInform(raw_content, (view_offset = uFileSize - 641));
							}
							else
							{
								ShowByteInform(raw_content, (view_offset = view_offset / 16 * 16));
							}
						}
						break;
			case 'f':	printf("please input the content you want to search:");
						//scanf("%s", target_str);
						gets(target_str_char);
						if(strcmp(target_str_char, target_str_temp) != 0)
						{
							search_offset = 0;
						}
						strcpy(target_str_temp, target_str_char);
						memcpy(target_str, target_str_char, 200);
						search_result = Search(target_str, raw_content, &search_offset, uFileSize);
						if(search_result == -1)
						{
							printf("target string is not found!\n");
							break;	
						}
						else
						{
							view_offset = search_result;
							printf("target string found! address: %0x\n", view_offset);
						}
						
						if(view_offset + 640 > uFileSize)
						{
							ShowByteInform(raw_content, (view_offset = uFileSize - 640));
							break;
						}
						else
						{
							ShowByteInform(raw_content, (view_offset = view_offset / 16 * 16));
							break;
						}
						break;
			case 'c':	
					CaptuerFromImage(raw_content);
						break;
			case 'n':	search_result = Search(target_str, raw_content, &search_offset, uFileSize);
						if(search_result == -1)
						{
							printf("target string is not found!\n");
							break;	
						}
						else
						{
							view_offset = search_result;
							printf("target string found! address: %0x\n", view_offset);
						}
						
						if(view_offset + 640 > uFileSize)
						{
							ShowByteInform(raw_content, (view_offset = uFileSize - 640));
							break;
						}
						else
						{
							ShowByteInform(raw_content, (view_offset = view_offset / 16 * 16));
							break;
						}
						break;
			case 0x1b:	return 0;	//monitor 'Esc' input
			}
		}
	}
	if(!strcmp(argv[1], "-d") || !strcmp(argv[1], "--dig"))
	{
		SaveDbToFile(SMS_INFO);
		SaveDbToFile(CONTACTS_INFO);
		SaveDbToFile(CALL_LOG_INFO);
		SaveDbToFile(MEMO_INFO);
		SaveDbToFile(BROWSER_HISTORY_INFO);
		SaveDbToFile(BROWSER_USR_PWD_INFO);
		SaveDbToFile(CALENDAR_INFO);
		SaveDbToFile(EMAIL_INFO);
		SaveDbToFile(EMAIL_USR_PWD_INFO);
		return 0;
	}
	if(!strcmp(argv[1], "-p") || !strcmp(argv[1], "--pull"))
	{
		system("adb pull /data/data ./");
		return 0;		
	}

	printf("input --h or --help for help\n");
	return 0;


}
