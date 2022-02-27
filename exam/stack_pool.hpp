

/* This program is a pool for fast stack whish is a LIFO (Fist In First Out) collection.
1)It store each node in a std::vector<node_t> where the address of the node is 1+idx, where idx is the
index where the node is store in the vector. 
2)The adress 0 is consider end().

3) The fisrt node stored in vector will be put at idx==0, but it will be referenced as 1 */
#include <iostream>
#include <vector>
#include <exception>  // this provide a way to react to exceptional circunstances (like  runtime errors) in 
// the program by transfering control to special functions called handlers.
#include <algorithm>


/*
The following class  iterator is develloped to access the sequence of elements
in stack_pool class in the direction that goes from its begining towards its ends.
*/
template <typename P, typename N, typename T>
class _iterator {
    // Iterator to be used on stack from the pool
    P* stackpool;
    N ptr; //stack_type

public:

    using value_type = T;
    using reference = value_type&;
    using pointer = value_type*;
    using difference_type = std::ptrdiff_t;
    using iterator_category = std::forward_iterator_tag;

    // Let's construct the  iterator
    _iterator(P* _stackpool, N _ptr) : stackpool{ _stackpool }, ptr{ _ptr } {}

    reference operator*() const { return stackpool->value(ptr); }// this is used  for overriding the operator * 

    _iterator& operator++() {// this is used for pre-incrementation  point to the next node and overriding the operator ++
        ptr = stackpool->next(ptr);
        return *this; // this return _iterator type
    }

    _iterator operator++(int) { // This is the post incrementation, it  copy the point this-> temp
		// increment _iterator and return the non incremented value 
        auto temp = *this;
        ++ptr; 
        return temp;
    }


    friend bool operator!=(const _iterator& x, const _iterator& y) { //  // friend pool have acces to private elements of _iterator 
        return !(x == y);
    }// This compare two iterator and return false if both are different.   

    friend bool operator==(const _iterator& x, const _iterator& y) {
        return x.ptr == y.ptr;
    }// this compare two interator and return TRUE if both are equal

};

// In the following line, three cases for exception are consider: 

//1)  Popping an empty stack -> exception 

struct EmptyStack : public std::exception { 
    std::string message;
    EmptyStack(std::string message) : message{ message } {}
};

//2) Popping a stack with non head value -> exception

struct NoHeadStack : public std::exception {
    std::string message;
    NoHeadStack(std::string message) : message{ message } {}
};

//

// 3) Different types passed to pool durring push 

struct DifferentType : public std::exception {
    std::string message;
    DifferentType(std::string message) : message{ message } {}
};


template <typename T, typename N = std::size_t>
class stack_pool { 

    struct node_t {
        T value{0};
        N next {end()};
        node_t( N _next) :  next{ _next }{}

    }; // structure of the node 

    using stack_type = N;
    using value_type = T;
    using size_type = typename std::vector<node_t>::size_type;
    std::vector<node_t> pool;  // vector to save the nodes

    stack_type free_nodes{ end() }; // initialize the node to empty

    node_t &node(stack_type x) noexcept { return pool[x - 1]; } //  noexcept is used to detect at compile-time if 
	// a function can throw an exception.
 
    const node_t &node(stack_type x) const noexcept { return pool[x - 1]; }// this synatxe and the one on above 
// will put the node at idx==0, but will be referency at 1 in order to use zero as the refrence for zero. 
// we are overloading node()


	void free_node(const stack_type x) noexcept {
		next(x) = free_nodes;
		free_nodes =x;} // add x as head if the node is free 

	void  check_free_nodes() {
		
		if(empty(free_nodes)){
			if(capacity())
				reserve(capacity()*size_type(2));
		
			else
			reserve(2); // 0 for end() and idx=1 for next() 
		}
		}


	stack_type get_node() {
		check_free_nodes(); 
		auto tmp = free_nodes;
		
		if(free_nodes == capacity()) { free_nodes = end();}
		else {
			if(next(free_nodes)){ free_nodes=next(free_nodes);}
			else {
				if(capacity() !=pool.size()){
					free_nodes = free_nodes + stack_type(1);
					pool.push_back(node_t{end()});
				}else { free_nodes= end();}
			     }
		     }
		return tmp;
	}

