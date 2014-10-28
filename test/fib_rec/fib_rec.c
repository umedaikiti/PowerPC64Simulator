int __attribute__ ((noinline)) fib(unsigned int n)
{
	if(n <= 1) return 1;
	return fib(n-1) + fib(n-2);
}

int main()
{
	return fib(10);
}
