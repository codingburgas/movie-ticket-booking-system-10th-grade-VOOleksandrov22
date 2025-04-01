#include "pch.h"
#include "CppUnitTest.h"

#include "../Date/date.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace DateUnitTests
{
	TEST_CLASS(DateUnitTests)
	{
	public:
		
		TEST_METHOD(TestMethod1)
		{
			Assert::IsTrue(Date("2010-05-13 12:00:00") == Date(2010, 5, 13, 12, 0, 0));

			
		}
	};
}
