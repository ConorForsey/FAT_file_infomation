#include <stdio.h>
#include <stdlib.h>

//Created By Conor Forsey

#define BLOCK 512		//Define block size

#define FILESIZE 0x13		//TASK 1
#define FILESIZE2 0x14		//TASK 1

#define NUMFATS 0x10		//TASK 2

#define FATSIZE 0x16		//TASK 3
#define FATSIZE2 0x17		//TASK 3

#define ROOTENTRIES 0x11	//TASK 4
#define ROOTENTRIES2 0x12	//TASK 4

#define ENTRYSIZE 32		//TASK 6
#define NAMESIZE 11		//TASK 6
#define DATE 0x18		//TASK 6
#define DATE2 0x19		//TASK 6

#define TIME 0x16		//TASK 7
#define TIME2 0x17		//TASK 7

#define CLUSTER 0x1a		//TASK 8
#define CLUSTER2 0x1b		//TASK 8

int main(int argc, char *argv[])
{
	FILE * strm = fopen("testdisk.img", "r");
	unsigned char buf[BLOCK];
	int size_of_file;
	int num_fats;
	int size_of_single_fat;
	int start_of_root;  	
	int root_entries;
	int size_of_root;	
	int data_offset;
	char *looking;

	if(strm == NULL){
	fprintf(stderr, "Opening of input file failed %s\n", strm);
	exit(EXIT_FAILURE);}	
	
	fread(&buf[0], 1, BLOCK, strm); //Read the first block into memory

	//TASK 1	
	
	size_of_file = (buf[FILESIZE2] << 8 | buf[FILESIZE]);
	printf("%d is the size of the file system in blocks\n\n", size_of_file);	
	
	//TASK 2
	
	num_fats = buf[NUMFATS]; 				
	printf("%d is the number of FATs in decimal \n", num_fats);
        printf("%02x is the number of FATs in hexidecimal \n\n", num_fats);

	//TASK 3		

	size_of_single_fat = buf[FATSIZE2] << 8 | buf[FATSIZE];
	printf("%d is the size of a single FAT in blocks\n\n", size_of_single_fat);
	
	//TASK 4	

	start_of_root = (size_of_single_fat * buf[NUMFATS]) + 1;
	printf("%d is the start block of root in decimal\n", start_of_root);
        printf("%02x is the start block of root in hexidecimal\n", start_of_root);

	root_entries = (buf[ROOTENTRIES2] << 8 | buf[ROOTENTRIES]);
	printf("%d number of root entries in decimal\n",root_entries);
        printf("%02x number of root entries in hexidecimal\n",root_entries);
	
	size_of_root = (root_entries * 32) / BLOCK;
	printf("%d is the size of the root directory in blocks\n\n",size_of_root); 
	
	//TASK 5	

	fseek(strm, start_of_root*BLOCK, SEEK_SET);
        fread(&buf[0], 1, BLOCK, strm);
	int seventh_ent;
	char name[13];
	char ch;
	int i;
	int n = 0;

	seventh_ent = 0x20*0x06;	

	for(i = 0; i < 8; i++){
	ch = buf[seventh_ent+i];
	if(ch == ' ') continue;
	name[n++] = ch;
	} 
	name[n++] = '.';
	
	for(i = 0; i < 3; i++){
	ch = buf[seventh_ent + 8 + i];
	if(ch == ' ') continue;
	name[n++] = ch;
	}
	name[n] = '\0';
	printf("%s is the filename in the seventh entry of the root directory\n\n",name);

	//TASK 6
	
	looking = "FSTAB   DAT ";
	int filedate;
	unsigned short int day;
	unsigned short int month;
	unsigned short int year;
	for(i = 0; i < BLOCK; i = i + ENTRYSIZE){
	if(strncmp(&buf[i], looking, NAMESIZE) == 0){
		filedate = (buf[i+DATE2] << 8) |  buf[i+DATE]; 
				
		day = filedate & 0x1f;
		month = (filedate >> 5) & 0x0f;
		year = ((filedate >> 9) & 0x7f)+1980;
		printf("FSTAB.DAT was created on %d/%d/%d\n\n", day, month, year);
		break;
		}
	}		
	
	//TASK 7

	looking = "CODETAB TXT";
	int filetime;
	unsigned short int second; 
        unsigned short int minute;
        unsigned short int hour;
	for(i = 0; i < BLOCK; i = i + ENTRYSIZE){
	if(strncmp(&buf[i], looking, NAMESIZE) == 0){
		filetime = (buf[i+TIME2] << 8) | buf[i+TIME];
		second = (filetime & 0x1f) * 2;		//Multiplied by 2 as secods are store as half the actual value
		minute = (filetime >> 5) & 0x3f;
		hour = (filetime >> 11) & 0x1f;
		printf("CODETAB.TXT was created at: %d:%d:%d\n\n", hour, minute, second);
		break;
		}
	}
	
	//TASK 8

	looking = "BIRTH   TOK";
	data_offset = start_of_root + size_of_root;
	int cluster;
	int cluster_location;
	
	for(i = 0; i < BLOCK; i = i + ENTRYSIZE){
        if(strncmp(&buf[i], looking, NAMESIZE) == 0){
		cluster = (buf[i+CLUSTER2] << 8) | buf[i+CLUSTER];
		cluster_location = data_offset + (cluster - 2);
		printf("Cluster offset = %d\n", cluster_location);
                break;
                }
        }

	int j;
	int cluster_num;
	for(j = 0; j<3;j++){
	cluster_num = cluster_location + j;
	fseek(strm, cluster_num, SEEK_SET);
	fread(&buf[0], 1, BLOCK, strm);
	
	printf("Cluster %d\n",cluster_num);	
	for(i=0; i<BLOCK; i++){
	printf("%02X ", buf[i]);
	if((i+1)%16 == 0) printf("\n");
	}
	printf("\n");
	}
	//Unable to find how to get the second and third block	
	
	fclose(strm);
	exit(EXIT_SUCCESS);
}
