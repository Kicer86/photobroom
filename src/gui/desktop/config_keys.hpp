
#ifndef CONFIG_KEYS_HPP
#define CONFIG_KEYS_HPP

namespace ViewConfigKeys
{
    const char* const itemsMargin    = "view::margin";
    const char* const itemsSpacing   = "view::spacing";
    const char* const thumbnailWidth = "view::thumbnailWidth";
    const char* const bkg_color_odd  = "view::background_color_odd";
    const char* const bkg_color_even = "view::background_color_even";
}

namespace UpdateConfigKeys
{
    const char* const updateFrequency = "updater::frequency";
    const char* const updateEnabled   = "updater::enabled";
    const char* const lastCheck       = "updater::last_check";
}

#endif // CONFIG_KEYS_HPP
