extern "C" void (*start_ctors)();
extern "C" void (*end_ctors)();

extern "C" void call_constructors()
{
	void (**i)();
	for (i=&start_ctors; i!=&end_ctors; i++)
		(*i)();
}

