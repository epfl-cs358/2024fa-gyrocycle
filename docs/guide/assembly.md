# Assembly

Once you have obtained the [required components](./bom), printed the [3D parts](./3d-parts), soldered the [circuitry](./soldering) and laser-cut the [MDF parts](./laser-cutting), you are ready to assemble all those pieces together.

Refer to our CAD, our blueprints and our electrical schema to assemble and wire everything together.

| Description | Download link |
|-|-|
| CAD `.step` file | <a href="/step/complete-bicycle.step" download>Download</a> |
| Bicycle blueprints (`.pdf`) | <a href="/pdf/bicycle-blueprints.pdf" download>Download</a> |
| Electrical Schema (`.pdf`) | <a href="/pdf/electrical-schema.pdf" download>Download</a> |
| Electrical Schema (`.kicad_sch`) | <a href="/sch/electrical-schema.kicad_sdh" download>Download</a> |

Just a few notice before you start:

- Make sure to keep the ODrive's micro-USB port accessible, as the next step includes configuring it with `odrivetool`.

- You might observe that the shafts for the front and rear wheels of the bike have a too small diameter to properly fit in the bearings without floating.

    We solved this problem by adding tape on the shafts. This allowed us to add a few millimeters of diameter without re-printing the parts.

Once you assembled your bike, congratulations, you have the hardware ready! To make it work properly, there's one more step you need to take, and that would be the software setup.