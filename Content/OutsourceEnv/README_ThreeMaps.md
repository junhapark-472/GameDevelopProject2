# Three additional environment maps

Open in Unreal Engine 5.8.4. These editable scenes are added alongside the existing
`/Game/OutsourceEnv/L_EscapeRoom`, which is unchanged by this delivery.

| Map | Saved scene actors |
| --- | ---: |
| `/Game/OutsourceEnv/L_Reception` | 21 |
| `/Game/OutsourceEnv/L_Home` | 65 |
| `/Game/OutsourceEnv/L_CafeExterior` | 156 |

Scene models, materials and textures are under `/Game/OutsourceEnv/model/L_Reception`,
`L_Home`, and `L_CafeExterior`. The source levels were named `L_Reception_PBR`,
`L_Home_PBR`, and `L_CafeExterior_PBR`. Root map names follow the delivery brief.

The exterior uses Ultra Dynamic Sky. Its required package dependency closure is
included at its original `/Game/UltraDynamicSky` path because relocating the vendor
blueprints caused compilation failures. This is the sole extra game-content root.

Validation: all three maps opened in an isolated UE 5.8.4 project containing only
the delivered assets. Actor identities, transforms, mesh assignments and material
assignments matched the saved source after package relocation. No missing game
packages or static mesh assignments were found. GameMode Override is unset in all
three maps. This check did not render the scenes or test gameplay, collisions or
puzzle interactions in the shared team project.

Only scene assets and this document are added. Existing project code, characters,
input settings, other team files and the earlier escape room assets are unchanged.
