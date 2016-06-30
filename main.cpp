#include <stdio.h>
#include <stdlib.h>

#include <memory.h>

#define DAT_FILE "./data/qqwry.dat"
#define OUT_FILE "./data/out.txt"

char *g_temp = NULL;
#define TEMP_SIZE 10*1024*1024
FILE *out = NULL;

void showIp(unsigned int ip)
{
	char buffer[20] = { 0 };
	sprintf_s(buffer, "%u.%u.%u.%u  ", (ip >> 24) & 0xff, (ip >> 16) & 0xff, (ip >> 8) & 0xff, ip & 0xff);
	fputs(buffer, out);
}

void getString(FILE *fp)	//�xȡ�ַ�����д���ļ�����0Ϊ��β��־
{
	int index = 0;
	int k = EOF;
	while (1)
	{
		char c = fgetc(fp);
		if (c == EOF)
			break;
		if (c == 0)
			break;

		g_temp[index] = c;
		index++;
		if (index > TEMP_SIZE)
		{
			//printf("g_temp : %s\n", g_temp);
			printf("exit : %d\n", index);
			fclose(out);
			exit(0);
		}
	}
	fputs(g_temp, out);
	memset(g_temp, 0, index);
}

void getInfo(FILE *fp, int flag)	//��ȡ�������ַ�����Ϣ��flagΪ1ʱ��ʾ�Ѿ�����һ���ַ���
{
	unsigned char mod;
	long int offset = 0;
	long int preOffset = 0;

	mod = fgetc(fp);			//��ȡģʽ
	if (mod > 2)				//������ض���
	{
		fseek(fp, -1L, SEEK_CUR);	//����
		getString(fp);			//��ȡһ���ַ���
		if (flag == 0)			//������ú���ʱ��δ��
		{
			getInfo(fp, 1);		//��־��1���ݹ����
		}
		return;
	}
	fread(&offset, 3, 1, fp);		//������ض��򣬻�ȡ�ض���ƫ��
	if (mod == 1)					//���ģʽһ
	{
		fseek(fp, offset, SEEK_SET);	//seek��Ŀ���ַ
		getInfo(fp, flag);				//�ݹ��
		return;
	}
	preOffset = ftell(fp);			//���ģʽ������ס��ǰƫ��
	fseek(fp, offset, SEEK_SET);	//ѰĿ���ַ
	getInfo(fp, 1);					//��һ���ַ���
	if (flag == 0)					//������ú���ʱ��δ��
	{
		fseek(fp, preOffset, SEEK_SET);		//��Ҫ�ص�ԭ��λ��
		getInfo(fp, 1);						//�ٶ�һ��
	}
}

void parseRecord(FILE *fp, long int recordOffset)
{
	unsigned int endIp = 0;

	fseek(fp, recordOffset, SEEK_SET);	//seek��������
	fread(&endIp, 4, 1, fp);		//��ȡ����IP
	showIp(endIp);					//��ʾ����IP
	getInfo(fp, 0);					//��ȡ��Ϣ
	fputs("\n", out);
}

void parseDat(FILE *fp)
{
	long int startIndexOffset = 0;
	long int endIndexOffset = 0;
	long int indexOffset = 0;
	long int recordOffset = 0;
	unsigned int startIp = 0;

	fseek(fp, 0L, SEEK_SET);			//seek�ļ�ͷ
	fread(&startIndexOffset, 4, 1, fp);	//��������ʼƫ����
	fread(&endIndexOffset, 4, 1, fp);	//����������ƫ����
	indexOffset = startIndexOffset;
	while (indexOffset <= endIndexOffset)	//ѭ���Ӹ���������ȡ����
	{
		fseek(fp, indexOffset, SEEK_SET);	//seek������λ��
		fread(&startIp, 4, 1, fp);			//��ȡ�����еĿ�ʼIP
		fread(&recordOffset, 3, 1, fp);		//��ȡ��Ӧ��������ƫ����
		indexOffset = ftell(fp);			//��ס������ƫ����
		showIp(startIp);					//��ʾ��ʼIP
		parseRecord(fp, recordOffset);		//��ȡ������
		//printf("indexOffset : %d\n", indexOffset);
	}
}

int main(void)
{
	g_temp = new char[TEMP_SIZE];
	memset(g_temp, 0, TEMP_SIZE);

	FILE *fp = NULL;

	fopen_s(&out, OUT_FILE, "wb");
	fopen_s(&fp, DAT_FILE, "rb");
	if (fp == NULL)
	{
		printf("open file error!\n");
		return 0;
	}
	parseDat(fp);
	fclose(fp);

	delete g_temp;

	return 0;
}
