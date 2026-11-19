# Escape room environment delivery — revised scene

Open `/Game/OutsourceEnv/L_EscapeRoom` in Unreal Engine 5.8.4.

This revision replaces the earlier escape room scene with the user's saved
`EscapeRoom_ClosedDoor_CenteredPivots_PBR` level. The existing map entry remains
unchanged; models, materials and textures belong to the new revision.

Contents: 1 map, 425 static meshes,
425 materials/material instances, 1250 textures;
430 scene actors. Assets are under
`/Game/OutsourceEnv/model/EscapeRoom_ClosedDoor_CenteredPivots_PBR`.

Validation: loaded in an isolated UE 5.8.4 project containing only this folder.
Actor identities, transforms and mesh references matched the saved source
after package relocation. All game-content dependencies are present in this
folder. No GameMode Override is set. The shared project's gameplay and
interactions were not tested by this delivery.

The local source project is unchanged. This commit only updates `Content/OutsourceEnv`.
The earlier version remains available in Git history (commit `2f90a97`).
`Texture_Credits.md` is retained as the earlier delivery's source record;
new texture provenance was not independently established in this upload.
