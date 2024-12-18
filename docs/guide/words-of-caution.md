# A few Words of Caution

Before we go any further into the manufacturing and assembling of the bicycle, a few words of caution are advisable.

## Using a LIPO battery

Because the bicycle requires high currents to balance (the motor of the flywheel peaks to change direction and tilt the bike in the right direction), we use a LIPO battery for the project.

LIPO batteries are dangerous components. If not used right, they will likely explode in a fireball and/or release toxic chemicals. There are tons of YouTube videos that show this, for example [this one](https://www.youtube.com/watch?v=aZOKLpOn_W4).

If you want to build GyroCycle for yourself, make sure you know how to properly handle LIPO batteries, or that you know someone who does. As computer science students, we are unfortunately not qualified to write a complete guide on how to properly handle LIPO batteries.

## High-Speed Flywheel

The central balancing component of the bicycle is its flywheel. The role of the flywheel is to turn in a certain direction at a certain speed to counter the bicycle's tilting, thus making it stay stable.

To accomplish this role, the flywheel might start turning at very high speeds, which can be scary and dangerous if not solidly fixed.

Our strategy to mitigate this risk was to test the flywheel progressively. The motor driver we use provides a built-in maximum speed configuration. You should test your build with a small max speed at first. The bicycle might not balance very well with small speeds, but it will allow you to see whether your flywheel can withstand higher speeds.

## Disclaimer

We cannot provide any guarantee of the bicycle's safety. We built and tested it ourselves, however depending on how you build it you might encounter problems we do not know of.

Be careful, use common sense, and test step by step (for example, increase the flywheel's max speed progressively during the tests) to avoid injuring yourself or others.

With that in mind, let's dive into what you'll need to manufacture the bike.