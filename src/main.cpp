#define WLR_USE_UNSTABLE

#include <hyprland/src/config/ConfigManager.hpp>
#include <hyprland/src/debug/Log.hpp>
#include <hyprland/src/managers/input/TextInput.hpp>
#include <hyprlang.hpp>

#include "globals.hpp"

inline CFunctionHook *textInputEnableHook = nullptr;
typedef void (*origOnEnable)(void *, CWLSurfaceResource *surfV1);
inline CFunctionHook *textInputDisableHook = nullptr;
typedef void (*origOnDisable)(void *);

APICALL EXPORT std::string PLUGIN_API_VERSION() { return HYPRLAND_API_VERSION; }

void onTextInputEnable(void *thisptr, CWLSurfaceResource *surfV1) {
  static auto *const ONFOCUS =
      (Hyprlang::STRING const *)HyprlandAPI::getConfigValue(
          PHANDLE, "plugin:showmyosk:on_focus")
          PHANDLE, "plugin:show-my-osk:on_focus")
          ->getDataStaticPtr();
  Debug::log(LOG, "[show-my-osk] input field focused");
  HyprlandAPI::invokeHyprctlCommand("dispatch", *ONFOCUS);

  (*(origOnEnable)textInputEnableHook->m_pOriginal)(thisptr, surfV1);
  (*(origOnEnable)textInputEnableHook->m_original)(thisptr, surfV1);
}

void onTextInputDisable(void *thisptr) {
  static auto *const ONLEAVE =
      (Hyprlang::STRING const *)HyprlandAPI::getConfigValue(
          PHANDLE, "plugin:showmyosk:on_leave")
          PHANDLE, "plugin:show-my-osk:on_leave")
          ->getDataStaticPtr();
  Debug::log(LOG, "[show-my-osk] input field unfocused");
  HyprlandAPI::invokeHyprctlCommand("dispatch", *ONLEAVE);

  // Why this crashes Hyprland???
  //(*(origOnDisable)textInputDisableHook->m_pOriginal)(thisptr);
  // Why this crashes Hyprland..?
  //(*(origOnDisable)textInputDisableHook->m_original)(thisptr);
}

APICALL EXPORT std::string PLUGIN_API_VERSION() { return HYPRLAND_API_VERSION; }
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
    throw std::runtime_error("[show-my-osk] function hook failed");

  HyprlandAPI::addConfigValue(PHANDLE, "plugin:showmyosk:on_focus",
                              Hyprlang::STRING{"exec pkill -USR2 wvkbd"});
  HyprlandAPI::addConfigValue(PHANDLE, "plugin:showmyosk:on_leave",
                              Hyprlang::STRING{"exec pkill -USR1 wvkbd"});
  HyprlandAPI::addConfigValue(PHANDLE, "plugin:show-my-osk:on_focus",
                              Hyprlang::STRING{"pkill -USR2 wvkbd"});
  HyprlandAPI::addConfigValue(PHANDLE, "plugin:show-my-osk:on_leave",
                              Hyprlang::STRING{"pkill -USR1 wvkbd"});

  return {"Show My OSK",
  return {"show-my-osk",
          "Pops up On-screen keyboard automatically when focusing an input "
          "field",
          "ckgxrg", "0.1"};
}

APICALL EXPORT void PLUGIN_EXIT() { ; }
