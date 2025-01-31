#pragma once

#include <concepts>
#include "./Self.hpp"

template <typename T>
concept HasInitFinish = requires(T t) {
	requires std::is_member_function_pointer_v<decltype(&T::init)>;
	{ t.finish() } noexcept /* note: LZZ doesn't allow noexcept */;
};
template <typename T, typename... Args>
concept HasInitFinishBool = requires(T t, Args... args) {
	{ t.init(args...) } -> std :: convertible_to<bool>;
	{ t.finish() } noexcept /* note: LZZ doesn't allow noexcept */;
};
template <typename T, typename... Args>
concept HasInitFinishVoid = requires(T t, Args... args) {
	{ t.init(args...) } -> std::same_as<void>;
	{ t.finish() } noexcept /* note: LZZ doesn't allow noexcept */;
};
template <typename T> requires (HasInitFinish<T>)
struct Resource : public T {
	bool ok = true;
	template <typename... Args> requires (HasInitFinishBool<T, Args...>)
	explicit inline Resource(T t, Args&&... args) : T(t) {
		self.ok = self.init(args...);
	}
	template <typename... Args> requires (HasInitFinishBool<T, Args...>)
	explicit inline Resource(Args&&... args) {
		self.ok = self.init(args...);
	}
	template <typename... Args> requires (HasInitFinishVoid<T, Args...>)
	explicit inline Resource(T t, Args&&... args) : T(t) {
		self.init(args...);
		self.ok = true;
	}
	template <typename... Args> requires (HasInitFinishVoid<T, Args...>)
	explicit inline Resource(Args&&... args) {
		self.init(args...);
		self.ok = true;
	}
	~Resource() {
		self.finish(); //	we could add some way to check result of finish through bool*
	}
	Resource(Resource<T>&) = delete;
	Resource(Resource<T>&&) = delete;
};

template <typename T> struct  IsResourceT              : std :: false_type {};
template <typename T> struct  IsResourceT<Resource<T>> : std ::  true_type {};
template <typename T> concept IsResource = IsResourceT<T>::value;
template <typename T> concept IsNotResource = !IsResource<T>;

//	RAII_init(var, Type)
//	evade RAII 
//	you should call `new (&var) Type(args)` in init (this is called placement new)
//	and `var.~Type()` in finish
//	you will have to define destructor for enclosing class
#define RAII_init(var, ...) union { char _resource_ ## var = 0; __VA_ARGS__ (var); }
//	#define RAII_init(var, ...) union { char _resource_ ## var[sizeof(__VA_ARGS__)] {0?}; __VA_ARGS__ (var); }







//	struct that uses RAII
//	struct RAII_Struct {
//		RAII_Struct() { /* take resource */ }
//		~RAII_Struct() { /* release resource */ }
//	};
//	
//	struct A {
//		RAII_init(raii, RAII_Struct); //	evade RAII
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
