#ifndef STATE_STACK_H
#define STATE_STACK_H

#include <vector>
#include <mutex>
#include <iostream>
#include <utility>
#include <string>
#include <algorithm>
#include <stdio.h>
#include "raw_algorithms.h"

#define DEBUG_DISPLAY

#ifdef DEBUG_DISPLAY
	#define log_info(...) printf(__VA_ARGS__);    
	#define display_stack() display();
#else
	#define log_info(...)
	#define display_stack()
#endif

template<class T>
class state_stack
{
public:
	//ctor & dtor
	state_stack() = delete;
	explicit state_stack(const T& undefined_state);	//always require an undefined state for construction
	~state_stack() = default;

	
	//copy, move constructors
	state_stack(const state_stack&) = default;
	state_stack(state_stack&&) = default;
	
	//assignment operators
	state_stack& operator= (const state_stack&) = default;
	state_stack& operator= (state_stack&&) = default;

	
	//sizing
	typedef typename std::vector<T*>::size_type size_type;

	void reserve(size_type new_cap);
	size_type count() const;

	void reset();


	//accessors
	T get() const;
	T get_last() const;
	bool is_state_set(const T& state) const;
	
	void set(const T& new_state);
	
	T clear();
	void clear(const T& state);


	//state entry & exit handlers
	using entry_exit_functor = std::function<void(const T& old_state, const T& new_state)>;
	using execution_functor = std::function<void()>;

	void add_exit_handler(const T& state, entry_exit_functor functor);
	void add_entry_handler(const T& state, entry_exit_functor functor);


	//exector runs immediately
	void execute(execution_functor functor) const;


	//used for displaying text representations of any given state
	void state_strings(const std::vector<std::pair<T, const char*>>& strings);
	std::string display_state(const T& state) const;
	void stack_label(const std::string& label) { m_label = std::move(label); }


private:
	//these run automatically after each add or clear on the stack
	void process_exit_handlers(const T& old_state, const T& new_state) const;
	void process_entry_handlers(const T& old_state, const T& new_state) const;


	void display() const;


	//vectors for the states and string representations
	std::vector<T> m_states;
	std::vector<std::pair<T, const char*>> m_state_strings;


	//we need an undefined state to return when the vector is empty
	//this is provided to the constructor
	T m_undefined_state;
    std::string m_label;


	//entry and exit handlers that fire BEFORE the state change is complete
	//each handler gets the current state and previous state
	//
	//these handlers do NOT have access to the private members of the template
	//
	//access to the state stack must be passed into the lambda
	//however, care must be taken to ensure that the handlers do not out live 
	//any variables passed in (the results are undefned behavior)
	std::vector<std::pair<T, entry_exit_functor>> m_entry_handlers;
	std::vector<std::pair<T, entry_exit_functor>> m_exit_handlers;


	//this allows functions to call each other while maintaining the lock
	//calls on other threads block normally
	mutable std::recursive_mutex m_mutex;
	#ifdef STATE_STACK_NO_LOCK
		#define lock_stack
	#else
		#define lock_stack std::lock_guard<std::recursive_mutex> lock(m_mutex);
	#endif
};

//ctors
template<class T>
state_stack<T>::state_stack(const T& undefined_state) : 
	m_states(), 
	m_state_strings(), 
	m_undefined_state(undefined_state),
    m_label(),
	m_entry_handlers(),
	m_exit_handlers(), 
	m_mutex()
{}

template<class T>
void state_stack<T>::reserve(size_type new_cap)
{
	lock_stack
	
	m_states.reserve(new_cap);
}

template<class T>
typename std::vector<T*>::size_type state_stack<T>::count() const
{
	lock_stack

	return m_states.size();
}

template<class T>
void state_stack<T>::reset()
{
	lock_stack
	
	m_states.clear();
}

template<class T>
T state_stack<T>::get() const
{
	lock_stack
	
	if (m_states.size() == 0)
		return m_undefined_state;

	return m_states[m_states.size() - 1];
}

