# Prerequisites

Unlink open-source software, open-hardware requires you to *build* the thing. Hence, you will need access to some tools to manufacture the bike:

- **3D Printer**
    
    Parts of the bicycle have to be 3D-printed. You will need access to a 3D-printer that can print with [**PETG**](https://en.wikipedia.org/wiki/Polyethylene_terephthalate) and [**TPU**](https://en.wikipedia.org/wiki/Thermoplastic_polyurethane) (for the bike's tires).

    We used a Prusa MK4 for the original build of GyroCycle, with a build volume of `250 x 210 x 220` millimeters.

- **Laser Cutter**

    The bike's rigid frame is made out of [MDF](https://en.wikipedia.org/wiki/Medium-density_fibreboard). Ideally, you will need a laser-cutter to manufacture its parts, as it needs to be quite precise.

    We used a BCL-0605MU laser cuter for the original build of GyroCycle, with a cutting area of `500 x 600` millimeters.

    ::: info
    We used another, bigger laser cuter for the stand of the bicycle, but the parts of the bicycle itself can be manufactured on a smaller area.
    :::

- **Soldering Equipment**

    The bicycle consists of several electronical components that need to be wired together. Most of the connections require cables to be [soldered](https://en.wikipedia.org/wiki/Soldering) together or to a board.

- **Milling Machine, Schaublin and Circular Sawivaan**

    Since the flywheel can go to great speeds and torques to control the bike's tilt, we manufactured the shaft it rotates on in steel using a few manufacturing machines.

    We used EPFL's [SPOT](https://www.epfl.ch/education/educational-initiatives/discovery-learning-program-2/prototyping/page-111858-en-html/) machines for the original build of GyroCycle. Specifically an [*Evolution Circular Sawivaan*](https://make.epfl.ch/equipment/113/evolution-circular-sawivaan), a [*Schaublin 102 Mi Cf Lathe*](https://make.epfl.ch/equipment/82/schaublin-102-mi-cf-lathe) and an [*Aciera F1 Milling Machine*](https://make.epfl.ch/equipment/84/aciera-f1-milling-machine).

- **Drill**

    Some screw holes might be missing from the CAD and the provided files to laser-cut. You will need to drill additional screw holes where needed.

In addition to those tools, you will need actual components and pieces to assemble the bicycle and make it work, move and balance. This is described in the next section, the Bill of Materials (BoM).