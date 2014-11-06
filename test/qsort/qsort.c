void qsort100(int *a, unsigned int n)
{
	int m;
	int a1[100];
	int a2[100];
	unsigned int i, i1 = 0, i2 = 0;

	m = a[0];
	for(i=1;i<n;i++){
		if(a[i] != m){
			if(a[i] > m){
				m = a[i];
			}
			break;
		}
	}
	if(i == n) return;

	for(i=0;i<n;i++){
		if(a[i] < m){
			a1[i1++] = a[i];
		}
		else{
			a2[i2++] = a[i];
		}
	}
	qsort100(a1, i1);
	qsort100(a2, i2);

	for(i=0;i<i1;i++){
		a[i] = a1[i];
	}
	for(i=0;i<i2;i++){
		a[i1+i] = a2[i];
	}
}

int test1[] = {2, -1, 3, 4, 2, 1, 1, 5};
int test2[] = {-1, 0};
int test3[] = {2, 2};
int test4[] = {3, 2};
int test5[] = {1, 1, 4, 3, 2};

int main()
{
	qsort100(test1, sizeof(test1)/sizeof(test1[0]));
	qsort100(test2, sizeof(test2)/sizeof(test2[0]));
	qsort100(test3, sizeof(test3)/sizeof(test3[0]));
	qsort100(test4, sizeof(test4)/sizeof(test4[0]));
	qsort100(test5, sizeof(test5)/sizeof(test5[0]));
	return 0;
}
