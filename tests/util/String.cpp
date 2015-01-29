// Copyright (C) 2009-2015, Panagiotis Christopoulos Charitos.
// All rights reserved.
// Code licensed under the BSD License.
// http://www.anki3d.org/LICENSE

#include "tests/framework/Framework.h"
#include "anki/util/String.h"
#include <string>

namespace anki {

//==============================================================================
ANKI_TEST(Util, String)
{
	HeapAllocator<U8> alloc(allocAligned, nullptr);

	// Copy
	{
		String a, b;
		ANKI_TEST_EXPECT_NO_ERR(a.create(alloc, "123"));
		ANKI_TEST_EXPECT_NO_ERR(b.create(alloc, a));

		ANKI_TEST_EXPECT_EQ(a, b);
		ANKI_TEST_EXPECT_EQ(b, "123");

		b.destroy(alloc);
		a.destroy(alloc);
		ANKI_TEST_EXPECT_NO_ERR(b.create(alloc, "321"));
		ANKI_TEST_EXPECT_NO_ERR(a.create(alloc, b));
		ANKI_TEST_EXPECT_EQ(a, b);
		ANKI_TEST_EXPECT_EQ(a, "321");

		b.destroy(alloc);
		a.destroy(alloc);
	}


	// Move
	{
		String a;
		ANKI_TEST_EXPECT_NO_ERR(a.create(alloc, "123"));
		String b(std::move(a));
		ANKI_TEST_EXPECT_EQ(a.isEmpty(), true);
		ANKI_TEST_EXPECT_EQ(b, "123");

		b.destroy(alloc);
		ANKI_TEST_EXPECT_NO_ERR(b.create(alloc, "321"));
		a = std::move(b);
		ANKI_TEST_EXPECT_EQ(a, "321");
		ANKI_TEST_EXPECT_EQ(b.isEmpty(), true);
		a.destroy(alloc);
	}

	// Accessors
	{
		const char* s = "123";
		String a;
		ANKI_TEST_EXPECT_NO_ERR(a.create(alloc, s));
		ANKI_TEST_EXPECT_EQ(a[0], '1');
		ANKI_TEST_EXPECT_EQ(a[1], '2');
		ANKI_TEST_EXPECT_EQ(a[2], '3');

		U count = 0;
		for(char& c : a)
		{
			++c;
			++count;
		}
	
		ANKI_TEST_EXPECT_EQ(a, "234");
		ANKI_TEST_EXPECT_EQ(count, 3);

		ANKI_TEST_EXPECT_EQ(a.begin(), &a[0]);
		ANKI_TEST_EXPECT_EQ(a.end(), &a[0] + 3);
		a.destroy(alloc);
	}


	// Append
	{
		String a, b;

		ANKI_TEST_EXPECT_NO_ERR(b.create(alloc, "123"));

		ANKI_TEST_EXPECT_NO_ERR(a.append(alloc, b));
		ANKI_TEST_EXPECT_EQ(a, "123");

		ANKI_TEST_EXPECT_NO_ERR(a.append(alloc, "456789"));
		ANKI_TEST_EXPECT_NO_ERR(a.append(alloc, String()));
		ANKI_TEST_EXPECT_NO_ERR(a.append(alloc, ""));
		ANKI_TEST_EXPECT_NO_ERR(a.append(alloc, "0"));
		ANKI_TEST_EXPECT_EQ(a, "1234567890");
		a.destroy(alloc);
		b.destroy(alloc);
	}

	// Compare
	{
#define COMPARE(x_, y_, op_) \
	ANKI_TEST_EXPECT_NO_ERR(a.append(alloc, x_)); \
	ANKI_TEST_EXPECT_NO_ERR(b.append(alloc, y_)); \
	ANKI_TEST_EXPECT_EQ(a op_ b, \
	std::string(x_) op_ std::string(y_)) \
	a.destroy(alloc); \
	b.destroy(alloc); 
	
		String a, b;
		COMPARE("123", "1233", <);
		COMPARE("0123", "1233", <=);
		COMPARE("ASDFA", "asdf90f", >);
		COMPARE(" %^*^^&", "aslkdfjb", >=);

#undef COMPARE
	}

	// sprintf
	{
		String a;

		// Simple
		ANKI_TEST_EXPECT_NO_ERR(a.sprintf(alloc, "12%c  %d", '3', 123));
		ANKI_TEST_EXPECT_EQ(a, "123  123");
		a.destroy(alloc);

		// Extreme
		const char* s = "1234567890ABCDEF!@#$%^&*()_+asfghjkl:,.;ljk\"><{}[]/";
		ANKI_TEST_EXPECT_NO_ERR(a.sprintf(alloc, "%s%s%s%s%s%s%s%s%s%s%s %d", 
			s, s, s, s, s, s, s, s, s, s, s, 88));

		String b;
		for(U i = 0; i < 11; i++)
		{
			ANKI_TEST_EXPECT_NO_ERR(b.append(alloc, s));
		}
		ANKI_TEST_EXPECT_NO_ERR(b.append(alloc, " 88"));

		ANKI_TEST_EXPECT_EQ(a, b);
		ANKI_TEST_EXPECT_EQ(a.getLength(), b.getLength());

		a.destroy(alloc);
		b.destroy(alloc);
	}

	// sprintf #2: Smaller result (will trigger another path)
	{
		String a;

		// Simple
		ANKI_TEST_EXPECT_NO_ERR(a.sprintf(alloc, "12%c  %d", '3', 123));
		ANKI_TEST_EXPECT_EQ(a, "123  123");
		a.destroy(alloc);

		// Extreme
		const char* s = "12345";
		ANKI_TEST_EXPECT_NO_ERR(a.sprintf(alloc, "%s%s %d", s, s, 88));

		String b;
		for(U i = 0; i < 2; i++)
		{
			ANKI_TEST_EXPECT_NO_ERR(b.append(alloc, s));
		}
		ANKI_TEST_EXPECT_NO_ERR(b.append(alloc, " 88"));

		ANKI_TEST_EXPECT_EQ(a, b);
		ANKI_TEST_EXPECT_EQ(a.getLength(), b.getLength());

		a.destroy(alloc);
		b.destroy(alloc);
	}

	// Other create
	{
		String a;

		ANKI_TEST_EXPECT_NO_ERR(a.create(alloc, '1', 3));
		ANKI_TEST_EXPECT_EQ(a, "111");
		ANKI_TEST_EXPECT_EQ(a.getLength(), 3);

		a.destroy(alloc);
	}

	// toString
	{
		String a;
		ANKI_TEST_EXPECT_NO_ERR(a.toString(alloc, 123));
		ANKI_TEST_EXPECT_EQ(a, "123");
		a.destroy(alloc);

		ANKI_TEST_EXPECT_NO_ERR(a.toString(alloc, 123.123));
		ANKI_TEST_EXPECT_EQ(a, "123.123000");
		a.destroy(alloc);
	}

	// To number
	{
		I64 i;
		String a;
		ANKI_TEST_EXPECT_NO_ERR(a.create(alloc, "123456789"));
		ANKI_TEST_EXPECT_NO_ERR(a.toI64(i));
		ANKI_TEST_EXPECT_EQ(i, 123456789);
		a.destroy(alloc);

		ANKI_TEST_EXPECT_NO_ERR(a.create(alloc, "-9223372036854775807"));
		ANKI_TEST_EXPECT_NO_ERR(a.toI64(i));
		ANKI_TEST_EXPECT_EQ(i, -9223372036854775807);
		a.destroy(alloc);

		F64 f;
		ANKI_TEST_EXPECT_NO_ERR(a.create(alloc, "123456789.145"));
		ANKI_TEST_EXPECT_NO_ERR(a.toF64(f));
		ANKI_TEST_EXPECT_EQ(f, 123456789.145);
		a.destroy(alloc);
	}
}

} // end namespace anki

