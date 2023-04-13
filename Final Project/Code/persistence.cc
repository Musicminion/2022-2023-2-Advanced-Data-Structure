#include <iostream>
#include <cstdint>
#include <string>
#include <cassert>

#include "test.h"

class PersistenceTest : public Test {
private:
	const uint64_t TEST_MAX = 1024 * 32;
	void prepare(uint64_t max)
	{
		uint64_t i;

		// Clean up
		store.reset();

		// Test multiple key-value pairs
		for (i = 0; i < max; ++i) {
			store.put(i, std::string(i+1, 's'));
			EXPECT(std::string(i+1, 's'), store.get(i));
		}
		phase();

		// Test after all insertions
		for (i = 0; i < max; ++i)
			EXPECT(std::string(i+1, 's'), store.get(i));
		phase();

		// Test deletions
		for (i = 0; i < max; i+=2)
			EXPECT(true, store.del(i));

		// Prepare data for Test Mode
		for (i = 0; i < max; ++i) {
			switch (i & 3) {
			case 0:
				EXPECT(not_found, store.get(i));
				store.put(i, std::string(i+1, 't'));
				break;
			case 1:
				EXPECT(std::string(i+1, 's'), store.get(i));
				store.put(i, std::string(i+1, 't'));
				break;
			case 2:
				EXPECT(not_found, store.get(i));
				break;
			case 3:
				EXPECT(std::string(i+1, 's'), store.get(i));
				break;
			default:
				assert(0);
			}
		}

		phase();


		report();

		/**
		 * Write 10MB data to drain previous data out of memory.
		 */
		for (i = 0; i <= 1024; ++i)
			store.put(max + i, std::string(1024, 'x'));

		std::cout << "Data is ready, please press ctrl-c/ctrl-d to"
			" terminate this program!" << std::endl;
		std::cout.flush();

		while (true) {
			volatile int dummy;
			for (i = 0; i <= 1024; ++i) {
				// The loop slows down the program
				for (i = 0; i <= 1000; ++i)
					dummy = i;

				// store.del(max + i);

				// for (i = 0; i <= 1000; ++i)
					dummy = i;

				// store.put(max + i, std::string(1024, '.'));

				for (i = 0; i <= 1000; ++i)
					dummy = i;

				// store.put(max + i, std::string(512, 'x'));
			}
		}
	}

	void test(uint64_t max)
	{
		uint64_t i;
		// Test data
		for (i = 0; i < max; ++i) {
			switch (i & 3) {
			case 0:
				if(std::string(i+1, 't') != store.get(i)){
					std::cout << "测试编号:" << i <<  ",应该输出 [t] 的个数是" << i + 1 << std::endl;
					std::cout << "实际输出的长度是 " << store.get(i).size() << " " << store.get(i).substr(0,10)  << std::endl;
					std::cout << "\n";
				}
				EXPECT(std::string(i+1, 't'), store.get(i));
				break;
			case 1:
				if(std::string(i+1, 't') != store.get(i)){
					std::cout << "测试编号:" << i <<  ",应该输出 [t] 的个数是" << i + 1 << std::endl;
					std::cout << "实际输出的长度是 " << store.get(i).size() << " " << store.get(i).substr(0,10)  << std::endl;
					std::cout << "\n";
				}
				EXPECT(std::string(i+1, 't'), store.get(i));
				break;
			case 2:
				if(not_found != store.get(i)){
					std::cout << "测试编号:" << i <<  ",应该输出找不到" << i + 1 << std::endl;
					std::cout << "实际输出的长度是 " << store.get(i).size() << " " << store.get(i).substr(0,10)  << std::endl;
					std::cout << "\n";
				}
				EXPECT(not_found, store.get(i));
				break;
			case 3:
				if(std::string(i+1, 's') != store.get(i)){
					std::cout << "测试编号:" << i <<  ",应该输出 [s] 的个数是" << i + 1 << std::endl;
					std::cout << "实际输出的长度是 " << store.get(i).size() << " " << store.get(i).substr(0,10)  << std::endl;
					std::cout << "\n";
				}
				EXPECT(std::string(i+1, 's'), store.get(i));
				break;
			default:
				assert(0);
			}
		}

		phase();

		
		
		for (i = 0; i <= 1024; ++i){
			EXPECT(std::string(1024, 'x'), store.get(i + max));
		}
		phase();

		
		// Test scan
		std::list<std::pair<uint64_t, std::string> > list_ans;
		std::list<std::pair<uint64_t, std::string> > list_stu;
		store.scan(max / 4, 3 * max / 4,list_stu);
		
		for (i = max / 4; i <= 3 * max / 4; ++i){
			switch (i & 3) {
				case 0:
					list_ans.push_back({i,std::string(i+1, 't')});
					break;
				case 1:
					list_ans.push_back({i,std::string(i+1, 't')});
					break;
				case 2:
					break;
				case 3:
					list_ans.push_back({i,std::string(i+1, 's')});
					break;
				default:
					assert(0);
			}
		}

		std::cout << list_ans.size() << " " << list_stu.size() << "\n";

		EXPECT(list_ans.size(), list_stu.size());
		auto ap = list_ans.begin();
		auto sp = list_stu.begin();
		while(ap != list_ans.end()) {
			if (sp == list_stu.end()) {
				EXPECT((*ap).first, -1);
				EXPECT((*ap).second, not_found);
				ap++;
			}
			else {
				EXPECT((*ap).first, (*sp).first);
				EXPECT((*ap).second, (*sp).second);
				ap++;
				sp++;
			}
		}


		phase();

		report();
	}

public:
	PersistenceTest(const std::string &dir, bool v=true) : Test(dir, v)
	{
	}

