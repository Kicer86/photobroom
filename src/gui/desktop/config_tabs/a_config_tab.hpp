
#ifndef A_CONFIG_TAB_HPP
#define A_CONFIG_TAB_HPP

#include <cassert>

#include <ui_utils/iconfig_dialog_manager.hpp>


struct IConfiguration;

template<typename T>
class AConfigTab: public IConfigTab
{
    public:
        AConfigTab(): m_tab(nullptr), m_configuration(nullptr) {}
        AConfigTab(const AConfigTab &) = delete;

        AConfigTab& operator=(const AConfigTab &) = delete;

        void set(IConfiguration* config)
        {
            assert(m_configuration == nullptr);
            m_configuration = config;
        }

    protected:
        void setTabWidget(T* tab)
        {
            assert(m_tab == nullptr);
            m_tab = tab;
        }

        IConfiguration* configuration() const
        {
            return m_configuration;
        }

        T* tabWidget() const
        {
            return m_tab;
        }

    private:
        T* m_tab;
        IConfiguration* m_configuration;
};

#endif
