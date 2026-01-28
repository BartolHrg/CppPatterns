#pragma once

#include <type_traits>

#ifndef fun
#define fun
#endif

//	defers execution of a code block.
//	It can use variables declared before it.
//	defer {/* code */};
#define defer _DeferVar const _defer_macro1(__COUNTER__) = [&] noexcept
#define _defer_macro1(cnt) _defer_macro2(cnt)
#define _defer_macro2(cnt) tmp_defer_var_ ## cnt
template <typename F> requires (std :: is_nothrow_invocable_v<F>)
struct _DeferVar {
	F f;
	fun inline _DeferVar(F f) : f(f) {}
	fun inline ~_DeferVar() {
		f();
	}
};








//	e.g.
//	bool f() {
//		FILE* f = fopen(...);
//		if (!f) { return false; }
//		defer { fclose(f); };
//		//	file operations
//		fscanf(f, "%d", ...);
//	}




//	void h() noexcept {
//		std :: cout << "from h with love" << std :: endl;
//	}
//	
//	// void m() {
//	auto& m() {
//		defer { h(); };
//		defer { //	this is preceeded by noexcept by-reference lambda: [&] noexcept 
//			std :: cout << "lambda" << std :: endl;
//		};
//		return std :: cout << "hello" << std :: endl;
//	}
//	int main() {
//		m();
//	}
//	//	hello
//	//	lambda
//	//	from h with love