<script setup>
    import StlViewer from "../.vitepress/theme/components/StlViewer.vue";
</script>

# Meet Our CAD

The bicycle was designed using the student trial of Fusion360. We provide both the CAD of the bicycle itself and the CAD of the stand we designed for the bicycle.

## Bicycle

You can download the complete bike CAD as a `.step` file from <a href="/assets/step/complete-bicycle.step" download>this link</a>.

::: details Complete CAD Preview
<StlViewer stlUrl="/assets/stl/complete-bicycle.stl" />
:::

There are two things we want to point out about this CAD.

1. If you look at the front wheel axle, at the top, you will see two gears used by the Servo to steer.

    The larger gear might seem too large because it overlaps on the smaller one. This is however expected. When 3D-printed, the two gears perfectly match.

2. The flywheel is linked to the motor with a belt and two pulleys. The top pulley is visible through a hole, but the bottom one is not.

    This is an improvement that we recommend you do when you laser-cut your own bicycle. Put a hole at the bottom of the bike where the flywheel motor is, so that you can see inside.

    We had several problems with the bottom pulley, and we always had to disassemble a good part of the bike to access it. Adding a hole would make this process much easier.

## Stand

We designed a stand to put the bike on when it is not balancing. You can download its CAD as a `.step` file from <a href="/assets/step/stand-without-bicycle.step" download>this link</a>.

::: details Stand CAD Preview
<StlViewer stlUrl="/assets/stl/stand-without-bicycle.stl" meshColor="#a16207" />
:::

Note that we added some text at the time of laser-cutting the MDF, to write the name of the project on the stand along with the QR code required to connect to RemoteXY (see the part about software).

::: tip TODO
Add a picture of the actual bicycle stand we manufactured.
:::

Now that those have been pointed out, let's get into the manufacturing, starting with some soldering.