
#ifndef CHAIN_CONTROLLER_HPP
#define CHAIN_CONTROLLER_HPP

#include <memory>

#include "task_executor_utils.hpp"


class Job
{
    public:
        template<typename Executor, typename Action>
        Job& on(Executor* e, Action&& action)
        {
            auto task = [action]
            {
                auto action_wrapper = [action]
                {
                    const auto result = action();

                };

                ExecutorTraits::exec(action_wrapper, e);
            };
        }

    private:
        struct Data
        {

        } m_data;

        Job() {}

        friend Job chain();
};

Job chain()
{
    return Job{};
}


#endif
