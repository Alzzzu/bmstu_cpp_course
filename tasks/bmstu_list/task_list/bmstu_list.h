#include <cstddef>
#include <iterator>
#include <ostream>
#include "abstract_iterator.h"

namespace bmstu
{
template <typename T>
class list
{
	struct node
	{
		node() = default;

		node(node* prev, const T& value, node* next)
			: next_node_(next), prev_node_(prev), value_(value)
		{
		}

		T value_;
		node* next_node_ = nullptr;
		node* prev_node_ = nullptr;
	};

   public:
	struct iterator
		: public abstract_iterator<iterator, T, std::bidirectional_iterator_tag>
	{
		node* current;
		iterator() : current(nullptr) {}
		iterator(node* node) : current(node) {}
		iterator& operator++() override
		{
			current = current->next_node_;
			return *this;
		}
		iterator& operator--() override
		{
			current = current->prev_node_;
			return *this;
		}
		iterator operator++(int) override
		{
			iterator temp = iterator(current);
			current = current->next_node_;
			return temp;
		}
		iterator operator--(int) override
		{
			iterator temp = iterator(current);
			current = current->prev_node_;
			return temp;
		}
		iterator& operator+=(
			const typename abstract_iterator<
				iterator,
				T,
				std::bidirectional_iterator_tag>::difference_type& n) override
		{
			for (int i = 0; i < n; i++)
			{
				current = current->next_node_;
			}
			return *this;
		}
		iterator& operator-=(
			const typename abstract_iterator<
				iterator,
				T,
				std::bidirectional_iterator_tag>::difference_type& n) override
		{
			for (int i = 0; i < n; i++)
			{
				current = current->prev_node_;
			}
			return *this;
		}
		iterator operator+(const typename abstract_iterator<
						   iterator,
						   T,
						   std::bidirectional_iterator_tag>::difference_type& n)
			const override
		{
			iterator new_it = iterator(current);
			new_it += n;
			return new_it;
		}
		iterator operator-(const typename abstract_iterator<
						   iterator,
						   T,
						   std::bidirectional_iterator_tag>::difference_type& n)
			const override
		{
			iterator new_it = iterator(current);
			new_it -= n;
			return new_it;
		}
		typename abstract_iterator<iterator,
								   T,
								   std::bidirectional_iterator_tag>::reference
		operator*() const override
		{
			return current->value_;
		}
		typename abstract_iterator<iterator,
								   T,
								   std::bidirectional_iterator_tag>::pointer
		operator->() const override
		{
			return &(current->value_);
		}
		bool operator==(const iterator& other) const override
		{
			return current == other.current;
		}
		bool operator!=(const iterator& other) const override
		{
			return current != other.current;
		}
		explicit operator bool() const override { return current != nullptr; }
		typename abstract_iterator<
			iterator,
			T,
			std::bidirectional_iterator_tag>::difference_type
		operator-(const iterator& other) const override
		{
			size_t cnt = 0;
			iterator tracker = iterator(current);
			while (tracker != other)
			{
				cnt++;
				tracker--;
			}

			return cnt;
		}
	};
	using const_iterator = iterator;

	list()
	{
		tail_ = new node();
		head_ = new node();
		head_->next_node_ = tail_;
		tail_->prev_node_ = head_;
		size_ = 0;
	}

	template <typename it>
	list(it begin, it end)
	{
		tail_ = new node();
		head_ = new node();
		node* curr = head_;
		for (it i = begin; i != end; i++)
		{
			curr->next_node_ = new node(curr, *i, nullptr);
			curr = curr->next_node_;
			size_++;
		}
		curr->next_node_ = tail_;
		tail_->prev_node_ = curr;
	}

	list(std::initializer_list<T> values)
	{
		tail_ = new node();
		head_ = new node();
		node* curr = head_;

		for (T el : values)
		{
			curr->next_node_ = new node(curr, el, nullptr);
			curr = curr->next_node_;
		}
		size_ = values.size();
		curr->next_node_ = tail_;
		tail_->prev_node_ = curr;
	}

	list(const list& other)
	{
		tail_ = new node();
		head_ = new node();
		node* curr = head_;
		for (iterator i = other.begin(); i != other.end(); i++)
		{
			curr->next_node_ = new node(curr, *i, nullptr);
		}
		size_ = other.size_;
		curr->next_node_ = tail_;
		tail_->prev_node_ = curr;
	}

