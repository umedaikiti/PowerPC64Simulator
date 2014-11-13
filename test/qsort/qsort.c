void qsort(int a[], int n, int m)
{
	int p = a[n];
	int i, j;

	if (m - n <= 1) return;

	for(i=n+1;i<m;i++){
		if(p != a[i]){
			if(p < a[i]) p = a[i];
			break;
		}
	}
	if(i == m) return;
	i = n;
	j = m-1;
	for(;;){
		int tmp;
		while(a[i] < p) i++;
		while(a[j] >= p) j--;
		if(i >= j) break;
		tmp = a[i];
		a[i] = a[j];
		a[j] = tmp;
	}
	qsort(a, n, i-1);
	qsort(a, i, m);
}

int test1[] = {2, -1, 3, 4, 2, 1, 1, 5};
int test2[] = {-1, 0};
int test3[] = {2, 2};
int test4[] = {3, 2};
int test5[] = {1, 1, 4, 3, 2};

int main()
{
	qsort(test1, 0, sizeof(test1)/sizeof(test1[0]));
	qsort(test2, 0, sizeof(test2)/sizeof(test2[0]));
	qsort(test3, 0, sizeof(test3)/sizeof(test3[0]));
	qsort(test4, 0, sizeof(test4)/sizeof(test4[0]));
	qsort(test5, 0, sizeof(test5)/sizeof(test5[0]));
	return 0;
}
