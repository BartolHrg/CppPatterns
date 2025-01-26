# CppPatterns
my common patterns for C++

## self

```c++
#define self (*this)
struct S {
	int x;
	int f() {
		return self.x;
	}
}
```

## Resource
Resource Acquisition Is NOT Initialization!

```c++
struct Logger {
	bool init(char const* const path) {
		self.file = fopen(path, "w");
		return self.file != nullptr;
	}
	void finish() noexcept {
		fclose(self.file);
	}
	void write(int n) {
		fprintf(self.file, "%d\n", n); 
	}
private:
	FILE* file;
};
int main() {
	Resource<Logger> log("./a.log");
	log.write(8);
}
```
Another example  
```c++
struct RAII_Struct {
	RAII_Struct() { /* take resource */ }
	~RAII_Struct() { /* release resource */ }
};
struct A {
	RAII_init(raii, RAII_Struct);
	A(int x) {}
	~A() {} //	you have to define destructor if using RAII_init
	void init(float y) { new (&self.raii) RAII_Struct; }
	void finish() noexcept { self.raii.~RAII_Struct(); }
	void use() {}
};
int main() {
	Resource<A> a({4}, 1.5f); //	{4} is instance of A; 1.5f is argument of init
	a.use();
	/* you can even do */ 
	A b = a;
	b.use();
	/* but that is only needed if A::ok exists that you would want to use */
}
```
Combination with smart pointers  
(even though smart pointers are not needed when using init/finish + Resource)
```c++
int main() {
	std::shared_ptr<Resource<Logger>> log = std::make_shared<Resource<Logger>>("./a.log");
	log->write(8);
}
```