	// Let's pushes element at the front of the stack
	// the following will be used to insert the head

	template<typename X> stack_type _push(X &&x, stack_type head){
		auto tmp = get_node();
		next(tmp) = head;	
		value(tmp) = std::forward<X>(x);
		return tmp;
		}

public:

    stack_pool() = default; // Initialize empty stack 

    explicit stack_pool(size_type n) { 
		pool.reserve(n);
		pool.push_back(node_t{end()});
		free_nodes =1;
		next(free_nodes) = end();
	 }// reserve n node in the pool.

    using iterator = _iterator<stack_pool, N, T>; // iterator on stack_pool
    using const_iterator = _iterator<stack_pool, N, const T>;

    iterator begin(stack_type x) noexcept { return iterator(this, x); }
    iterator end(stack_type) noexcept { return iterator(this, end()); }

    const_iterator begin(stack_type x) const noexcept { return const_iterator(this,&node(x)); }
// we use const_iterator to fixe the refernce 
    const_iterator end(stack_type) const noexcept { return const_iterator(this, end()); }

    const_iterator cbegin(stack_type x) const noexcept { return const_iterator(this, x); }
    const_iterator cend(stack_type) const noexcept { return const_iterator(this, end()); }

    stack_type new_stack()const noexcept { return end(); }//  new empty stack of type stact_pool





    void reserve(size_type n) { 
		auto tmp = capacity();

	if(n> tmp){
		pool.reserve(n);
		if(free_nodes != pool.size() || free_nodes ==end())
		{
			pool.push_back(node_t{end()});
			free_nodes = tmp +1;
		}
		}
	 }// update the size of the vector 

    size_type capacity() const noexcept { return pool.capacity(); }// get the  std::vector size




    bool empty(stack_type x) const noexcept { return x == end(); }// This return a booleen for stack empty or not.

    stack_type end() const noexcept { return stack_type(0); }// return the last element of stack

    T &value(stack_type x) { return node(x).value; }
    const T& value(stack_type x) const { return node(x).value; } // return top node value 


    stack_type &next(stack_type x) { return node(x).next; }// this return the index of the next node
    const stack_type &next(stack_type x) const { return node(x).next; }// return the index of the next node.



    stack_type push(const T &val, stack_type head) { return _push(val, head); }// this insert the value to the head
// it could throw exception for invalide insertion type. 

    stack_type push(T &&val, stack_type head) { return _push(std::move(val), head);}



// The folowing  method will be used for head insertion on the stack
//    template<typename H>
 

  // stack_type _push(H&& val, const stack_type head) {
    //   
   //     if (empty(free_nodes)) {
   //         pool.emplace_back(std::forward<H>(val), head, true);
    //        last(head) = false;
      //      return static_cast<stack_type>(pool.size());
       // }
     //   else {
       //     auto tmp = free_nodes;
        //    value(free_nodes) = val;
         //   free_nodes = next(free_nodes);
          //  next(tmp) = head;
           // last(head) = false;
           // last(tmp) = true;
           // return tmp;
       // }
   // }

   
    stack_type pop(stack_type x) {// pop the head of the stack  and and throw the exception if exist. 
//        if (x.size()==2) {
  //          throw NoHeadStack("Trying to pop an stack by passing no-head.");
    //    }
        if (empty(x)) {
            throw EmptyStack("Trying to pop an Empty Stack.");
        }

        auto tmp = next(x);
        next(x) = free_nodes;
        free_nodes = x;

        return tmp;
    }
   
    stack_type free_stack(stack_type x) { //  free entire stack 
        while (x != end())
            x = pop(x);
        return x;
    }
   
    void print_stack(stack_type x) { // this funtion is used to print the stack
        std::cout << "The elment of stacks are:" << std::endl;
        for (auto start = begin(x), finish = end(x); start != finish; ++start)
        {
            std::cout << "[" << *start << "]";
        }
        std::cout << "\n";
    }
   
    void print_pool(stack_type pool) noexcept {
        std::cout << "Stack pool is: " << std::endl;
        for (auto i = 0; i < pool.size(); i++) {
            std::cout << "[" << pool[i].value << "]";
        }
        std::cout << std::endl;
    }
};
