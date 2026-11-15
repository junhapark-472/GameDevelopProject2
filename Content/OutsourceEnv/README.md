# Escape room environment delivery

Open `/Game/OutsourceEnv/L_EscapeRoom` in Unreal Engine 5.8.4.

This folder contains the editable four-room escape room environment: 1 map,
640 static meshes, 426 materials, and 1178 textures. The scene contains 1029
editor actors. All game-content dependencies are contained in this folder.
The full roof is included, and key scene props remain separate actors.

The map has no GameMode Override, so it inherits the shared project's existing
first-person setup. Puzzle interactions are left for the shared project's
interaction system. Existing code, characters, input, project settings, and
other members' assets are unchanged by this delivery.

Validation: the map was loaded in a clean UE 5.8.4 project containing only this
folder; actor identities and transforms matched the saved source scene, and
no missing or external `/Game` package dependencies were found. This was an
editor-load validation, not a gameplay test of the shared C++ project.

Texture source details and CC0 credits are in [Texture_Credits.md](Texture_Credits.md).
