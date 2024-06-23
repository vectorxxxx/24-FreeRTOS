/*
1.�Ƽ���C���Ի�������
	��C���Գ������:�ִ�������
	1-16�¶���C�����﷨������
	https://book.douban.com/subject/4279678/

2.����C�����еĻ���֪ʶ
	1).��������(����char, short, int , long, signed, unsigned, float, double, sizeof)
	2).����Ϳ���( =, +, -, *, while, do-while, if, else, switch, case, continue, break)
	3).���ݴ洢(static, extern, const, volatile) 
	4).�ṹ(struct, enum, union, typedef) 
	5).λ�������߼�����(<<, >>, &, |, ~, ^)
	6).Ԥ����(#define, #include, #if...#elif...#else...#endif��)

3.λ���ֽڡ��ֵĹ�ϵ
	λ(bit):�������������ĵ�λ,��Ӧ0��1
	�ֽ�(Byte):����������ݴ洢�Ļ�����λ,8λ=1�ֽ�
	��(Word):������������ݴ��������ĵ�λ��32λ����,4�ֽ�=1��;64λ����,8�ֽ�=1��;
*/


char heap_buf[1024];
int pos = 0;

void *my_malloc(int size)
{
	int old_pos = pos;
	pos += size;
	return &heap_buf[old_pos];
}

void my_free(void *buf)
{
	/* err */
}


int main(void)
{
	char ch = 65; // char ch = 'A';
	int i;
	char *buf = my_malloc(100);
	
	unsigned char uch = 200;
	
	for (i = 0; i < 26; i++)
		buf[i] = 'A' + i;
	
	return 0;
}

