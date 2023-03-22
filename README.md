# Evergreen
Evergreen is a general purpose application engine for building desktop applications with a UI and/or 3D rendering/simulation.
Evergeen is intended to be built as a DLL in imported by the client application.
See the `Sandbox` project for an example of how to import the Evergreen DLL and create an application.
## Features
Evergreen allows you to create the UI directly in code, via JSON files, or both.
Here is the current list of supported UI controls:
- Text
- TextInput
- Button (and RoundedButton)
- RadioButton
- SliderInt and SliderFloat
- Pane 
-- This is simply a pop up window that is displayed on top of the main UI
- ScrollableLayout
-- This allows you to place controls within a region of the UI that is scrollable
- Viewport
-- This is a 3D viewport that allows you to render a 3D scene onto

## JSON
A portion or the entire UI can be specified in one or more JSON files.
To load a UI that is defined in JSON, the client application simply needs to make two function calls:
```c++
m_ui->SetUIRoot("src/json/");
m_ui->LoadUI("main.json");
```
See `Evergreen/src/Evergreen/UI/json-examples/` for documentation on how to define each type of control within JSON.

## 3D Rendering
At the moment, all 3D Rendering is performed by Direct3D11 and the UI is rendered using Direct2D.
I did begin to add an implementation of Evergreen so that it could be compiled to use Direct3D12.
However, in order to use Direct2D with Direct3D12, you would need to use Direct3D11On12 and I was unhappy with
the amount of warnings D3D11On12 was giving me and people also reported poor performance. So for the time being,
I am only supported Direct3D11.

Also to note, while Evergreen strives provide a lot of UI support with several built-in UI controls, the approach
to 3D rendering is quite different. Evergreen primarily concerns itself with creating the device, device context, 
swap chain, render target, and depth stencil. It is up to the client application to create shaders, buffers, etc.