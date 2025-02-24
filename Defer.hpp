#include <iostream>
#include <type_traits>

template <auto f> requires std::is_nothrow_invocable_v<decltype(f)>
struct DeferVar {
	~DeferVar() {
		f();
	}
};
//	this is surronded by noexcept by-reference lambda: [&] noexcept {  }
#define defer(...) _defer_macro2(__COUNTER__, __VA_ARGS__)
#define _defer_macro2(...) _defer_macro1(__VA_ARGS__)
#define _defer_macro1(cnt, ...) [[no_unique_address]] DeferVar<[&]noexcept{__VA_ARGS__}> const tmp_defer_var_ ## cnt












//	void h() noexcept {
//		std :: cout << "from h with love" << std :: endl;
//	}

//	// void m() {
//	auto& m() {
//		defer(h(););
//		defer({h();});
//		defer( //	this is surronded by noexcept by-reference lambda: [&] noexcept {  }
//			std :: cout << "lambda" << std :: endl;
//		);
//		defer({
//			std :: cout << "lambda" << std :: endl;
//		});
//		DeferVar<[&] noexcept {
//			std :: cout << "custom variable" << std :: endl;
//			return;
//		}> tmp;
//		return std :: cout << "hello" << std :: endl;
//	}
//	int main() {
//		m();
//	}
//	//	hello
//	//	custom variable
//	//	lambda
//	//	lambda
//	//	from h with love
//	//	from h with love