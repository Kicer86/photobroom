
#include <sstream>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <core/ts_multi_queue.hpp>


TEST(TS_MultiQueueTest, isConstructible)
{
    EXPECT_NO_THROW(
        {
            TS_MultiQueue<int> q;
        });
}


TEST(TS_MultiQueueTest, QuitsWithTimeoutWhenNoDataIncomes)
{
    TS_MultiQueue<int> q;
    using namespace std::literals;

    auto r = q.pop_for(1ms);

    EXPECT_EQ(false, r.has_value());
}


TEST(TS_MultiQueueTest, ReturnsWhatProducerGenerated)
{
    TS_MultiQueue<int> q;
    using namespace std::literals;

    auto p = q.prepareProducer();

    p->push(1);
    p->push(2);
    p->push(3);
    p->push(4);

    auto r = q.pop();
    EXPECT_EQ(1, *r);

    r = q.pop();
    EXPECT_EQ(2, *r);

    r = q.pop();
    EXPECT_EQ(3, *r);

    r = q.pop();
    EXPECT_EQ(4, *r);

    r = q.pop_for(1ms);

    EXPECT_EQ(false, r.has_value());
}


TEST(TS_MultiQueueTest, ReturnsMixedProductionOfManyProducers)
{
    TS_MultiQueue<int> q;
    using namespace std::literals;

    auto p1 = q.prepareProducer();
    auto p2 = q.prepareProducer();
    auto p3 = q.prepareProducer();
    auto p4 = q.prepareProducer();

    p1->push(101);
    p1->push(102);
    p1->push(103);
    p1->push(104);

    p2->push(201);
    p2->push(202);
    p2->push(203);
    p2->push(204);

    p3->push(301);
    p3->push(302);
    p3->push(303);
    p3->push(304);

    p4->push(401);
    p4->push(402);
    p4->push(403);
    p4->push(404);

    int sum = 0;

    // Expected order in pop is: 101, 201, 301, 401, 102, 202, 302, 402, 103, ...
    // Outputs from producers should be mixed.
    for(int i = 0; i < 16; i++)
    {
        auto r = q.pop();

        const int p = (*r)/100;          // producer id
        const int e_p = i % 4 + 1;       // expected producer id

        EXPECT_EQ(e_p, p);               // we should be expecting this producer

        sum += (*r);
    }

    EXPECT_EQ(4040, sum);

    auto r = q.pop_for(1ms);

    EXPECT_EQ(false, r.has_value());
}
