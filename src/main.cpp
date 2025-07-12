#define WLR_USE_UNSTABLE

#include <hyprland/src/config/ConfigManager.hpp>
#include <hyprland/src/debug/Log.hpp>
#include <hyprland/src/managers/input/TextInput.hpp>
#include <hyprlang.hpp>

#include "globals.hpp"
#include "summon.hpp"

inline CFunctionHook *textInputEnableHook = nullptr;
typedef void (*origOnEnable)(void *, CWLSurfaceResource *surfV1);
inline CFunctionHook *textInputDisableHook = nullptr;
typedef void (*origOnDisable)(void *);

bool enabled;

void onTextInputEnable(void *thisptr, CWLSurfaceResource *surfV1) {
  if (enabled) {
    static auto *const ONFOCUS =
        (Hyprlang::STRING const *)HyprlandAPI::getConfigValue(
            PHANDLE, "plugin:show-my-osk:on-focus")
            ->getDataStaticPtr();
    Debug::log(LOG, "[show-my-osk] input field focused");
    bool status = summon(*ONFOCUS);
    if (!status) {
      Debug::log(LOG, "[show-my-osk] failed to execute on-focus command");
    }
  }

  (*(origOnEnable)textInputEnableHook->m_original)(thisptr, surfV1);
}

void onTextInputDisable(void *thisptr) {
  if (enabled) {
    static auto *const ONLEAVE =
        (Hyprlang::STRING const *)HyprlandAPI::getConfigValue(
            PHANDLE, "plugin:show-my-osk:on-unfocus")
            ->getDataStaticPtr();
    Debug::log(LOG, "[show-my-osk] input field unfocused");
    bool status = summon(*ONLEAVE);
    if (!status) {
      Debug::log(LOG, "[show-my-osk] failed to execute on-unfocus command");
    }
  }

  // Why this crashes Hyprland..?
  //(*(origOnDisable)textInputDisableHook->m_original)(thisptr);
}

static SDispatchResult enablePlugin(std::string in) {
  enabled = false;
  return SDispatchResult{};
}
static SDispatchResult disablePlugin(std::string in) {
  enabled = false;
  return SDispatchResult{};
}
static SDispatchResult togglePlugin(std::string in) {
  enabled = !enabled;
  return SDispatchResult{};
}

APICALL EXPORT std::string PLUGIN_API_VERSION() { return HYPRLAND_API_VERSION; }

APICALL EXPORT PLUGIN_DESCRIPTION_INFO PLUGIN_INIT(HANDLE handle) {
  PHANDLE = handle;
  enabled = true;

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

  HyprlandAPI::addConfigValue(PHANDLE, "plugin:show-my-osk:on-focus",
                              Hyprlang::STRING{"pkill -USR2 wvkbd"});
  HyprlandAPI::addConfigValue(PHANDLE, "plugin:show-my-osk:on-unfocus",
                              Hyprlang::STRING{"pkill -USR1 wvkbd"});

  bool dispatcherRes = true;
  dispatcherRes = dispatcherRes &&
                  HyprlandAPI::addDispatcherV2(
                      PHANDLE, "plugin:show-my-osk:enable", ::enablePlugin);
  dispatcherRes = dispatcherRes &&
                  HyprlandAPI::addDispatcherV2(
                      PHANDLE, "plugin:show-my-osk:disable", ::disablePlugin);
  dispatcherRes = dispatcherRes &&
                  HyprlandAPI::addDispatcherV2(
                      PHANDLE, "plugin:show-my-osk:toggle", ::togglePlugin);
  if (!dispatcherRes)
    throw std::runtime_error("[show-my-osk] register dispatchers failed");

  return {"show-my-osk",
          "Pops up On-screen keyboard automatically when focusing an input "
          "field",
          "ckgxrg", "0.1"};
}

APICALL EXPORT void PLUGIN_EXIT() { ; }