	void start_test(void *args = NULL) override
	{
		bool testmode = (args && *static_cast<bool *>(args));

		std::cout << "KVStore Persistence Test" << std::endl;

		if (testmode) {
			std::cout << "<<Test Mode>>" << std::endl;
			test(TEST_MAX);
		} else {
			std::cout << "<<Preparation Mode>>" << std::endl;
			prepare(TEST_MAX);
		}
	}
};

void usage(const char *prog, const char *verb, const char *mode)
{
	std::cout << "Usage: " << prog  << " [-t] [-v]" << std::endl;
	std::cout << "  -t: test mode for persistence test,"
		" if -t is not given, the program only prepares data for test."
		" [currently " << mode << "]" << std::endl;
	std::cout << "  -v: print extra info for failed tests [currently ";
	std::cout << verb << "]" << std::endl;
	std::cout << std::endl;
	std::cout << " NOTE: A normal usage is as follows:" << std::endl;
	std::cout << "    1. invoke `" << prog << "`;" << std::endl;
	std::cout << "    2. terminate (kill) the program when data is ready;";
	std::cout << std::endl;
	std::cout << "    3. invoke `" << prog << " -t ` to test." << std::endl;
	std::cout << std::endl;
	std::cout.flush();
}


int main(int argc, char *argv[])
{
	bool verbose = false;
	bool testmode = false;

	if (argc == 2) {
		verbose = std::string(argv[1]) == "-v";
		testmode = std::string(argv[1]) == "-t";
	} else if (argc == 3) {
		verbose = std::string(argv[1]) == "-v" ||
			std::string(argv[2]) == "-v";
		testmode = std::string(argv[1]) == "-t" ||
			std::string(argv[2]) == "-t";
	} else if (argc > 3) {
		std::cerr << "Too many arguments." << std::endl;
		usage(argv[0], "OFF", "Preparation Mode");
		exit(-1);
	}
	usage(argv[0], verbose ? "ON" : "OFF",
	      testmode ? "Test Mode" : "Preparation Mode");

	PersistenceTest test("./data", verbose);

	test.start_test(static_cast<void *>(&testmode));

	return 0;
}


// #include <iostream>
// #include <cstdint>
// #include <string>
// #include <cassert>

// #include "test.h"

// class PersistenceTest : public Test {
// private:
// 	const uint64_t TEST_MAX = 1024 * 32;
// 	void prepare(uint64_t max)
// 	{
// 		uint64_t i;

// 		// Clean up
// 		store.reset();

// 		// Test multiple key-value pairs
// 		for (i = 0; i < max; ++i) {
// 			store.put(i, std::string(i+1, 's'));
// 			EXPECT(std::string(i+1, 's'), store.get(i));
// 		}
// 		phase();

// 		// Test after all insertions
// 		for (i = 0; i < max; ++i)
// 			EXPECT(std::string(i+1, 's'), store.get(i));
// 		phase();

// 		// Test deletions
// 		for (i = 0; i < max; i+=2){
// 			EXPECT(true, store.del(i));
// 		}

// 		// Prepare data for Test Mode
// 		for (i = 0; i < max; ++i) {
// 			switch (i & 3) {
// 			case 0:
// 				EXPECT(not_found, store.get(i));
// 				store.put(i, std::string(i+1, 't'));
// 				break;
// 			case 1:
// 				EXPECT(std::string(i+1, 's'), store.get(i));
// 				store.put(i, std::string(i+1, 't'));
// 				break;
// 			case 2:
// 				EXPECT(not_found, store.get(i));
// 				break;
// 			case 3:
// 				EXPECT(std::string(i+1, 's'), store.get(i));
// 				break;
// 			default:
// 				assert(0);
// 			}
// 		}

// 		phase();

// 		for (i = 0; i < max; ++i) {
// 			switch (i & 3) {
// 			case 0:
// 				EXPECT(std::string(i+1, 't'), store.get(i));
// 				break;
// 			case 1:
// 				EXPECT(std::string(i+1, 't'), store.get(i));
// 				break;
// 			case 2:
// 				EXPECT(not_found, store.get(i));
// 				break;
// 			case 3:
// 				EXPECT(std::string(i+1, 's'), store.get(i));
// 				break;
// 			default:
// 				assert(0);
// 			}
// 		}
// 		phase();

