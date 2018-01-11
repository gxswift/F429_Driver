
/******************************************
//参考 《常用算法程序集 （C语言描述 第三版)》

//最小二乘法
//x[n] y[n] 已知输入
//n输入点个数
//a[m] 返回m-1次拟合多项式的m个系数
//m  拟合多项式的项数,即拟合多项式的最高次为m-1
//dt[3] dt[0]返回拟合多项式与各数据点误差的平方和，
        dt[1]返回拟合多项式与各数据点误差的绝对值之和
        dt[2]返回拟合多项式与各数据点误差的绝对值的最大值

//
//拟合多项式的输出
//Y(x) = a0 + a1(x-X) + a2(x-X)^2 + …… am(x-X)^m
// 其中X为已知点x的平均值
******************************************/

#include "math.h"

/*
double x[20] = {0.0013,0.2848,0.7351,1.4506,1.9468,2.3291};
double y[20] = { 0, 0.2, 0.5, 1, 1.5, 2 };
int n = 6;
double a[20] = { 0 };
int m = 4;*/


void fun(double*x,double*y,int n,double*a,int m)
{ 
	 int m1, i, j, l, ii, k, im, ix[21];
	 float h[21], ha, hh, y1, y2, h1, h2, d, hm;
	for (i = 0; i <= m; i++) a[i] = 0.0;
	if (m >= n) m = n - 1;
	if (m >= 20) m = 19;
	m1 = m + 1;
	ha = 0.0;
	ix[0] = 0; ix[m] = n - 1;
	l = (n - 1) / m; j = l;
	for (i = 1; i <= m - 1; i++)
	{
		ix[i] = j; j = j + l;
	}
	while (1 == 1)
	{
		hh = 1.0;
		for (i = 0; i <= m; i++)
		{
			a[i] = y[ix[i]]; h[i] = -hh; hh = -hh;
		}
		for (j = 1; j <= m; j++)
		{
			ii = m1; y2 = a[ii - 1]; h2 = h[ii - 1];
			for (i = j; i <= m; i++)
			{
				d = x[ix[ii - 1]] - x[ix[m1 - i - 1]];
				y1 = a[m - i + j - 1];
				h1 = h[m - i + j - 1];
				a[ii - 1] = (y2 - y1) / d;
				h[ii - 1] = (h2 - h1) / d;
				ii = m - i + j; y2 = y1; h2 = h1;
			}
		}
		hh = -a[m] / h[m];
		for (i = 0; i <= m; i++)
			a[i] = a[i] + h[i] * hh;
		for (j = 1; j <= m - 1; j++)
		{
			ii = m - j; d = x[ix[ii - 1]];
			y2 = a[ii - 1];
			for (k = m1 - j; k <= m; k++)
			{
				y1 = a[k - 1]; a[ii - 1] = y2 - d*y1;
				y2 = y1; ii = k;
			}
		}
		hm = fabs(hh);
		if (hm <= ha) { a[m] = -hm; return; }
		a[m] = hm; ha = hm; im = ix[0]; h1 = hh;
		j = 0;
		for (i = 0; i <= n - 1; i++)
		{
			if (i == ix[j])
			{
				if (j<m) j = j + 1;
			}
			else
			{
				h2 = a[m - 1];
				for (k = m - 2; k >= 0; k--)
					h2 = h2*x[i] + a[k];
				h2 = h2 - y[i];
				if (fabs(h2)>hm)
				{
					hm = fabs(h2); h1 = h2; im = i;
				}
			}
		}
		if (im == ix[0]) return;
		i = 0; l = 1;
		while (l == 1)
		{
			l = 0;
			if (im >= ix[i])
			{
				i = i + 1;
				if (i <= m) l = 1;
			}
		}
		if (i>m) i = m;
		if (i == (i / 2) * 2) h2 = -hh;
		else h2 = hh;
		if (h1*h2 >= 0.0) ix[i] = im;
		else
		{
			if (im<ix[0])
			{
				for (j = m - 1; j >= 0; j--)
					ix[j + 1] = ix[j];
				ix[0] = im;
			}
			else
			{
				if (im>ix[m])
				{
					for (j = 1; j <= m; j++)
						ix[j - 1] = ix[j];
					ix[m] = im;
				}
				else ix[i - 1] = im;
			}
		}
	}
}
/*
int main()
{
	FILE *fil;
	int i;
	char put;
	int flag;

	printf("曲线拟合计算：\t\t\t作者:gx\r\n\r\n");

	printf("\r\n是否需要导出文件(Y/N):");
	put = getch();
	printf("%c\r\n", put);

	if (put == 'Y' || put == 'y')
		flag = 1;
	else
		flag = 0;

	printf("\r\n输入点数(小于20):");
	scanf_s("%d", &n);
	printf("\r\n输入拟合阶数(小于20):");
	scanf_s("%d", &m);

	printf("\r\n用空格隔开，或者输入后回车(可复制粘贴):");
	printf("\r\n依次输入X轴坐标:");
	for (i = 0; i < n; i++)
		scanf_s("%lf", &x[i]);

	printf("\r\n依次输入Y轴坐标:");
	for (i = 0; i < n; i++)
		scanf_s("%lf", &y[i]);



	fun();
	printf("\r\n-------------------------------------------------------------\r\n");
	for (i = 0; i < n; i++)
		printf("x%d:%lf\t", i, x[i]); printf("\r\n");

	for (i = 0; i < n; i++)
		printf("y%d:%lf\t", i, y[i]); printf("\r\n");
	printf("\r\n-------------------------------------------------------------\r\n");

	for (i = 0; i < m; i++){
		printf("系数%d:%lf\t", i, a[i]); 
	}
	printf("\r\n");
	printf("\r\n多项式:\tY=%lf", a[0]);
	for (i = 1; i < m; i++)
	if (a[i] >= 0){
		printf("+%lfx^%d", a[i], i);
	}
	else{
	printf("%lfx^%d", a[i], i);
	}
	printf("\r\n-------------------------------------------------------------\r\n");
	printf("\r\n代入多项式结果验证\r\n");
	for (i = 0; i < n; i++)
		printf("y%d:%lf\t", i, a[0] + a[1] * x[i] + a[2] * x[i] * x[i] + a[3] * x[i] * x[i] * x[i]);
//--------------------------------------------------------------------------------------
	if (flag){
		fopen_s(&fil, "曲线拟合.txt", "w+");

		fprintf(fil,"导出数据\r\n-------------------------------------------------------------\r\n");
		
		for (i = 0; i < n; i++)
			fprintf(fil,"x%d:%lf\t", i, x[i]); printf("\r\n");
		fprintf_s(fil, "\r\n");
		for (i = 0; i < n; i++)
			fprintf(fil,"y%d:%lf\t", i, y[i]); printf("\r\n");
		fprintf(fil,"\r\n-------------------------------------------------------------\r\n");
		for (i = 0; i < m; i++)
			fprintf_s(fil, "系数%d:%lf\t", i, a[i]); 
		fprintf_s(fil, "\r\n");
		fprintf(fil,"\r\n多项式:\tY=%lf", a[0]);
		for (i = 1; i < m; i++)
		{
			if (a[i] >= 0){
				fprintf(fil, "+%lfx^%d", a[i], i);
			}
			else
				fprintf(fil, "%lfx^%d", a[i], i);
		}
		fprintf(fil, "%lfx^%d", a[i], i);


		fclose(fil);
		printf("\r\n\r\n已导出文件\r\n");
	}

	while (1);
	return 1;

}*/