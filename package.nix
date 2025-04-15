{
  lib,
  hyprland,
  hyprlandPlugins,
}:
hyprlandPlugins.mkHyprlandPlugin hyprland {
  pluginName = "show-my-osk";
  version = "0.1";
  src = ./.;

  inherit (hyprland) nativeBuildInputs;

  meta = with lib; {
    homepage = "https://github.com/ckgxrg-salt/ShowMyOSK";
    description = "Automatically pops up on-screen keyboard when an input field is focused for Hyprland.";
    license = licenses.bsd2;
    platforms = platforms.linux;
  };
}
