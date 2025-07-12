# Show My OSK
A Hyprland plugin that automatically pops up your On-screen Keyboard when an input field is focused.   
This is just a simple listener for the Wayland `text_input_v3` protocol's enable and disable events and execute user-defined commands then.   

## Why use `text_input_v3` protocol?
Some On-screen Keyboards have auto-popup feature via the `input_method_v2` protocol. However, Wayland by-design only allows one such applications as the "input method" there, so it breaks any other input methods like ibus or fcitx, and unluckily I must use them, and listening to `text_input_v3` protocol at the compositor level is the one way to do this.   

## Usage
Add the following options to your hyprland configuration file (using [wvkbd](https://github.com/jjsullivan5196/wvkbd) as an example) :
```
plugin {
  show-my-osk {
    on-focus=pkill -USR2 wvkbd
    on-unfocus=pkill -USR1 wvkbd
  }
}
```

If you want to toggle the plugin at runtime, use the following dispatchers:
- plugin:show-my-osk:enable
- plugin:show-my-osk:disable
- plugin:show-my-osk:toggle
