#pragma warning( disable : 4996 )


int main()
{
	printf("In main\n\n\n");

	cout << "Data Type Sizes:" << "\n";
	cout << "================" << "\n";
	cout << "bool........." << sizeof(bool) << " bytes" << "\n";
	cout << "char........." << sizeof(char) << " bytes" << "\n";
	cout << "int.........." << sizeof(int) << " bytes" << "\n";
	cout << "long........." << sizeof(long) << " bytes" << "\n";
	cout << "long long...." << sizeof(long long) << " bytes" << "\n";
	cout << "float........" << sizeof(float) << " bytes" << "\n";
	cout << "double......." << sizeof(double) << " bytes" << "\n";
	cout << "\n" << "\n";


	enum class run_state {
		undefined,
		running,
		starting_up,
		docked,
		recording,
		uploading,
		sleeping,
		shutting_down
	};

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


	state_stack<run_state> states(run_state::undefined);
	states.state_strings(run_state_strings);
	states.add_entry_handler(run_state::docked, [&states](run_state old_state, run_state new_state)
	{
		cout << "Entering Docked state\n";
	});
	states.add_exit_handler(run_state::docked, [&states](run_state old_state, run_state new_state)
	{
		cout << "Exiting Docked state\n";
	});

	states.set(run_state::running);
	states.set(run_state::docked);
	states.clear();
	states.clear();
	states.set(run_state::recording);
	states.set(run_state::docked);
	
	states.execute([&]()
	{
		cout << "Current state is: " << states.display_state(states.get()) << "\n";
	});

	build_stack();
	cout << "my_stack[0] = " << my_stack[0] << "\n";
	cout << "my_stack[6] = " << my_stack[6] << "\n";

	stack2<int> new_stack1;
	new_stack1 = my_stack;
	auto new_stack2 = my_stack;
	stack2<int> new_stack3(my_stack);

	reduce_stack();

	printf("Old Stack size = %d\n", new_stack1.size());
	printf("Old Stack size = %d\n", new_stack2.size());
	printf("Old Stack size = %d\n", new_stack3.size());
	printf("\n");

	
 	return 0;
}
