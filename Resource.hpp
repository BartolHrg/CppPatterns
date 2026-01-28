#pragma once

#include <concepts>

#ifndef fun
#define fun
#endif

#define self (*this)



template <typename>
struct FunctionTraits;

template <typename T, typename... Args>
struct FunctionTraits<T(Args...)> {
	using ReturnType = T;
};
template <typename T, typename... Args>
struct FunctionTraits<T (*)(Args...)> {
	using ReturnType = T;
};
template <typename T, typename C, typename... Args>
struct FunctionTraits<T (C::*)(Args...)> {
	using ReturnType = T;
};
template <typename F>
using ReturnType = typename FunctionTraits<F> :: ReturnType;

template <typename T>
concept Resourceable = requires(T t) {
	requires std :: is_member_function_pointer_v<decltype(&T :: init)>;
	requires std :: convertible_to<ReturnType<decltype(&T :: init)>, bool>;
	{ t.finish() } noexcept;
};
template <typename T, typename... Args>
concept ActuallyResourceable = requires(T t, Args... args) {
	{ t.init(args...) } -> std :: convertible_to<bool>;
	{ t.finish() } noexcept;
};
template <typename T> requires (Resourceable<T>)
struct Resource : T {
	bool _resource_ok = true;
	template <typename... Args> requires (ActuallyResourceable<T, Args...>)
	fun explicit inline Resource(T t, Args&&... args) : T(t) {
		self._resource_ok = self.init(args...);
	}
	template <typename... Args> requires (ActuallyResourceable<T, Args...>)
	fun explicit inline Resource(Args&&... args) {
		self._resource_ok = self.init(args...);
	}
	fun inline ~Resource() {
		self.finish();
	}
	// Resource(Resource<T>) = delete;
	fun Resource(Resource<T>&) = delete;
	fun Resource(Resource<T>&&) = delete;
	fun operator bool() { return self._resource_ok; }
};


template <typename T> struct  IsResourceT              : std :: false_type {};
template <typename T> struct  IsResourceT<Resource<T>> : std ::  true_type {};
template <typename T> concept IsResource = IsResourceT<T>::value;
template <typename T> concept IsNotResource = !IsResource<T>;

//	RAII_init(var, Type) - bypass RAII 
//	you should call `new (&var) Type(args)` in init (this is called placement new)
//	and `var.~Type()` in finish
//	you will have to define destructor for enclosing class
#define RAII_init(var, ...) union { char _resource_ ## var = 0; __VA_ARGS__ (var); }
//	#define RAII_init(var, ...) union { char _resource_ ## var[sizeof(__VA_ARGS__)] {0?}; __VA_ARGS__ (var); }



// struct ResourceCollectionWatcher {
	
// };




//	struct that uses RAII
//	struct RAII_Struct {
//		RAII_Struct() { /* take resource */ }
//		~RAII_Struct() { /* release resource */ }
//	};
//	
//	struct A {
//		RAII_init(raii, RAII_Struct); //	bypass RAII
//		A(int x) {}
//		~A() {} //	you have to define destructor if using RAII_init
//		void init(float y) { new (&self.raii) RAII_Struct; }
//		void finish() noexcept { self.raii.~RAII_Struct(); }
//		void use() {}
//	};
//	int main() {
//		Resource<A> a({4}, 1.5f);
//		a.use();
//		/* you can even do */ /* but that is only needed if A has member ok that you would want to use */
//		A b = a;
//		b.use();
//	}