template<class T>
T state_stack<T>::get_last() const
{
	lock_stack
	
	if (m_states.size() < 2)
		return m_undefined_state;

	return m_states[m_states.size() - 2];
}

template<class T>
bool state_stack<T>::is_state_set(const T& state) const
{
	lock_stack

	if (m_states.size() == 0)
		return false;

	return std::any_of(m_states.begin(), m_states.end(),
		[&](const T& item)
		{
			return item == state;
		});
}

template<class T>
void state_stack<T>::set(const T& new_state)
{
	lock_stack
	
	//don't allow the same state to be pushed onto the stack
	T old_state = get();
	if (new_state == old_state)
		return;


	process_entry_handlers(old_state, new_state);

	//check again to make sure that the entry handlers did not change the state we care about
	if (new_state == get())
		return;
	
	
	log_info(">>>Adding %s state: %s\n", m_label.c_str(), display_state(new_state).c_str());

	m_states.push_back(new_state);

	display_stack();
}

template<class T>
T state_stack<T>::clear()
{
	lock_stack

	if (m_states.size() == 0) //nothing to do
		return m_undefined_state;


	T& old_state = m_states.back();
	log_info(">>>Clearing %s State: %s", m_label.c_str(), display_state(old_state).c_str());

	T new_state = get_last();
	process_exit_handlers(old_state, new_state);

	//check again to make sure that the entry handlers did not do this for us
	if (m_states.size() > 0 && old_state == m_states.back())
		m_states.pop_back();

	display_stack();

	return old_state;
}

template<class T>
void state_stack<T>::clear(const T& state)
{
	lock_stack

	if (m_states.size() == 0) //nothing to do
		return;

	//clear off the top
	if (m_states.back() == state)
	{
		clear();
		return;
	}
	
	//clear out of the middle
	auto it = std::find(m_states.rbegin(), m_states.rend(), state);
	if (it != m_states.rend())
	{
		m_states.erase(it.base()-1);
		log_info(">>>Clearing %s State: %s (NOT OFF THE TOP)", m_label.c_str(), display_state(state).c_str());
		display_stack();
	}
}

template<class T>
void state_stack<T>::add_exit_handler(const T& state, entry_exit_functor functor)
{
	m_exit_handlers.push_back(std::make_pair(state, functor));
}

template<class T>
void state_stack<T>::add_entry_handler(const T& state, entry_exit_functor functor)
{
	m_entry_handlers.push_back(std::make_pair(state, functor));
}

template<class T>
void state_stack<T>::execute(execution_functor functor) const
{
	lock_stack

	functor();
}

template<class T>
void state_stack<T>::process_exit_handlers(const T& old_state, const T& new_state) const
{
	lock_stack

	for (const auto& functor : m_exit_handlers)
		if (functor.first == old_state || functor.first == m_undefined_state)
			functor.second(old_state, new_state);
}

template<class T>
void state_stack<T>::process_entry_handlers(const T& old_state, const T& new_state) const
{
	lock_stack

	for (const auto& functor : m_entry_handlers)
		if (functor.first == new_state || functor.first == m_undefined_state)
			functor.second(old_state, new_state);
}

template<class T>
std::string state_stack<T>::display_state(const T& state) const
{
	for (const auto& s : m_state_strings)
		if (s.first == state)
			return s.second;

	return "Unknown";
}

template<class T>
void state_stack<T>::state_strings(const std::vector<std::pair<T, const char*>>& strings)
{
	m_state_strings = std::move(strings);
}

template<class T>
void state_stack<T>::display() const
{
	std::cout << ">>>Current " << m_label << " States " << m_states.size() << "\n";
	int i = m_states.size()-1;
	for (const auto& s : reverse_iterate(m_states))
		std::cout << ">>> " << i-- << " " << display_state(s) << "\n";
}

#endif //STATE_STACK_H