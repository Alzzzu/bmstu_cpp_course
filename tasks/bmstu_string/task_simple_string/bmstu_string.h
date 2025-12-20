#pragma once

#include <exception>
#include <iostream>

namespace bmstu
{
template <typename T>
class basic_string;

typedef basic_string<char> string;
typedef basic_string<wchar_t> wstring;
typedef basic_string<char16_t> u16string;
typedef basic_string<char32_t> u32string;

template <typename T>
class basic_string;

typedef basic_string<char> string;
typedef basic_string<wchar_t> wstring;
// typedef basic_string<char8_t> u8string;
typedef basic_string<char16_t> u16string;
typedef basic_string<char32_t> u32string;

template <typename T>
#ifdef _MSC_VER
class basic_string
#else
class basic_string
#endif
{
   public:
	/// Конструктор по умолчанию
	basic_string() : ptr_(new T[1]{0}), size_(0) {}

	basic_string(size_t size) : ptr_(new T[size + 1]), size_(size) {
		for (size_t i=0;i<size_;i++){
			ptr_[i]  =' ';
		}
		ptr_[size_] = '\0';
	}

	basic_string(std::initializer_list<T> il)
		: ptr_(new T[il.size() + 1]), size_(il.size())
	{
		for(size_t i =0; i<size_;i++){
			ptr_[i] =*(il.begin()+i);
		}
		ptr_[size_] ='\0';

	}

	/// Конструктор с параметром си-с
	basic_string(const T* c_str) {
		size_ = strlen_(c_str);
		ptr_ = new T[size_+1];
		for(size_t i = 0;i<size_;i++){
			ptr_[i] = c_str[i]; 
		}
		ptr_[size_] = '\0';
	}

	/// Конструктор копирования
	basic_string(const basic_string& other) {
		size_ = other.size_;
		ptr_ = new T[size_+1];
		for (size_t i =0; i<size_;i++){
			ptr_[i] = other.ptr_[i];
		}
		ptr_[size_] = '\0';
	}

	/// Перемещающий конструктор
	basic_string(basic_string&& dying) : size_(dying.size_), ptr_(dying.ptr_) {
		dying.ptr_ =nullptr;
		dying.size_ = 0;

	}

	/// Деструктор
	~basic_string() {
		delete[] ptr_;
	}

	/// Геттер на си-строку
	const T* c_str() const { return ptr_; }

	size_t size() const { return size_; }

	/// Оператор перемещающего присваивания
	basic_string& operator=(basic_string&& other) { 		
		if(this==&other) return *this;
		delete[] ptr_;
		size_ = other.size_;
		ptr_ = other.ptr_;
		other.ptr_ = new T[1]{'\0'};
		other.size_ = 0;
		return *this; 
	}

	/// Оператор копирующего присваивания си строки
	basic_string& operator=(const T* c_str) {
		delete[] ptr_;
		size_ = strlen_(c_str);
		T* new_ptr_ = new T[size_+1];
		for(size_t i = 0;i<size_;i++){
			new_ptr_[i] = c_str[i]; 
		}
		new_ptr_[size_] = '\0';
		ptr_=new_ptr_;
		return *this; 
	}

	/// Оператор копирующего присваивания
	basic_string& operator=(const basic_string& other) { 
		if(this==&other) return *this;
		delete[] ptr_;
		size_ = other.size_;
		T* new_ptr_ = new T[size_+1];
		for (size_t i =0; i<size_;i++){
			new_ptr_[i] = other.ptr_[i];
		}
		new_ptr_[size_] = '\0';
		ptr_ = new_ptr_;
		return *this; 
	}

	friend basic_string<T> operator+(const basic_string<T>& left,
									 const basic_string<T>& right)
	{
		basic_string new_string(left.size_+right.size_);
		for(size_t l =0;l< left.size_;l++){
			new_string.ptr_[l] = left.ptr_[l];
		}
		for(size_t r =0;r< right.size_;r++){
			new_string.ptr_[left.size_+r] = right.ptr_[r];
		}
		new_string.ptr_[new_string.size_] = '\0';
		return new_string;
	}

	template <typename S>
	friend S& operator<<(S& os, const basic_string& obj)
	{
		os<<obj.c_str();
		return os;
	}

	template <typename S>
	friend S& operator>>(S& is, basic_string& obj)
	{
		obj.clean_();
		obj.ptr_=new T[1]{'\0'};
		T ch;
		while(is.get(ch)){
			obj+=ch;
		}
		return is;
	}

	basic_string& operator+=(const basic_string& other) { 
		T* new_ptr_ = new T[size_+other.size_+1];
		for(size_t i =0; i<size_;i++){
			new_ptr_[i] = ptr_[i];
		}
		for(size_t j = 0; j<other.size_;j++){
			new_ptr_[j+size_] = other.ptr_[j];
		}
		new_ptr_[size_+other.size_] ='\0';
		size_ += other.size_;
		delete[] ptr_;
		ptr_ =new_ptr_;
		return *this; 
	}

	basic_string& operator+=(T symbol) { 
		T* new_ptr_ = new T[size_+2];
		for(size_t i =0; i<size_;i++){
			new_ptr_[i] = ptr_[i];
		}
		new_ptr_[size_] = symbol;
		new_ptr_[size_+1] = '\0';
		delete[] ptr_;
		ptr_ = new_ptr_;
		size_++;
		return *this; 
	}

	T& operator[](size_t index) noexcept { return *(ptr_ + index); }

	T& at(size_t index) { 
		if(index<0||index>=size_){
		throw std::out_of_range("Wrong index"); 
		}
		return ptr_[index]; 
	}

	T* data() { return ptr_; }

   private:
	static size_t strlen_(const T* str) { 
		size_t len = 0;
		while (str[len]!='\0'){
			len++;
		}
		return len; 
	}

	void clean_() {
		delete[] ptr_;
		ptr_ =nullptr;
		size_=0;
	}

	T* ptr_ = new T[1]{'\0'};
	size_t size_;
};
}  // namespace bmstu
