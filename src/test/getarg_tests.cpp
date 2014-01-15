#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/test/unit_test.hpp>

#include "util.h"

BOOST_AUTO_TEST_SUITE(getarg_tests)

static void
ResetArgs(const std::string& strArg)
{
    std::vector<std::string> vecArg;
    boost::split(vecArg, strArg, boost::is_space(), boost::token_compress_on);

    // Insert dummy executable name:
    vecArg.insert(vecArg.begin(), "testbitcoin");

    // Convert to char*:
    std::vector<const char*> vecChar;
    BOOST_FOREACH(std::string& s, vecArg)
        vecChar.push_back(s.c_str());

    ParseParameters(vecChar.size(), &vecChar[0]);
}

BOOST_AUTO_TEST_CASE(boolarg)
{
    ResetArgs("-BRD");
    BOOST_CHECK(GetBoolArg("-BRD"));
    BOOST_CHECK(GetBoolArg("-BRD", false));
    BOOST_CHECK(GetBoolArg("-BRD", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-BRDo"));
    BOOST_CHECK(!GetBoolArg("-BRDo", false));
    BOOST_CHECK(GetBoolArg("-BRDo", true));

    ResetArgs("-BRD=0");
    BOOST_CHECK(!GetBoolArg("-BRD"));
    BOOST_CHECK(!GetBoolArg("-BRD", false));
    BOOST_CHECK(!GetBoolArg("-BRD", true));

    ResetArgs("-BRD=1");
    BOOST_CHECK(GetBoolArg("-BRD"));
    BOOST_CHECK(GetBoolArg("-BRD", false));
    BOOST_CHECK(GetBoolArg("-BRD", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-noBRD");
    BOOST_CHECK(!GetBoolArg("-BRD"));
    BOOST_CHECK(!GetBoolArg("-BRD", false));
    BOOST_CHECK(!GetBoolArg("-BRD", true));

    ResetArgs("-noBRD=1");
    BOOST_CHECK(!GetBoolArg("-BRD"));
    BOOST_CHECK(!GetBoolArg("-BRD", false));
    BOOST_CHECK(!GetBoolArg("-BRD", true));

    ResetArgs("-BRD -noBRD");  // -BRD should win
    BOOST_CHECK(GetBoolArg("-BRD"));
    BOOST_CHECK(GetBoolArg("-BRD", false));
    BOOST_CHECK(GetBoolArg("-BRD", true));

    ResetArgs("-BRD=1 -noBRD=1");  // -BRD should win
    BOOST_CHECK(GetBoolArg("-BRD"));
    BOOST_CHECK(GetBoolArg("-BRD", false));
    BOOST_CHECK(GetBoolArg("-BRD", true));

    ResetArgs("-BRD=0 -noBRD=0");  // -BRD should win
    BOOST_CHECK(!GetBoolArg("-BRD"));
    BOOST_CHECK(!GetBoolArg("-BRD", false));
    BOOST_CHECK(!GetBoolArg("-BRD", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--BRD=1");
    BOOST_CHECK(GetBoolArg("-BRD"));
    BOOST_CHECK(GetBoolArg("-BRD", false));
    BOOST_CHECK(GetBoolArg("-BRD", true));

    ResetArgs("--noBRD=1");
    BOOST_CHECK(!GetBoolArg("-BRD"));
    BOOST_CHECK(!GetBoolArg("-BRD", false));
    BOOST_CHECK(!GetBoolArg("-BRD", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-BRD", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-BRD", "eleven"), "eleven");

    ResetArgs("-BRD -bar");
    BOOST_CHECK_EQUAL(GetArg("-BRD", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-BRD", "eleven"), "");

    ResetArgs("-BRD=");
    BOOST_CHECK_EQUAL(GetArg("-BRD", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-BRD", "eleven"), "");

    ResetArgs("-BRD=11");
    BOOST_CHECK_EQUAL(GetArg("-BRD", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-BRD", "eleven"), "11");

    ResetArgs("-BRD=eleven");
    BOOST_CHECK_EQUAL(GetArg("-BRD", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-BRD", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-BRD", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-BRD", 0), 0);

    ResetArgs("-BRD -bar");
    BOOST_CHECK_EQUAL(GetArg("-BRD", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);

    ResetArgs("-BRD=11 -bar=12");
    BOOST_CHECK_EQUAL(GetArg("-BRD", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 12);

    ResetArgs("-BRD=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-BRD", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--BRD");
    BOOST_CHECK_EQUAL(GetBoolArg("-BRD"), true);

    ResetArgs("--BRD=verbose --bar=1");
    BOOST_CHECK_EQUAL(GetArg("-BRD", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-bar", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-noBRD");
    BOOST_CHECK(!GetBoolArg("-BRD"));
    BOOST_CHECK(!GetBoolArg("-BRD", true));
    BOOST_CHECK(!GetBoolArg("-BRD", false));

    ResetArgs("-noBRD=1");
    BOOST_CHECK(!GetBoolArg("-BRD"));
    BOOST_CHECK(!GetBoolArg("-BRD", true));
    BOOST_CHECK(!GetBoolArg("-BRD", false));

    ResetArgs("-noBRD=0");
    BOOST_CHECK(GetBoolArg("-BRD"));
    BOOST_CHECK(GetBoolArg("-BRD", true));
    BOOST_CHECK(GetBoolArg("-BRD", false));

    ResetArgs("-BRD --noBRD");
    BOOST_CHECK(GetBoolArg("-BRD"));

    ResetArgs("-noBRD -BRD"); // BRD always wins:
    BOOST_CHECK(GetBoolArg("-BRD"));
}

BOOST_AUTO_TEST_SUITE_END()
