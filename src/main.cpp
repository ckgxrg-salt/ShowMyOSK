#include <hyprlang.hpp>
#define WLR_USE_UNSTABLE

#include <hyprland/src/config/ConfigManager.hpp>
#include <hyprland/src/managers/input/TextInput.hpp>
#include <hyprland/src/plugins/PluginAPI.hpp>

#include <unistd.h>

inline HANDLE PHANDLE = nullptr;
inline CFunctionHook *textInputEnableHook = nullptr;
inline CFunctionHook *textInputDisableHook = nullptr;

APICALL EXPORT std::string PLUGIN_API_VERSION() { return HYPRLAND_API_VERSION; }

void onTextInputEnable() {
  static auto *const ONFOCUS =
      (Hyprlang::STRING const *)HyprlandAPI::getConfigValue(
          PHANDLE, "plugin:showmyosk:on_focus")
          ->getDataStaticPtr();
  HyprlandAPI::invokeHyprctlCommand("dispatch", *ONFOCUS);
}

void onTextInputDisable() {
  static auto *const ONLEAVE =
      (Hyprlang::STRING const *)HyprlandAPI::getConfigValue(
          PHANDLE, "plugin:showmyosk:on_leave")
          ->getDataStaticPtr();
  HyprlandAPI::invokeHyprctlCommand("dispatch", *ONLEAVE);
}

APICALL EXPORT PLUGIN_DESCRIPTION_INFO PLUGIN_INIT(HANDLE handle) {
  PHANDLE = handle;

  auto fns = HyprlandAPI::findFunctionsByName(PHANDLE, "onEnabled");
  for (auto &fn : fns) {
    if (!fn.demangled.contains("CTextInput"))
      continue;
    textInputEnableHook = HyprlandAPI::createFunctionHook(
        PHANDLE, fn.address, (void *)&onTextInputEnable);
  }

  fns = HyprlandAPI::findFunctionsByName(PHANDLE, "onDisabled");
  for (auto &fn : fns) {
    if (!fn.demangled.contains("CTextInput"))
      continue;
    textInputDisableHook = HyprlandAPI::createFunctionHook(
        PHANDLE, fn.address, (void *)&onTextInputDisable);
  }

  bool hookRes = textInputEnableHook->hook();
  hookRes = hookRes && textInputDisableHook->hook();
  if (!hookRes)
    throw std::runtime_error("show-my-osk: function hook failed");

  HyprlandAPI::addConfigValue(PHANDLE, "plugin:showmyosk:on_focus",
                              Hyprlang::STRING{"exec pkill -USR2 wvkbd"});
  HyprlandAPI::addConfigValue(PHANDLE, "plugin:showmyosk:on_leave",
                              Hyprlang::STRING{"exec pkill -USR1 wvkbd"});

  return {"Show My OSK",
          "Pops up On-screen keyboard automatically when focusing an input "
          "field",
          "ckgxrg", "0.1"};
}

APICALL EXPORT void PLUGIN_EXIT() { ; }
