#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestCase.h>
#include <phapi.h>
#include <phx.h>
#include <eXosip/eXosip.h>

class PhxTest : public CppUnit::TestCase
{
	CPPUNIT_TEST_SUITE(PhxTest);
	CPPUNIT_TEST(testPhxInit);
	CPPUNIT_TEST_SUITE_END();

	void testPhxInit() {
		phCallbacks_t cbk = {
			NULL, NULL, NULL, NULL, NULL, NULL, NULL
		};

		phInit(&cbk, "127.0.0.1", 0);
		phRegister("test", "len-1.host.wengo.fr");
	}

};

CPPUNIT_TEST_SUITE_REGISTRATION(PhxTest);

int main( int argc, char **argv) {
	CppUnit::TextUi::TestRunner runner;
	runner.addTest(PhxTest::suite());
	runner.run();
	return 0;
}