// 		// std::cout << "开始写入额外的数据\n";		
// 		/**
// 		 * Write 10MB data to drain previous data out of memory.
// 		 */
// 		for (i = 0; i <= 10240; ++i)
// 			store.put(max + i, std::string(1024, 'x'));

// 		// for (i = 0; i < max; ++i) {
// 		// 	switch (i & 3) {
// 		// 	case 0:
// 		// 		EXPECT(std::string(i+1, 't'), store.get(i));
// 		// 		break;
// 		// 	case 1:
// 		// 		EXPECT(std::string(i+1, 't'), store.get(i));
// 		// 		break;
// 		// 	case 2:
// 		// 		EXPECT(not_found, store.get(i));
// 		// 		break;
// 		// 	case 3:
// 		// 		EXPECT(std::string(i+1, 's'), store.get(i));
// 		// 		break;
// 		// 	default:
// 		// 		assert(0);
// 		// 	}
// 		// }
// 		// phase();




// 		report();

// 		std::cout << "Data is ready, please press ctrl-c/ctrl-d to"
// 			" terminate this program!" << std::endl;
// 		std::cout.flush();
		
		

// 		while (true) {
// 			volatile int dummy;
// 			for (i = 0; i <= 1024; ++i) {
// 				// The loop slows down the program
// 				for (i = 0; i <= 1000; ++i)
// 					dummy = i;

// 				store.del(max + i);

// 				for (i = 0; i <= 1000; ++i)
// 					dummy = i;

// 				store.put(max + i, std::string(1024, '.'));

// 				for (i = 0; i <= 1000; ++i)
// 					dummy = i;

// 				store.put(max + i, std::string(512, 'x'));
// 			}
// 		}
// 	}

// 	void test(uint64_t max)
// 	{
// 		uint64_t i;
// 		// Test data
// 		for (i = 0; i < max; ++i) {
// 			switch (i & 3) {
// 			case 0:
// 				if(std::string(i+1, 't') != store.get(i)){
// 					std::cout  << "测试号"<<  i <<" 我的程序输出了：" << store.get(i) << '\n';
// 					return;
// 				}
// 				EXPECT(std::string(i+1, 't'), store.get(i));
// 				break;
// 			case 1:
// 				EXPECT(std::string(i+1, 't'), store.get(i));
// 				break;
// 			case 2:
// 				EXPECT(not_found, store.get(i));
// 				break;
// 			case 3:
// 				EXPECT(std::string(i+1, 's'), store.get(i));
// 				break;
// 			default:
// 				assert(0);
// 			}
// 		}

// 		phase();

// 		report();
// 	}

// public:
// 	PersistenceTest(const std::string &dir, bool v=true) : Test(dir, v)
// 	{
// 	}

// 	void start_test(void *args = NULL) override
// 	{
// 		bool testmode = (args && *static_cast<bool *>(args));

// 		std::cout << "KVStore Persistence Test" << std::endl;

// 		if (testmode) {
// 			std::cout << "<<Test Mode>>" << std::endl;
// 			test(TEST_MAX);
// 		} else {
// 			std::cout << "<<Preparation Mode>>" << std::endl;
// 			prepare(TEST_MAX);
// 		}
// 	}
// };

// void usage(const char *prog, const char *verb, const char *mode)
// {
// 	std::cout << "Usage: " << prog  << " [-t] [-v]" << std::endl;
// 	std::cout << "  -t: test mode for persistence test,"
// 		" if -t is not given, the program only prepares data for test."
// 		" [currently " << mode << "]" << std::endl;
// 	std::cout << "  -v: print extra info for failed tests [currently ";
// 	std::cout << verb << "]" << std::endl;
// 	std::cout << std::endl;
// 	std::cout << " NOTE: A normal usage is as follows:" << std::endl;
// 	std::cout << "    1. invoke `" << prog << "`;" << std::endl;
// 	std::cout << "    2. terminate (kill) the program when data is ready;";
// 	std::cout << std::endl;
// 	std::cout << "    3. invoke `" << prog << " -t ` to test." << std::endl;
// 	std::cout << std::endl;
// 	std::cout.flush();
// }


// int main(int argc, char *argv[])
// {
// 	bool verbose = false;
// 	bool testmode = false;

// 	if (argc == 2) {
// 		verbose = std::string(argv[1]) == "-v";
// 		testmode = std::string(argv[1]) == "-t";
// 	} else if (argc == 3) {
// 		verbose = std::string(argv[1]) == "-v" ||
// 			std::string(argv[2]) == "-v";
// 		testmode = std::string(argv[1]) == "-t" ||
// 			std::string(argv[2]) == "-t";
// 	} else if (argc > 3) {
// 		std::cerr << "Too many arguments." << std::endl;
// 		usage(argv[0], "OFF", "Preparation Mode");
// 		exit(-1);
// 	}
// 	usage(argv[0], verbose ? "ON" : "OFF",
// 	      testmode ? "Test Mode" : "Preparation Mode");

// 	PersistenceTest test("./data", verbose);

// 	test.start_test(static_cast<void *>(&testmode));

// 	return 0;
// }
