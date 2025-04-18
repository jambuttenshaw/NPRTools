# NPRTools
Configurable shaders for performing non-photorealistic rendering post-processing effects.

## Setup
### Project Configuration
NPRTools exposes the current NPR parameters through custom world settings. This allows NPR effects to be assigned on a per-level basis.

**Method 1 - Using NPRWorldSettings**
NPRTools provides a custom world settings actor that can be used to expose the currently assigned NPR Parameters.

To modify the default world settings class, go to:
`Project Settings > Engine > General Settings > Default Classes`
Set World Settings Class to `NPRWorldSettings`.

Once you have restarted the editor, open `Window > World Settings` and now you can assign a NPR Parameters Asset to the `NPRParameters Asset` field.

**Method 2 - Using Custom World Settings**
In case you already implement your own custom world settings, you can implement the `INPRWorldSettingsInterface` to enable your custom world settings class to work with NPR.

See the implementation of `ANPRWorldSettings` for an example of how to implement this interface for your own world settings.

### Creating a Parameters Asset
To create your own custom effects, you can create a custom NPR Parameters Data Asset.