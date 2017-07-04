
#ifndef A_CONFIG_TAB_HPP
#define A_CONFIG_TAB_HPP

#include <ui_utils/iconfig_dialog_manager.hpp>


struct IConfiguration;

template<typename T>
class AConfigTab: public IConfigTab
{
    public:
        AConfigTab(): m_tab(nullptr) {}
        AConfigTab(const AConfigTab &) = delete;

        AConfigTab& operator=(const AConfigTab &) = delete;

        void set(IConfiguration* config)
        {
            m_configuration = config;
        }

        QWidget* constructTab() override
        {
            assert(m_tab == nullptr);
            m_tab = new T;

            return m_tab;
        }

    protected:
        IConfiguration* configuration() const
        {
            return m_configuration;
        }

    private:
        T* m_tab;
        IConfiguration* m_configuration;
};

#endif
