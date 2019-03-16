#include "TSSTP.hpp"
#include "catch.hpp"

TEST_CASE("Thread_private non-owner", "[thread-private]") {
	TST::Thread_private<int, TST::Non_owner> tp;
	WHEN("Default constructing") {
		REQUIRE(tp == nullptr);
		REQUIRE(nullptr == tp);
		REQUIRE(not tp);
	}
	int value;
	WHEN("Assigning a value") {
		tp = &value;
		REQUIRE(tp == &value);
		REQUIRE(&value == tp);
	}
	WHEN("Constructing a value") {
		TST::Thread_private<int, TST::Non_owner> tp = &value;
		REQUIRE(tp == &value);
		REQUIRE(&value == tp);
	}
	WHEN("Moving") {
		tp = &value;
		auto tpcopy = std::move(tp);
		REQUIRE(tpcopy == &value);
		REQUIRE(&value == tpcopy);
		REQUIRE(tp == nullptr);
		REQUIRE(nullptr == tp);
		REQUIRE(not tp);
	}
}

TEST_CASE("Thread_private owner creation", "[thread-private]") {
	TST::Thread_private<int, TST::Owner> tp;
	WHEN("Default constructing") {
		REQUIRE(tp == nullptr);
		REQUIRE(nullptr == tp);
		REQUIRE(not tp);
	}
	WHEN("Assigning a value") {
		auto up = std::make_unique<int>();
		auto upv = up.get();
		tp = std::move(up);
		REQUIRE(tp == upv);
		REQUIRE(upv == tp);
	}
	WHEN("Constructing a value") {
		auto up = std::make_unique<int>();
		auto upv = up.get();
		TST::Thread_private<int, TST::Owner> tp = std::move(up);
		REQUIRE(tp == upv);
		REQUIRE(upv == tp);
	}
}

TEST_CASE("When converting to other TSTP", "[thread-private]") {
	int value;
	TST::Thread_private<int, TST::Non_owner> tp = &value;
	WHEN("Converting to Mutex_protected") {
		INFO("Value address: " << &value);
		TST::Mutex_protected<int, TST::Non_owner, TST::Waitable> mp = std::move(tp);
		REQUIRE(mp == &value);
		REQUIRE(tp == nullptr);
	}
}