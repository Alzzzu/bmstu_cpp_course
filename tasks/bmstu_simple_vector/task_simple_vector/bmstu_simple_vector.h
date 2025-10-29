#include <ostream>
#include <stdexcept>
#include <utility>
#include "array_ptr.h"

namespace bmstu
{
template <typename T>
class simple_vector
{
   public:
	class iterator
	{
	   public:
		using iterator_category = std::contiguous_iterator_tag;
		using value_type = T;
		using pointer = T*;
		using reference = T&;
		using difference_type = std::ptrdiff_t;

		iterator() = default;

		iterator(const iterator& other) : ptr_(other.ptr_) {}

		iterator(std::nullptr_t) noexcept : ptr_(nullptr) {}

		iterator(iterator&& other) noexcept : ptr_(other.ptr_)
		{
			other.ptr_ = nullptr;
		}

		explicit iterator(pointer ptr) : ptr_(ptr) {}

		reference operator*() const { return *ptr_; }

		pointer operator->() const { return ptr_; }

		friend pointer to_address(const iterator& it) noexcept
		{
			return it.ptr_;
		}

		iterator& operator=(const iterator& other)
		{
			ptr_ = other.ptr_;
			return *this;
		};

		iterator& operator=(iterator&& other) noexcept
		{
			ptr_ = other.ptr_;
			other.ptr_ = nullptr;
			return *this;
		}

#pragma region Operators
		iterator& operator++()
		{
			ptr_++;
			return *this;
		}

		iterator& operator--()
		{
			ptr_--;
			return *this;
		}

		iterator operator++(int)
		{
			iterator temp = iterator(ptr_);
			ptr_++;
			return temp;
		}

		iterator operator--(int)
		{
			iterator temp = iterator(ptr_);
			ptr_--;
			return temp;
		}

		explicit operator bool() const { return ptr_ != nullptr; }

		friend bool operator==(const iterator& lhs, const iterator& rhs)
		{
			return lhs.ptr_ == rhs.ptr_;
		}

		friend bool operator==(const iterator& lhs, std::nullptr_t)
		{
			return lhs.ptr_ == nullptr;
		}

		iterator& operator=(std::nullptr_t) noexcept
		{
			ptr_ = nullptr;
			return *this;
		}

		friend bool operator==(std::nullptr_t, const iterator& rhs)
		{
			return rhs.ptr_ == nullptr;
		}

		friend bool operator!=(const iterator& lhs, const iterator& rhs)
		{
			return lhs.ptr_ != rhs.ptr_;
		}

		iterator operator+(const difference_type& n) const noexcept
		{
			return iterator(ptr_ + n);
		}

		iterator operator+=(const difference_type& n) noexcept
		{
			ptr_ += n;
			return *this;
		}

		iterator operator-(const difference_type& n) const noexcept
		{
			return iterator(ptr_ - n);
		}

		iterator operator-=(const difference_type& n) noexcept
		{
			ptr_ -= n;
			return *this;
		}

		friend difference_type operator-(const iterator& end,
										 const iterator& begin) noexcept
		{
			return end.ptr_ - begin.ptr_;
		}

#pragma endregion
	   private:
		pointer ptr_ = nullptr;
	};

	simple_vector() noexcept = default;

	~simple_vector() { clear(); };

	simple_vector(std::initializer_list<T> init) noexcept
	{
		size_ = init.size();
		capacity_ = init.size();
		T* new_ptr = static_cast<T*>(operator new(sizeof(T) * (capacity_)));
		data_ = array_ptr<T>(new_ptr);
		size_t i = 0;
		for (T el : init)
		{
			new (&data_[i]) T(el);
			i++;
		}
	}

	simple_vector(const simple_vector& other)
	{
		size_ = other.size_;
		capacity_ = other.capacity_;
		T* new_ptr = static_cast<T*>(operator new(sizeof(T) * (capacity_)));
		for (size_t i = 0; i < size_; i++)
		{
			new (&new_ptr[i]) T(other[i]);
		}
		data_ = array_ptr<T>(new_ptr);
	}

	simple_vector(simple_vector&& other) noexcept { swap(other); }

