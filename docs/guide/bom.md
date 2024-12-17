# Bill of Materials (BoM)

In addition to tools, 3D-printed parts and laser-cut parts, you will need generic pieces and electronical components to build the bike. Here is a list ([BoM](https://en.wikipedia.org/wiki/Bill_of_materials)) of the required components for GyroCycle:

<table>
    <thead>
        <tr>
            <th>Category</th>
            <th>Description</th>
            <th>Reference</th>
            <th>Qty</th>
        </tr>
    </thead>
    <tbody>
        <tr>
            <td>MCU</td>
            <td>Main microcontroller</td>
            <td>NodeMCU ESP32</td>
            <td>1</td>
        </tr>
        <tr>
            <td rowspan="4">Power</td>
            <td>LIPO battery</td>
            <td>3S-11.1V, 5500 mAh, 20C (110A)</td>
            <td>1</td>
        </tr>
        <tr>
            <td>LIPO protection circuit</td>
            <td>-</td>
            <td>1</td>
        </tr>
        <tr>
            <td>LIPO connectors</td>
            <td>XT90 male + female</td>
            <td>2</td>
        </tr>
        <tr>
            <td>Buck converter</td>
            <td>Joy-it SBC-Buck01</td>
            <td>1</td>
        </tr>
        <tr>
            <td rowspan="8">Balancing</td>
            <td>Gyroscope and accelerometer</td>
            <td>MPU 6050</td>
            <td>1</td>
        </tr>
        <tr>
            <td>Flywheel motor</td>
            <td>Racerstar 3536</td>
            <td>1</td>
        </tr>
        <tr>
            <td>Flywheel motor optical position sensor</td>
            <td>AMT102-V</td>
            <td>1</td>
        </tr>
        <tr>
            <td>Flywheel motor FOC controller</td>
            <td>ODrive v3.6</td>
            <td>1</td>
        </tr>
        <tr>
            <td>Bearings for wheels</td>
            <td><a href="https://www.galaxus.ch/fr/s4/product/bones-bearings-reds-roulement-a-billes-6424172?supplier=2403620">Pack of 8 Bones Bearing Reds</a></td>
            <td>1x8</td>
        </tr>
        <tr>
            <td>Flywheel pulley</td>
            <td>GT2 Pulley 40 teeth 8mm Bore</td>
            <td>1</td>
        </tr>
        <tr>
            <td>Flywheel motor pulley</td>
            <td>GT2 Pulley 20 Teeth 6.35mm Bore</td>
            <td>1</td>
        </tr>
        <tr>
            <td>Belt between flywheel and motor</td>
            <td>TODO</td>
            <td>1</td>
        </tr>
        <tr>
            <td rowspan="3">Moving</td>
            <td>Propulsion motor</td>
            <td>Yellow gear motor</td>
            <td>1</td>
        </tr>
        <tr>
            <td>Propulsion motor driver</td>
            <td>L298N</td>
            <td>1</td>
        </tr>
        <tr>
            <td>Steering Servo</td>
            <td>DMS15</td>
            <td>1</td>
        </tr>
        <tr>
            <td rowspan="1">Miscellaneous</td>
            <td>USB isolator</td>
            <td>-</td>
            <td>1</td>
        </tr>
    </tbody>
</table>

::: tip TODO
What to do about the LIPO connectors?
:::

::: tip TODO
Reference for the "yellow" gear motor?
:::

::: tip TODO
Reference for the LIPO protection board?
:::

::: tip TODO
Ask others to review the pulleys and confirm those are the ones we used.
:::

::: tip TODO
Ask others the reference/specs for the belt we ended up using
:::

Now that the required equipment and components have been layed out, the next step is to start the actual manufacturing of the bike by meeting our CAD.