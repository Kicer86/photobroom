
#ifndef CONFIG_KEYS_HPP
#define CONFIG_KEYS_HPP

namespace ViewConfigKeys
{
    [[deprecated]] const char* const itemsMargin    = "view::margin";
    [[deprecated]] const char* const itemsSpacing   = "view::spacing";
    [[deprecated]] const char* const thumbnailWidth = "view::thumbnailWidth";
    [[deprecated]] const char* const bkg_color_odd  = "view::background_color_odd";
    [[deprecated]] const char* const bkg_color_even = "view::background_color_even";
}

namespace UpdateConfigKeys
{
    [[deprecated]] const char* const updateFrequency = "updater::frequency";
    const char* const updateEnabled   = "updater::enabled";
    const char* const lastCheck       = "updater::last_check";
}

#endif // CONFIG_KEYS_HPP
