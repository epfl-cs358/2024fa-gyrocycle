<script setup>
    import StlViewer from "../.vitepress/theme/components/StlViewer.vue";
    import BicyclePartsToLaserCutTable from "../.vitepress/theme/components/BicyclePartsToLaserCutTable.vue";
    import StandPartsToLaserCutTable from "../.vitepress/theme/components/StandPartsToLaserCutTable.vue";
</script>

# Laser-Cut Parts

The bike's rigid frame is made out of MDF `5mm`. You will need a laser cutter to manufacture those pieces in a precise way.

Files for laser-cutting MDF parts are provided. There are two things to point out, though:

- Some of the screw holes were not added to the CAD. Hence, you might have to **drill additional screw holes** yourself if they are not already laser-cut.

- You might want to add a hole in one of the MDF parts to **make the bottom pulley in front of the motor visible**. In our original design, one has to disassemble a significant part of the bicycle before one can even see the bottom pulley, which caused us some problems for debugging.

Following is the list of parts to be laser-cut in an MDF plate, divided into two parts:

1. Parts for the bicycle itself.
2. Parts for the bicycle's stand. Those are not necessary if you only want to build the bike.

## Bicycle Parts

MDF parts to be laser-cut for the bicycle itself form the rigid frame of the bicycle.

::: details Preview of the rigid frame
<StlViewer stlUrl="/stl/bicycle-mdf-frame.stl" meshColor="#a16207" />
:::

<BicyclePartsToLaserCutTable />

## Stand Parts

We designed a stand to put the bike on when it is not balancing. Those are the parts needed to manufacture it.

::: details Preview of the stand with the bike on
<StlViewer stlUrl="/stl/stand-with-bicycle.stl" meshColor="#a16207" />
:::

::: details Preview of the stand without the bike
<StlViewer stlUrl="/stl/stand-without-bicycle.stl" meshColor="#a16207" />
:::

<StandPartsToLaserCutTable />

Once you have 3D-printed necessary parts and laser-cut others, you still need to manufacture a central piece of the bicycle: the metal shaft for the flywheel. This is the subject of the next section.