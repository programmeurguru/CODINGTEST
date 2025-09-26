
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "Randomizer.h"
#include "DistributionData.h"
#include "NumAndProbability.h"
#include "ProbabilisticRandomGen.h"

using ::testing::InSequence;
using ::testing::Return;

int main(int argc, char** argv) {
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}

namespace probabilistic_generator
{
    TEST(probabilistic_generator, TestNextFromSampleNormalUsage)
    {
        RandomizerProxy tr;

        double d = tr.randomize();
        EXPECT_TRUE(d >= 0.0 && d <= 1.0);

        std::vector<NumAndProbability> v{ {1, 0.1 }, {2, 0.2}, {3, 0.7} };
        try
        {
            ProbabilisticRandomGen pgen(tr, v);
            int i = pgen.nextFromSample();
            EXPECT_TRUE(i == 1 || i == 2 || i == 3);
        }
        catch (...)
        {
            FAIL() << "Error unexpected";
        }
    }

    TEST(probabilistic_generator, TestDistributionUniform)
    {
        std::vector<NumAndProbability> v{ {13, 1.0 } };
        DistributionData d;
		if(!d.initialize(v))
        {
            FAIL() << "Error unexpected";
        }

        int i = d.getIntFromDistrib(0.0001);
        EXPECT_EQ(i, 13);
        i = d.getIntFromDistrib(0.5);
        EXPECT_EQ(i, 13);
        i = d.getIntFromDistrib(1.0);
        EXPECT_EQ(i, 13);
    }

    TEST(probabilistic_generator, TestDistribution)
    {

        std::vector<NumAndProbability> data{ {1, 0.1 }, {2, 0.2}, {3, 0.7} };

        DistributionData d;
        if (!d.initialize(data))
        {
			FAIL() << "not initialized";
        }

        try 
        {
            int i = d.getIntFromDistrib(-0.0001);
            FAIL() << "expected : out of range";
        }
        catch (std::out_of_range const& err) {
            EXPECT_EQ(err.what(), std::string("error : proba out of range ]0,1]"));
        }
        catch (...) {
            FAIL() << "Expected std::out_of_range";
        }

        try
        {
            int i = d.getIntFromDistrib(0.0001);
            EXPECT_EQ(i, 1);
            i = d.getIntFromDistrib(0.09);
            EXPECT_EQ(i, 1);
            i = d.getIntFromDistrib(0.1);
            EXPECT_EQ(i, 1);
            i = d.getIntFromDistrib(0.11);
            EXPECT_EQ(i, 2);
            i = d.getIntFromDistrib(0.29);
            EXPECT_EQ(i, 2);
            i = d.getIntFromDistrib(0.30);
            EXPECT_EQ(i, 2);
            i = d.getIntFromDistrib(0.31);
            EXPECT_EQ(i, 3);
            i = d.getIntFromDistrib(0.99);
            EXPECT_EQ(i, 3);
            i = d.getIntFromDistrib(1.0);
            EXPECT_EQ(i, 3);
        }
        catch (...) {
            FAIL() << "Error unexpected";
        }

        try
        {
            int i = d.getIntFromDistrib(1.01);
            FAIL() << "expected : out of range";
        }
        catch (std::out_of_range const& err) {
            EXPECT_EQ(err.what(), std::string("error : proba out of range ]0,1]"));
        }
        catch (...) {
            FAIL() << "Expected std::out_of_range";
        }
    }
	// simple mMOC of randomizer to have a fixed value
    class TestRandomizerProxy : public IRandomizer
    {
    public:
        double _expected;

        TestRandomizerProxy(double expected) : _expected(expected)
        {
        }

        double randomize() override
        {
            return _expected;
        }
    };

    TEST(probabilistic_generator, TestNextFromSampleWithFixedRandomizer)
    {
        TestRandomizerProxy tr(0.2);
        std::vector<NumAndProbability> v{ {1, 0.1 }, {2, 0.2}, {3, 0.7} };
        try 
        {
            ProbabilisticRandomGen pgen(tr, v);
            int i = pgen.nextFromSample();
            EXPECT_EQ(i, 2);
            i = pgen.nextFromSample();
            EXPECT_EQ(i, 2);
        }
        catch (...) 
        {
            FAIL() << "Error unexpected";
		}
    }

    class MockRandomizer : public IRandomizer {
    public:
        MOCK_METHOD(double, randomize, (), (override));
    };

    TEST(probabilistic_generator, TestNextFromSampleWithFiveRandomValues)
    {
		// we mock the randomizer to have a predictable sequence of values
        MockRandomizer mockRng;

        std::vector<NumAndProbability> dist{{1, 0.1},{2, 0.2},{3, 0.7}};
        {
            InSequence seq;
            EXPECT_CALL(mockRng, randomize())
                .WillOnce(Return(0.05))   // [0.00–0.10[ -> 1
                .WillOnce(Return(0.15))   // [0.10–0.30[ -> 2
                .WillOnce(Return(0.25))   // [0.10–0.30[ -> 2
                .WillOnce(Return(0.60))   // [0.30–1.00] -> 3
                .WillOnce(Return(0.95));  // [0.30–1.00] -> 3
        }

        try 
        {
            ProbabilisticRandomGen pgen(mockRng, dist);

            EXPECT_EQ(pgen.nextFromSample(), 1);
            EXPECT_EQ(pgen.nextFromSample(), 2);
            EXPECT_EQ(pgen.nextFromSample(), 2);
            EXPECT_EQ(pgen.nextFromSample(), 3);
            EXPECT_EQ(pgen.nextFromSample(), 3);
        }
        catch (...)
        {
            FAIL() << "Error unexpected";
        }
    }
    
} // namespace probabilistic_generator