	list(list&& other)
	{
		tail_ = new node();
		head_ = new node();
		head_->next_node_ = tail_;
		tail_->prev_node_ = head_;
		size_ = 0;
		swap(other);
	}

#pragma endregion
#pragma region pushs

	template <typename Type>
	void push_back(const Type& value)
	{
		node* last = tail_->prev_node_;
		node* new_last = new node(tail_->prev_node_, value, tail_);
		tail_->prev_node_ = new_last;
		last->next_node_ = new_last;
		++size_;
	}

	template <typename Type>
	void push_front(const Type& value)
	{
		// адрес реального последнего элемента
		node* first = head_->next_node_;
		node* new_first = new node(head_, value, first);
		head_->next_node_ = new_first;
		first->prev_node_ = new_first;
		++size_;
	}

#pragma endregion

	bool empty() const

		noexcept
	{
		return (size_ == 0u);
	}

	~list()
	{
		clear();
		delete head_;
		delete tail_;
	}

	void clear()
	{
		node* curr = head_->next_node_;
		for (int i = 0; i < size_; i++)
		{
			curr = curr->next_node_;
			delete curr->prev_node_;
		}
		head_->next_node_ = tail_;
		tail_->prev_node_ = head_;
		size_ = 0;
	}

	size_t size() const { return size_; }

	void swap(list& other)

		noexcept
	{
		std::swap(size_, other.size_);
		std::swap(head_, other.head_);
		std::swap(tail_, other.tail_);
	}

	friend void swap(list& l, list& r) { l.swap(r); }

#pragma region iterators

	iterator begin()

		noexcept
	{
		return iterator{head_->next_node_};
	}

	iterator end()

		noexcept
	{
		return iterator{tail_};
	}

	const_iterator begin() const

		noexcept
	{
		return const_iterator{head_->next_node_};
	}

	const_iterator end() const

		noexcept
	{
		return const_iterator{tail_};
	}

	const_iterator cbegin() const

		noexcept
	{
		return const_iterator{head_->next_node_};
	}

	const_iterator cend() const

		noexcept
	{
		return const_iterator{tail_};
	}

#pragma endregion

	T operator[](size_t pos) const
	{
		iterator current = begin();
		for (size_t i = 0; i < pos; i++)
		{
			current++;
		}
		return *current;
	}

	T& operator[](size_t pos)
	{
		iterator current = begin();
		for (size_t i = 0; i < pos; i++)
		{
			current++;
		}
		return *current;
		//	return *(static_cast<T*>((void*)&pos));
	}

	friend bool operator==(const list& l, const list& r)
	{
		if (l.size_ != r.size_)
			return false;
		iterator j = r.begin();
		for (iterator i = l.begin(); i != l.end(); i++, j++)
		{
			if (*i != *j)
				return false;
		}
		return true;
	}

	friend bool operator!=(const list& l, const list& r) { return !(l == r); }

	friend auto operator<=>(const list& lhs, const list& rhs)
	{
		iterator j = rhs.begin();
		for (iterator i = lhs.begin(); i != lhs.end() && j != rhs.end();
			 i++, j++)
		{
			if (*i != *j)
			{
				return *i <=> *j;
			}
		}
		return lhs.size_ <=> rhs.size_;
	}

	friend std::ostream& operator<<(std::ostream& os, const list& other)
	{
		os << "{";
		for (iterator i = other.begin(); i != other.end();)
		{
			os << *i;
			os << (++i == other.end() ? "" : ", ");
		}
		os << "}";
		return os;
	}

	iterator insert(const_iterator pos, const T& value)
	{
		node* prev = pos.current->prev_node_;
		node* new_node = new node(prev, value, pos.current);
		prev->next_node_ = new_node;
		pos.current->prev_node_ = new_node;
		return --pos;
	}

   private:
	static bool lexicographical_compare_(const list<T>& l, const list<T>& r)
	{
		iterator j = r.begin();
		for (iterator i = l.begin(); i != l.end() && j != r.end(); i++, j++)
		{
			if (*i != *j)
			{
				return *i < *j;
			}
		}
		return l.size_ < r.size_;
	}

	size_t size_ = 0;
	node* tail_ = nullptr;
	node* head_ = nullptr;
};
}  // namespace bmstu