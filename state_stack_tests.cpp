#include "CppUnitTest.h"
#include "..\state_stack.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#define MSG(msg) [&]{ std::wstringstream _s; _s << msg; return _s.str(); }().c_str()

namespace state_stack_tests
{		
	TEST_CLASS(verify_state_stack)
	{
	public:
		enum class run_state { undefined,
							   running, 
							   starting_up, 
							   docked, 
							   recording, 
							   uploading, 
							   sleeping, 
							   shutting_down };

		std::vector<std::pair<run_state, const char*>> run_state_strings = 
		{
			{run_state::undefined,     "Undefined"		},
			{run_state::running,       "Running"		},
			{run_state::starting_up,   "Starting Up"	},
			{run_state::docked,        "Docked"			},
			{run_state::recording,     "Recording"		},
			{run_state::uploading,     "Uploading"		},
			{run_state::sleeping,      "Sleeping"		},
			{run_state::shutting_down, "Shutting Down"	}
		};


		TEST_METHOD(state_stack_verify_initial_size)
		{
			state_stack<run_state> states(run_state::undefined);
			Assert::AreEqual((int)states.count(), 0, L"Stack not initialized (should be)", LINE_INFO());
		}

		TEST_METHOD(state_stack_verify_single_set)
		{
			state_stack<run_state> states(run_state::undefined);
			states.set(run_state::running);

			Assert::AreEqual((int)states.count(), 1, L"set failed", LINE_INFO());
		}

		TEST_METHOD(state_stack_verify_get)
		{
			state_stack<run_state> states(run_state::undefined);
			states.set(run_state::running);

			Assert::AreEqual((int)states.get(), (int)run_state::running, L"Stack does not return currect state", LINE_INFO());
		}

		TEST_METHOD(state_stack_verify_no_display_string)
		{
			state_stack<run_state> states(run_state::undefined);
			states.set(run_state::running);

			Assert::AreEqual(states.display_state(states.get()), std::string("Unknown"), L"Stack does not return 'Unknown' when strings are not defined", LINE_INFO());
		}

		TEST_METHOD(state_stack_verify_display_string)
		{
			state_stack<run_state> states(run_state::undefined);
			states.state_strings(run_state_strings);

			states.set(run_state::running);

			Assert::AreEqual(states.display_state(states.get()).c_str(), run_state_strings[1].second, L"State string did not display correctly", LINE_INFO());
		}

		TEST_METHOD(state_stack_verify_set)
		{
			state_stack<run_state> states(run_state::undefined);
			states.set(run_state::running);
			states.set(run_state::recording);

			Assert::AreEqual((int)states.get(), (int)run_state::recording, L"State not seted", LINE_INFO());
		}

		TEST_METHOD(state_stack_verify_set_updated_count)
		{
			state_stack<run_state> states(run_state::undefined);
			states.set(run_state::running);
			states.set(run_state::recording);

			Assert::AreEqual((int)states.count(), 2, L"Count is incorrect after seting state", LINE_INFO());
		}

		TEST_METHOD(state_stack_verify_set_same_state)
		{
			state_stack<run_state> states(run_state::undefined);
			states.set(run_state::running);
			states.set(run_state::running);

			Assert::AreEqual((int)states.count(), 1, L"Duplicate state was seted", LINE_INFO());
		}

		TEST_METHOD(state_stack_verify_clear)
		{
			state_stack<run_state> states(run_state::undefined);
			states.set(run_state::running);
			states.set(run_state::recording);
			states.clear();

			Assert::AreEqual((int)states.get(), (int)run_state::running, L"State not cleared", LINE_INFO());
		}

		TEST_METHOD(state_stack_verify_clear_base_state)
		{
			state_stack<run_state> states(run_state::undefined);
			states.set(run_state::running);
			states.set(run_state::recording);
			states.clear();
			states.clear();
			states.clear(run_state::running);

			Assert::AreEqual((int)states.get(), (int)run_state::undefined, L"States not cleared to base state", LINE_INFO());
		}

		TEST_METHOD(state_stack_verify_clear_specific_state)
		{
			state_stack<run_state> states(run_state::undefined);
			states.set(run_state::running);
			states.set(run_state::recording);
			states.clear(run_state::recording);

			Assert::AreEqual((int)states.get(), (int)run_state::running, L"Named state not cleared", LINE_INFO());
		}

		TEST_METHOD(state_stack_verify_clear_specific_state_not_on_top)
		{
			state_stack<run_state> states(run_state::undefined);
			states.set(run_state::running);
			states.set(run_state::recording);
			states.clear(run_state::running);

			Assert::AreEqual(states.is_state_set(run_state::running), (bool)false, L"Named state not cleared (not on top)", LINE_INFO());
		}

		TEST_METHOD(state_stack_verify_clear_specific_state_not_there)
		{
			state_stack<run_state> states(run_state::undefined);
			states.set(run_state::running);
			states.set(run_state::recording);
			states.clear(run_state::docked);

			Assert::AreEqual((int)states.get(), (int)run_state::recording, L"Clearing on non-existant state failed", LINE_INFO());
		}

		TEST_METHOD(state_stack_verify_reset)
		{
			state_stack<run_state> states(run_state::undefined);
			states.set(run_state::running);
			states.set(run_state::recording);
			states.set(run_state::docked);
			states.reset();

			Assert::AreEqual((int)states.get(), (int)run_state::undefined, L"Reset failed", LINE_INFO());
		}

		TEST_METHOD(state_stack_verify_get_last_in_base_state)
		{
			state_stack<run_state> states(run_state::undefined);
			states.set(run_state::running);

			Assert::AreEqual((int)states.get_last(), (int)run_state::undefined, L"Able to get last state when it is undefined", LINE_INFO());
		}

		TEST_METHOD(state_stack_verify_get_last)
		{
			state_stack<run_state> states(run_state::undefined);
			states.set(run_state::running);
			states.set(run_state::docked);

			Assert::AreEqual((int)states.get_last(), (int)run_state::running, L"Unable to get last state", LINE_INFO());
		}

		TEST_METHOD(state_stack_verify_is_set_true)
		{
			state_stack<run_state> states(run_state::undefined);
			states.set(run_state::running);
			states.set(run_state::docked);
			states.set(run_state::recording);

			Assert::AreEqual(states.is_state_set(run_state::docked), (bool)true, L"Failed to detect set state", LINE_INFO());
		}

		TEST_METHOD(state_stack_verify_is_set_false)
		{
			state_stack<run_state> states(run_state::undefined);
			states.set(run_state::running);
			states.set(run_state::docked);
			states.set(run_state::recording);

			Assert::AreEqual(states.is_state_set(run_state::uploading), (bool)false, L"Failed to detect non-set state", LINE_INFO());
		}

		TEST_METHOD(state_stack_verify_entry_handler)
		{
			state_stack<run_state> states(run_state::undefined);
			states.set(run_state::running);
			states.add_entry_handler(run_state::docked, [&states](run_state old_state, run_state new_state)
			{
				states.set(run_state::recording);
			});
			
			//trigger the entry handler
			states.set(run_state::docked);
			
			Assert::AreEqual((int)states.get_last(), (int)run_state::recording, L"Entry handler failed", LINE_INFO());
		}

		TEST_METHOD(state_stack_verify_exit_handler)
		{
			state_stack<run_state> states(run_state::undefined);
			states.set(run_state::running);
			states.add_exit_handler(run_state::docked, [&states](run_state old_state, run_state new_state)
			{
				states.set(run_state::recording);
			});
			states.set(run_state::docked);
			
			//trigger the exit handler
			states.clear();

			Assert::AreEqual((int)states.get(), (int)run_state::recording, L"Exit handler failed", LINE_INFO());
		}

		TEST_METHOD(state_stack_verify_executor)
		{
			state_stack<run_state> states(run_state::undefined);
			states.set(run_state::running);
			states.set(run_state::docked);
			states.set(run_state::recording);
			states.execute([&]()
			{
				states.clear();
			});

			Assert::AreEqual((int)states.get(), (int)run_state::docked, L"Executor failed", LINE_INFO());
		}
	};
}