	simple_vector& operator=(const simple_vector& other)
	{
		clear();
		size_ = other.size_;
		capacity_ = other.capacity_;
		T* new_ptr = static_cast<T*>(operator new(sizeof(T) * (capacity_)));
		for (size_t i = 0; i < size_; i++)
		{
			new (&new_ptr[i]) T(other[i]);
		}
		data_ = array_ptr<T>(new_ptr);
		return *this;
	}

	simple_vector& operator=(simple_vector&& dying)
	{
		clear();
		swap(dying);
		return *this;
	}

	simple_vector(size_t size, const T& value = T{})
	{
		size_ = size;
		capacity_ = size;
		data_ = array_ptr<T>(size, value);
	}

	iterator begin() noexcept { return iterator(data_.get()); }

	iterator end() noexcept { return iterator(data_.get() + size_); }

	using const_iterator = iterator;

	const_iterator begin() const noexcept { return iterator(data_.get()); }

	const_iterator end() const noexcept
	{
		return iterator(data_.get() + size_);
	}

	typename iterator::reference operator[](size_t index) noexcept
	{
		return data_[index];
	}

	typename const_iterator::reference operator[](size_t index) const noexcept
	{
		return data_.get()[index];
	}

	typename iterator::reference at(size_t index)
	{
		if (index >= size_)
			throw std::out_of_range("Index out of range");
		return data_.get()[index];
	}

	typename const_iterator::reference at(size_t index) const
	{
		if (index >= size_)
			throw std::out_of_range("Index out of range");
		return data_.get()[index];
	}

	size_t size() const noexcept { return size_; }

	size_t capacity() const noexcept { return capacity_; }

	void swap(simple_vector& other) noexcept
	{
		std::swap(this->size_, other.size_);
		std::swap(this->capacity_, other.capacity_);
		std::swap(this->data_, other.data_);
	}

	friend void swap(simple_vector& lhs, simple_vector& rhs) noexcept
	{
		lhs.swap(rhs);
	}

	void reserve(size_t new_cap)
	{
		if (new_cap > capacity_)
		{
			capacity_ = new_cap;
			T* new_data =
				static_cast<T*>(operator new(sizeof(T) * (capacity_)));
			for (size_t i = 0; i < size_; i++)
			{
				new (&new_data[i]) T(std::move(data_[i]));
				data_[i].~T();
			}
			data_ = array_ptr<T>(new_data);
		}
	}

	void resize(size_t new_size)
	{
		if (new_size > capacity_)
		{
			capacity_ = new_size;
			T* new_ptr = static_cast<T*>(operator new(sizeof(T) * (capacity_)));
			for (size_t i = 0; i < size_; i++)
			{
				new (&new_ptr[i]) T(std::move(data_[i]));
				data_[i].~T();
			}
			data_ = array_ptr<T>(new_ptr);
		}
		for (size_t i = new_size; i < size_; i++)
		{
			data_[i].~T();
			new (&data_[i]) T{};
		}
		size_ = new_size;
		return;
	}

	iterator insert(const_iterator where, T&& value)
	{
		size_t index = to_address(where) - data_.get();
		if (size_ + 1 > capacity_)
		{
			capacity_ = capacity_ == 0 ? 1 : capacity_ * 2;
			T* new_data = static_cast<T*>(operator new(sizeof(T) * capacity_));
			for (size_t i = 0; i < index; i++)
			{
				new (&new_data[i]) T(std::move(data_[i]));
				data_[i].~T();
			}
			new (&new_data[index]) T(std::move(value));
			for (size_t i = index + 1; i <= size_; i++)
			{
				new (&new_data[i]) T(std::move(data_[i - 1]));
				data_[i - 1].~T();
			}
			data_ = array_ptr<T>(new_data);
			size_++;
		}
		else
		{
			for (size_t i = index + 1; i <= size_; i++)
			{
				new (&data_[i]) T(std::move(data_[i - 1]));
				data_[i - 1].~T();
			}
			new (&data_[index]) T(std::move(value));
		}
		return iterator(data_.get() + index);
	}

