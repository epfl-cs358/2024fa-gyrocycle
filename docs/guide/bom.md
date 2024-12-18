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
            <td rowspan="6">Power</td>
            <td>LIPO battery</td>
            <td>3S-11.1V, 5500 mAh, 20C (110A)</td>
            <td>1</td>
        </tr>
        <tr>
            <td>LIPO protection circuit</td>
            <td>BMS 3S (12.6V – 60A) Lithium Battery Protection Module (Balanced)</td>
            <td>1</td>
        </tr>
        <tr>
            <td>Connector to plug the LIPO in</td>
            <td>XT90 male</td>
            <td>1</td>
        </tr>
        <tr>
            <td>Intermediate connectors for LIPO</td>
            <td>XT60 male + female pair</td>
            <td>2</td>
        </tr>
        <tr>
            <td>On-Off switch</td>
            <td>-</td>
            <td>1</td>
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
            <td>GT2 belt 6mm wide 232mm long (we used <a href="https://www.3dware.ch/en/accessories/mechanics-accessories/3dware-ch-handelsware-01900920-232-gt2-belt-6mm-wide-232mm-long">this one</a>)</td>
            <td>1</td>
        </tr>
        <tr>
            <td rowspan="3">Moving</td>
            <td>Propulsion motor</td>
            <td>TT motor yellow geared</td>
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

A few notes about the above BoM.

- We do not provide a reference for the USB isolator, because no particular one should be needed. It is only a component meant to protect your laptop in case of back-EMF or problems with the LIPO, we recommend you use one.

- We do not provide a reference for the On-Off switch either because it shouldn't matter, just make sure it can whistand the voltage and current provided by the protection circuit to the bicycle components.

- In the original build, we had problems with the LIPO protection circuit (it prevented us from giving enough current to the motor), so we removed it (on course staff recommendation, and after talking to the SPOT staff). **[LIPOs are dangerous components](./words-of-caution), this was no trivial decision. Do not try this at home unless you know exactly what you are doing.**

- If you decide to go with no protection board (again, don't do this unless you know what you are doing), you could technically do without the XT60 connectors listed in the BoM, if you connect the LIPO directly to the power supply of the bicycle with a male XT90.

    We did not do this, as we had already soldered the XT60, and it would have taken more time to do with only one XT90 connector, but you might want to try this instead if you know what you are doing.

Now that the required equipment and components have been layed out, the next step is to start the actual manufacturing of the bike by meeting our CAD.