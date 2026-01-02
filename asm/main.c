int fact(int n);

int main() 
{
	return fact(10);
}

int fact(int n) 
{
	if (n <= 1) return 1;
	return fact(n - 1) * n;
}