	iterator insert(const_iterator where, const T& value)
	{
		size_t index = to_address(where) - data_.get();
		if (size_ + 1 > capacity_)
		{
			capacity_ = capacity_ == 0 ? 1 : capacity_ * 2;
			T* new_data = static_cast<T*>(operator new(sizeof(T) * capacity_));
			for (size_t i = 0; i < index; i++)
			{
				new (&new_data[i]) T(std::move(data_[i]));
				data_[i].~T();
			}
			new (&new_data[index]) T(value);
			for (size_t i = index + 1; i <= size_; i++)
			{
				new (&new_data[i]) T(std::move(data_[i - 1]));
				data_[i - 1].~T();
			}
			data_ = array_ptr<T>(new_data);
			size_++;
		}
		else
		{
			for (size_t i = index + 1; i <= size_; i++)
			{
				new (&data_[i]) T(std::move(data_[i - 1]));
				data_[i - 1].~T();
			}
			new (&data_[index]) T(value);
		}
		return iterator(data_.get() + index);
	}

	void push_back(T&& value)
	{
		if (size_ >= capacity_)
		{
			capacity_ = capacity_ == 0 ? 1 : capacity_ * 2;
			T* new_data =
				static_cast<T*>(operator new(sizeof(T) * (capacity_)));
			for (size_t i = 0; i < size_; i++)
			{
				new (&new_data[i]) T(std::move(data_[i]));
				data_[i].~T();
			}
			data_ = array_ptr<T>(new_data);
		}
		new (&data_[size_]) T(std::move(value));
		size_++;
	}

	void clear() noexcept
	{
		for (size_t i = 0; i < size_; i++)
		{
			data_[i].~T();
		}
		size_ = 0;
	}

	void push_back(const T& value)
	{
		if (size_ + 1 > capacity_)
		{
			capacity_ = capacity_ == 0 ? 1 : capacity_ * 2;
			T* new_data =
				static_cast<T*>(operator new(sizeof(T) * (capacity_)));
			for (size_t i = 0; i < size_; i++)
			{
				new (&new_data[i]) T(std::move(data_[i]));
				data_[i].~T();
			}
			data_ = array_ptr<T>(new_data);
		}
		new (&data_[size_]) T(value);
		size_++;
	}

	bool empty() const noexcept { return size_ == 0; }

	void pop_back()
	{
		if (size_ > 0)
		{
			data_[size_ - 1].~T();
			size_--;
		}
		return;
	}

	friend bool operator==(const simple_vector& lhs, const simple_vector& rhs)
	{
		if (lhs.size_ != rhs.size_)
			return false;
		for (size_t i = 0; i < lhs.size_; i++)
		{
			if (lhs[i] != rhs[i])
				return false;
		}
		return true;
	}

	friend bool operator!=(const simple_vector& lhs, const simple_vector& rhs)
	{
		return !(lhs == rhs);
	}

	friend auto operator<=>(const simple_vector& lhs, const simple_vector& rhs)
	{
		for (size_t i = 0; i < std::min(lhs.size_, rhs.size_); i++)
		{
			if (lhs.data_[i] != rhs.data_[i])
			{
				return lhs.data_[i] <=> rhs.data_[i];
			}
		}
		return lhs.size_ <=> rhs.size_;
	}

	friend std::ostream& operator<<(std::ostream& os, const simple_vector& vec)
	{
		os << "[";
		for (size_t i = 0; i < vec.size_; i++)
		{
			os << vec[i];
			if (i != vec.size_ - 1)
				os << ", ";
		}
		os << "]";
		return os;
	}
	iterator erase(iterator where)
	{
		size_t index = to_address(where) - data_.get();
		data_[index].~T();
		for (size_t i = index; i < size_ - 1; i++)
		{
			new (&data_[i]) T(std::move(data_[i + 1]));
			data_[i + 1].~T();
		}

		size_--;
		return iterator(data_.get() + index);
	}

   private:
	static bool alphabet_compare(const simple_vector<T>& lhs,
								 const simple_vector<T>& rhs)
	{
		for (size_t i = 0; i < std::min(lhs.size_, rhs.size_); i++)
		{
			if (lhs.data_[i] != rhs.data_[i])
			{
				return lhs.data_[i] < rhs.data_[i];
			}
		}
		return lhs.size_ < rhs.size_;
	}
	array_ptr<T> data_;
	size_t size_ = 0;
	size_t capacity_ = 0;
};
}  // namespace bmstu
