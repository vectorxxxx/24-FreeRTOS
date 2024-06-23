/*
1.推荐的C语言基础资料
	《C语言程序设计:现代方法》
	1-16章都是C语言语法基础。
	https://book.douban.com/subject/4279678/

2.简述C语言中的基础知识
	1).数据类型(常用char, short, int , long, signed, unsigned, float, double, sizeof)
	2).运算和控制( =, +, -, *, while, do-while, if, else, switch, case, continue, break)
	3).数据存储(static, extern, const, volatile) 
	4).结构(struct, enum, union, typedef) 
	5).位操作和逻辑运算(<<, >>, &, |, ~, ^)
	6).预处理(#define, #include, #if...#elif...#else...#endif等)

3.位、字节、字的关系
	位(bit):计算机中最基本的单位,对应0或1
	字节(Byte):计算机中数据存储的基本单位,8位=1字节
	字(Word):计算机进行数据处理和运算的单位。32位机中,4字节=1字;64位机中,8字节=1字;
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

